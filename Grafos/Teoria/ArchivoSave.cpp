#include <cstdlib>
#include <iostream>
#include <fstream>
using namespace std;

struct Pulsom
{
    int hora;
    int pulsaciones;
};
int main()
{
    fstream f;
    f.open("c:\\files\\ENTRENAM.DAT", ios::out | ios::binary);
    if (!f.good())
    {
        cout << "Error de apertura ";
        exit(1);
    }
    // lectura de 30 datos del pulsómetro
    Pulsom pulsacion;
    cout << "\n--- Datos pulsómetro ---" << endl;
    for (int m = 0; m < 30; m++)
    {
        pulsacion.hora = m;
        pulsacion.pulsaciones = m*2;
            f.write(reinterpret_cast<char*>(&pulsacion), sizeof(Pulsom));
    }
    f.close();
    cout << " Lectura del archivo \n";
    f.open("c:\\files\\ENTRENAM.DAT", ios::in | ios::binary);
    //lectura de los distintos datos del pulsómetro de f
    for (int m = 0; m < 30; m++)
    {
        f.read(reinterpret_cast<char*>(&pulsacion), sizeof(Pulsom));
        cout << " Hora : " << pulsacion.hora << endl;
        cout << "Pulsaciones : " << pulsacion.pulsaciones << endl;
    }
} 