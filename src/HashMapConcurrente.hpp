#ifndef HMC_HPP
#define HMC_HPP

#include <atomic>
#include <string>
#include <vector>

#include "ListaAtomica.hpp"

typedef std::pair<std::string, unsigned int> hashMapPair;

struct Lightswitch{
   unsigned int counter;
   std::mutex self;

   Lightswitch(){
      counter = 0;
   }

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
    static const unsigned int cantLetras = 26;

    HashMapConcurrente();

    void incrementar(std::string clave);
    std::vector<std::string> claves();
    unsigned int valor(std::string clave);

    hashMapPair maximo();
    hashMapPair maximoParalelo(unsigned int cantThreads);

 private:
    ListaAtomica<hashMapPair> *tabla[HashMapConcurrente::cantLetras];
    std::vector<std::string> _claves;

    static unsigned int hashIndex(std::string clave);
};

#endif  /* HMC_HPP */