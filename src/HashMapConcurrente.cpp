#ifndef CHM_CPP
#define CHM_CPP

#include <iostream>
#include <pthread.h>
#include <thread>

#include "HashMapConcurrente.hpp"

using namespace std;

array<mutex, HashMapConcurrente::cantLetras> mtx;
Lightswitch *clavesSwitch;
Lightswitch *incrementarSwitch;
mutex noIncrementar;
mutex noClaves;
mutex maximomtx;
hashMapPair* maximoGlobal;

atomic<uint> nextPos;

HashMapConcurrente::HashMapConcurrente() {

    for (uint i = 0; i < HashMapConcurrente::cantLetras; i++)
        tabla[i] = new ListaAtomica<hashMapPair>();

    clavesSwitch = new Lightswitch();
    incrementarSwitch = new Lightswitch();
    maximoGlobal = &{"", 0};
    nextPos = 0;
}

uint HashMapConcurrente::hashIndex(string clave) {
    return (uint)(clave[0] - 'a');
}

void HashMapConcurrente::incrementar(string clave) {

    uint index = hashIndex(clave);
    ListaAtomica<hashMapPair>* list = tabla[index];
    lock_guard<mutex> lock(mtx[index]);

    auto it = list->crearIt();

    while (it.haySiguiente() and it.siguiente().first != clave) it.avanzar();

    if (not it.haySiguiente()) {
        //noIncrementar.lock();
        //incrementarSwitch->lock(noClaves);
        //noIncrementar.unlock();

        noClaves.lock();

        list->insertar(hashMapPair(clave, 1));
        _claves.push_back(clave);
    
        noClaves.unlock();

        //incrementarSwitch->unlock(noClaves);
    } else it.siguiente().second++;
}

/*
vector<string> HashMapConcurrente::claves(){
    clavesSwitch->lock(noIncrementar);
    noClaves.lock();
    vector<string> res = _claves;
    noClaves.unlock();
    clavesSwitch->unlock(noIncrementar);
    return res;
}
*/

vector<string> HashMapConcurrente::claves() {

    noClaves.lock();

    vector<string> res = _claves;

    noClaves.unlock();
    
    return res;
}

uint HashMapConcurrente::valor(string clave) {

    uint index = hashIndex(clave);

    ListaAtomica<hashMapPair>* list = tabla[index];

    {
        lock_guard<mutex> lock(mtx[index]);

        auto it = list->crearIt();
        int value = 0;

        while (it.haySiguiente()) {
            if (it.siguiente().first == clave) value = it.siguiente().second;
            it.avanzar();
        }
    }
    return value;
}

hashMapPair HashMapConcurrente::maximo() {

    hashMapPair* max = new hashMapPair();
    max->second = 0;

    for (uint idx = 0; idx < HashMapConcurrente::cantLetras; idx++) mtx[idx].lock();

    for (uint idx = 0; idx < HashMapConcurrente::cantLetras; idx++) {
        for (auto it = tabla[idx]->crearIt(); it.haySiguiente(); it.avanzar()) {
            if (it.siguiente().second > max->second) {
                max->first = it.siguiente().first;
                max->second = it.siguiente().second;
            }
        }
        mtx[idx].unlock();
    }
    return *max;
}

void calculateMaxPerRow() {

    uint pos;

    while ((pos = nextPos.fetch_add(1)) < HashMapConcurrente::cantLetras) {
    
        hashMapPair* max = new hashMapPair();

        mtx[pos].lock();

        for (auto it = tabla[pos]->crearIt(); it.haySiguiente(); it.avanzar())
            if (it.siguiente().second > max->second) max = it.siguiente();

        mtx[pos].unlock();

        maximomtx.lock();

        if (max->second > maximoGlobal->second) maximoGlobal = max;

        maximomtx.unlock();
    }
}

hashMapPair HashMapConcurrente::maximoParalelo(uint cantThreads) {
    
    vector<thread> threads(cantThreads);

    for (int i = 0; i < cantThreads; i++) threads.emplace_back(calculateMaxPerRow);
    for(auto &t : threads) t.join();
    
    return maximoGlobal;
}

#endif