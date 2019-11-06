//
// Created by oxabz on 24/10/2019.
//

#include "Generateur.h"

Generateur::Generateur(ostream &mFile) : m_niveau(0), m_file(mFile){
    if (m_file.fail()) // si le fichier ne peut-être lu...
        throw FichierException();
}

void Generateur::incNiveau() {
    m_niveau++;
}

void Generateur::decNiveau() {
    m_niveau--;
}

void Generateur::ecrire(string i) {
    m_file<<i;
}

void Generateur::ecrireLigne(string i) {
    m_file<<endl;
    for (int j = 0; j < m_niveau; ++j) {
        m_file<<"\t";
    }
    m_file<<i;
}




