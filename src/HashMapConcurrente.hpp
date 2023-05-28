#ifndef HMC_HPP
#define HMC_HPP

#include <atomic>
#include <string>
#include <vector>

#include "ListaAtomica.hpp"

typedef std::pair<std::string, uint> hashMapPair;

struct Lightswitch{
   uint counter;
   std::mutex self;

   Lightswitch(){ counter = 0; }

   void lock(std::mutex& barrier){
      self.lock();
         counter++;
         if (counter == 1)
            barrier.lock();
      self.unlock();
   }
   
   void unlock(std::mutex& barrier){
      self.lock();
         counter--;
         if (counter == 0)
            barrier.unlock();
      self.unlock();
   }
};

class HashMapConcurrente {
 public:
    static const uint cantLetras = 26;

    HashMapConcurrente();

    void incrementar(std::string clave);
    std::vector<std::string> claves();
    uint valor(std::string clave);

    hashMapPair maximo();
    hashMapPair maximoParalelo(uint cantThreads);

 private:
    ListaAtomica<hashMapPair> *tabla[HashMapConcurrente::cantLetras];
    std::vector<std::string> _claves;

    static uint hashIndex(std::string clave);
};

#endif  /* HMC_HPP */