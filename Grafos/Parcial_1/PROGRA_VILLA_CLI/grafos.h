#ifndef GRAFOS_H
#define GRAFOS_H

#include <iostream>
#include <fstream>
#include <string>
#include <sstream>
#include <cstdlib>

using namespace std;

struct nodo {
    char nombre;
    char tipo[20];       // REFUGIO, HOSPITAL, SUPERMERCADO, etc.
    int bloqueado;       // 0 = libre, 1 = zona roja
    struct nodo* sgte;
    struct arista* ady;
};

struct arista {
    struct nodo* destino;
    struct arista* sgte;
    int peso;            // nivel de peligro
};

typedef struct nodo* Tnodo;
typedef struct arista* Tarista;

Tnodo p = NULL;
int tipoGrafo = 1;

// ========== FUNCIONES BASICAS ==========

Tnodo buscar_nodo(char nombre)
{
    Tnodo aux = p;
    while (aux != NULL)
    {
        if (aux->nombre == nombre)
            return aux;
        aux = aux->sgte;
    }
    return NULL;
}

bool existe_arista(Tnodo origen, Tnodo destino)
{
    Tarista q;
    if (origen == NULL || destino == NULL)
        return false;
    q = origen->ady;
    while (q != NULL)
    {
        if (q->destino == destino)
            return true;
        q = q->sgte;
    }
    return false;
}

void agrega_arista(Tnodo& aux, Tnodo& aux2, Tarista& nuevo)
{
    Tarista q;
    nuevo->destino = aux2;
    if (aux->ady == NULL)
    {
        aux->ady = nuevo;
    }
    else
    {
        q = aux->ady;
        while (q->sgte != NULL)
            q = q->sgte;
        q->sgte = nuevo;
    }
}

int contar_nodos()
{
    int c = 0;
    Tnodo aux = p;
    while (aux != NULL)
    {
        c++;
        aux = aux->sgte;
    }
    return c;
}

int posicion_nodo(Tnodo v[], int n, Tnodo buscado)
{
    for (int i = 0; i < n; i++)
    {
        if (v[i] == buscado)
            return i;
    }
    return -1;
}

void vaciar_grafo()
{
    Tnodo aux = p;
    while (aux != NULL)
    {
        Tarista q = aux->ady;
        while (q != NULL)
        {
            Tarista r = q;
            q = q->sgte;
            delete(r);
        }
        Tnodo r = aux;
        aux = aux->sgte;
        delete(r);
    }
    p = NULL;
}

// ========== CARGAR DESDE ARCHIVO ==========

void CargarGrafo(const char* nombre_archivo)
{
    vaciar_grafo();

    ifstream archivo(nombre_archivo);
    if (!archivo.good()) {
        cout << "Error: No se pudo abrir el archivo " << nombre_archivo << endl;
        return;
    }

    string linea;
    while (getline(archivo, linea))
    {
        if (linea.empty()) continue;

        istringstream iss(linea);
        char cmd;
        iss >> cmd;

        if (cmd == 'T')
        {
            iss >> tipoGrafo;
        }
        else if (cmd == 'N')
        {
            char nombre;
            char tipo[20];
            int bloqueado;
            iss >> nombre >> tipo >> bloqueado;

            Tnodo nuevo = new struct nodo;
            nuevo->nombre = nombre;
            strcpy(nuevo->tipo, tipo);
            nuevo->bloqueado = bloqueado;
            nuevo->sgte = NULL;
            nuevo->ady = NULL;

            if (p == NULL)
                p = nuevo;
            else
            {
                Tnodo t = p;
                while (t->sgte != NULL) t = t->sgte;
                t->sgte = nuevo;
            }
        }
        else if (cmd == 'A')
        {
            char origen, destino;
            int peso, bidireccional;
            iss >> origen >> destino >> peso >> bidireccional;

            Tnodo nodo_origen = buscar_nodo(origen);
            Tnodo nodo_destino = buscar_nodo(destino);

            if (nodo_origen != NULL && nodo_destino != NULL && !existe_arista(nodo_origen, nodo_destino))
            {
                Tarista nueva = new struct arista;
                nueva->sgte = NULL;
                nueva->peso = peso;
                agrega_arista(nodo_origen, nodo_destino, nueva);

                if (bidireccional == 1 && !existe_arista(nodo_destino, nodo_origen))
                {
                    Tarista nueva_inv = new struct arista;
                    nueva_inv->sgte = NULL;
                    nueva_inv->peso = peso;
                    agrega_arista(nodo_destino, nodo_origen, nueva_inv);
                }
            }
        }
    }
    archivo.close();
}

// ========== MOSTRAR GRAFO ==========

