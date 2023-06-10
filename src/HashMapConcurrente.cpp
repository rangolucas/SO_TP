#ifndef CHM_CPP
#define CHM_CPP

#include <iostream>
#include <thread>

#include "HashMapConcurrente.hpp"

using namespace std;

mutex mutex_entradas[HashMapConcurrente::cantLetras];
mutex mutex_claves;
mutex mutex_maximo;
hashMapPair* maximo_global;
atomic<uint> proxima_entrada;

HashMapConcurrente::HashMapConcurrente() {

    for (auto & bucket : tabla) bucket = new ListaAtomica<hashMapPair>();

}

uint HashMapConcurrente::hashIndex(string clave) {
    return (uint)(clave[0] - 'a');
}

void HashMapConcurrente::incrementar(string clave) {

    uint idx = hashIndex(clave);

    ListaAtomica<hashMapPair>* lista = tabla[idx];
    lock_guard<mutex> lock_bucket(mutex_entradas[idx]);

    auto it = lista->crearIt();

    while (it.haySiguiente() and it.siguiente().first != clave) it.avanzar();

    if (not it.haySiguiente()) {

        lock_guard<mutex> lock_claves(mutex_claves);
        lista->insertar(hashMapPair(clave, 1));
        _claves.push_back(clave);
    
    } else it.siguiente().second++;
}

vector<string> HashMapConcurrente::claves() {
    lock_guard<mutex> lock(mutex_claves);
    return _claves;
}

uint HashMapConcurrente::valor(string clave) {

    uint idx = hashIndex(clave);

    ListaAtomica<hashMapPair>* lista = tabla[idx];

    lock_guard<mutex> lock(mutex_entradas[idx]);

    auto it = lista->crearIt();

    while (it.haySiguiente() and it.siguiente().first != clave) it.avanzar();

    return it.haySiguiente() ? it.siguiente().second : 0;
}

hashMapPair HashMapConcurrente::maximo() {

    hashMapPair max;
    max.second = 0;

    for (auto & mutex_entrada : mutex_entradas) mutex_entrada.lock();

    for (uint idx = 0; idx < HashMapConcurrente::cantLetras; idx++) {
        for (auto it = tabla[idx]->crearIt(); it.haySiguiente(); it.avanzar())
            if (it.siguiente().second > max.second) max = it.siguiente();
        mutex_entradas[idx].unlock();
    }
    return max;
}

void HashMapConcurrente::maximoPorEntrada() {

    uint entrada;

    while ((entrada = proxima_entrada.fetch_add(1)) < HashMapConcurrente::cantLetras) {
    
        hashMapPair* max = new hashMapPair();

        mutex_entradas[entrada].lock();

        for (auto it = tabla[entrada]->crearIt(); it.haySiguiente(); it.avanzar())
            if (it.siguiente().second > max->second) max = &it.siguiente();

        mutex_entradas[entrada].unlock();

        mutex_maximo.lock();

        if (max->second > maximo_global->second) maximo_global = max;

        mutex_maximo.unlock();
    }
}


hashMapPair HashMapConcurrente::maximoParalelo(uint cantThreads) {

    maximo_global = new hashMapPair();
    maximo_global->second = 0;
    proxima_entrada = 0;
    
    vector<thread> threads;

    for (uint i = 0; i < cantThreads; i++) threads.emplace_back(&HashMapConcurrente::maximoPorEntrada, this);
    for (auto &t : threads) t.join();
    
    return *maximo_global;
}

#endif