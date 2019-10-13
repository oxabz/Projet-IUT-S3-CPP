#include <stdlib.h>
#include "ArbreAbstrait.h"
#include "Symbole.h"
#include "SymboleValue.h"
#include "Exceptions.h"

////////////////////////////////////////////////////////////////////////////////
// NoeudSeqInst
////////////////////////////////////////////////////////////////////////////////

NoeudSeqInst::NoeudSeqInst() : m_instructions() {
}

int NoeudSeqInst::executer() {
  for (unsigned int i = 0; i < m_instructions.size(); i++)
    m_instructions[i]->executer(); // on exécute chaque instruction de la séquence
  return 0; // La valeur renvoyée ne représente rien !
}

void NoeudSeqInst::ajoute(Noeud* instruction) {
  if (instruction!=nullptr) m_instructions.push_back(instruction);
}

////////////////////////////////////////////////////////////////////////////////
// NoeudAffectation
////////////////////////////////////////////////////////////////////////////////

NoeudAffectation::NoeudAffectation(Noeud* variable, Noeud* expression)
: m_variable(variable), m_expression(expression) {
}

int NoeudAffectation::executer() {
  int valeur = m_expression->executer(); // On exécute (évalue) l'expression
  ((SymboleValue*) m_variable)->setValeur(valeur); // On affecte la variable
  return 0; // La valeur renvoyée ne représente rien !
}

////////////////////////////////////////////////////////////////////////////////
// NoeudOperateurBinaire
////////////////////////////////////////////////////////////////////////////////

NoeudOperateurBinaire::NoeudOperateurBinaire(Symbole operateur, Noeud* operandeGauche, Noeud* operandeDroit)
: m_operateur(operateur), m_operandeGauche(operandeGauche), m_operandeDroit(operandeDroit) {
}

int NoeudOperateurBinaire::executer() {
  int og, od, valeur;
  if (m_operandeGauche != nullptr) og = m_operandeGauche->executer(); // On évalue l'opérande gauche
  if (m_operandeDroit != nullptr) od = m_operandeDroit->executer(); // On évalue l'opérande droit
  // Et on combine les deux opérandes en fonctions de l'opérateur
  if (this->m_operateur == "+") valeur = (og + od);
  else if (this->m_operateur == "-") valeur = (og - od);
  else if (this->m_operateur == "*") valeur = (og * od);
  else if (this->m_operateur == "==") valeur = (og == od);
  else if (this->m_operateur == "!=") valeur = (og != od);
  else if (this->m_operateur == "<") valeur = (og < od);
  else if (this->m_operateur == ">") valeur = (og > od);
  else if (this->m_operateur == "<=") valeur = (og <= od);
  else if (this->m_operateur == ">=") valeur = (og >= od);
  else if (this->m_operateur == "et") valeur = (og && od);
  else if (this->m_operateur == "ou") valeur = (og || od);
  else if (this->m_operateur == "non") valeur = (!og);
  else if (this->m_operateur == "/") {
    if (od == 0) throw DivParZeroException();
    valeur = og / od;
  }
  return valeur; // On retourne la valeur calculée
}

////////////////////////////////////////////////////////////////////////////////
// NoeudInstSi
////////////////////////////////////////////////////////////////////////////////

NoeudInstSi::NoeudInstSi(Noeud* condition, Noeud* sequence)
: m_condition(condition), m_sequence(sequence) {
}

int NoeudInstSi::executer() {
  if (m_condition->executer()) m_sequence->executer();
  return 0; // La valeur renvoyée ne représente rien !
}

////////////////////////////////////////////////////////////////////////////////
// NoeudInstSiRiche
////////////////////////////////////////////////////////////////////////////////
NoeudInstSiRiche::NoeudInstSiRiche(const vector<Noeud *> &mConditions, const vector<Noeud *> &mSequences)
        : m_conditions(mConditions), m_sequences(mSequences) {}

int NoeudInstSiRiche::executer() {
    int i = 0;
    while (i<m_conditions.size()&&!m_conditions[i]->executer()){
        i++;
    }
    if (i<m_conditions.size() || m_conditions.size()+1==m_sequences.size()){
        m_sequences[i]->executer();
    }
    return 0;
}

////////////////////////////////////////////////////////////////////////////////
// NoeudInstTantQue
////////////////////////////////////////////////////////////////////////////////
NoeudInstTantQue::NoeudInstTantQue(Noeud *condition, Noeud *sequence) : m_condition(condition), m_sequence(sequence){

}

int NoeudInstTantQue::executer() {
    while (m_condition->executer()){
        m_sequence->executer();
    }
    return 0;
}

////////////////////////////////////////////////////////////////////////////////
// NoeudInstPour
////////////////////////////////////////////////////////////////////////////////
NoeudInstPour::NoeudInstPour(Noeud *mAssignation, Noeud *mIncrementation, Noeud *mCondition, Noeud *mSequence)
        : m_assignation(mAssignation), m_incrementation(mIncrementation), m_condition(mCondition),
          m_sequence(mSequence) {}

int NoeudInstPour::executer() {
    m_assignation->executer();
    while (m_condition->executer()){
        m_sequence->executer();
        m_incrementation->executer();
    }
    return 0;
}

////////////////////////////////////////////////////////////////////////////////
// NoeudInstEcrire
////////////////////////////////////////////////////////////////////////////////

NoeudInstEcrire::~NoeudInstEcrire() {

}

int NoeudInstEcrire::executer() {
    for (int i = 0; i < noeuds.size(); ++i) {
        if  (typeid(*noeuds[i])==typeid(SymboleValue) && *((SymboleValue*)noeuds[i])== "<CHAINE>" ){
            std::string res = ((SymboleValue*)noeuds[i])->getChaine();
            cout<<res.substr(1,res.size()-2);
        }else{
            cout<<noeuds[i]->executer();
        }
    }
    return 0;
}

void NoeudInstEcrire::ajoute(Noeud *instruction) {
    noeuds.push_back(instruction);
}

NoeudInstEcrire::NoeudInstEcrire(const vector<Noeud *> &noeuds) : noeuds(noeuds) {}
