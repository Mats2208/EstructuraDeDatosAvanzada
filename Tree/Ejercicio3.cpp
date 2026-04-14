/*
Balanceando árboles
Ahora debemos balancear los arboles, asegurando que el balance se mantenga luego de cada operación de inserción y eliminación.

Incluiremos la construcción de métodos para verificar el factor de balanceo en los árboles en cualquier momento.
*/

#include <iostream>
#include <stdlib.h>
using namespace std;
struct nodo {
	int nro;
	struct nodo* izq, * der;
};
typedef struct nodo* ABB;
/* es un puntero de tipo nodo que hemos llamado ABB, que ulitizaremos
para mayor facilidad de creacion de variables */
ABB crearNodo(int x)


{
	ABB nuevoNodo = new(struct nodo);
	nuevoNodo->nro = x;
	nuevoNodo->izq = NULL;
	nuevoNodo->der = NULL;
	return nuevoNodo;
}
void insertar(ABB& arbol, int x)
{
	if (arbol == NULL)
	{
		arbol = crearNodo(x);
	}
	else if (x < arbol->nro)
		insertar(arbol->izq, x);
	else if (x > arbol->nro)
		insertar(arbol->der, x);
}
void preOrden(ABB arbol)
{
	if (arbol != NULL)
	{
		cout << arbol->nro << " ";
		preOrden(arbol->izq);
		preOrden(arbol->der);
	}
}
void enOrden(ABB arbol)
{
	if (arbol != NULL)
	{
		enOrden(arbol->izq);
		cout << arbol->nro << " ";
		enOrden(arbol->der);
	}
}
void postOrden(ABB arbol)
{
	if (arbol != NULL)
	{
		postOrden(arbol->izq);
		postOrden(arbol->der);
		cout << arbol->nro << " ";
	}
}
// Retorna el menor nodo del subárbol (el más a la izquierda)
ABB menorNodo(ABB arbol)
{
	while (arbol->izq != NULL)
		arbol = arbol->izq;
	return arbol;
}

// Calcula la altura de un nodo
int altura(ABB arbol)
{
	if (arbol == NULL)
		return 0;
	int altIzq = altura(arbol->izq);
	int altDer = altura(arbol->der);
	return 1 + (altIzq > altDer ? altIzq : altDer);
}

// Obtiene el factor de balanceo de un nodo
int factorBalanceo(ABB arbol)
{
	if (arbol == NULL)
		return 0;
	return altura(arbol->izq) - altura(arbol->der);
}

// Rotación a la derecha
ABB rotarDerecha(ABB arbol)
{
	ABB nuevo = arbol->izq;
	arbol->izq = nuevo->der;
	nuevo->der = arbol;
	return nuevo;
}

// Rotación a la izquierda
ABB rotarIzquierda(ABB arbol)
{
	ABB nuevo = arbol->der;
	arbol->der = nuevo->izq;
	nuevo->izq = arbol;
	return nuevo;
}

// Muestra el factor de balanceo de cada nodo
void mostrarBalance(ABB arbol, int n)
{
	if (arbol == NULL)
		return;
	mostrarBalance(arbol->der, n + 1);
	for (int i = 0; i < n; i++)
		cout << " ";
	cout << "Nodo: " << arbol->nro << " | Balance: " << factorBalanceo(arbol) 
	     << " | Altura: " << altura(arbol) << endl;
	mostrarBalance(arbol->izq, n + 1);
}

void eliminar(ABB& arbol, int x)
{
	if (arbol == NULL)
		return;

	if (x < arbol->nro)
		eliminar(arbol->izq, x);
	else if (x > arbol->nro)
		eliminar(arbol->der, x);
	else
	{
		// Nodo encontrado
		if (arbol->izq == NULL && arbol->der == NULL)
		{
			// Caso 1: sin hijos
			delete arbol;
			arbol = NULL;
		}
		else if (arbol->izq == NULL)
		{
			// Caso 2: solo hijo derecho
			ABB temp = arbol;
			arbol = arbol->der;
			delete temp;
		}
		else if (arbol->der == NULL)
		{
			// Caso 2: solo hijo izquierdo
			ABB temp = arbol;
			arbol = arbol->izq;
			delete temp;
		}
		else
		{
			// Caso 3: dos hijos → reemplazar con el menor del subárbol derecho
			ABB sucesor = menorNodo(arbol->der);
			arbol->nro = sucesor->nro;
			eliminar(arbol->der, sucesor->nro);
		}
	}
}

void verArbol(ABB arbol, int n)
{
	if (arbol == NULL)
		return;
	verArbol(arbol->der, n + 1);
	for (int i = 0; i < n; i++)
		cout << " ";
	cout << arbol->nro << endl;
	verArbol(arbol->izq, n + 1);
}
int main()
{
	ABB arbol = NULL; // creado Arbol
	int n; // numero de nodos del arbol
	int x; // elemento a insertar en cada nodo
	cout << "\n\t\t ..[ ARBOL BINARIO DE BUSQUEDA ].. \n\n";
	cout << " Numero de nodos del arbol: ";
	cin >> n;
	cout << endl;
	for (int i = 0; i < n; i++)
	{
		cout << " Numero del nodo " << i + 1 << ": ";
		cin >> x;
		insertar(arbol, x);
	}
	cout << "\n Mostrando ABB \n\n";
	verArbol(arbol, 0);
	cout << "\n Factor de Balanceo de cada nodo:\n\n";
	mostrarBalance(arbol, 0);
	cout << "\n Recorridos del ABB";
	cout << "\n\n En orden : "; enOrden(arbol);
	cout << "\n\n Pre Orden : "; preOrden(arbol);
	cout << "\n\n Post Orden : "; postOrden(arbol);

	int e;
	cout << "\n\n Nodo a eliminar: ";
	cin >> e;
	eliminar(arbol, e);
	cout << "\n Arbol tras eliminar " << e << ":\n\n";
	verArbol(arbol, 0);
	cout << "\n Factor de Balanceo tras eliminacion:\n\n";
	mostrarBalance(arbol, 0);
	cout << "\n En orden : "; enOrden(arbol);
	cout << endl << endl;
	system("pause");
	return 0;
}

