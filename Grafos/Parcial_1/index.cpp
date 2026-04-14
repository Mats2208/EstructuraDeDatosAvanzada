#include "GrafosGame.h"

using namespace std;

int main(){
    int op;
    while(true){
        menu();
        cin >> op;
        switch(op){
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
            case 12:   
                warshall();
                break;
            case 13:
                floyd();
                break;
            case 14:
                CargarGrafo();
                break;
            case 15:
                guardar_grafo_h();
                break;
            case 0:
                break;
            default:
                cout << "OPCION NO VALIDA...!!!";
                break;
        }
        if(op == 0){
            break;
        }
    }

    cout << "Existe el nodo A?" << endl;
    if(buscar_nodo('A') != NULL){
        cout << "Si existe el nodo A" << endl;
    } else {
        cout << "No existe el nodo A" << endl;
    }

    cout << "El nodo esta bloqueado?" << endl;
    
}