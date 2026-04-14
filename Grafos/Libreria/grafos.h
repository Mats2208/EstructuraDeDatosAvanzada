#ifndef GRAFOS_H
#define GRAFOS_H

#include <iostream>
#include  <fstream>
#include <string>
#include <sstream>
#include <cstdlib>

using namespace std;

struct nodo {
    char nombre;
    struct nodo* sgte;
    struct arista* ady;
};

struct arista {
    struct nodo* destino;
    struct arista* sgte;
    int peso;
};

typedef struct nodo* Tnodo;
typedef struct arista* Tarista;

Tnodo p = NULL;//puntero cabeza
int tipoGrafo = 1;

void menu()
{
    std::cout << "\n\tREPRESENTACION DE GRAFOS";
    if (tipoGrafo == 1)
        std::cout << " DIRIGIDOS\n\n";
    else if (tipoGrafo == 2)
        std::cout << " NO DIRIGIDOS\n\n";
    else
        std::cout << " MIXTOS\n\n";

    std::cout << " 1. INSERTAR UN NODO " << std::endl;
    std::cout << " 2. INSERTAR UNA ARISTA " << std::endl;
    std::cout << " 3. ELIMINAR UN NODO " << std::endl;
    std::cout << " 4. ELIMINAR UNA ARISTA " << std::endl;
    std::cout << " 5. MOSTRAR GRAFO " << std::endl;
    std::cout << " 6. MOSTRAR ARISTAS DE UN NODO " << std::endl;
    std::cout << " 7. RECORRER GRAFO EN AMPLITUD " << std::endl;
    std::cout << " 8. RECORRER GRAFO EN PROFUNDIDAD " << std::endl;
    std::cout << " 9. MOSTRAR MATRIZ DE ADYACENCIA " << std::endl;
    std::cout << " 10. CAMBIAR TIPO DE GRAFO " << std::endl;
    std::cout << " 11. ALGORITMO DE DIJKSTRA " << std::endl;
    std::cout << " 12. ALGORITMO DE WARSHALL " << std::endl;
    std::cout << " 13. ALGORITMO DE FLOYD " << std::endl;
    std::cout << " 14. Cargar un grafo desde archivo " << std::endl;
    std::cout << " 15. Guardar el grafo actual en un archivo " << std::endl;
    std::cout << " 0. SALIR " << std::endl;
    std::cout << "\n INGRESE OPCION: ";
}

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

void insertar_nodo()
{
    Tnodo t, nuevo = new struct nodo;
    std::cout << "INGRESE VARIABLE:";
    std::cin >> nuevo->nombre;

    if (buscar_nodo(nuevo->nombre) != NULL)
    {
        std::cout << "EL NODO YA EXISTE...!!!";
        delete(nuevo);
        return;
    }

    nuevo->sgte = NULL;
    nuevo->ady = NULL;
    if (p == NULL)
    {
        p = nuevo;
        std::cout << "PRIMER NODO...!!!";
    }
    else
    {
        t = p;
        while (t->sgte != NULL)
        {
            t = t->sgte;
        }
        t->sgte = nuevo;
        std::cout << "NODO INGRESADO...!!!";
    }
}

void agrega_arista(Tnodo& aux, Tnodo& aux2, Tarista& nuevo)
{
    Tarista q;
    if (aux->ady == NULL)
    {
        aux->ady = nuevo;
        nuevo->destino = aux2;
    }
    else
    {
        q = aux->ady;
        while (q->sgte != NULL)
            q = q->sgte;
        nuevo->destino = aux2;
        q->sgte = nuevo;
    }
}

