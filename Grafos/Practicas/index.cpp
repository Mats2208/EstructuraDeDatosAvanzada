// Grafos.cpp : Este archivo contiene la función "main". La ejecución del programa comienza y termina ahí.

#include <iostream>

struct nodo {
    char nombre;//nombre del vertice o nodo
    struct nodo* sgte;
    struct arista* ady;//puntero hacia la primera arista
    //del nodo
};

struct arista {
    struct nodo* destino;//puntero hacia el nodo de llegada
    struct arista* sgte;
};

typedef struct nodo* Tnodo;// Tipo Nodo
typedef struct arista* Tarista; //Tipo Arista

Tnodo p;//puntero cabeza
int tipoGrafo = 1; // 1 dirigido, 2 no dirigido

void menu();
void insertar_nodo();
void agrega_arista(Tnodo&, Tnodo&, Tarista&);
void insertar_arista();
void vaciar_aristas(Tnodo&);
void eliminar_nodo();
void eliminar_arista();
void mostrar_grafo();
void mostrar_aristas();
void mostrar_matriz();
void configurar_tipo();
void recorre_grafo_en_amplitud();
void recorre_grafo_en_profundidad();
void dijkstra();

Tnodo buscar_nodo(char);
bool existe_arista(Tnodo, Tnodo);
bool eliminar_arista_simple(Tnodo, Tnodo);
void eliminar_referencias_a_nodo(Tnodo);
int contar_nodos();
int posicion_nodo(Tnodo[], int, Tnodo);
void dfs(Tnodo, Tnodo[], int, int[]);

/* Funcion Principal
---------------------------------------------------------------------*/
int main(void)
{
    p = NULL;
    int op; // opcion del menu
    do
    {
        menu();
        std::cin >> op;
        switch (op)
        {
        case 1:
            insertar_nodo();
            break;
        case 2:
            insertar_arista();
            break;
        case 3:
            eliminar_nodo();
            break;
        case 4:
            eliminar_arista();
            break;
        case 5:
            mostrar_grafo();
            break;
        case 6:
            mostrar_aristas();
            break;
        case 7:
            recorre_grafo_en_amplitud();
            break;
        case 8:
            recorre_grafo_en_profundidad();
            break;
        case 9:
            mostrar_matriz();
            break;
        case 10:
            configurar_tipo();
            break;
        case 11:
            dijkstra();
            break;
        case 0:
            break;
        default:
            std::cout << "OPCION NO VALIDA...!!!";
            break;
        }
        if (op != 0)
        {
            std::cout << std::endl << std::endl;
        }
    } while (op != 0);
    getchar();
    return 0;
}

/* Menu
---------------------------------------------------------------------*/
void menu()
{
    std::cout << "\n\tREPRESENTACION DE GRAFOS";
    if (tipoGrafo == 1)
        std::cout << " DIRIGIDOS\n\n";
    else
        std::cout << " NO DIRIGIDOS\n\n";

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
    std::cout << " 0. SALIR " << std::endl;
    std::cout << "\n INGRESE OPCION: ";
}

/* Buscar nodo por nombre
---------------------------------------------------------------------*/
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

/* Verificar si ya existe arista
---------------------------------------------------------------------*/
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

/* INSERTAR NODO AL GRAFO
---------------------------------------------------------------------*/
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

/* AGREGAR ARISTA
---------------------------------------------------------------------*/
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

/* INSERTAR ARISTA
---------------------------------------------------------------------*/
void insertar_arista()
{
    char ini, fin;
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
    agrega_arista(aux, aux2, nuevo);

    if (tipoGrafo == 2 && aux != aux2 && !existe_arista(aux2, aux))
    {
        Tarista nuevo2 = new struct arista;
        nuevo2->sgte = NULL;
        agrega_arista(aux2, aux, nuevo2);
    }

    std::cout << "ARISTA AGREGADA...!!!!";
}

/* FUNCION PARA BORRAR TODAS LAS ARISTAS DE UN NODO
---------------------------------------------------------------------*/
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

/* Eliminar una arista simple origen -> destino
---------------------------------------------------------------------*/
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

/* Eliminar referencias a un nodo desde otros nodos
---------------------------------------------------------------------*/
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

/* ELIMINAR NODO
---------------------------------------------------------------------*/
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

/* ELIMINAR ARISTA
---------------------------------------------------------------------*/
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

/* MOSTRAR GRAFO
---------------------------------------------------------------------*/
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
                std::cout << " " << ar->destino->nombre;
                ar = ar->sgte;
            }
        }
        ptr = ptr->sgte;
        std::cout << std::endl;
    }
}

/* MOSTRAR ARISTAS
---------------------------------------------------------------------*/
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
        std::cout << ar->destino->nombre << " ";
        ar = ar->sgte;
    }
    std::cout << std::endl;
}

/* Contar nodos
---------------------------------------------------------------------*/
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

/* Retorna posicion de un nodo en un arreglo
---------------------------------------------------------------------*/
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

/* MOSTRAR MATRIZ
---------------------------------------------------------------------*/
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
                matriz[i][j] = 1;
            ar = ar->sgte;
        }
    }

    std::cout << "\nMATRIZ DE ADYACENCIA\n\n   ";
    for (i = 0; i < n; i++)
    {
        std::cout << nodos[i]->nombre << " ";
    }
    std::cout << std::endl;

    for (i = 0; i < n; i++)
    {
        std::cout << nodos[i]->nombre << "  ";
        for (j = 0; j < n; j++)
        {
            std::cout << matriz[i][j] << " ";
        }
        std::cout << std::endl;
    }
}

/* Recorrido en amplitud
---------------------------------------------------------------------*/
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

/* DFS auxiliar
---------------------------------------------------------------------*/
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

/* Recorrido en profundidad
---------------------------------------------------------------------*/
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

/* Cambiar tipo de grafo
---------------------------------------------------------------------*/
void configurar_tipo()
{
    int op;
    std::cout << "1. DIRIGIDO\n";
    std::cout << "2. NO DIRIGIDO\n";
    std::cout << "INGRESE OPCION:";
    std::cin >> op;

    if (op == 1 || op == 2)
    {
        tipoGrafo = op;
        std::cout << "TIPO DE GRAFO ACTUALIZADO...!!!";
    }
    else
    {
        std::cout << "OPCION NO VALIDA...!!!";
    }
}
void dijkstra()
{
    /*
    */

    Tnodo ptr;
    Tarista ar;
    ptr = p;
    std::cout << "Por implementar\n";
}