# Onkos
Este es un motor gráfico que utiliza la API de DirectX 11 para el renderizado de modelos 3D, encapsulando la lógica por medio de clases abstractas y sus implementaciones, esto con el objetivo de tener un código limpio, modular, con responsabilidades únicas y escalable.

<img width="500" height="5000" alt="image" src="https://github.com/user-attachments/assets/3b8bc90e-9962-445c-971f-faf1047a1e1b" />


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

-------------------------------------------------------------------------------------------------
# Uso del ModelLoader.h (Parser obj -> Onkos Engine)
## ModelLoader (ModelLoader.h)

`ModelLoader` es una clase de utilidad esencial diseñada para parsear (analizar) archivos de geometría en formato `.obj`. Su única responsabilidad es leer el archivo `.obj`, procesar los datos de vértices, y llenar un `MeshComponent` con la geometría lista para usarse en la CPU.

Esta clase está desacoplada de DirectX y no crea ningún recurso de GPU. Su trabajo es preparar el `MeshComponent`, que luego es utilizado por la clase `Buffer` para crear los `VertexBuffer` e `IndexBuffer` correspondientes.

## Archivos .obj
Un archivo .obj separa sus datos según sus prefijos al inicio de cada línea del archivo. Estos prefijos se distinguen entre:
* **v:** Para las posiciones X, Y, Z del vértice en el espacio
* **vt:** Da las coordenadas de textura (UV's)
* **vn:** Igualmente almacena las posiciones X, Y, Z pero de las normales del modelo
* **f:** Instruye respecto a como ensamblar los vértices, indicando el índice de la posición, la textura y la normal (en ese órden)
  * Las líneas **"f"** se subdividen en bloques como en el siguiente ejemplo: 1/7/5
  * Pueden tener 3 bloques para formar triángulos o 4 para formar Quads (el ModelLoader.h puede manejar ambos casos con un "Fan Triangulation"

## Cómo Utilizar

El flujo de trabajo estándar es crear una instancia del *loader*, crear un `MeshComponent` vacío, y pasarle el componente por referencia para que el *loader* lo llene.

**Ejemplo de código en `BaseApp::init()`:**

```cpp
#include "ModelLoader.h"
#include "MeshComponent.h"
#include "Buffer.h"

// ...

// 1. Crear una instancia del loader (no necesita nada en su constructor)
ModelLoader myLoader;

// 2. Crear el componente que contendrá los datos de CPU
// (m_mesh ya existe como variable miembro en BaseApp)
// MeshComponent m_mesh; 

// 3. Llamar a la función de carga
bool loadSuccess = myLoader.loadModel("ruta/a/mi_modelo.obj", m_mesh);

if (!loadSuccess)
{
    // Manejar el error si no se pudo cargar el archivo
    ERROR("BaseApp.cpp", "init", "No se pudo cargar el modelo.");
    return E_FAIL;
}

// 4. Si la carga fue exitosa, la variable m_mesh ahora está llena.
//    Ahora podemos usarla para inicializar los buffers de la GPU.
hr = m_vertexBuffer.init(m_device, m_mesh, D3D11_BIND_VERTEX_BUFFER);
hr = m_indexBuffer.init(m_device, m_mesh, D3D11_BIND_INDEX_BUFFER);

// ...
```

### Funciones Públicas

```cpp
/**
 * @brief Carga un modelo desde un archivo .obj y llena un MeshComponent.
 * @param fileName El path (ruta) al archivo .obj.
 * @param outMesh El componente de malla (pasado por referencia) que se llenará.
 * @return true si la carga fue exitosa, false si falló.
 */
bool loadModel(const std::string& fileName, MeshComponent& outMesh);
```

### Características y Limitaciones

* **Indexación de Vértices:** Utiliza un `std::map` para indexar vértices únicos, asegurando que no se dupliquen datos de vértices en el `VertexBuffer` (ahorrando VRAM).
* **Triangulación:** Soporta triangulación automática para caras de 4 vértices (quads) usando el método "fan triangulation" (`0,1,2` y `0,2,3`).
* **⚠️ Limitación Actual:** Esta implementación asume que el archivo `.obj` **debe** incluir datos de posición, textura y normales (`v`, `vt`, `vn`) para cada vértice. El *parser* fallará si el archivo solo contiene posiciones y UVs (ej: `f 1/1 2/2 3/3`).
----------------------------------------------------------------------------------------------------------------------------------------------------------
