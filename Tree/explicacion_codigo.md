# Explicación del código — BinaryTree.cpp

## ¿Qué hace el programa?

Implementa un **árbol binario de búsqueda (BST)** en C++. El usuario puede crear el árbol ingresando valores desde el teclado, insertar más elementos, y recorrerlo en distintos órdenes.

---

## Estructura general del archivo

```
BinaryTree.cpp
│
├── typedef  →  define el tipo de dato del árbol
├── class Nodo          →  representa cada nodo del árbol
├── class ArbolBinario  →  contiene la lógica del árbol
├── void menu()         →  imprime el menú en pantalla
└── int main()          →  controla el flujo del programa
```

---

## `typedef std::string Tipoelemento`

```cpp
typedef std::string Tipoelemento;
//typedef int Tipoelemento;
```

Define un alias llamado `Tipoelemento` para no escribir `std::string` en todo el código.  
Si se descomenta la línea de `int`, el árbol trabajaría con números enteros en lugar de texto.  
Cambiar el tipo de dato aquí afecta **todo el programa automáticamente**.

---

## Clase `Nodo`

Representa un solo nodo del árbol. Cada nodo guarda:

| Atributo | Tipo | Descripción |
|---|---|---|
| `dato` | `Tipoelemento` | El valor que almacena el nodo |
| `izdo` | `Nodo*` | Puntero al hijo izquierdo |
| `dcho` | `Nodo*` | Puntero al hijo derecho |

### Constructores

```cpp
Nodo(Tipoelemento valor)
```
Crea un nodo hoja (sin hijos). `izdo` y `dcho` quedan en `NULL`.

```cpp
Nodo(Nodo* ramaIzdo, Tipoelemento valor, Nodo* ramaDcho)
```
Crea un nodo con hijos ya definidos. Se usa para construir subárboles completos de una sola vez.

### Métodos de acceso (getters)

| Método | Retorna |
|---|---|
| `valorNodo()` | El dato guardado en el nodo |
| `subarbolIzdo()` | Puntero al hijo izquierdo |
| `subarbolDcho()` | Puntero al hijo derecho |

### Métodos de modificación (setters)

| Método | Qué hace |
|---|---|
| `nuevoValor(d)` | Cambia el dato del nodo |
| `ramaIzdo(n)` | Asigna el hijo izquierdo |
| `ramaDcho(n)` | Asigna el hijo derecho |

### `visitar()`

```cpp
void visitar() { std::cout << dato << " \n"; }
```
Imprime el valor del nodo en pantalla. Los recorridos la llaman para mostrar cada nodo.

---

## Clase `ArbolBinario`

Contiene el árbol completo. Su único atributo es `raiz`, un puntero al nodo raíz.

### Constructores

```cpp
ArbolBinario()        // raiz = NULL, árbol vacío
ArbolBinario(Nodo* r) // raiz apunta al nodo r
```

### Métodos públicos principales

#### `Praiz(Nodo* r)` y `Oraiz()`
Setter y getter de la raíz. Permiten asignar o leer el nodo raíz desde fuera de la clase.

#### `esVacio()`
```cpp
bool esVacio() { return raiz == NULL; }
```
Devuelve `true` si el árbol no tiene ningún nodo.

#### `raizArbol()`
Devuelve una **copia** del nodo raíz. Si el árbol está vacío, lanza una excepción. Se usa en la opción 5 del menú para mostrar la raíz.

#### `hijoIzdo()` y `hijoDcho()`
Devuelven punteros al hijo izquierdo y derecho de la raíz. Lanzan excepción si el árbol está vacío.

#### `nuevoArbol(izqda, dato, drcha)`
Crea un nuevo nodo con dos ramas ya conectadas. Es una función auxiliar de construcción.

#### `insertar(valor)` ← **función nueva**
```cpp
void insertar(Tipoelemento valor)
```
Punto de entrada público. Llama a la versión privada con la raíz.

