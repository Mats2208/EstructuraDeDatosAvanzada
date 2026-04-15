#include "GrafosGameV1.h"

using namespace std;

int main()
{
    int opcion;

    while (true)
    {
        menu_game_v1();
        cin >> opcion;

        if (opcion == 0)
            break;

        if (opcion == 1)
        {
            string archivo;
            cout << "Archivo a cargar: ";
            cin >> archivo;

            if (cargar_ciudad_desde_archivo_game(archivo))
                cout << "Ciudad cargada correctamente\n";
            else
                cout << "No se pudo cargar el archivo\n";
        }
        else if (opcion == 2)
        {
            mostrar_mapa_game();
        }
        else if (opcion == 3)
        {
            char nombre;
            cout << "Nodo a bloquear: ";
            cin >> nombre;

            if (bloquear_nodo_game(nombre))
                cout << "Nodo bloqueado\n";
            else
                cout << "No existe ese nodo\n";
        }
        else if (opcion == 4)
        {
            char nombre;
            cout << "Nodo a desbloquear: ";
            cin >> nombre;

            if (desbloquear_nodo_game(nombre))
                cout << "Nodo desbloqueado\n";
            else
                cout << "No existe ese nodo\n";
        }
        else if (opcion == 5)
        {
            char inicio;
            char objetivo;
            cout << "Refugio inicial: ";
            cin >> inicio;
            cout << "Hospital objetivo: ";
            cin >> objetivo;
            buscarSuministrosBFS(inicio, objetivo);
        }
        else if (opcion == 6)
        {
            char origen;
            char destino;
            cout << "Zona norte: ";
            cin >> origen;
            cout << "Zona sur: ";
            cin >> destino;
            esRutaSeguraDFS(origen, destino);
        }
        else if (opcion == 7)
        {
            mostrar_conectividad_game();
        }
        else if (opcion == 8)
        {
            char origen;
            char destino;
            cout << "Origen: ";
            cin >> origen;
            cout << "Destino: ";
            cin >> destino;
            rutaMenorPeligroDijkstra(origen, destino);
        }
        else
        {
            cout << "Opcion invalida\n";
        }
    }

    limpiar_grafo_game();
    return 0;
}
