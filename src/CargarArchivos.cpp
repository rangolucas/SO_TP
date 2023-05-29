#ifndef CHM_CPP
#define CHM_CPP

#include <vector>
#include <iostream>
#include <fstream>
#include <thread>

#include "CargarArchivos.hpp"

using namespace std;

int cargarArchivo(HashMapConcurrente& hashMap, string filePath) {

    fstream file;
    int cant = 0;
    string palabraActual;

    // Abro el archivo.
    file.open(filePath, file.in);
    if (not file.is_open()) {
        cerr << "Error al abrir el archivo '" << filePath << "'" << endl;
        return -1;
    }
    while (file >> palabraActual) {
        hashMap.incrementar(palabraActual);
        cant++;
    }

    // Cierro el archivo.
    if (not file.eof()) {
        cerr << "Error al leer el archivo" << endl;
        file.close();
        return -1;
    }
    file.close();
    
    return cant;
}

void cargarMultiplesArchivos(HashMapConcurrente& hashMap, uint cantThreads, vector<string> filePaths) {

    atomic<uint> proximo_archivo(0);

    vector<thread> threads;

    for (int i = 0; i < cantThreads; i++)
        threads.emplace_back([&hashMap, &filePaths, &proximo_archivo]() {
            uint archivo = proximo_archivo.fetch_add(1);

            while (archivo < filePaths.size()) {
                cargarArchivo(hashMap, filePaths[archivo]);
                archivo = proximo_archivo.fetch_add(1);
            }
        });

    for (auto &t : threads) t.join();
}

#endif