void insertar_arista()
{
    char ini, fin;
    int peso;
    Tarista nuevo;
    Tnodo aux, aux2;

    if (p == NULL)
    {
        std::cout << "GRAFO VACIO...!!!!";
        return;
    }

    std::cout << "INGRESE NODO DE INICIO:";
    std::cin >> ini;
    std::cout << "INGRESE NODO FINAL:";
    std::cin >> fin;
    std::cout << "INGRESE PESO:";
    std::cin >> peso;

    aux = buscar_nodo(ini);
    aux2 = buscar_nodo(fin);

    if (aux == NULL)
    {
        std::cout << "EL NODO DE INICIO NO EXISTE...!!!";
        return;
    }

    if (aux2 == NULL)
    {
        std::cout << "EL NODO FINAL NO EXISTE...!!!";
        return;
    }

    if (existe_arista(aux, aux2))
    {
        std::cout << "LA ARISTA YA EXISTE...!!!";
        return;
    }

    nuevo = new struct arista;
    nuevo->sgte = NULL;
    nuevo->peso = peso;
    agrega_arista(aux, aux2, nuevo);

    if (tipoGrafo == 2 && aux != aux2 && !existe_arista(aux2, aux))
    {
        Tarista nuevo2 = new struct arista;
        nuevo2->sgte = NULL;
        nuevo2->peso = peso;
        agrega_arista(aux2, aux, nuevo2);
    }
    else if (tipoGrafo == 3 && aux != aux2 && !existe_arista(aux2, aux))
    {
        int bidireccional;
        std::cout << "¿ARISTA BIDIRECCIONAL? (1=SI, 0=NO): ";
        std::cin >> bidireccional;
        if (bidireccional == 1)
        {
            Tarista nuevo2 = new struct arista;
            nuevo2->sgte = NULL;
            nuevo2->peso = peso;
            agrega_arista(aux2, aux, nuevo2);
        }
    }

    std::cout << "ARISTA AGREGADA...!!!!";; 
}

void vaciar_aristas(Tnodo& aux)
{
    Tarista q, r;
    q = aux->ady;

    while (q != NULL)
    {
        r = q;
        q = q->sgte;
        delete(r);
    }

    aux->ady = NULL;
}

bool eliminar_arista_simple(Tnodo origen, Tnodo destino)
{
    Tarista q, ant;

    if (origen == NULL || destino == NULL)
        return false;

    q = origen->ady;
    ant = NULL;

    while (q != NULL)
    {
        if (q->destino == destino)
        {
            if (ant == NULL)
                origen->ady = q->sgte;
            else
                ant->sgte = q->sgte;

            delete(q);
            return true;
        }

        ant = q;
        q = q->sgte;
    }

    return false;
}

void eliminar_referencias_a_nodo(Tnodo destino)
{
    Tnodo aux = p;

    while (aux != NULL)
    {
        while (eliminar_arista_simple(aux, destino))
        {
        }
        aux = aux->sgte;
    }
}

void eliminar_nodo()
{
    char var;
    Tnodo aux, ant;
    aux = p;
    ant = NULL;

    std::cout << "ELIMINAR UN NODO\n";
    if (p == NULL)
    {
        std::cout << "GRAFO VACIO...!!!!";
        return;
    }

    std::cout << "INGRESE NOMBRE DE VARIABLE:";
    std::cin >> var;

    while (aux != NULL && aux->nombre != var)
    {
        ant = aux;
        aux = aux->sgte;
    }

    if (aux == NULL)
    {
        std::cout << "EL NODO NO EXISTE...!!!";
        return;
    }

    eliminar_referencias_a_nodo(aux);
    vaciar_aristas(aux);

    if (aux == p)
    {
        p = p->sgte;
    }
    else
    {
        ant->sgte = aux->sgte;
    }

    delete(aux);
    std::cout << "NODO ELIMINADO...!!!!";
}

void eliminar_arista()
{
    char ini, fin;
    Tnodo aux, aux2;
    bool eliminado;

    std::cout << "\n ELIMINAR ARISTA\n";

    if (p == NULL)
    {
        std::cout << "GRAFO VACIO...!!!!";
        return;
    }

    std::cout << "INGRESE NODO DE INICIO:";
    std::cin >> ini;
    std::cout << "INGRESE NODO FINAL:";
    std::cin >> fin;

    aux = buscar_nodo(ini);
    aux2 = buscar_nodo(fin);

    if (aux == NULL)
    {
        std::cout << "EL NODO DE INICIO NO EXISTE...!!!";
        return;
    }

    if (aux2 == NULL)
    {
        std::cout << "EL NODO FINAL NO EXISTE...!!!";
        return;
    }

    eliminado = eliminar_arista_simple(aux, aux2);

    if (tipoGrafo == 2 && aux != aux2)
    {
        eliminar_arista_simple(aux2, aux);
    }

    if (eliminado)
        std::cout << "ARISTA ELIMINADA.....!!!!";
    else
        std::cout << "LA ARISTA NO EXISTE...!!!";
}