void mostrar_grafo()
{
    Tnodo ptr = p;
    cout << "NODO|TIPO|BLOQUEADO|LISTA DE ADYACENCIA\n";
    while (ptr != NULL)
    {
        cout << " " << ptr->nombre << " | " << ptr->tipo << " | " << (ptr->bloqueado ? "ZONA ROJA" : "LIBRE") << " |";
        Tarista ar = ptr->ady;
        while (ar != NULL)
        {
            cout << " " << ar->destino->nombre << "(" << ar->peso << ")";
            ar = ar->sgte;
        }
        ptr = ptr->sgte;
        cout << endl;
    }
}

// ========== BFS - BUSCAR SUMINISTROS ==========

void buscarSuministrosBFS(char inicioChar, char objetivoChar)
{
    if (p == NULL) { cout << "GRAFO VACIO\n"; return; }

    Tnodo nodos[100];
    int n = 0;
    Tnodo aux = p;
    while (aux != NULL) { nodos[n++] = aux; aux = aux->sgte; }

    Tnodo inicio = buscar_nodo(inicioChar);
    Tnodo objetivo = buscar_nodo(objetivoChar);

    if (inicio == NULL || objetivo == NULL)
    {
        cout << "NODO NO ENCONTRADO\n";
        return;
    }

    Tnodo cola[100];
    int visitado[100] = {0};
    int padre[100];
    for (int i = 0; i < n; i++) padre[i] = -1;

    int frente = 0, fin = 0;
    cola[fin] = inicio;
    fin++;
    int posInicio = posicion_nodo(nodos, n, inicio);
    visitado[posInicio] = 1;
    bool encontrado = false;

    while (frente < fin && !encontrado)
    {
        Tnodo actual = cola[frente];
        frente++;

        if (actual == objetivo)
        {
            encontrado = true;
            break;
        }

        Tarista ar = actual->ady;
        while (ar != NULL)
        {
            int posDest = posicion_nodo(nodos, n, ar->destino);
            if (posDest != -1 && !visitado[posDest])
            {
                visitado[posDest] = 1;
                padre[posDest] = posicion_nodo(nodos, n, actual);
                cola[fin] = ar->destino;
                fin++;
            }
            ar = ar->sgte;
        }
    }

    cout << "\n=== BUSQUEDA DE SUMINISTROS (BFS) ===\n";
    cout << "Desde: " << inicioChar << " (" << inicio->tipo << ") -> Hasta: " << objetivoChar << " (" << objetivo->tipo << ")\n";

    if (!encontrado)
    {
        cout << "NO HAY CAMINO AL HOSPITAL\n";
        return;
    }

    int posObj = posicion_nodo(nodos, n, objetivo);
    int camino[100], len = 0;
    int actual = posObj;
    while (actual != -1)
    {
        camino[len++] = actual;
        actual = padre[actual];
    }

    cout << "CAMINO MAS CORTO (menor calles):\n";
    cout << "  ";
    for (int i = len - 1; i >= 0; i--)
    {
        cout << nodos[camino[i]]->nombre;
        if (i > 0) cout << " -> ";
    }
    cout << "\n  Calles atravesadas: " << len - 1 << endl;
}

// ========== DFS - RUTA SEGURA ==========

bool dfs_ruta(Tnodo actual, Tnodo objetivo, Tnodo nodos[], int n, int visitado[], int camino[], int& len, int posActual)
{
    int pos = posicion_nodo(nodos, n, actual);
    if (pos == -1 || visitado[pos] || actual->bloqueado)
        return false;

    visitado[pos] = 1;
    camino[posActual] = pos;

    if (actual == objetivo)
    {
        len = posActual + 1;
        return true;
    }

    Tarista ar = actual->ady;
    while (ar != NULL)
    {
        if (dfs_ruta(ar->destino, objetivo, nodos, n, visitado, camino, len, posActual + 1))
            return true;
        ar = ar->sgte;
    }

    return false;
}

void esRutaSeguraDFS(char inicioChar, char objetivoChar)
{
    if (p == NULL) { cout << "GRAFO VACIO\n"; return; }

    Tnodo nodos[100];
    int n = 0;
    Tnodo aux = p;
    while (aux != NULL) { nodos[n++] = aux; aux = aux->sgte; }

    Tnodo inicio = buscar_nodo(inicioChar);
    Tnodo objetivo = buscar_nodo(objetivoChar);

    if (inicio == NULL || objetivo == NULL)
    {
        cout << "NODO NO ENCONTRADO\n";
        return;
    }

    int visitado[100] = {0};
    int camino[100];
    int len = 0;

    cout << "\n=== RUTA DE RESCATE (DFS) ===\n";
    cout << "Desde: " << inicioChar << " -> Hasta: " << objetivoChar << " (evitando Zonas Rojas)\n";

    bool existe = dfs_ruta(inicio, objetivo, nodos, n, visitado, camino, len, 0);

    if (!existe)
    {
        cout << "NO EXISTE RUTA SEGURA (todo esta bloqueado)\n";
        return;
    }

    cout << "RUTA ENCONTRADA:\n  ";
    for (int i = 0; i < len; i++)
    {
        cout << nodos[camino[i]]->nombre;
        if (i < len - 1) cout << " -> ";
    }
    cout << "\n";
}

