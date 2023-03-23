# Screen Saver Conjunto de Mandelbrot
El proyecto consiste en la elabarión de un screesaver mediante el famoso fractal del conjunto de Mandelbrot.<br/>

## ¿Cómo ejecutar el proyecto? 🚀
Para poder visualizar el screensaver es necesario que valides que cumplas con los requisitos a continuación detallados. Este tutorial está específicamente orientado para la configuración del entorno en Clion, también puedes realizarlo en VSCODE, lo que debes de asegurar es poder correr SDL con el IDE o la configuración de tu preferencia.<br/>

### Prerrequisitos
Primero, asegúrate de tener instaladas las siguientes dependencias<br/>

* Clion 
[Descargar CLION](https://www.oracle.com/java/technologies/javase/jdk18-archive-downloads.html)<br/>
* SDL → especialmente el que sea de acorde a tu compilador de c++<br/>
[SDL MINGW Windows](https://github.com/libsdl-org/SDL/releases/download/release-2.26.4/SDL2-devel-2.26.4-mingw.zip)<br/>
* SDL Linux
 ```sh
    sudo apt-get install libsdl2-dev
  ```

### Configuración de SDL en CLION
La configuración de SDL en CLION se realizará mediante CMAKE, para ello debes de seguir los siguientes pasos provistos por [Pablo Méndez](https://github.com/El-Mendez/Paralela-Proyecto1/blob/main/CMakeLists.txt):<br/>

1. Guardar SDL dentro de un directorio llamado lib fuera del proyecto
 2. En el caso de Windows es necesario crear un perfil de Cmake para agregar ciertas variables de entorno. Si deseas utilizar el CMAKE provisto dentro del              repositorio deberás hacer las siguientes modificaciones sobre el CMAKE default o el que te reconozca CLION
  1. Ve a Settings >> Build, Execution, Deployment >> CMake y creas una copia del perfil default. Por ahora, vas a tener que deshabilitar el perfil                      default dándole clic al checkbox que sale.
  2. En la parte de environments le vas a agregar esto:
                     * SDL2_DIR=...\libs\SDL2-2.26.3 <br/>
                     alternativamente:<br/>
                     * CMAKE_PREFIX_PATH=...\libs <br/>
  3. En el caso de Windows: Por default, sdl también va a requerir un runtime library, para ello se copia el archivo de  SDL2-2.26.3\x86_64-w64          mingw32\bin\SDL.dll en cmake-debug-blah-blah.
<br/>
Una vez se tenga configurado SDL ya se puede compilar y posteriormente ejecutar el archivo, solo ten en cuenta que necesitas definir los siguientes parámetros de línea de comandos:<br/>
* Dirección de zoom
* Número de iteraciones
* Velocidad de zoom
<br/>
Esto lo puedes hacer en donde aparece el nombre asignado a tu ejecutable (en la parte superior a la derecha de CLION) le das click a la flecha hacia abajo >> Editar configuraciones >> Ingresas los argumentos en Argumentos de comandos, estos solo deben de estar separados por espacios.
<br/>
Una vez tengas la configuración anterior, puede dar click al botón de play y te mostrará el screen saver del conjunto de Mandelbrot<br/>
![mandelbruht](https://user-images.githubusercontent.com/54918136/227076621-09f7e87f-f7e0-46cf-950c-3814883fe863.png)


