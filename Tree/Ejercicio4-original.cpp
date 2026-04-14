/*
Código generado con Claude.ai 
Limpio sin recurrir a bibliotecas externas
22/02/2025
*/

#include <iostream>
using namespace std;

class NodoAVL {
public:
    int valor;
    NodoAVL* izquierdo;
    NodoAVL* derecho;
    int altura;

    NodoAVL(int val) {
        valor = val;
        izquierdo = nullptr;
        derecho = nullptr;
        altura = 1;
    }
};

class ArbolAVL {
private:
    NodoAVL* raiz;

    // Función para obtener el máximo de dos números
    int maximo(int a, int b) {
        return (a > b) ? a : b;
    }

    // Obtener altura del nodo
    int getAltura(NodoAVL* nodo) {
        if (nodo == nullptr) return 0;
        return nodo->altura;
    }

    // Obtener factor de balance
    int getBalance(NodoAVL* nodo) {
        if (nodo == nullptr) return 0;
        return getAltura(nodo->izquierdo) - getAltura(nodo->derecho);
    }

    // Rotación derecha
    NodoAVL* rotacionDerecha(NodoAVL* y) {
        NodoAVL* x = y->izquierdo;
        NodoAVL* T2 = x->derecho;

        x->derecho = y;
        y->izquierdo = T2;

        y->altura = maximo(getAltura(y->izquierdo), getAltura(y->derecho)) + 1;
        x->altura = maximo(getAltura(x->izquierdo), getAltura(x->derecho)) + 1;

        return x;
    }

    // Rotación izquierda
    NodoAVL* rotacionIzquierda(NodoAVL* x) {
        NodoAVL* y = x->derecho;
        NodoAVL* T2 = y->izquierdo;

        y->izquierdo = x;
        x->derecho = T2;

        x->altura = maximo(getAltura(x->izquierdo), getAltura(x->derecho)) + 1;
        y->altura = maximo(getAltura(y->izquierdo), getAltura(y->derecho)) + 1;

        return y;
    }

    // Función para contar dígitos de un número
    int contarDigitos(int num) {
        if (num == 0) return 1;
        int count = 0;
        if (num < 0) {
            count = 1;  // Para el signo negativo
            num = -num;
        }
        while (num > 0) {
            count++;
            num /= 10;
        }
        return count;
    }

    // Función para imprimir espacios
    void imprimirEspacios(int n) {
        for (int i = 0; i < n; i++) {
            cout << " ";
        }
    }

    // Insertar nodo recursivamente
    NodoAVL* insertarNodo(NodoAVL* nodo, int valor) {
        if (nodo == nullptr)
            return new NodoAVL(valor);

        if (valor < nodo->valor)
            nodo->izquierdo = insertarNodo(nodo->izquierdo, valor);
        else if (valor > nodo->valor)
            nodo->derecho = insertarNodo(nodo->derecho, valor);
        else
            return nodo;

        nodo->altura = maximo(getAltura(nodo->izquierdo), getAltura(nodo->derecho)) + 1;

        int balance = getBalance(nodo);

        // Casos de rotación
        if (balance > 1 && valor < nodo->izquierdo->valor)
            return rotacionDerecha(nodo);

        if (balance < -1 && valor > nodo->derecho->valor)
            return rotacionIzquierda(nodo);

        if (balance > 1 && valor > nodo->izquierdo->valor) {
            nodo->izquierdo = rotacionIzquierda(nodo->izquierdo);
            return rotacionDerecha(nodo);
        }

        if (balance < -1 && valor < nodo->derecho->valor) {
            nodo->derecho = rotacionDerecha(nodo->derecho);
            return rotacionIzquierda(nodo);
        }

        return nodo;
    }

    // Recorrido inorden
    void inordenRec(NodoAVL* nodo) {
        if (nodo != nullptr) {
            inordenRec(nodo->izquierdo);
            cout << nodo->valor << " ";
            inordenRec(nodo->derecho);
        }
    }

    // Funciones auxiliares para mostrar el árbol gráficamente
    int getAlturaTotalArbol(NodoAVL* nodo) {
        if (nodo == nullptr) return 0;
        return 1 + maximo(getAlturaTotalArbol(nodo->izquierdo), 
                         getAlturaTotalArbol(nodo->derecho));
    }

    void imprimirNivel(NodoAVL* raiz, int nivel, int espaciado) {
        if (raiz == nullptr) {
            imprimirEspacios(espaciado);
            cout << "·";
            return;
        }
        if (nivel == 1) {
            imprimirEspacios(espaciado - contarDigitos(raiz->valor)/2);
            cout << raiz->valor;
            imprimirEspacios(espaciado - contarDigitos(raiz->valor)/2);
        }
        else if (nivel > 1) {
            imprimirNivel(raiz->izquierdo, nivel-1, espaciado);
            imprimirNivel(raiz->derecho, nivel-1, espaciado);
        }
    }

public:
    ArbolAVL() {
        raiz = nullptr;
    }

    void insertar(int valor) {
        raiz = insertarNodo(raiz, valor);
    }

    void inorden() {
        inordenRec(raiz);
        cout << "\n";
    }

    // Función para mostrar el árbol gráficamente
    void mostrarArbol() {
        if (raiz == nullptr) {
            cout << "Árbol vacío\n";
            return;
        }

        int altura = getAlturaTotalArbol(raiz);
        int espaciado = 4;
        for (int i = 0; i < altura - 1; i++) {
            espaciado *= 2;
        }

        for (int i = 1; i <= altura; i++) {
            imprimirNivel(raiz, i, espaciado/(1 << (i-1)));
            cout << "\n\n";
        }
    }
};

int main() {
    ArbolAVL arbol;
    
    // Insertamos algunos valores de ejemplo
    arbol.insertar(10);
    arbol.insertar(20);
    arbol.insertar(30);
    arbol.insertar(5);
    arbol.insertar(15);
    
    cout << "Recorrido inorden: ";
    arbol.inorden();
    
    cout << "\nRepresentación gráfica del árbol:\n\n";
    arbol.mostrarArbol();
    
    return 0;
}