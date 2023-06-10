double medirCargarArchivos(HashMapConcurrente& hashMap, uint cantThreads, vector<string> filePaths) {
    auto start = chrono::high_resolution_clock::now();

    cargarMultiplesArchivos(hashMap, cantThreads, filePaths);

    auto stop = chrono::high_resolution_clock::now();
    chrono::duration<double> diff = stop - start;
    return diff.count();
}

double medirComputarMaximo(HashMapConcurrente& hashMap, uint cantThreads) {
    auto start = chrono::high_resolution_clock::now();

    hashMap.maximoParalelo(cantThreads);

    auto stop = chrono::high_resolution_clock::now();
    chrono::duration<double> diff = stop - start;
    return diff.count();
}

int main() {
    int cantArchivos = 5000;
    vector<string> archivos(cantArchivos);
    for (int i = 0; i < cantArchivos; i++) archivos[i] = "archivo-" + to_string(i);

    ofstream output_file;
    output_file.open("tiempos-" + to_string(cantArchivos) + ".csv");
    output_file << "cant_threads,tiempo_archivos,tiempo_maximo,cant_archivos\n";

    int cantExperimentos = 5000;

    for (int cantThreads = 2; cantThreads < cantExperimentos; cantThreads *= 2) {
        HashMapConcurrente hashMap;
        double tiempoArchivos = medirCargarArchivos(hashMap, cantThreads, archivos);
        double tiempoMaximo = medirComputarMaximo(hashMap, cantThreads);
        output_file << cantThreads << "," << tiempoArchivos << "," << tiempoMaximo << endl;
    }
    output_file.close();
    return 0;
}
