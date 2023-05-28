#ifndef CHM_CPP
#define CHM_CPP

#include <vector>
#include <iostream>
#include <fstream>
#include <pthread.h>

#include "CargarArchivos.hpp"

int cargarArchivo(HashMapConcurrente &hashMap, std::string filePath) {
    std::fstream file;
    int cant = 0;
    std::string palabraActual;

    // Abro el archivo.
    file.open(filePath, file.in);
    if (!file.is_open()) {
        std::cerr << "Error al abrir el archivo '" << filePath << "'" << std::endl;
        return -1;
    }
    while (file >> palabraActual) {
        hashMap.incrementar(palabraActual);
        cant++;
    }
    // Cierro el archivo.
    if (!file.eof()) {
        std::cerr << "Error al leer el archivo" << std::endl;
        file.close();
        return -1;
    }
    file.close();
    return cant;
}

void loadFiles(HashMapConcurrente &hashMap, std::vector<std::string> filePaths){
    uint pos;
    while((pos = nextFile.fetch_sub(1)) >= 0)
        cargarArchivo(hashMap, filePaths[pos]);
}

void cargarMultiplesArchivos(HashMapConcurrente &hashMap, unsigned int cantThreads, std::vector<std::string> filePaths) {
    vector<std::thread> threads(cantThreads);
    nextFile = filePaths.size() - 1;
    for (int i = 0; i < cantThreads; i++)
        threads.emplace_back(loadFiles, hashMap, filePaths);

    for(auto &t : threads)
        t.join();
}

#endif