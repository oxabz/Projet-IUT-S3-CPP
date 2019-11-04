//
// Created by oxabz on 02/11/2019.
//

#include "Procedure.h"

Procedure::Procedure(const vector<Symbole> &parameter, TableSymboles *tableSymboles, Noeud *arbre) : parameters(
                parameter), tableSymboles(tableSymboles), arbre(arbre) {}

void Procedure::execute(vector<Noeud *> arguments){
    for(int i = 0; i < parameters.size(); i++){
        tableSymboles->chercheAjoute(parameters[i])->setValeur(((SymboleValue *)arguments[i])->getValeur());
    }
    arbre->executer();
    for(int i = 0; i < parameters.size(); i++){
        ((SymboleValue *)arguments[i])->setValeur(tableSymboles->chercheAjoute(parameters[i])->getValeur());
    }
}

Noeud *Procedure::getArbre() const {
    return arbre;
}

TableSymboles *Procedure::getTableSymboles() const {
    return tableSymboles;
}

const vector<Symbole> &Procedure::getParameters() const {
    return parameters;
}
