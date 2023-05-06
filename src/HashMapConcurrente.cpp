#ifndef CHM_CPP
#define CHM_CPP

#include <iostream>
#include <pthread.h>

#include "HashMapConcurrente.hpp"

HashMapConcurrente::HashMapConcurrente() {
    for (unsigned int i = 0; i < HashMapConcurrente::cantLetras; i++) {
        tabla[i] = new ListaAtomica<hashMapPair>();
    }
}

unsigned int HashMapConcurrente::hashIndex(std::string clave) {
    return (unsigned int)(clave[0] - 'a');
}

void HashMapConcurrente::incrementar(std::string clave) {
    unsigned int index = hashIndex(clave);
    ListaAtomica<hashMapPair> *pLista = tabla[index];
    tablaMtx[index].lock();
    auto iterador = pLista->crearIt();
    while (iterador.haySiguiente() && iterador.siguiente().first != clave) {
        iterador.avanzar();
    }
    if (!iterador.haySiguiente()) {
        pLista->insertar(hashMapPair(clave, 1));

    } else {
        iterador.siguiente().second++;
    }
    tablaMtx[index].unlock();
}

std::vector<std::string> HashMapConcurrente::claves() {
    // Completar (Ejercicio 2)
}

unsigned int HashMapConcurrente::valor(std::string clave) {
    // Completar (Ejercicio 2)
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
