#ifndef HMC_HPP
#define HMC_HPP

#include <atomic>
#include <string>
#include <vector>

#include "ListaAtomica.hpp"

using namespace std;

typedef pair<string, uint> hashMapPair;

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

    ListaAtomica<hashMapPair>* tabla[HashMapConcurrente::cantLetras]{};
    vector<string> _claves;

    static uint hashIndex(string clave);
};

#endif  /* HMC_HPP */