//
// Created by oxabz on 02/11/2019.
//

#ifndef MCOMP_PROCEDURE_H
#define MCOMP_PROCEDURE_H


#include "TableSymboles.h"

class Procedure {
public:
    Procedure(const vector<Symbole> &parameter, TableSymboles *tableSymboles, Noeud *arbre);

    void execute(vector<Noeud*> argument);

    Noeud *getArbre() const;

    TableSymboles *getTableSymboles() const;

    const vector<Symbole> &getParameters() const;

private:
    vector<Symbole> parameters;
    TableSymboles *tableSymboles;
    Noeud *arbre;
};


#endif //MCOMP_PROCEDURE_H