void mostrar_grafo()
{
    Tnodo ptr;
    Tarista ar;
    ptr = p;
    std::cout << "NODO|LISTA DE ADYACENCIA\n";
    while (ptr != NULL)
    {
        std::cout << " " << ptr->nombre << "|";
        if (ptr->ady != NULL)
        {
            ar = ptr->ady;
            while (ar != NULL)
            {
                std::cout << " " << ar->destino->nombre << "(" << ar->peso << ")";
                ar = ar->sgte;
            }
        }
        ptr = ptr->sgte;
        std::cout << std::endl;
    }
}

void mostrar_aristas()
{
    Tnodo aux;
    Tarista ar;
    char var;
    std::cout << "MOSTRAR ARISTAS DE NODO\n";
    std::cout << "INGRESE NODO:";
    std::cin >> var;

    aux = buscar_nodo(var);

    if (aux == NULL)
    {
        std::cout << "EL NODO NO EXISTE...!!!";
        return;
    }

    if (aux->ady == NULL)
    {
        std::cout << "EL NODO NO TIENE ARISTAS...!!!!";
        return;
    }

    std::cout << "NODO|LISTA DE ADYACENCIA\n";
    std::cout << " " << aux->nombre << "|";
    ar = aux->ady;
    while (ar != NULL)
    {
        std::cout << ar->destino->nombre << "(" << ar->peso << ") ";
        ar = ar->sgte;
    }
    std::cout << std::endl;
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
    int i;
    for (i = 0; i < n; i++)
    {
        if (v[i] == buscado)
            return i;
    }
    return -1;
}

void mostrar_matriz()
{
    int n, i, j;
    int matriz[100][100];
    Tnodo nodos[100];
    Tnodo aux;
    Tarista ar;

    if (p == NULL)
    {
        std::cout << "GRAFO VACIO...!!!!";
        return;
    }

    n = contar_nodos();
    aux = p;
    i = 0;
    while (aux != NULL)
    {
        nodos[i] = aux;
        aux = aux->sgte;
        i++;
    }

    for (i = 0; i < n; i++)
    {
        for (j = 0; j < n; j++)
        {
            matriz[i][j] = 0;
        }
    }

    for (i = 0; i < n; i++)
    {
        ar = nodos[i]->ady;
        while (ar != NULL)
        {
            j = posicion_nodo(nodos, n, ar->destino);
            if (j != -1)
                matriz[i][j] = ar->peso;
            ar = ar->sgte;
        }
    }

    std::cout << "\nMATRIZ DE PESOS (0 = sin arista)\n\n     ";
    for (i = 0; i < n; i++)
    {
        std::cout << nodos[i]->nombre << "    ";
    }
    std::cout << std::endl;

    for (i = 0; i < n; i++)
    {
        std::cout << nodos[i]->nombre << "  ";
        for (j = 0; j < n; j++)
        {
            std::cout << matriz[i][j] << "    ";
        }
        std::cout << std::endl;
    }
}

void recorre_grafo_en_amplitud()
{
    Tnodo nodos[100], cola[100];
    int visitado[100];
    int n, i, frente, fin, posActual, posDestino;
    Tnodo aux, actual, inicio;
    Tarista ar;
    char inicioChar;

    if (p == NULL)
    {
        std::cout << "GRAFO VACIO...!!!!";
        return;
    }

    n = contar_nodos();
    aux = p;
    i = 0;
    while (aux != NULL)
    {
        nodos[i] = aux;
        visitado[i] = 0;
        aux = aux->sgte;
        i++;
    }

    std::cout << "INGRESE NODO DE INICIO:";
    std::cin >> inicioChar;

    inicio = buscar_nodo(inicioChar);

    if (inicio == NULL)
    {
        std::cout << "EL NODO NO EXISTE...!!!";
        return;
    }

    frente = 0;
    fin = 0;
    cola[fin] = inicio;
    fin++;

    posActual = posicion_nodo(nodos, n, inicio);
    visitado[posActual] = 1;

    std::cout << "RECORRIDO EN AMPLITUD: ";

    while (frente < fin)
    {
        actual = cola[frente];
        frente++;

        std::cout << actual->nombre << " ";

        ar = actual->ady;
        while (ar != NULL)
        {
            posDestino = posicion_nodo(nodos, n, ar->destino);
            if (posDestino != -1 && visitado[posDestino] == 0)
            {
                visitado[posDestino] = 1;
                cola[fin] = ar->destino;
                fin++;
            }
            ar = ar->sgte;
        }
    }
}

