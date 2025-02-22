#ifndef ARBREABSTRAIT_H
#define ARBREABSTRAIT_H

// Contient toutes les déclarations de classes nécessaires
//  pour représenter l'arbre abstrait

#include <vector>
#include <queue>
#include <iostream>
#include <iomanip>
using namespace std;

#include "Symbole.h"
#include "Exceptions.h"

class Generateur;
class Procedure;

////////////////////////////////////////////////////////////////////////////////
class Noeud {
// Classe abstraite dont dériveront toutes les classes servant à représenter l'arbre abstrait
// Remarque : la classe ne contient aucun constructeur
  public:
    virtual int  executer() =0 ; // Méthode pure (non implémentée) qui rend la classe abstraite
    virtual void traduire(Generateur * os) = 0;
    virtual void ajoute(Noeud* instruction) { throw OperationInterditeException(); }
    virtual ~Noeud() {} // Présence d'un destructeur virtuel conseillée dans les classes abstraites
};

////////////////////////////////////////////////////////////////////////////////
class NoeudSeqInst : public Noeud {
// Classe pour représenter un noeud "sequence d'instruction"
//  qui a autant de fils que d'instructions dans la séquence
  public:
     NoeudSeqInst();         // Construit une séquence d'instruction vide
    ~NoeudSeqInst() {}       // A cause du destructeur virtuel de la classe Noeud
    int executer() override; // Exécute chaque instruction de la séquence
    void traduire(Generateur* os) override;
    void ajoute(Noeud* instruction) override;  // Ajoute une instruction à la séquence

    Noeud * getInst(int i);
    int length();

    Noeud * operator[] (int i);

  private:
    vector<Noeud *> m_instructions; // pour stocker les instructions de la séquence
};

////////////////////////////////////////////////////////////////////////////////
class NoeudAffectAbstr : public Noeud{
public:
    virtual void traduireInline(Generateur *os) = 0;
};

class NoeudAffectation : public NoeudAffectAbstr {
// Classe pour représenter un noeud "affectation"
//  composé de 2 fils : la variable et l'expression qu'on lui affecte
  public:
     NoeudAffectation(Noeud* variable, Noeud* expression); // construit une affectation
    ~NoeudAffectation() {}   // A cause du destructeur virtuel de la classe Noeud
    int executer() override; // Exécute (évalue) l'expression et affecte sa valeur à la variable
    void traduire(Generateur *os) override;
    void traduireInline(Generateur *os) override ;

private:
    Noeud* m_variable;
    Noeud* m_expression;
};

////////////////////////////////////////////////////////////////////////////////
class NoeudOperateurBinaire : public Noeud {
// Classe pour représenter un noeud "opération binaire" composé d'un opérateur
//  et de 2 fils : l'opérande gauche et l'opérande droit
  public:
    NoeudOperateurBinaire(Symbole operateur, Noeud* operandeGauche, Noeud* operandeDroit);
    // Construit une opération binaire : operandeGauche operateur OperandeDroit
   ~NoeudOperateurBinaire() {} // A cause du destructeur virtuel de la classe Noeud
    int executer() override;   // Exécute (évalue) l'opération binaire)
    void traduire(Generateur *os) override;

private:
    Symbole m_operateur;
    Noeud*  m_operandeGauche;
    Noeud*  m_operandeDroit;
};

////////////////////////////////////////////////////////////////////////////////
class NoeudInstSi : public Noeud {
// Classe pour représenter un noeud "instruction si"
//  et ses 2 fils : la condition du si et la séquence d'instruction associée
  public:
    NoeudInstSi(Noeud* condition, Noeud* sequence);
     // Construit une "instruction si" avec sa condition et sa séquence d'instruction
   ~NoeudInstSi() {}         // A cause du destructeur virtuel de la classe Noeud
    int executer() override; // Exécute l'instruction si : si condition vraie on exécute la séquence
    void traduire(Generateur *os) override;

private:
    Noeud*  m_condition;
    Noeud*  m_sequence;
};

/////////////////////////////////////////////////////////////////////////////
class NoeudInstSiRiche : public Noeud {
public:
    NoeudInstSiRiche(const vector<Noeud *> &mConditions, const vector<Noeud *> &mSequences);
    ~NoeudInstSiRiche(){}

    int executer() override;

    void traduire(Generateur *os) override;

private:
    vector<Noeud*> m_conditions;
    vector<Noeud*> m_sequences;

};

/////////////////////////////////////////////////////////////////////////////
class NoeudInstTantQue : public Noeud {
// Classe pour représenter un noeud "instruction si"
//  et ses 2 fils : la condition du si et la séquence d'instruction associée
public:
    NoeudInstTantQue(Noeud* condition, Noeud* sequence);
    // Construit une "instruction si" avec sa condition et sa séquence d'instruction
    ~NoeudInstTantQue() {}         // A cause du destructeur virtuel de la classe Noeud
    int executer() override; // Exécute l'instruction si : si condition vraie on exécute la séquence
    void traduire(Generateur *os) override;

private:
    Noeud*  m_condition;
    Noeud*  m_sequence;
};

