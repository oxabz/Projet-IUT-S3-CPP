//
// Created by oxabz on 24/10/2019.
//

#ifndef MCOMP_GENERATEUR_H
#define MCOMP_GENERATEUR_H

#include <ostream>
#include <string>
#include "ArbreAbstrait.h"
#include "TableSymboles.h"

using namespace std;

class Generateur {
public:
    Generateur(ostream &mFile);

    void incNiveau();
    void decNiveau();

    void ecrire(string i);
    void ecrireLigne(string i);


private:
    ostream & m_file;
    unsigned int m_niveau;
};


#endif //MCOMP_GENERATEUR_H
