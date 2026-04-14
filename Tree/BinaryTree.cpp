/* El programa base de árboles binarios tiene los arboles escritos en duro 
en el código fuente del main(); deben mejorar el programa creando la función o las 
funciones necesarias para que los datos de los nodos para armar el árbol sean 
introducidos por el usuario desde el teclado. */

#include <iostream>
#include <string>


typedef std::string Tipoelemento;
//typedef int Tipoelemento;
class Nodo
{
protected:
    Tipoelemento dato;
    Nodo* izdo;
    Nodo* dcho;
public:
    Nodo(Tipoelemento valor)
    {
        dato = valor;
        izdo = dcho = NULL;
    }
    Nodo(Nodo* ramaIzdo, Tipoelemento valor, Nodo* ramaDcho)
    {
        dato = valor;
        izdo = ramaIzdo;
        dcho = ramaDcho;
    }
    // operaciones de acceso
    Tipoelemento valorNodo() { return dato; }
    Nodo* subarbolIzdo() { return izdo; }
    Nodo* subarbolDcho() { return dcho; }
    // operaciones de modificación
    void nuevoValor(Tipoelemento d) { dato = d; }
    void ramaIzdo(Nodo* n) { izdo = n; }
    void ramaDcho(Nodo* n) { dcho = n; }
    void visitar()
    {
        std::cout << dato << " \n";
    }
};

class ArbolBinario 
{
protected:
    Nodo* raiz;
public:
    ArbolBinario()
    {
        raiz = NULL;
    }
    ArbolBinario(Nodo* r)
    {
        raiz = r;
    }
    void Praiz(Nodo* r)
    {
        raiz = r;
    }
    Nodo* Oraiz()
    {
        return raiz;
    }
    Nodo raizArbol()
    {
        if (raiz)
            return *raiz;
        else
            throw " arbol vacio";
        std::cout << " arbol vacio";
    }
    bool esVacio()
    {
        return raiz == NULL;
        std::cout << " arbol vacio";
    }
    Nodo* hijoIzdo()
    {
        if (raiz)
            return raiz->subarbolIzdo();
        else
            throw " arbol vacio";
        std::cout << " arbol vacio";
    }
    Nodo* hijoDcho()
    {
        if (raiz)
            return raiz->subarbolDcho();
        else
            throw " arbol vacio";
        std::cout << " arbol vacio";
    }
    Nodo* nuevoArbol(Nodo* ramaIzqda, Tipoelemento dato, Nodo* ramaDrcha)
    {
        return new Nodo(ramaIzqda, dato, ramaDrcha);
    }
    // inserta un valor: menores a la izquierda, mayores a la derecha
    void insertar(Tipoelemento valor)
    {
        insertar(raiz, valor);
    }
    // recorrido en preorden
    void preorden()
    {
        preorden(raiz);
    }
    // recorrido en inorden
    void inorden()
    {
        inorden(raiz);
    }
    // recorrido en postorden
    void postorden()
    {
        postorden(raiz);
    }
private:
    void insertar(Nodo*& r, Tipoelemento valor)
    {
        if (r == NULL)
        {
            r = new Nodo(valor);
            return;
        }
        Nodo* actual = r;
        while (true)
        {
            if (valor < actual->valorNodo())
            {
                if (actual->subarbolIzdo() == NULL)
                {
                    actual->ramaIzdo(new Nodo(valor));
                    return;
                }
                actual = actual->subarbolIzdo();
            }
            else
            {
                if (actual->subarbolDcho() == NULL)
                {
                    actual->ramaDcho(new Nodo(valor));
                    return;
                }
                actual = actual->subarbolDcho();
            }
        }
    }
    void preorden(Nodo* r)
    {
        if (r != NULL)
        {
            r->visitar();
            preorden(r->subarbolIzdo());
            preorden(r->subarbolDcho());
        }
    }
    void inorden(Nodo* r)
    {
        if (r != NULL)
        {
            inorden(r->subarbolIzdo());
            r->visitar();
            inorden(r->subarbolDcho());
        }
    }
    void postorden(Nodo* r)
    {
        if (r != NULL)
        {
            postorden(r->subarbolIzdo());
            postorden(r->subarbolDcho());
            r->visitar();
        }
    }
};

/* Menu
---------------------------------------------------------------------*/
void menu()
{
    std::cout << "\n\tTRABAJANDO CON ARBOLES:\n\n";
    std::cout << "1.- Crear el arbol" << std::endl;
    std::cout << "2.- Insertar nodos en el arbol" << std::endl;
    std::cout << "3.- Mostrar el arbol" << std::endl;
    std::cout << "4.- Esta vacio el arbol?" << std::endl;
    std::cout << "5.- Cual es la raiz del arbol?" << std::endl;
    std::cout << "0.- Salir" << std::endl;
    std::cout << "\n INGRESE OPCION: ";
}

int main()
{
    int op;
    ArbolBinario a;

    do
    {
        menu();
        std::cin >> op;

        switch (op)
        {
        case 1:
        {
            Tipoelemento valor;
            int cantidad;
            std::cout << "Cuantos elementos vas a ingresar? ";
            std::cin >> cantidad;
            for (int i = 1; i <= cantidad; i++)
            {
                std::cout << "Elemento " << i << ": ";
                std::cin >> valor;
                a.insertar(valor);
            }
            std::cout << "Hemos creado el arbol\n";
            break;
        }
        case 2:
        {
            Tipoelemento valor;
            std::cout << "Que elemento deseas insertar? ";
            std::cin >> valor;
            a.insertar(valor);
            std::cout << "Elemento insertado!\n";
            break;
        }
        case 3:
        {
            if (a.esVacio())
            {
                std::cout << "El arbol esta vacio\n";
                break;
            }
            int orden;
            std::cout << "Como deseas recorrer el arbol?\n";
            std::cout << "  1.- Preorden\n";
            std::cout << "  2.- Inorden\n";
            std::cout << "  3.- Postorden\n";
            std::cout << "Opcion: ";
            std::cin >> orden;
            if (orden == 1)
            {
                std::cout << "Recorrido en preorden:\n";
                a.preorden();
            }
            else if (orden == 2)
            {
                std::cout << "Recorrido en inorden:\n";
                a.inorden();
            }
            else if (orden == 3)
            {
                std::cout << "Recorrido en postorden:\n";
                a.postorden();
            }
            else
            {
                std::cout << "Opcion invalida\n";
            }
            break;
        }
        case 4:
            if (a.esVacio())
                std::cout << "El arbol esta vacio\n";
            else
                std::cout << "El arbol no esta vacio\n";
            break;
        case 5:
            if (a.esVacio())
                std::cout << "El arbol esta vacio\n";
            else
            {
                std::cout << "La raiz del arbol es:\n";
                a.raizArbol().visitar();
            }
            break;
        }

        std::cout << std::endl;
    } while (op != 0);

    return 0;
}
