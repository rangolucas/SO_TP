#ifndef CHM_CPP
#define CHM_CPP

#include <iostream>
#include <pthread.h>
#include <thread>

#include "HashMapConcurrente.hpp"

using namespace std;

mutex mutex_entradas[HashMapConcurrente::cantLetras];
mutex mutex_claves;
mutex mutex_maximo;
hashMapPair* maximo_global;
atomic<uint> proxima_entrada;

// Lightswitch *clavesSwitch;
// Lightswitch *incrementarSwitch;
// mutex noIncrementar;

HashMapConcurrente::HashMapConcurrente() {

    for (uint i = 0; i < HashMapConcurrente::cantLetras; i++)
        tabla[i] = new ListaAtomica<hashMapPair>();

    // clavesSwitch = new Lightswitch();
    // incrementarSwitch = new Lightswitch();
}

uint HashMapConcurrente::hashIndex(string clave) {
    return (uint)(clave[0] - 'a');
}

void HashMapConcurrente::incrementar(string clave) {

    uint idx = hashIndex(clave);

    ListaAtomica<hashMapPair>* lista = tabla[idx];
    lock_guard<mutex> lock(mutex_entradas[idx]);

    auto it = lista->crearIt();

    while (it.haySiguiente() and it.siguiente().first != clave) it.avanzar();

    if (not it.haySiguiente()) {

        //noIncrementar.lock();
        //incrementarSwitch->lock(mutex_claves);
        //noIncrementar.unlock();

        mutex_claves.lock();

        lista->insertar(hashMapPair(clave, 1));
        _claves.push_back(clave);
    
        mutex_claves.unlock();

        //incrementarSwitch->unlock(mutex_claves);
    } else it.siguiente().second++;
}

/*
vector<string> HashMapConcurrente::claves(){
    clavesSwitch->lock(noIncrementar);
    mutex_claves.lock();
    vector<string> res = _claves;
    mutex_claves.unlock();
    clavesSwitch->unlock(noIncrementar);
    return res;
}
*/

vector<string> HashMapConcurrente::claves() {

    mutex_claves.lock();

    vector<string> res = _claves;

    mutex_claves.unlock();
    
    return res;
}

// Que pasa si claves() es asi?
// vector<string> HashMapConcurrente::claves() {
//     return _claves;
// }
// La operacion return es atomica? Quizas no hace falta el mutex aca.

uint HashMapConcurrente::valor(string clave) {

    uint idx = hashIndex(clave);

    // si nos guardamos una copia de la lista no hace falta usar mutex, no?
    // ya tenemos la copia que no va a cambiar, y podemos dejar que se siga modificando la tabla sin problema
    ListaAtomica<hashMapPair>* lista = tabla[idx];

    lock_guard<mutex> lock(mutex_entradas[idx]);

    auto it = lista->crearIt();

    while (it.haySiguiente()) {
        if (it.siguiente().first == clave) break;
        it.avanzar();
    }

    return it.haySiguiente() ? it.siguiente().second : 0;
}

hashMapPair HashMapConcurrente::maximo() {

    hashMapPair* max = new hashMapPair();
    max->second = 0;

    for (uint idx = 0; idx < HashMapConcurrente::cantLetras; idx++) mutex_entradas[idx].lock();

    for (uint idx = 0; idx < HashMapConcurrente::cantLetras; idx++) {
        for (auto it = tabla[idx]->crearIt(); it.haySiguiente(); it.avanzar()) {
            if (it.siguiente().second > max->second) {
                max->first = it.siguiente().first;
                max->second = it.siguiente().second;
            }
        }
        mutex_entradas[idx].unlock();
    }
    return *max;
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

    for (uint i = 0; i < cantThreads; i++) threads.emplace_back([this]() { return maximoPorEntrada(); });
    for (auto &t : threads) t.join();
    
    return *maximo_global;
}

#endif