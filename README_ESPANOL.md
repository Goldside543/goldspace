![Drawing](https://github.com/user-attachments/assets/341a14bd-a23a-4742-ac67-bb30a29c6892)

# Goldspace
Un kernel tipo Unix que estoy haciendo. Tenemos el cargador de arranque, el kernel, el script del enlazador, el shell Gash, el Makefile...
Prácticamente es mejor que Linux.

# Gash
Ahora pasemos al shell, Gash. Gash es una combinación de "Gold" y "Bash", y está diseñado específicamente para Goldspace. ¿Cómo demonios le doy entrada, preguntas? ¡Bueno, el núcleo del kernel tiene un controlador de teclado dentro de él!

# Instrucciones de compilación:
* `git clone https://github.com/Goldside543/goldspace.git`
* `cd goldspace`
* `make` <br> <br>
(para compilar en 64 bits, los siguientes comandos van entre `cd goldspace` y `make`) <br> <br>
* `chmod +x scripts/64-bitifier.sh`
* `sed -i "s|path/to/kernel/source|$(pwd)|" scripts/64-bitifier.sh`
* `scripts/64-bitifier.sh`

# Lista de comandos:
* `help` muestra la ayuda.
* `clear` limpia la pantalla.
* `echo <mensaje>` te permite escribir algo, ¡y será repetido!
* `create <nombre_archivo>` ¡crea un archivo!
* `write <nombre_archivo> <datos>` escribe datos en un archivo.
* `read <nombre_archivo>` lee un archivo.
* `delete <nombre_archivo>` elimina un archivo.
* `render` realiza una prueba de renderizado en la GPU.
* `panic` causa un pánico del kernel.
* `builddate` imprime la fecha y hora de la compilación.

# Para documentación adicional...
¡Consulta el manual de usuario o el manual del desarrollador, convenientemente ubicados en el directorio de Documentación!

# Número de versión: 
Ver README.md
