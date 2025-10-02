# Onkos
Este es un motor gráfico que utiliza la API de DirectX 11 para el renderizado de modelos 3D, encapsulando la lógica por medio de clases abstractas y sus implementaciones, esto con el objetivo de tener un código limpio, modular, con responsabilidades únicas y escalable.

<img width="674" height="2033" alt="Pipeline Diagram" src="https://github.com/user-attachments/assets/670df238-d5e3-44fd-80ef-25b08943adf8" />

# Pipeline
El flujo se divide en 3 fases principales:
1. Inicialización
2. Bucle de Renderizado
3. Limpieza

# Inicialización
Prepara a la GPU y todos los componentes de la API para comenzar a dibujar.

## Crear el entorno
### Window
Encapsula la cración y gestión de la ventana donde se mostrará la información renderizada, la cual procede del sistema operativo.

### Device
Es la fábrica de recursos necesarios para el renderizado: texturas, buffers, shaders, etc.

### Device Context
Su responsabilidad es enviar comandos directos a la GPU como configuraciones del pipeline o comandos de dibujo.

### Swap Chain
Gestiona los buffers de imagen, así como de utilizar la función Present() para dibujarlos en la pantalla.
+ Front Buffer: Almacena la información ya procesada por los shaders, lista para solo leerse y dibujarse.
+ Back Buffer: Destino de todos los comandos de dibujo del Device Context, así como del Render Target View.

## Preparar los lienzos de renderizado
### Texture
Tiene la función de crear los recursos que se utilizarán para el back buffer y el depth buffer.

### Render Target View
Define al back buffer como destino para el almacenamiento de los datos de renderizado.

### Depth Stencil View
Configura el depth buffer para almacenar los valores de profundidad, esencial para la oclusión de los objetos en pantalla.

## Definir datoss y programas de la GPU
### Compilación de shaders
Se compilan los programas Vertex Shader y Pixel Shader desde sus respectivos archivos.

### Creación de buffers
Se crea el vertexBuffer y el indexBuffer, donde es almacenan los datos de los vértices y los índices que indican como conectarlos para formar primitivas.

### Carga de recursos visuales
Se cargan las texturas desde sus respectivos archivos y se crea un Sampler State para definir como tratar a las texturas, el como se filtrarán y se leerán en los shaders.

## Failure
En caso de no poder inicializar alguno de lo componentes, se liberan los recursos del sistema y se finaliza el programa.

# Bucle de Renderizado
## Actualización de la escena
La CPU actualiza los datos que cambian en cada fotograma, como las matrices de transformación de los objetos, copiando dichos datos a los buffers de constantes por medio del Device Context.

## Renderizado
Para crear un nuevo fotograma, se limpia el back buffer por medio del Render Target View (quien se establece como el destino del siguiente dibujo) utilizando un color de fondo.
Posteriormente se limpia el depth buffer para calibrar las pruebas de profundidad de los píxeles.

## Ejecución de comandos de dibujo
El Device Context configura el pipeline, asignando los vertex y pixel shaders a utilizar, enlazando los buffers de constantes, texturas y samplers de los shaders para procesar los datos.
Se llama a la función DrawIndexed() para que la GPU renderice la geometría definida.

## Presentación en pantalla
Por medio del Swap Chain, se intercambian el Front Buffer y el Back Buffer por medio del método Present() para hacer visible el nuevo fotograma recién renderizado.

# Limpieza
Cuando la aplicación se cierra (o falla en algún punto), se llama a la función CleanupDevice() para liberar todos los recursos (texturas, shaders, buffers, etc) en orden inverso para evitar fugas de memoria.
