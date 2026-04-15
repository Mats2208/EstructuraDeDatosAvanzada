# GAME_V1

Base visual para Progra-Villa usando `GrafosGameV2` como motor.

## Estructura

- `include/GrafosGameV2Base.h`: copia local del motor `V2`
- `src/main_cli.cpp`: prueba de consola sin dependencias graficas
- `src/main_sfml.cpp`: demo visual en SFML
- `data/progra_villa_v2.txt`: mapa base
- `assets/categories/`: carpeta reservada para sprites fijos por categoria

## Build

### Solo CLI

```bash
clang++ -std=c++17 -I GAME_V1/include GAME_V1/src/main_cli.cpp -o GAME_V1/game_v1_cli
./GAME_V1/game_v1_cli
```

### Con CMake

```bash
cd GAME_V1
cmake -S . -B build
cmake --build build
```

Si SFML no esta instalado, CMake construye solo `game_v1_cli`.
La configuracion actual soporta SFML 2.5 por `find_package` y SFML 3 por `pkg-config`.
En esta maquina el shell actual corre como `x86_64`, pero SFML esta en `arm64`, asi que conviene construir con `arch -arm64`.

### Build recomendado en esta maquina

```bash
cd GAME_V1
arch -arm64 cmake -S . -B build -DCMAKE_OSX_ARCHITECTURES=arm64
arch -arm64 cmake --build build -j 4
arch -arm64 ./build/game_v1_sfml
```

O directamente:

```bash
./build_and_run_sfml.sh
```

## Instalar SFML en macOS

Normalmente:

```bash
brew install sfml
```

Despues:

```bash
cd GAME_V1
cmake -S . -B build
cmake --build build
./build/game_v1_sfml
```

## Controles de la demo SFML

- Click izquierdo: seleccionar origen
- Click derecho: seleccionar destino
- Click medio: bloquear o desbloquear un nodo
- `B`: ruta BFS
- `D`: ruta DFS
- `J`: ruta Dijkstra
- `Space`: ciclar entre algoritmos
- `C`: limpiar ruta
- `R`: recargar mapa
- `Esc`: salir

## Consola embebida

La app ahora trae una consola dentro de la ventana. Comandos utiles:

```text
help
reset
clear
addnode CASA1 CASA 12 20 0
addedge CASA1 R 3 1
block CASA1
unblock CASA1
bfs R H
dfs N Z
dijkstra R H
```

## Categorias fijas

La UI ya reconoce y estiliza estas categorias:

- `HOSPITAL`
- `CASA`
- `REFUGIO`
- `SUPERMERCADO`
- `FARMACIA`
- `POLICIA`
- `NORTE`
- `SUR`
- `BARRICADA`
- `ZONAROJA`

Por ahora se renderizan con colores e iniciales fijas. El siguiente paso natural es poner PNGs con esos mismos nombres dentro de `assets/categories/`.
