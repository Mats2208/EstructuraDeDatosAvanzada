#include <iostream>
#include <fstream>
#include <string>
#include <sstream>
#include <cstdlib>
#include "grafos.h"

using namespace std;

void mostrarMenu();
void guardar_grafo();
void cargar_grafo();

int main(){
    p = NULL;
    int op;
    
    do {
        mostrarMenu();
        
        if(!(cin >> op)){ 
            cin.clear();
            cin.ignore(10000, '\n');
            cout << "Opcion no valida" << endl;
            continue;
        }
        
        switch (op){
            case 1:
                insertar_nodo();
                break;
            case 2:
                insertar_arista();
                break;
            case 3:
                mostrar_grafo();
                break;
            case 4:
                guardar_grafo();
                break;
            case 5:
                cargar_grafo();
                break;
            case 6:
                cout << "Salir" << endl;
                break;
            case 7:
                dijkstra();
                break;
            case 8:
                warshall();
                break;
            case 9:
                floyd();
                break;
            default:
                cout << "Opcion no valida" << endl;
        }
    } while (op != 6);
    
    return 0;
}

void mostrarMenu(){
    cout << "\nIngrese la funcion a realizar"<<endl;
    cout << "1. Ingresar nodo" <<endl;
    cout << "2. Ingresar arista" << endl;
    cout << "3. Mostrar grafo" << endl;
    cout << "4. Guardar grafo en archivo" << endl;
    cout << "5. Cargar un grafo desde archivo" << endl;
    cout << "7. Dijkstra" << endl;
    cout << "8. Warshall" << endl;
    cout << "9. Floyd" << endl;
    cout << "6. Salir" << endl;
    cout << "Opcion: ";
}

void guardar_grafo(){
    string nombre_archivo;
    string ruta_carga = "carga";
    ofstream archivo;
    
    // Crear la carpeta "carga" si no existe (compatible con Windows, Mac y Linux)
    #ifdef _WIN32
        system("if not exist carga mkdir carga");
    #else
        system("mkdir -p carga");
    #endif
    
    cout << "Ingrese nombre del archivo (sin extension): ";
    cin >> nombre_archivo;
    
    // Construir la ruta compatible con todas las plataformas
    string ruta_completa = ruta_carga + "/" + nombre_archivo + ".txt";
    
    archivo.open(ruta_completa, ios::out);
    
    if (!archivo.good()) {
        cout << "Error al abrir archivo" << endl;
        return;
    }
    
    Tnodo ptr;
    Tarista ar;
    ptr = p;

    while (ptr != NULL)
    {
        archivo << " " << ptr->nombre << "|";
        if (ptr->ady != NULL)
        {
            ar = ptr->ady;
            while (ar != NULL)
            {
                // Verificar si existe arista inversa (es bidireccional)
                int bidireccional = existe_arista(ar->destino, ptr) ? 1 : 0;
                archivo << " " << ar->destino->nombre << ":" << ar->peso << ":" << bidireccional;
                ar = ar->sgte;
            }
        }
        ptr = ptr->sgte;
        archivo << "\n";
    }
    
    archivo.close();
    cout << "Grafo guardado en: " << ruta_completa << endl;
};

void cargar_grafo(){
    string nombre_archivo;
    string ruta_carga = "carga";
    ifstream archivo;
    
    cout << "Ingrese nombre del archivo a cargar (sin extension): ";
    cin >> nombre_archivo;
    
    string ruta_completa = ruta_carga + "/" + nombre_archivo + ".txt";
    
    archivo.open(ruta_completa);
    
    if (!archivo.good()) {
        cout << "Error: No se pudo abrir el archivo " << ruta_completa << endl;
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
    
    archivo.close();
    cout << "Grafo cargado correctamente desde: " << ruta_completa << endl;
    cout << "Mostrando el grafo cargado:" << endl;
    mostrar_grafo();
}