#ifndef HMC_ARCHIVOS_HPP
#define HMC_ARCHIVOS_HPP

#include <ctype.h>
#include <vector>
#include <string>
#include <pthread.h>

#include "HashMapConcurrente.hpp"

using namespace std;

int cargarArchivo(HashMapConcurrente &hashMap, string filePath);

void cargarMultiplesArchivos(HashMapConcurrente &hashMap, uint cantThreads, vector<string> filePaths);

uint nextFile;

#endif /* HMC_ARCHIVOS_HPP */