#ifndef CHM_CPP
#define CHM_CPP

#include <iostream>
#include <pthread.h>
#include <thread>

#include "HashMapConcurrente.hpp"

std::array<std::mutex, HashMapConcurrente::cantLetras> mtx;
Lightswitch *clavesSwitch;
Lightswitch *incrementarSwitch;
std::mutex noIncrementar;
std::mutex noClaves;
std::mutex maximomtx;
hashMapPair* maximoGlobal;

std::atomic<uint> nextPos;

HashMapConcurrente::HashMapConcurrente(){
    for (uint i = 0; i < HashMapConcurrente::cantLetras; i++)
        tabla[i] = new ListaAtomica<hashMapPair>();
        
    clavesSwitch = new Lightswitch();
    incrementarSwitch = new Lightswitch();
    maximoGlobal = &{"", 0};
    nextPos = 0;
}

uint HashMapConcurrente::hashIndex(std::string clave){
    return (uint)(clave[0] - 'a');
}

void HashMapConcurrente::incrementar(std::string clave){
    uint index = hashIndex(clave);
    ListaAtomica<hashMapPair> *list = tabla[index];
    std::lock_guard<std::mutex> lock(mtx[index]);

    auto it = tabla[index]->crearIt();

    while (it.haySiguiente() && it.siguiente().first != clave) it.avanzar();

    if (!it.haySiguiente()){
        //noIncrementar.lock();
        //incrementarSwitch->lock(noClaves);
        //noIncrementar.unlock();

        noClaves.lock();

        list->insertar(hashMapPair(clave, 1));
        _claves.push_back(clave);
    
        noClaves.unlock();

        //incrementarSwitch->unlock(noClaves);
    } else {
        it.siguiente().second++;
    }
}
/*
std::vector<std::string> HashMapConcurrente::claves(){
    clavesSwitch->lock(noIncrementar);
    noClaves.lock();
    std::vector<std::string> res = _claves;
    noClaves.unlock();
    clavesSwitch->unlock(noIncrementar);
    return res;
}
*/
std::vector<std::string> HashMapConcurrente::claves(){
    noClaves.lock();

    std::vector<std::string> res = _claves;

    noClaves.unlock();
    
    return res;
}

uint HashMapConcurrente::valor(std::string clave){
    uint index = hashIndex(clave);

    ListaAtomica<hashMapPair> *list = tabla[index];

    {
        std::lock_guard<std::mutex> lock(mtx[index]);

        auto it = list->crearIt();
        int value = 0;

        while (it.haySiguiente()){
            if (it.siguiente().first == clave)
                value = it.siguiente().second;
            it.avanzar();
        }
    }    

    return value;
}

hashMapPair HashMapConcurrente::maximo(){
    hashMapPair *max = new hashMapPair();
    max->second = 0;

    for(uint index = 0; index < HashMapConcurrente::cantLetras; index++) mtx[index].lock();

    for (uint index = 0; index < HashMapConcurrente::cantLetras; index++){
        for (auto it = tabla[index]->crearIt(); it.haySiguiente(); it.avanzar()){
            if (it.siguiente().second > max->second){
                max->first = it.siguiente().first;
                max->second = it.siguiente().second;
            }
        }
        mtx[index].unlock();
    }
    return *max;
}

void calculateMaxPerRow(){
    uint pos;

    while((pos = nextPos.fetch_add(1)) < HashMapConcurrente::cantLetras){
    
        hashMapPair *max = new hashMapPair();

        mtx[pos].lock();

        for (auto it = tabla[pos]->crearIt(); it.haySiguiente(); it.avanzar()){
            if (it.siguiente().second > max->second)
                max = it.siguiente();
        }
        mtx[pos].unlock();

        maximomtx.lock();

        if(max->second > maximoGlobal->second) maximoGlobal = max;

        maximomtx.unlock();
    }
}

hashMapPair HashMapConcurrente::maximoParalelo(uint cantThreads){
    vector<std::thread> threads(cantThreads);
    for (int i = 0; i < cantThreads; i++)
        threads.emplace_back(calculateMaxPerRow);

    for(auto &t : threads)
        t.join();
    
    return maximoGlobal;
}

#endif
