#include <windows.h>
#include <stdio.h>
#include <tchar.h>
#include <wincrypt.h> // Para calcular el checksum (MD5)

#pragma comment(lib, "Advapi32.lib") // Librería para funciones criptográficas

#define MD5LEN 16 // Longitud del checksum MD5

// Función para calcular el checksum MD5 de un archivo
BOOL GetFileChecksum(const TCHAR *filePath, BYTE *checksum) {
    BOOL result = FALSE;
    HANDLE hFile = CreateFile(filePath, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
    if (hFile == INVALID_HANDLE_VALUE) return FALSE;

    HCRYPTPROV hProv = 0;
    HCRYPTHASH hHash = 0;
    BYTE buffer[4096];
    DWORD bytesRead = 0;

    if (CryptAcquireContext(&hProv, NULL, NULL, PROV_RSA_FULL, CRYPT_VERIFYCONTEXT) &&
        CryptCreateHash(hProv, CALG_MD5, 0, 0, &hHash)) {
        while (ReadFile(hFile, buffer, sizeof(buffer), &bytesRead, NULL) && bytesRead != 0) {
            if (!CryptHashData(hHash, buffer, bytesRead, 0)) {
                CloseHandle(hFile);
                CryptReleaseContext(hProv, 0);
                CryptDestroyHash(hHash);
                return FALSE;
            }
        }

        DWORD checksumSize = MD5LEN;
        if (CryptGetHashParam(hHash, HP_HASHVAL, checksum, &checksumSize, 0)) {
            result = TRUE;
        }

        CryptDestroyHash(hHash);
    }

    CryptReleaseContext(hProv, 0);
    CloseHandle(hFile);
    return result;
}

// Copiar archivo o carpeta
void CopyFileOrFolder(const TCHAR *source, const TCHAR *destination) {
    if (GetFileAttributes(source) & FILE_ATTRIBUTE_DIRECTORY) {
        // Crear la carpeta si no existe
        CreateDirectory(destination, NULL);
    } else {
        // Copiar el archivo
        CopyFile(source, destination, FALSE);
    }
}

// Comparar carpetas y copiar archivos diferentes
void CompareAndExtract(const TCHAR *oldFolder, const TCHAR *newFolder, const TCHAR *outputFolder) {
    WIN32_FIND_DATA findFileData;
    TCHAR searchPath[MAX_PATH], oldFilePath[MAX_PATH], newFilePath[MAX_PATH], outputFilePath[MAX_PATH];
    HANDLE hFind;

    // Crear la carpeta de salida
    CreateDirectory(outputFolder, NULL);

    // Construir la ruta de búsqueda en la nueva carpeta
    _stprintf(searchPath, _T("%s\\*"), newFolder);

    hFind = FindFirstFile(searchPath, &findFileData);
    if (hFind == INVALID_HANDLE_VALUE) {
        _tprintf(_T("No se encontró la carpeta: %s\n"), newFolder);
        return;
    }

    do {
        if (_tcscmp(findFileData.cFileName, _T(".")) == 0 || _tcscmp(findFileData.cFileName, _T("..")) == 0)
            continue;

        // Construir rutas completas
        _stprintf(newFilePath, _T("%s\\%s"), newFolder, findFileData.cFileName);
        _stprintf(oldFilePath, _T("%s\\%s"), oldFolder, findFileData.cFileName);
        _stprintf(outputFilePath, _T("%s\\%s"), outputFolder, findFileData.cFileName);

        if (findFileData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) {
            // Si es una carpeta, procesarla recursivamente
            CompareAndExtract(oldFilePath, newFilePath, outputFilePath);
        } else {
            // Comparar archivos
            BYTE oldChecksum[MD5LEN], newChecksum[MD5LEN];
            BOOL oldExists = GetFileChecksum(oldFilePath, oldChecksum);
            BOOL newExists = GetFileChecksum(newFilePath, newChecksum);

            if (!oldExists || memcmp(oldChecksum, newChecksum, MD5LEN) != 0) {
                // Si el archivo no existe en la versión vieja o difiere, copiarlo
                CopyFileOrFolder(newFilePath, outputFilePath);
            }
        }
    } while (FindNextFile(hFind, &findFileData) != 0);

    FindClose(hFind);
}

int _tmain(int argc, TCHAR *argv[]) {
    if (argc != 4) {
        _tprintf(_T("Uso: %s <carpeta_vieja> <carpeta_nueva> <carpeta_salida>\n"), argv[0]);
        return 1;
    }

    const TCHAR *oldFolder = argv[1];
    const TCHAR *newFolder = argv[2];
    const TCHAR *outputFolder = argv[3];

    CompareAndExtract(oldFolder, newFolder, outputFolder);

    _tprintf(_T("Archivos y carpetas diferentes copiados a: %s\n"), outputFolder);
    return 0;
}