Internamente recorre el árbol de forma **iterativa**:
- Si el árbol está vacío, crea la raíz.
- Si el valor es **menor** que el nodo actual → va a la **izquierda**.
- Si el valor es **mayor o igual** → va a la **derecha**.
- Continúa bajando hasta encontrar un espacio vacío (`NULL`) y ahí crea el nodo.

Esto forma un **árbol binario de búsqueda (BST)**: los elementos quedan ordenados de forma que el recorrido inorden los muestra de menor a mayor.

```
Ejemplo — insertar: 30, 10, 50, 20, 40

        30
       /  \
      10   50
       \   /
       20 40
```

#### `preorden()`, `inorden()`, `postorden()`
Métodos públicos que arrancan el recorrido desde la raíz llamando a sus versiones privadas.

---

### Métodos privados de recorrido

Los tres recorridos son **recursivos**: se llaman a sí mismos con el hijo izquierdo y luego con el derecho, cambiando solo el momento en que visitan el nodo actual.

#### `preorden(Nodo* r)` — Raíz → Izquierda → Derecha
```
visitar(r)
preorden(izquierda)
preorden(derecha)
```
Muestra la raíz **primero**. Útil para copiar o serializar el árbol.

#### `inorden(Nodo* r)` — Izquierda → Raíz → Derecha
```
inorden(izquierda)
visitar(r)
inorden(derecha)
```
Muestra los nodos **en orden ascendente** (de menor a mayor en un BST).

#### `postorden(Nodo* r)` — Izquierda → Derecha → Raíz
```
postorden(izquierda)
postorden(derecha)
visitar(r)
```
Muestra la raíz **al final**. Útil para eliminar el árbol liberando hijos antes que el padre.

#### `insertar(Nodo*& r, valor)` — privado
Recibe la raíz por **referencia** (`Nodo*&`) para poder modificarla si el árbol está vacío.  
Usa un bucle `while(true)` que baja por el árbol hasta encontrar un hueco libre.

---

## `void menu()`

Función simple que imprime las opciones disponibles en pantalla. No tiene lógica, solo `cout`.

---

## `int main()`

Controla el flujo completo del programa.

```cpp
ArbolBinario a;   // se crea un árbol vacío
```

Entra en un bucle `do-while` que se repite hasta que el usuario elige `0`.  
En cada vuelta: muestra el menú → lee la opción → ejecuta el `switch`.

### Opciones del menú

| Opción | Qué hace |
|---|---|
| `1` | Pregunta cuántos elementos ingresar, los lee uno a uno y los inserta con `a.insertar()` |
| `2` | Pide un solo elemento y lo inserta en el árbol existente |
| `3` | Pregunta el tipo de recorrido (1=pre, 2=in, 3=post) y llama al método correspondiente |
| `4` | Llama a `esVacio()` e imprime el resultado |
| `5` | Llama a `raizArbol().visitar()` para mostrar el valor de la raíz |
| `0` | Termina el bucle y cierra el programa |

---

## Flujo típico de uso

```
1) Elegir opción 1 → ingresar 5 elementos: 30 10 50 20 40
2) Elegir opción 3 → inorden → imprime: 10 20 30 40 50
3) Elegir opción 2 → insertar 25
4) Elegir opción 3 → inorden → imprime: 10 20 25 30 40 50
5) Elegir opción 5 → muestra la raíz: 30
6) Elegir opción 0 → salir
```

---

## Diagrama de clases

```
┌──────────────┐          ┌────────────────────┐
│    Nodo      │          │   ArbolBinario     │
├──────────────┤          ├────────────────────┤
│ dato         │◄─────────│ raiz: Nodo*        │
│ izdo: Nodo*  │          ├────────────────────┤
│ dcho: Nodo*  │          │ + insertar()       │
├──────────────┤          │ + preorden()       │
│ + valorNodo()│          │ + inorden()        │
│ + ramaIzdo() │          │ + postorden()      │
│ + ramaDcho() │          │ + esVacio()        │
│ + visitar()  │          │ + raizArbol()      │
└──────────────┘          └────────────────────┘
```
