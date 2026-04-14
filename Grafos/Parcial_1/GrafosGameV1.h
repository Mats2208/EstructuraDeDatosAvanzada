#ifndef GRAFOS_GAME_V1_H
#define GRAFOS_GAME_V1_H

#include <fstream>
#include <iostream>
#include <sstream>
#include <string>

using namespace std;

struct nodo_game {
    char nombre;
    string tipoLugar;
    bool isblocked;
    struct nodo_game* sgte;
    struct arista_game* ady;
};

struct arista_game {
    struct nodo_game* destino;
    struct arista_game* sgte;
    int peligro;
};

typedef struct nodo_game* TnodoGame;
typedef struct arista_game* TaristaGame;

TnodoGame pGame = NULL;
int tipoGrafoGame = 2;

void limpiar_grafo_game()
{
    TnodoGame actual = pGame;
    while (actual != NULL)
    {
        TaristaGame ar = actual->ady;
        while (ar != NULL)
        {
            TaristaGame borrar = ar;
            ar = ar->sgte;
            delete borrar;
        }

        TnodoGame borrarNodo = actual;
        actual = actual->sgte;
        delete borrarNodo;
    }
    pGame = NULL;
}

TnodoGame buscar_nodo_game(char nombre)
{
    TnodoGame aux = pGame;
    while (aux != NULL)
    {
        if (aux->nombre == nombre)
            return aux;
        aux = aux->sgte;
    }
    return NULL;
}

int contar_nodos_game()
{
    int c = 0;
    TnodoGame aux = pGame;
    while (aux != NULL)
    {
        c++;
        aux = aux->sgte;
    }
    return c;
}

int posicion_nodo_game(TnodoGame v[], int n, TnodoGame buscado)
{
    for (int i = 0; i < n; i++)
    {
        if (v[i] == buscado)
            return i;
    }
    return -1;
}

bool existe_arista_game(TnodoGame origen, TnodoGame destino)
{
    if (origen == NULL || destino == NULL)
        return false;

    TaristaGame q = origen->ady;
    while (q != NULL)
    {
        if (q->destino == destino)
            return true;
        q = q->sgte;
    }
    return false;
}

void agregar_arista_lista_game(TnodoGame origen, TnodoGame destino, int peligro)
{
    TaristaGame nueva = new struct arista_game;
    nueva->destino = destino;
    nueva->peligro = peligro;
    nueva->sgte = NULL;

    if (origen->ady == NULL)
    {
        origen->ady = nueva;
        return;
    }

    TaristaGame q = origen->ady;
    while (q->sgte != NULL)
        q = q->sgte;
    q->sgte = nueva;
}

bool insertar_nodo_game(char nombre, const string& tipoLugar, bool bloqueado)
{
    if (buscar_nodo_game(nombre) != NULL)
        return false;

    TnodoGame nuevo = new struct nodo_game;
    nuevo->nombre = nombre;
    nuevo->tipoLugar = tipoLugar;
    nuevo->isblocked = bloqueado;
    nuevo->sgte = NULL;
    nuevo->ady = NULL;

    if (pGame == NULL)
    {
        pGame = nuevo;
        return true;
    }

    TnodoGame t = pGame;
    while (t->sgte != NULL)
        t = t->sgte;
    t->sgte = nuevo;
    return true;
}

bool insertar_arista_game(char origenNombre, char destinoNombre, int peligro, bool bidireccional)
{
    TnodoGame origen = buscar_nodo_game(origenNombre);
    TnodoGame destino = buscar_nodo_game(destinoNombre);

    if (origen == NULL || destino == NULL)
        return false;

    if (!existe_arista_game(origen, destino))
        agregar_arista_lista_game(origen, destino, peligro);

    if (bidireccional && !existe_arista_game(destino, origen))
        agregar_arista_lista_game(destino, origen, peligro);

    return true;
}