// ========== COMPONENTES CONEXAS ==========

void dfs_componente(Tnodo actual, Tnodo nodos[], int n, int visitado[], int comp[])
{
    int pos = posicion_nodo(nodos, n, actual);
    if (pos == -1 || visitado[pos]) return;

    visitado[pos] = 1;
    comp[pos] = 1;

    Tarista ar = actual->ady;
    while (ar != NULL)
    {
        dfs_componente(ar->destino, nodos, n, visitado, comp);
        ar = ar->sgte;
    }
}

void mapaConectividad()
{
    if (p == NULL) { cout << "GRAFO VACIO\n"; return; }

    Tnodo nodos[100];
    int n = 0;
    Tnodo aux = p;
    while (aux != NULL) { nodos[n++] = aux; aux = aux->sgte; }

    int visitado[100] = {0};
    int numComp = 0;

    cout << "\n=== MAPA DE CONECTIVIDAD ===\n";

    for (int i = 0; i < n; i++)
    {
        if (!visitado[i])
        {
            numComp++;
            int comp[100] = {0};
            dfs_componente(nodos[i], nodos, n, visitado, comp);

            cout << "COMPONENTE " << numComp << ": ";
            for (int j = 0; j < n; j++)
            {
                if (comp[j])
                    cout << nodos[j]->nombre << " ";
            }
            cout << endl;
        }
    }

    if (numComp == 1)
        cout << "La ciudad esta COMPLETAMENTE CONECTADA\n";
    else
        cout << "ALERTA: La ciudad esta DIVIDIDA en " << numComp << " componentes\n";
}

// ========== DIJKSTRA - RUTA MENOR PELIGRO ==========

void dijkstraRuta(char inicioChar, char objetivoChar)
{
    if (p == NULL) { cout << "GRAFO VACIO\n"; return; }

    const int INF = 999999;
    Tnodo nodos[100];
    int n = 0;
    Tnodo aux = p;
    while (aux != NULL) { nodos[n++] = aux; aux = aux->sgte; }

    Tnodo inicio = buscar_nodo(inicioChar);
    Tnodo objetivo = buscar_nodo(objetivoChar);

    if (inicio == NULL || objetivo == NULL)
    {
        cout << "NODO NO ENCONTRADO\n";
        return;
    }

    int dist[100], visitado[100], prev[100];
    for (int i = 0; i < n; i++) { dist[i] = INF; visitado[i] = 0; prev[i] = -1; }

    int posInicio = posicion_nodo(nodos, n, inicio);
    dist[posInicio] = 0;

    for (int iter = 0; iter < n; iter++)
    {
        int u = -1;
        for (int i = 0; i < n; i++)
            if (!visitado[i] && (u == -1 || dist[i] < dist[u]))
                u = i;

        if (u == -1 || dist[u] == INF) break;
        visitado[u] = 1;

        Tarista ar = nodos[u]->ady;
        while (ar != NULL)
        {
            int v = posicion_nodo(nodos, n, ar->destino);
            if (v != -1 && dist[u] + ar->peso < dist[v])
            {
                dist[v] = dist[u] + ar->peso;
                prev[v] = u;
            }
            ar = ar->sgte;
        }
    }

    cout << "\n=== DIJKSTRA - RUTA MENOR PELIGRO ===\n";
    cout << "Desde: " << inicioChar << " -> Hasta: " << objetivoChar << "\n";

    int posObj = posicion_nodo(nodos, n, objetivo);
    if (dist[posObj] == INF)
    {
        cout << "NO HAY CAMINO POSIBLE\n";
        return;
    }

    int camino[100], len = 0, cur = posObj;
    while (cur != -1) { camino[len++] = cur; cur = prev[cur]; }

    cout << "RUTA MAS SEGURA (menor peligro):\n  ";
    for (int i = len - 1; i >= 0; i--)
    {
        cout << nodos[camino[i]]->nombre;
        if (i > 0) cout << " -> ";
    }
    cout << "\n  Total zombies/peso: " << dist[posObj] << endl;
}

#endif
