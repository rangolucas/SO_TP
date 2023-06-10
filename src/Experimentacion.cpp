#include <vector>
#include <string>
#include <iostream>
#include <fstream>

#include "HashMapConcurrente.hpp"
#include "CargarArchivos.hpp"

void medirTiempoSecuencial(const vector<string> &archivos, ofstream &output_file);

void medirTiempoConcurrente(vector<string> &archivos, ofstream &output_file);

using namespace std;

double medirTiempo(const function<void()> &f) {
    auto start = chrono::high_resolution_clock::now();
    f();
    auto stop = chrono::high_resolution_clock::now();
    chrono::duration<double> diff = stop - start;
    return diff.count();
}

int main() {
    int cantArchivos = 800;
    vector<string> archivos(cantArchivos);
    for (int i = 0; i < cantArchivos; i++) archivos[i] = "experimentacion/inputs/archivo-" + to_string(i);

    ofstream output_file;
    output_file.open("experimentacion/outputs/tiempos-" + to_string(cantArchivos) + ".csv");
    string csvHeader = "cant_threads,tiempo_archivos,tiempo_maximo\n";
    output_file << csvHeader;
    cout << csvHeader;

    medirTiempoSecuencial(archivos, output_file);
    medirTiempoConcurrente(archivos, output_file);

    output_file.close();
    return 0;
}

void medirTiempoConcurrente(vector<string> &archivos, ofstream &output_file) {
    int maxThreads = 1024;
    for (int cantThreads = 2; cantThreads <= maxThreads; cantThreads *= 2) {
        HashMapConcurrente hashMap;
        double tiempoArchivos = medirTiempo([&hashMap, cantThreads, &archivos]() { cargarMultiplesArchivos(hashMap, cantThreads, archivos); });
        double tiempoMaximo = medirTiempo([&hashMap, cantThreads]() { hashMap.maximoParalelo(cantThreads); });
        output_file << cantThreads << "," << tiempoArchivos << "," << tiempoMaximo << endl;
        cout << cantThreads << "," << tiempoArchivos << "," << tiempoMaximo << endl;
    }
}

void medirTiempoSecuencial(const vector<string> &archivos, ofstream &output_file) {
    HashMapConcurrente hashMap;
    double tiempoCargaSecuencial = medirTiempo([&hashMap, &archivos]() { cargarMultiplesArchivosSecuencial(hashMap, archivos); });
    double tiempoMaximoSecuencial = medirTiempo([&hashMap]() { hashMap.maximo(); });
    output_file << "1," << tiempoCargaSecuencial << "," << tiempoMaximoSecuencial << endl;
    cout << "1," << tiempoCargaSecuencial << "," << tiempoMaximoSecuencial << endl;
}
