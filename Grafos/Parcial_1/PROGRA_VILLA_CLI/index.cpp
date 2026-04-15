#include "grafos.h"
#include <iostream>

using namespace std;

void menu()
{
    cout << "\n";
    cout << "  ==================================\n";
    cout << "     PROGRAMA VILLA - ZOMBIE EDITION\n";
    cout << "  ==================================\n";
    cout << "  1. Cargar mapa de la ciudad\n";
    cout << "  2. Mostrar grafo completo\n";
    cout << "  3. Buscar suministros (BFS)\n";
    cout << "  4. Ruta de rescate (DFS)\n";
    cout << "  5. Mapa de conectividad\n";
    cout << "  6. Ruta segura Dijkstra\n";
    cout << "  0. Salir\n";
    cout << "  ==================================\n";
    cout << "  Ingrese opcion: ";
}

int main()
{
    int op;
    char origen, destino;
    const char* archivo = "progra_villa_v1.txt";

    do
    {
        menu();
        cin >> op;

        switch(op)
        {
            case 1:
                cout << "\nCargando mapa: " << archivo << "\n";
                CargarGrafo(archivo);
                mostrar_grafo();
                break;

            case 2:
                cout << "\n";
                mostrar_grafo();
                break;

            case 3:
                cout << "\nBUSCAR SUMINISTROS (BFS)\n";
                cout << "Ingrese nodo inicio (Refugio R): ";
                cin >> origen;
                cout << "Ingrese nodo objetivo (Hospital H): ";
                cin >> destino;
                buscarSuministrosBFS(origen, destino);
                break;

            case 4:
                cout << "\nRUTA DE RESCATE (DFS)\n";
                cout << "Ingrese nodo norte (N): ";
                cin >> origen;
                cout << "Ingrese nodo sur (Z): ";
                cin >> destino;
                esRutaSeguraDFS(origen, destino);
                break;

            case 5:
                mapaConectividad();
                break;

            case 6:
                cout << "\nRUTA SEGURA DIJKSTRA\n";
                cout << "Ingrese nodo inicio: ";
                cin >> origen;
                cout << "Ingrese nodo objetivo: ";
                cin >> destino;
                dijkstraRuta(origen, destino);
                break;

            case 0:
                cout << "\nSaliendo...\n";
                break;

            default:
                cout << "\nOpcion no valida\n";
                break;
        }

    } while (op != 0);

    vaciar_grafo();
    return 0;
}
