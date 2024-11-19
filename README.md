# Compadre - El comparador campechano
Pequeño comparador de archivos, con extracción de los archivos que tienen distinto checksum. Creado con ChatGPT

## Uso
En terminal o cmd, ejecuta esta línea:
```console
compadre.exe "C:\\VersionVieja" "C:\\VersionNueva" "C:\\CarpetaSalida"
```

El programa comparará ambas versiones, y si algun archivo en la nueva tiene checksum distinto, o directamente no existe en la vieja, lo copirará a la carpeta de salida.

### Notas
Este script compara archivos byte por byte, ignorando la fecha de modificación u otros atributos.
Asegúrate de que los permisos de las carpetas permitan lectura y escritura.
El script es eficiente con archivos pequeños a medianos; para archivos extremadamente grandes, se podrían considerar técnicas adicionales como buffering optimizado. (No implementado actualmente)

## Compilación
Compila el script con un compilador como MinGW o MSVC, asegurándote de vincular la librería Advapi32.lib para las funciones criptográficas.

### * MinGW:
Usando su terminal sh, y teniendo gcc instalado, copia este código:
```console
gcc compadre.c -o compadre.exe -lAdvapi32
```

### * MSVC:
Usando cmd o el compilador que lleva MSVC, usa este código:
```console
cl compadre.c Advapi32.lib
```

### * TDM ( Descárgalo de [aquí](https://jmeubank.github.io/tdm-gcc/download/)):
Usando la terminal de Windows, usa este código:
```console
gcc -o compadre.exe compadre.c -static
```