bool bloquear_nodo_game(char nombre)
{
    TnodoGame nodo = buscar_nodo_game(nombre);
    if (nodo == NULL)
        return false;
    nodo->isblocked = true;
    return true;
}

bool desbloquear_nodo_game(char nombre)
{
    TnodoGame nodo = buscar_nodo_game(nombre);
    if (nodo == NULL)
        return false;
    nodo->isblocked = false;
    return true;
}

void mostrar_mapa_game()
{
    if (pGame == NULL)
    {
        cout << "MAPA VACIO\n";
        return;
    }

    cout << "\nMAPA DE PROGRA-VILLA\n";
    cout << "Nodo | Tipo | Estado | Calles\n";

    TnodoGame ptr = pGame;
    while (ptr != NULL)
    {
        cout << " " << ptr->nombre << " | " << ptr->tipoLugar << " | "
             << (ptr->isblocked ? "ZONA ROJA" : "SEGURO") << " | ";

        TaristaGame ar = ptr->ady;
        if (ar == NULL)
        {
            cout << "sin conexiones";
        }
        while (ar != NULL)
        {
            cout << ar->destino->nombre << "(" << ar->peligro << ")";
            if (ar->sgte != NULL)
                cout << " ";
            ar = ar->sgte;
        }
        cout << "\n";
        ptr = ptr->sgte;
    }
}

bool cargar_ciudad_desde_archivo_game(const string& nombreArchivo)
{
    ifstream archivo(nombreArchivo.c_str());
    if (!archivo.good())
        return false;

    limpiar_grafo_game();

    string linea;
    while (getline(archivo, linea))
    {
        if (linea.empty() || linea[0] == '#')
            continue;

        istringstream iss(linea);
        string comando;
        iss >> comando;

        if (comando == "TIPO")
        {
            iss >> tipoGrafoGame;
        }
        else if (comando == "N")
        {
            char nombre;
            string tipoLugar;
            int bloqueado;
            iss >> nombre >> tipoLugar >> bloqueado;
            insertar_nodo_game(nombre, tipoLugar, bloqueado == 1);
        }
        else if (comando == "A")
        {
            char origen;
            char destino;
            int peligro;
            int bidireccional;
            iss >> origen >> destino >> peligro >> bidireccional;
            insertar_arista_game(origen, destino, peligro, bidireccional == 1);
        }
    }

    return true;
}

bool reconstruir_camino_game(int prev[], TnodoGame nodos[], int posDestino)
{
    if (posDestino < 0)
        return false;

    int camino[100];
    int largo = 0;
    int cur = posDestino;

    while (cur != -1)
    {
        camino[largo++] = cur;
        cur = prev[cur];
    }

    for (int i = largo - 1; i >= 0; i--)
    {
        cout << nodos[camino[i]]->nombre;
        if (i > 0)
            cout << " -> ";
    }

    return true;
}

