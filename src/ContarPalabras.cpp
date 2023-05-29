#include <iostream>

#include "HashMapConcurrente.hpp"
#include "CargarArchivos.hpp"

using namespace std;

int main(int argc, char **argv) {

    if (argc < 4) {
        cout << "Error: faltan argumentos." << endl;
        cout << endl;
        cout << "Modo de uso: " << argv[0] << " <threads_lectura> <threads_maximo>" << endl;
        cout << "    " << "<archivo1> [<archivo2>...]" << endl;
        cout << endl;
        cout << "    threads_lectura: " << "Cantidad de threads a usar para leer archivos." << endl;
        cout << "    threads_maximo: " << "Cantidad de threads a usar para computar máximo." << endl;
        cout << "    archivo1, archivo2...: " << "Archivos a procesar." << endl;
        return 1;
    }

    int cantThreadsLectura = stoi(argv[1]);
    int cantThreadsMaximo = stoi(argv[2]);

    vector<string> filePaths = {};

    for (int i = 3; i < argc; i++) filePaths.push_back(argv[i]);

    HashMapConcurrente hashMap = HashMapConcurrente();
    cargarMultiplesArchivos(hashMap, cantThreadsLectura, filePaths);
    auto maximo = hashMap.maximoParalelo(cantThreadsMaximo);

    cout << maximo.first << " " << maximo.second << endl;

    return 0;
}