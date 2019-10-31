#ifndef SYMBOLEVALUE_H
#define SYMBOLEVALUE_H

#include <string.h>
#include <iostream>
using namespace std;

#include "Symbole.h"
#include "ArbreAbstrait.h"

class Generateur;

class SymboleValue : public Symbole, public Noeud{
public:
	  SymboleValue(const Symbole & s); // Construit un symbole valué à partir d'un symbole existant s
	  ~SymboleValue( ) {}
	  int  executer() override;        // exécute le SymboleValue (revoie sa valeur !)
    void traduire(Generateur *os) override;

    inline void setValeur(int valeur)    { this->m_valeur=valeur; m_defini=true;  } // accesseur
    inline int getValeur()    { return m_valeur;} // accesseur
	  inline bool estDefini()              { return m_defini;                       } // accesseur

	  friend ostream & operator << (ostream & cout, const SymboleValue & symbole); // affiche un symbole value sur cout

private:
	  bool m_defini;	// indique si la valeur du symbole est définie
	  int  m_valeur;	// valeur du symbole si elle est définie, zéro sinon

};

#endif /* SYMBOLEVALUE_H */
