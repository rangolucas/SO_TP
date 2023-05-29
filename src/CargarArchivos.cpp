#ifndef CHM_CPP
#define CHM_CPP

#include <vector>
#include <iostream>
#include <fstream>
#include <pthread.h>

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

void loadFiles(HashMapConcurrente& hashMap, vector<string> filePaths) {

    uint pos;

    while((pos = nextFile.fetch_sub(1)) >= 0) cargarArchivo(hashMap, filePaths[pos]);
}

void cargarMultiplesArchivos(HashMapConcurrente& hashMap, uint cantThreads, vector<string> filePaths) {

    vector<thread> threads(cantThreads);
    nextFile = filePaths.size() - 1;

    for (int i = 0; i < cantThreads; i++) threads.emplace_back(loadFiles, hashMap, filePaths);

    for(auto &t : threads) t.join();
}

#endif