void dfs(Tnodo actual, Tnodo nodos[], int n, int visitado[])
{
    Tarista ar;
    int pos, posDestino;

    pos = posicion_nodo(nodos, n, actual);
    if (pos == -1 || visitado[pos] == 1)
        return;

    visitado[pos] = 1;
    std::cout << actual->nombre << " ";

    ar = actual->ady;
    while (ar != NULL)
    {
        posDestino = posicion_nodo(nodos, n, ar->destino);
        if (posDestino != -1 && visitado[posDestino] == 0)
        {
            dfs(ar->destino, nodos, n, visitado);
        }
        ar = ar->sgte;
    }
}

void recorre_grafo_en_profundidad()
{
    Tnodo nodos[100];
    int visitado[100];
    int n, i;
    Tnodo aux, inicio;
    char inicioChar;

    if (p == NULL)
    {
        std::cout << "GRAFO VACIO...!!!!";
        return;
    }

    n = contar_nodos();
    aux = p;
    i = 0;
    while (aux != NULL)
    {
        nodos[i] = aux;
        visitado[i] = 0;
        aux = aux->sgte;
        i++;
    }

    std::cout << "INGRESE NODO DE INICIO:";
    std::cin >> inicioChar;

    inicio = buscar_nodo(inicioChar);

    if (inicio == NULL)
    {
        std::cout << "EL NODO NO EXISTE...!!!";
        return;
    }

    std::cout << "RECORRIDO EN PROFUNDIDAD: ";
    dfs(inicio, nodos, n, visitado);
}

void configurar_tipo()
{
    int op;
    std::cout << "\n1. DIRIGIDO\n";
    std::cout << "2. NO DIRIGIDO\n";
    std::cout << "3. MIXTO (dirigidas y bidireccionales)\n";
    std::cout << "INGRESE OPCION: ";
    std::cin >> op;

    if (op >= 1 && op <= 3)
    {
        tipoGrafo = op;
        std::cout << "TIPO DE GRAFO ACTUALIZADO...!!!";
    }
    else
    {
        std::cout << "OPCION NO VALIDA...!!!";
    }
}