bool buscarSuministrosBFS(char inicioNombre, char objetivoNombre)
{
    if (pGame == NULL)
    {
        cout << "MAPA VACIO\n";
        return false;
    }

    TnodoGame inicio = buscar_nodo_game(inicioNombre);
    TnodoGame objetivo = buscar_nodo_game(objetivoNombre);

    if (inicio == NULL || objetivo == NULL)
    {
        cout << "NODO INVALIDO\n";
        return false;
    }

    if (inicio->isblocked || objetivo->isblocked)
    {
        cout << "NO SE PUEDE INICIAR O TERMINAR EN UNA ZONA ROJA\n";
        return false;
    }

    int n = contar_nodos_game();
    TnodoGame nodos[100];
    TnodoGame cola[100];
    int visitado[100];
    int prev[100];
    int distancia[100];

    TnodoGame aux = pGame;
    for (int i = 0; i < n; i++)
    {
        nodos[i] = aux;
        visitado[i] = 0;
        prev[i] = -1;
        distancia[i] = -1;
        aux = aux->sgte;
    }

    int frente = 0;
    int fin = 0;
    cola[fin++] = inicio;

    int posInicio = posicion_nodo_game(nodos, n, inicio);
    int posObjetivo = posicion_nodo_game(nodos, n, objetivo);
    visitado[posInicio] = 1;
    distancia[posInicio] = 0;

    while (frente < fin)
    {
        TnodoGame actual = cola[frente++];
        if (actual == objetivo)
            break;

        TaristaGame ar = actual->ady;
        while (ar != NULL)
        {
            int posDestino = posicion_nodo_game(nodos, n, ar->destino);
            if (posDestino != -1 && !visitado[posDestino] && !ar->destino->isblocked)
            {
                visitado[posDestino] = 1;
                prev[posDestino] = posicion_nodo_game(nodos, n, actual);
                distancia[posDestino] = distancia[posInicio] + 1;
                if (prev[posDestino] != -1)
                    distancia[posDestino] = distancia[prev[posDestino]] + 1;
                cola[fin++] = ar->destino;
            }
            ar = ar->sgte;
        }
    }

    if (!visitado[posObjetivo])
    {
        cout << "NO EXISTE RUTA BFS HACIA EL OBJETIVO\n";
        return false;
    }

    cout << "\nEXPLORACION DE SUMINISTROS BFS\n";
    cout << "Ruta con menor cantidad de calles: ";
    reconstruir_camino_game(prev, nodos, posObjetivo);
    cout << "\nCalles atravesadas: " << distancia[posObjetivo] << "\n";
    return true;
}

bool dfs_seguro_game(TnodoGame actual, TnodoGame destino, TnodoGame nodos[], int n, int visitado[])
{
    int pos = posicion_nodo_game(nodos, n, actual);
    if (pos == -1 || visitado[pos] || actual->isblocked)
        return false;

    visitado[pos] = 1;
    if (actual == destino)
        return true;

    TaristaGame ar = actual->ady;
    while (ar != NULL)
    {
        if (dfs_seguro_game(ar->destino, destino, nodos, n, visitado))
            return true;
        ar = ar->sgte;
    }

    return false;
}

bool esRutaSeguraDFS(char origenNombre, char destinoNombre)
{
    if (pGame == NULL)
    {
        cout << "MAPA VACIO\n";
        return false;
    }

    TnodoGame origen = buscar_nodo_game(origenNombre);
    TnodoGame destino = buscar_nodo_game(destinoNombre);
    if (origen == NULL || destino == NULL)
    {
        cout << "NODO INVALIDO\n";
        return false;
    }

    int n = contar_nodos_game();
    TnodoGame nodos[100];
    int visitado[100];

    TnodoGame aux = pGame;
    for (int i = 0; i < n; i++)
    {
        nodos[i] = aux;
        visitado[i] = 0;
        aux = aux->sgte;
    }

    bool existe = dfs_seguro_game(origen, destino, nodos, n, visitado);
    cout << "\nRUTA DE RESCATE DFS\n";
    if (existe)
        cout << "SI EXISTE UNA RUTA SEGURA ENTRE " << origenNombre << " Y " << destinoNombre << "\n";
    else
        cout << "NO EXISTE RUTA SEGURA ENTRE " << origenNombre << " Y " << destinoNombre << "\n";
    return existe;
}

void dfs_componentes_game(TnodoGame actual, TnodoGame nodos[], int n, int visitado[])
{
    int pos = posicion_nodo_game(nodos, n, actual);
    if (pos == -1 || visitado[pos] || actual->isblocked)
        return;

    visitado[pos] = 1;

    TaristaGame ar = actual->ady;
    while (ar != NULL)
    {
        dfs_componentes_game(ar->destino, nodos, n, visitado);
        ar = ar->sgte;
    }
}