/////////////////////////////////////////////////////////////////////////////
class NoeudInstPour : public Noeud {
// Classe pour représenter un noeud "instruction si"
//  et ses 2 fils : la condition du si et la séquence d'instruction associée
public:
    NoeudInstPour(Noeud *mAssignation, Noeud *mIncrementation, Noeud *mCondition, Noeud *mSequence);

    // Construit une "instruction si" avec sa condition et sa séquence d'instruction
    ~NoeudInstPour() {}         // A cause du destructeur virtuel de la classe Noeud
    int executer() override; // Exécute l'instruction si : si condition vraie on exécute la séquence
    void traduire(Generateur *os) override;

private:
    Noeud*  m_assignation;
    Noeud*  m_incrementation;
    Noeud*  m_condition;
    Noeud*  m_sequence;
};

/////////////////////////////////////////////////////////////////////////////
class NoeudInstEcrire : public Noeud {
public:
    NoeudInstEcrire(const vector<Noeud *> &noeuds);

    virtual ~NoeudInstEcrire();

    int executer() override;

    void ajoute(Noeud *instruction) override;

    void traduire(Generateur *os) override;

private:
    vector<Noeud*> noeuds;
};

/////////////////////////////////////////////////////////////////////////////
class NoeudInstRepeter : public Noeud{
public:
    NoeudInstRepeter(Noeud *exp, Noeud *inst);

    int executer() override;

    void traduire(Generateur *os) override;
private:
    Noeud * exp;
    Noeud * inst;
};

/////////////////////////////////////////////////////////////////////////////
class NoeudInstLire : public Noeud{
public:
    NoeudInstLire(Noeud *symboleValue);

    int executer() override;

    void traduire(Generateur *os) override;

private:
    Noeud * symboleValue;
};

/////////////////////////////////////////////////////////////////////////////
class NoeudInc : public NoeudAffectAbstr {
public:

    NoeudInc(Noeud *symboleValue, Noeud *exp);

    int executer() override;

    void traduire(Generateur *os) override;
    void traduireInline(Generateur *os)override ;

private:
    Noeud * symboleValue;
    Noeud * exp;
};
/////////////////////////////////////////////////////////////////////////////
class NoeudDec : public NoeudAffectAbstr{
public:

    NoeudDec(Noeud *symboleValue, Noeud *exp);

    int executer() override;

    void traduire(Generateur *os) override;
    void traduireInline(Generateur *os)override ;

private:
    Noeud * symboleValue;
    Noeud * exp;
};
/////////////////////////////////////////////////////////////////////////////
class NoeudPostInc : public NoeudAffectAbstr{
public:
    NoeudPostInc(Noeud *symboleValue);

    int executer() override;

    void traduire(Generateur *os) override;
    void traduireInline(Generateur *os)override ;

private:
    Noeud * symboleValue;
};
/////////////////////////////////////////////////////////////////////////////
class NoeudPostDec : public NoeudAffectAbstr{
public:
    NoeudPostDec(Noeud *symboleValue);

    int executer() override;

    void traduire(Generateur *os) override;
    void traduireInline(Generateur *os)override ;

private:
    Noeud * symboleValue;
};
/////////////////////////////////////////////////////////////////////////////
class NoeudPreInc : public NoeudAffectAbstr{
public:
    NoeudPreInc(Noeud *symboleValue);

    int executer() override;

    void traduire(Generateur *os) override;
    void traduireInline(Generateur *os)override ;

private:
    Noeud * symboleValue;
};
/////////////////////////////////////////////////////////////////////////////
class NoeudPreDec : public NoeudAffectAbstr{
public:
    NoeudPreDec(Noeud *symboleValue);

    int executer() override;

    void traduire(Generateur *os) override;
    void traduireInline(Generateur *os)override ;

private:
    Noeud * symboleValue;
};
/////////////////////////////////////////////////////////////////////////////
class NoeudInstSelon : public Noeud{
public:
    NoeudInstSelon(Noeud * exp, const vector<Noeud*> &symboleValue, const vector<Noeud*> &sequinst, const vector<int>&breaks);

    int executer() override;

    void traduire(Generateur *os) override;

private:
    Noeud * exp;
    vector<Noeud *> symboleValue;
    vector<Noeud *> sequInst;
    vector<int> breaks;
};
/////////////////////////////////////////////////////////////////////////////
class NoeudInstAppel : public Noeud{
public:

    NoeudInstAppel(string procedureName, Procedure* procedure, vector<Noeud*> arguments);

    int executer() override;

    void traduire(Generateur *os) override;

private:

    string procedureName;
    Procedure * procedure;
    vector<Noeud *> arguments;

};
#endif /* ARBREABSTRAIT_H */
