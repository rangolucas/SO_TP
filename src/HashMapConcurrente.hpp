#ifndef HMC_HPP
#define HMC_HPP

#include <atomic>
#include <string>
#include <vector>

#include "ListaAtomica.hpp"

using namespace std;

typedef pair<string, uint> hashMapPair;

// struct Lightswitch {
//     uint counter;
//     mutex self;

//     Lightswitch() { counter = 0; }

//     void lock(mutex& barrier) {
//         self.lock();
//         counter++;
//         if (counter == 1) barrier.lock();
//         self.unlock();
//     }

//     void unlock(mutex& barrier) {
//         self.lock();
//         counter--;
//         if (counter == 0) barrier.unlock();
//         self.unlock();
//     }
// };

class HashMapConcurrente {

public:

    static const uint cantLetras = 26;

    HashMapConcurrente();

    void incrementar(string clave);
    vector<string> claves();
    uint valor(string clave);

    hashMapPair maximo();
    hashMapPair maximoParalelo(uint cantThreads);
    void maximoPorEntrada();

private:

    ListaAtomica<hashMapPair>* tabla[HashMapConcurrente::cantLetras];
    vector<string> _claves;

    static uint hashIndex(string clave);
};

#endif  /* HMC_HPP */