int contar_componentes_game()
{
    if (pGame == NULL)
        return 0;

    int n = contar_nodos_game();
    TnodoGame nodos[100];
    int visitado[100];

    TnodoGame aux = pGame;
    for (int i = 0; i < n; i++)
    {
        nodos[i] = aux;
        visitado[i] = 0;
        aux = aux->sgte;
    }

    int componentes = 0;
    for (int i = 0; i < n; i++)
    {
        if (!visitado[i] && !nodos[i]->isblocked)
        {
            componentes++;
            dfs_componentes_game(nodos[i], nodos, n, visitado);
        }
    }

    return componentes;
}

void mostrar_conectividad_game()
{
    int componentes = contar_componentes_game();
    cout << "\nMAPA DE CONECTIVIDAD\n";
    cout << "Componentes activas: " << componentes << "\n";
    if (componentes <= 1)
        cout << "La ciudad sigue conectada\n";
    else
        cout << "Hay refugios aislados por zombies o bloqueos\n";
}

bool rutaMenorPeligroDijkstra(char origenNombre, char destinoNombre)
{
    if (pGame == NULL)
    {
        cout << "MAPA VACIO\n";
        return false;
    }

    const int INF = 999999;
    TnodoGame origen = buscar_nodo_game(origenNombre);
    TnodoGame destino = buscar_nodo_game(destinoNombre);

    if (origen == NULL || destino == NULL)
    {
        cout << "NODO INVALIDO\n";
        return false;
    }

    if (origen->isblocked || destino->isblocked)
    {
        cout << "NO SE PUEDE CALCULAR LA RUTA EN UNA ZONA ROJA\n";
        return false;
    }

    int n = contar_nodos_game();
    TnodoGame nodos[100];
    int dist[100];
    int prev[100];
    int visitado[100];

    TnodoGame aux = pGame;
    for (int i = 0; i < n; i++)
    {
        nodos[i] = aux;
        dist[i] = INF;
        prev[i] = -1;
        visitado[i] = 0;
        aux = aux->sgte;
    }

    int posOrigen = posicion_nodo_game(nodos, n, origen);
    int posDestino = posicion_nodo_game(nodos, n, destino);
    dist[posOrigen] = 0;

    for (int iter = 0; iter < n; iter++)
    {
        int u = -1;
        for (int i = 0; i < n; i++)
        {
            if (!visitado[i] && !nodos[i]->isblocked && (u == -1 || dist[i] < dist[u]))
                u = i;
        }

        if (u == -1 || dist[u] == INF)
            break;

        visitado[u] = 1;

        TaristaGame ar = nodos[u]->ady;
        while (ar != NULL)
        {
            int v = posicion_nodo_game(nodos, n, ar->destino);
            if (v != -1 && !nodos[v]->isblocked && dist[u] + ar->peligro < dist[v])
            {
                dist[v] = dist[u] + ar->peligro;
                prev[v] = u;
            }
            ar = ar->sgte;
        }
    }

    if (dist[posDestino] == INF)
    {
        cout << "NO EXISTE UNA RUTA CON DIJKSTRA HACIA EL DESTINO\n";
        return false;
    }

    cout << "\nRUTA DE MENOR PELIGRO DIJKSTRA\n";
    cout << "Ruta: ";
    reconstruir_camino_game(prev, nodos, posDestino);
    cout << "\nPeligro acumulado: " << dist[posDestino] << "\n";
    return true;
}

void menu_game_v1()
{
    cout << "\nPROGRA-VILLA V1\n";
    cout << "1. Cargar ciudad desde archivo\n";
    cout << "2. Mostrar mapa\n";
    cout << "3. Bloquear zona roja\n";
    cout << "4. Desbloquear zona\n";
    cout << "5. Buscar suministros BFS\n";
    cout << "6. Verificar ruta segura DFS\n";
    cout << "7. Ver conectividad\n";
    cout << "8. Ruta de menor peligro Dijkstra\n";
    cout << "0. Salir\n";
    cout << "Opcion: ";
}

#endif
