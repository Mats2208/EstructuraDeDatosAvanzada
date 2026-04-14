#include "grafos.h"
#include <iostream>

int main(){
    p = NULL;
    int op;
    
    do {
        menu();
        std::cin >> op;
        
        switch (op){
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
                std::cout << "Saliendo..." << std::endl;
                break;
            default:
                std::cout << "Opcion no valida" << std::endl;
        }
        
        if (op != 0) std::cout << std::endl;
    } while (op != 0);
    
    return 0;
}