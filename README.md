# Estructura de Datos Avanzada

Repositorio del curso de **Estructura de Datos Avanzada** - Universidad.

---

## Estructura General

```
ESTRUCTURA DE DATOS AVANZADA/
├── Grafos/              # Curso de Grafos
│   ├── Libreria/        # Header reutilizable con implementaciones de grafos
│   ├── Teoria/          # Archivos de teoría y práctica
│   ├── Practicas/       # Ejercicios de práctica
│   └── Parcial_1/       # [PROYECTO PRINCIPAL] Progra-Villa Zombie Edition
└── Tree/                # Curso de Árboles Binarios y AVL
    ├── BinaryTree*.cpp   # BST básico con traversals
    ├── Ejercicio2.cpp   # BST con input de usuario
    ├── Ejercicio3.cpp   # Operaciones BST estándar
    ├── Ejercicio4.cpp   # Versión avanzada
    └── Ejercicio5.cpp   # AVL Tree (autobalanceado)
```

---

## Grafos / Parcial 1: Progra-Villa Zombie Edition

**Proyecto principal del primer parcial.** Simulación de navegación urbana en un escenario zombi usando grafos.

### Concepto

La ciudad de **Progra-Villa** está bajo ataque zombi. El grafo representa la red vial donde:
- **Nodos**: Ubicaciones (refugios, hospitales, supermercados, farmacias, comisarías, zonas rojas)
- **Aristas**: Calles con nivel de peligro (peso)
- **Zonas rojas**: Áreas bloqueadas/no transitables

### Algoritmos Implementados

| Algoritmo | Función | Descripción |
|-----------|---------|-------------|
| **BFS** | `buscarSuministrosBFS` | Busca ruta con **menos calles** (menos saltos) |
| **DFS** | `esRutaSeguraDFS` | Determina si existe un camino **seguro** evitando zonas rojas |
| **Dijkstra** | `buscarMenorPeligroDijkstra` | Busca ruta de **menor peligro acumulado** |
| **Componentes Conexas** | `componentesConexas` | Verifica conectividad de refugios |

### Formato de Archivo de Mapa

```
TIPO 2                                    # 1=dirigido, 2=no dirigido
N R REFUGIO 0 5 8                         # Nodo: ID, Tipo, Bloqueado(0/1), X, Y
N S SUPERMERCADO 0 20 5
...
A R S 2 1                                  # Arista: Origen, Destino, Peligro, Bidireccional
```

**Tipos de nodo**: `REFUGIO`, `SUPERMERCADO`, `FARMACIA`, `HOSPITAL`, `POLICIA`, `NORTE`, `SUR`, `BARRICADA`, `ZONAROJA`

---

## Versiones

### V1 - CLI (Command Line Interface)

**Ubicación**: `Grafos/Parcial_1/PROGRA_VILLA_CLI/`

```
PROGRA_VILLA_CLI/
├── index.cpp           # Main con menú interactivo
├── GrafosGameV1.h      # Lógica del juego (BFS, DFS, Dijkstra)
├── grafos.h            # Libreria base de grafos
├── progra_villa_v1.txt  # Mapa de prueba
└── prog_index          # Ejecutable compilado
```

**Funcionalidades V1**:
- Menú interactivo en terminal
- Cargar mapa desde archivo
- BFS para buscar suministros (menos calles)
- DFS para ruta segura evitando zonas rojas
- Dijkstra para menor peligro
- Ver matriz de adyacencia
- Ver lista de adyacencia

**Compilar y ejecutar**:
```bash
cd Grafos/Parcial_1/PROGRA_VILLA_CLI
g++ -std=c++17 index.cpp -o prog_index
./prog_index
```

---

### V2 - GUI (Graphical User Interface con SFML)

**Ubicación**: `Grafos/Parcial_1/PROGRA_VILLA_UI/`

```
PROGRA_VILLA_UI/
├── GrafosGameV2.h      # Motor del juego (refactorizado, namespace ggv2)
└── game_sfml/          # Aplicación SFML
    ├── CMakeLists.txt
    ├── build.sh
    ├── include/
    │   └── GrafosGameV2Base.h
    ├── src/
    │   ├── main_cli.cpp    # CLI basada en V2
    │   └── main_sfml.cpp  # GUI completa SFML
    └── assets/
        └── sprites/        # Sprites PNG para nodos
```

**Funcionalidades V2**:
- **Mapa ASCII** con visualización de rutas (`showAsciiMap`)
- **GUI SFML** con ventana interactiva (1440x900)
- **Sprites por tipo de nodo** (hospital, police, supermarket, etc.)
- **Selección interactiva** de nodos por click
- **Controles de cámara** (pan con flechas, zoom con +/-)
- **Consola de comandos** integrada:
  - `addnode`, `addedge`, `block`, `unblock`
  - `bfs`, `dfs`, `dijkstra`, `clear`
- **Resaltado visual** de rutas con etiquetas de peligro
- **Atajos de teclado**: B=BFS, D=DFS, J=Dijkstra, R=reload, Space=cycling modes

**Mapas de prueba** (`data/`):
- `progra_villa_v2.txt` - Standard (10 nodos)
- `progra_villa_small.txt` - Prueba pequeño
- `progra_villa_large.txt` - Mapa extenso
- `progra_villa_blocked.txt` - Con rutas bloqueadas
- `progra_villa_negative.txt` - Con pesos negativos

**Compilar GUI SFML**:
```bash
cd Grafos/Parcial_1/PROGRA_VILLA_UI/game_sfml
./build.sh
./bin/progra_villa_sfml
```

---

## Libreria Base de Grafos (`Grafos/Libreria/grafos.h`)

Header header-only con implementaciones de grafos:

- **Estructuras**: `nodo` (lista de adyacencia), `arista`
- **Tipos de grafo**: Dirigido, no dirigido, mixto
- **Operaciones**: Insertar/eliminar nodos y aristas
- **Recorridos**: BFS, DFS
- **Algoritmos**: Dijkstra, Warshall, Floyd
- **E/S**: Cargar/guardar grafos desde archivos

---

## Tree - Árboles Binarios

Progresión de ejercicios desde BST básico hasta AVL balanceado:

| Archivo | Descripción |
|---------|-------------|
| `BinaryTree.cpp` | BST con insercción, eliminación, traversals (pre/in/post orden) |
| `Ejercicio2.cpp` | BST con input de usuario |
| `Ejercicio3.cpp` | Operaciones BST estándar |
| `Ejercicio5.cpp` | **AVL Tree** con rotaciones (right/left) y factor de balance |

El AVL implementa:
- Inserciones con rebalanceo automático
- Rotaciones simples y compuestas
- Verificación de factor de balance
- Eliminación con rebalanceo
- Visualización gráfica del árbol

---

## Compilación Rápida

```bash
# CLI V1
cd Grafos/Parcial_1/PROGRA_VILLA_CLI && g++ -std=c++17 index.cpp -o prog_index

# CLI V2
cd Grafos/Parcial_1/PROGRA_VILLA_UI/game_sfml/src && g++ -std=c++17 main_cli.cpp -o main_cli

# Tree AVL
cd Tree && g++ -std=c++17 Ejercicio5.cpp -o Ejercicio5
```

---

## Tecnologías

- **Lenguaje**: C++ (C++17)
- **GUI**: SFML 2.x
- **Build**: CMake + Makefile
- **Estructuras**: Lista de adyacencia, matriz de adyacencia