void CargarGrafo(){
    string nombre_archivo;
    std::cout << "INGRESE NOMBRE DE ARCHIVO:";

    ifstream archivo;
    cout << "Ingresa el nombre del archivo a cargar (Tiene que ser un archivo .txt ubicado en la carpeta 'carga'): " << endl;
    cin >> nombre_archivo;
    nombre_archivo += ".txt";

    cout << "El archivo a buscar es: " << nombre_archivo << endl;

    archivo.open(nombre_archivo);
    if (!archivo.good()) {
        cout << "Error: No se pudo abrir el archivo " << nombre_archivo << endl;
        cout << "Asegúrese de que la carpeta 'carga' existe y el archivo se encuentra en ella." << endl;
        return;
    }

    string linea;
    while (getline(archivo, linea)) {
        if (linea.empty()) continue;
        size_t pos = linea.find('|');
        if (pos == string::npos) continue;
        
        string nombre_nodo_str = linea.substr(0, pos);
        nombre_nodo_str.erase(0, nombre_nodo_str.find_first_not_of(" \t"));
        nombre_nodo_str.erase(nombre_nodo_str.find_last_not_of(" \t") + 1);
        
                if (nombre_nodo_str.empty()) continue;
        char nombre_nodo = nombre_nodo_str[0];
        
        if (buscar_nodo(nombre_nodo) == NULL) {
            Tnodo nuevo = new struct nodo;
            nuevo->nombre = nombre_nodo;
            nuevo->sgte = NULL;
            nuevo->ady = NULL;
            
            if (p == NULL) {
                p = nuevo;
            } else {
                Tnodo t = p;
                while (t->sgte != NULL) t = t->sgte;
                t->sgte = nuevo;
            }
        }

        string adyacentes_str = linea.substr(pos + 1);
        istringstream iss(adyacentes_str);
        string adyacente;
        
        while (iss >> adyacente) {
            if (adyacente.empty()) continue;

            // Formato: "nodo:peso:bidireccional" o "nodo:peso" (bidireccional=0 por defecto)
            char nombre_adyacente;
            int peso_arista = 1;
            int bidireccional = 0;
            
            size_t sep1 = adyacente.find(':');
            if (sep1 != string::npos) {
                nombre_adyacente = adyacente[0];
                size_t sep2 = adyacente.find(':', sep1 + 1);
                if (sep2 != string::npos) {
                    peso_arista = atoi(adyacente.substr(sep1 + 1, sep2 - sep1 - 1).c_str());
                    bidireccional = atoi(adyacente.substr(sep2 + 1).c_str());
                } else {
                    peso_arista = atoi(adyacente.substr(sep1 + 1).c_str());
                }
            } else {
                nombre_adyacente = adyacente[0];
            }

            if (buscar_nodo(nombre_adyacente) == NULL) {
                Tnodo nuevo = new struct nodo;
                nuevo->nombre = nombre_adyacente;
                nuevo->sgte = NULL;
                nuevo->ady = NULL;
                
                Tnodo t = p;
                while (t->sgte != NULL) t = t->sgte;
                t->sgte = nuevo;
            }

            Tnodo origen = buscar_nodo(nombre_nodo);
            Tnodo destino = buscar_nodo(nombre_adyacente);
            
            if (origen != NULL && destino != NULL && !existe_arista(origen, destino)) {
                Tarista nueva_arista = new struct arista;
                nueva_arista->sgte = NULL;
                nueva_arista->peso = peso_arista;
                agrega_arista(origen, destino, nueva_arista);
                
                // Si es bidireccional, agregar arista inversa
                if (bidireccional == 1 && !existe_arista(destino, origen)) {
                    Tarista nueva_arista2 = new struct arista;
                    nueva_arista2->sgte = NULL;
                    nueva_arista2->peso = peso_arista;
                    agrega_arista(destino, origen, nueva_arista2);
                }
            }
        }
    }

}

void guardar_grafo_h(){
    string nombre_archivo;
    ofstream archivo;

    cout << "Ingresa el nombre del archivo a guardar (sin extension, se guardara en 'carga'): " << endl;
    cin >> nombre_archivo;
    nombre_archivo = "carga/" + nombre_archivo + ".txt";

    archivo.open(nombre_archivo, ios::out);
    if (!archivo.good()) {
        cout << "Error al abrir el archivo " << nombre_archivo << endl;
        return;
    }

    Tnodo ptr = p;
    Tarista ar;
    while (ptr != NULL)
    {
        archivo << " " << ptr->nombre << "|";
        ar = ptr->ady;
        while (ar != NULL)
        {
            // Verificar si existe arista inversa (es bidireccional)
            int bidireccional = existe_arista(ar->destino, ptr) ? 1 : 0;
            archivo << " " << ar->destino->nombre << ":" << ar->peso << ":" << bidireccional;
            ar = ar->sgte;
        }
        ptr = ptr->sgte;
        archivo << "\n";
    }

    archivo.close();
    cout << "Grafo guardado en: " << nombre_archivo << endl;
}

