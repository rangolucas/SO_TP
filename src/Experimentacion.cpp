#include <vector>
#include <string>
#include <iostream>
#include <fstream>

#include "HashMapConcurrente.hpp"
#include "CargarArchivos.hpp"

void medirTiempoSecuencial(const vector<string> &archivos, ofstream &output_file);

void medirTiempoConcurrente(vector<string> &archivos, ofstream &output_file);

void experimento1(vector<string> &archivos);

void experimento2(vector<string> &archivos, uint cantThreads);

using namespace std;

double medirTiempo(const function<void()> &f) {
    auto start = chrono::high_resolution_clock::now();
    f();
    auto stop = chrono::high_resolution_clock::now();
    chrono::duration<double> diff = stop - start;
    return diff.count();
}

int main() {
    int cantArchivos = 1600;
    vector<string> archivos(cantArchivos);
    for (int i = 0; i < cantArchivos; i++) archivos[i] = "experimentacion/inputs/archivo-" + to_string(i);

    vector<string> archivosExp1(archivos.begin(), archivos.end() - cantArchivos / 2);
    experimento1(archivosExp1);
    experimento2(archivos, 1);
    experimento2(archivos, 4);
    experimento2(archivos, 16);
    experimento2(archivos, 64);
    experimento2(archivos, 1024);

    return 0;
}

void experimento1(vector<string> &archivos) {
    ofstream output_file;
    output_file.open("experimentacion/experimento-1/outputs/tiempos-" + to_string(archivos.size()) + ".csv");
    string csvHeader = "cant_threads,tiempo_archivos,tiempo_maximo\n";
    output_file << csvHeader;
    cout << csvHeader;

    medirTiempoSecuencial(archivos, output_file);
    medirTiempoConcurrente(archivos, output_file);

    output_file.close();
}

void experimento2(vector<string> &archivos, uint cantThreads) {
    ofstream output_file;
    output_file.open("experimentacion/experimento-2/outputs/tiempos-" + to_string(archivos.size()) + "-" + to_string(cantThreads) + ".csv");
    string csvHeader = "cant_archivos,tiempo_archivos,tiempo_maximo\n";
    output_file << csvHeader;
    cout << csvHeader;

    for (int cantArchivos = 100; cantArchivos <= archivos.size(); cantArchivos += 100) {
        HashMapConcurrente hashMap;

        vector<string> archivosIteracion(archivos.begin(), archivos.begin() + cantArchivos);
        double tiempoArchivos = medirTiempo([&hashMap, cantThreads, &archivosIteracion]() {
            cargarMultiplesArchivos(hashMap, cantThreads, archivosIteracion);
        });
        double tiempoMaximo = medirTiempo([&hashMap, cantThreads]() {
            hashMap.maximoParalelo(cantThreads);
        });

        output_file << cantArchivos << "," << tiempoArchivos << "," << tiempoMaximo << endl;
        cout << cantArchivos << "," << tiempoArchivos << "," << tiempoMaximo << endl;
    }

    output_file.close();
}

void medirTiempoConcurrente(vector<string> &archivos, ofstream &output_file) {
    int maxThreads = 1024;
    for (int cantThreads = 2; cantThreads <= maxThreads; cantThreads *= 2) {
        HashMapConcurrente hashMap;
        double tiempoArchivos = medirTiempo([&hashMap, cantThreads, &archivos]() {
            cargarMultiplesArchivos(hashMap, cantThreads, archivos);
        });
        double tiempoMaximo = medirTiempo([&hashMap, cantThreads]() {
            hashMap.maximoParalelo(cantThreads);
        });
        output_file << cantThreads << "," << tiempoArchivos << "," << tiempoMaximo << endl;
        cout << cantThreads << "," << tiempoArchivos << "," << tiempoMaximo << endl;
    }
}

void medirTiempoSecuencial(const vector<string> &archivos, ofstream &output_file) {
    HashMapConcurrente hashMap;
    double tiempoCargaSecuencial = medirTiempo([&hashMap, &archivos]() {
        cargarMultiplesArchivosSecuencial(hashMap, archivos);
    });
    double tiempoMaximoSecuencial = medirTiempo([&hashMap]() {
        hashMap.maximo();
    });
    output_file << "1," << tiempoCargaSecuencial << "," << tiempoMaximoSecuencial << endl;
    cout << "1," << tiempoCargaSecuencial << "," << tiempoMaximoSecuencial << endl;
}
