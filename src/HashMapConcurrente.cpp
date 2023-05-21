#ifndef CHM_CPP
#define CHM_CPP

#include <iostream>
#include <fstream>
#include <pthread.h>

#include "HashMapConcurrente.hpp"

std::array<std::mutex, HashMapConcurrente::cantLetras> mtx;
Lightswitch* clavesSwitch; Lightswitch* incrementarSwitch;
std::mutex noIncrementar; std::mutex noClaves;

HashMapConcurrente::HashMapConcurrente() {
    for (unsigned int i = 0; i < HashMapConcurrente::cantLetras; i++) {
        tabla[i] = new ListaAtomica<hashMapPair>();
    }
    clavesSwitch = new Lightswitch();
    incrementarSwitch = new Lightswitch();
}

unsigned int HashMapConcurrente::hashIndex(std::string clave) {
    return (unsigned int)(clave[0] - 'a');
}

void HashMapConcurrente::incrementar(std::string clave) {
    unsigned int index = hashIndex(clave);
    ListaAtomica<hashMapPair> * list = tabla[index];
    std::lock_guard<std::mutex> lock(mtx[index]);

    auto it = tabla[index]->crearIt();

    while (it.haySiguiente() && it.siguiente().first != clave) it.avanzar();

    if(!it.haySiguiente()){
        noIncrementar.lock();
            incrementarSwitch->lock(noClaves);
        noIncrementar.unlock();

            list->insertar(hashMapPair(clave, 1));
            _claves.push_back(clave);

        incrementarSwitch->unlock(noClaves);
    } else {
        it.siguiente().second++;
    }
}

/**
 * Primera Idea:
 * Claves tiene prioridad sobre escritura, de esta forma cuando un proceso solicite todas las claves
 * existentes, no se generarán condiciones de carrera sobre estas.
 * Esto podría generar starvation en los procesos que intentan agregar una clave, pero al no hacerlo
 * se podrian generar condiciones de carrera sobre las claves.
 */
std::vector<std::string> HashMapConcurrente::claves() {
    clavesSwitch->lock(noIncrementar);
        noClaves.lock();
            std::vector<std::string> res = _claves;
        noClaves.unlock();
    clavesSwitch->unlock(noIncrementar);
    return res;
}

unsigned int HashMapConcurrente::valor(std::string clave) {
    ListaAtomica<hashMapPair> * list = tabla[hashIndex(clave)];

    auto it = list->crearIt();
    int value = 0;

    while (it.haySiguiente()){
        if (it.siguiente().first == clave)
            value = it.siguiente().second;
        it.avanzar();
    }
        
    return value;
}

hashMapPair HashMapConcurrente::maximo() {
    hashMapPair *max = new hashMapPair();
    max->second = 0;

    for (unsigned int index = 0; index < HashMapConcurrente::cantLetras; index++) {
        for (
            auto it = tabla[index]->crearIt();
            it.haySiguiente();
            it.avanzar()
        ) {
            if (it.siguiente().second > max->second) {
                max->first = it.siguiente().first;
                max->second = it.siguiente().second;
            }
        }
    }

    return *max;
}

hashMapPair HashMapConcurrente::maximoParalelo(unsigned int cantThreads) {
    // Completar (Ejercicio 3)
}

#endif