void dijkstra()
{
    if (p == NULL) { std::cout << "GRAFO VACIO...!!!!"; return; }

    const int INF = 999999;
    int n = contar_nodos();
    Tnodo nodos[100];
    int dist[100], visitado[100], prev[100];

    Tnodo aux = p;
    int i = 0;
    while (aux != NULL) { nodos[i++] = aux; aux = aux->sgte; }

    char inicioChar;
    std::cout << "INGRESE NODO DE INICIO: ";
    std::cin >> inicioChar;
    Tnodo inicio = buscar_nodo(inicioChar);
    if (inicio == NULL) { std::cout << "EL NODO NO EXISTE...!!!"; return; }

    int posInicio = posicion_nodo(nodos, n, inicio);
    for (i = 0; i < n; i++) { dist[i] = INF; visitado[i] = 0; prev[i] = -1; }
    dist[posInicio] = 0;

    for (int iter = 0; iter < n; iter++)
    {
        int u = -1;
        for (i = 0; i < n; i++)
            if (!visitado[i] && (u == -1 || dist[i] < dist[u])) u = i;
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

    std::cout << "\nDIJKSTRA - DISTANCIAS DESDE '" << inicioChar << "':\n\n";
    std::cout << "NODO | DISTANCIA | CAMINO\n";
    std::cout << "-----+-----------+----------------------------------\n";
    for (i = 0; i < n; i++)
    {
        std::cout << "  " << nodos[i]->nombre << "  |    ";
        if (dist[i] == INF)
        {
            std::cout << "INF      | SIN CAMINO";
        }
        else
        {
            std::cout << dist[i] << "        | ";
            int path[100], pathLen = 0, cur = i;
            while (cur != -1) { path[pathLen++] = cur; cur = prev[cur]; }
            for (int k = pathLen - 1; k >= 0; k--)
            {
                std::cout << nodos[path[k]]->nombre;
                if (k > 0) std::cout << " -> ";
            }
        }
        std::cout << std::endl;
    }
}

void warshall()
{
    if (p == NULL) { std::cout << "GRAFO VACIO...!!!!"; return; }

    int n = contar_nodos();
    Tnodo nodos[100];
    int matriz[100][100];

    Tnodo aux = p;
    int i = 0;
    while (aux != NULL) { nodos[i++] = aux; aux = aux->sgte; }

    for (int a = 0; a < n; a++)
        for (int b = 0; b < n; b++)
            matriz[a][b] = (a == b) ? 1 : 0;

    for (int a = 0; a < n; a++)
    {
        Tarista ar = nodos[a]->ady;
        while (ar != NULL)
        {
            int b = posicion_nodo(nodos, n, ar->destino);
            if (b != -1) matriz[a][b] = 1;
            ar = ar->sgte;
        }
    }

    for (int k = 0; k < n; k++)
        for (int a = 0; a < n; a++)
            for (int b = 0; b < n; b++)
                if (matriz[a][k] && matriz[k][b])
                    matriz[a][b] = 1;

    std::cout << "\nMATRIZ DE WARSHALL (CLAUSURA TRANSITIVA)\n\n   ";
    for (int a = 0; a < n; a++) std::cout << nodos[a]->nombre << " ";
    std::cout << std::endl;
    for (int a = 0; a < n; a++)
    {
        std::cout << nodos[a]->nombre << "  ";
        for (int b = 0; b < n; b++)
            std::cout << matriz[a][b] << " ";
        std::cout << std::endl;
    }
}

void floyd()
{
    if (p == NULL) { std::cout << "GRAFO VACIO...!!!!"; return; }

    const int INF = 999999;
    int n = contar_nodos();
    Tnodo nodos[100];
    int dist[100][100];

    Tnodo aux = p;
    int i = 0;
    while (aux != NULL) { nodos[i++] = aux; aux = aux->sgte; }

    for (int a = 0; a < n; a++)
        for (int b = 0; b < n; b++)
            dist[a][b] = (a == b) ? 0 : INF;

    for (int a = 0; a < n; a++)
    {
        Tarista ar = nodos[a]->ady;
        while (ar != NULL)
        {
            int b = posicion_nodo(nodos, n, ar->destino);
            if (b != -1) dist[a][b] = ar->peso;
            ar = ar->sgte;
        }
    }

    for (int k = 0; k < n; k++)
        for (int a = 0; a < n; a++)
            for (int b = 0; b < n; b++)
                if (dist[a][k] != INF && dist[k][b] != INF
                    && dist[a][k] + dist[k][b] < dist[a][b])
                    dist[a][b] = dist[a][k] + dist[k][b];

    std::cout << "\nMATRIZ DE FLOYD (DISTANCIAS MINIMAS)\n\n     ";
    for (int a = 0; a < n; a++) std::cout << nodos[a]->nombre << "    ";
    std::cout << std::endl;
    for (int a = 0; a < n; a++)
    {
        std::cout << nodos[a]->nombre << "  ";
        for (int b = 0; b < n; b++)
        {
            if (dist[a][b] == INF) std::cout << "INF  ";
            else std::cout << dist[a][b] << "    ";
        }
        std::cout << std::endl;
    }
}

#endif
