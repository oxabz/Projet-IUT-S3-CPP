#include <stdlib.h>
#include <tgmath.h>
#include "ArbreAbstrait.h"
#include "Symbole.h"
#include "SymboleValue.h"
#include "Exceptions.h"

#include "Generateur.h"
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

void NoeudSeqInst::traduire(Generateur *os) {
    os->ecrire("{");
    os->incNiveau();
    for (int i = 0; i < this->m_instructions.size(); ++i) {
        m_instructions[i]->traduire(os);
    }
    os->decNiveau();
    os->ecrireLigne("}");
}

int NoeudSeqInst::length() {
    return m_instructions.size();
}

Noeud *NoeudSeqInst::operator[](int i) {
    return m_instructions[i];
}

Noeud *NoeudSeqInst::getInst(int i) {
    return m_instructions[i];
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

void NoeudAffectation::traduire(Generateur *os) {
    os->ecrireLigne("");
    this->m_variable->traduire(os);
    os->ecrire(" = ");
    this->m_expression->traduire(os);
    os->ecrire(";");
}

void NoeudAffectation::traduireInline(Generateur *os) {
    this->m_variable->traduire(os);
    os->ecrire(" = ");
    this->m_expression->traduire(os);
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

void NoeudOperateurBinaire::traduire(Generateur *os) {
    os->ecrire("(");
    m_operandeGauche->traduire(os);
    os->ecrire(m_operateur.getChaine());
    m_operandeDroit->traduire(os);
    os->ecrire(")");
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

void NoeudInstSi::traduire(Generateur *os) {


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

void NoeudInstSiRiche::traduire(Generateur *os) {
    os->ecrireLigne("if");
    this->m_conditions[0]->traduire(os);
    if(typeid(*m_sequences[0])== typeid(NoeudSeqInst)){
        m_sequences[0]->traduire(os);
    }else{
        os->incNiveau();
        m_sequences[0]->traduire(os);
        os->decNiveau();
    }
    for (int i = 1; i < this->m_conditions.size(); ++i) {
        os->ecrire("else if");
        this->m_conditions[i]->traduire(os);
        if(typeid(*m_sequences[i])== typeid(NoeudSeqInst)){
            m_sequences[i]->traduire(os);
        }else{
            os->incNiveau();
            m_sequences[i]->traduire(os);
            os->decNiveau();
        }
    }
    if(m_conditions.size()+1==m_sequences.size()) {
        os->ecrireLigne("else");
        if(typeid(*m_sequences[m_sequences.size()-1])== typeid(NoeudSeqInst)){
            m_sequences[m_sequences.size()-1]->traduire(os);
        }else{
            os->incNiveau();
            m_sequences[m_sequences.size()-1]->traduire(os);
            os->decNiveau();
        }
    }

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

void NoeudInstTantQue::traduire(Generateur *os) {
    os->ecrireLigne("while");
    m_condition->traduire(os);
    if(typeid(*m_sequence)== typeid(NoeudSeqInst)){
        m_sequence->traduire(os);
    }else{
        os->incNiveau();
        m_sequence->traduire(os);
        os->decNiveau();
    }
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

void NoeudInstPour::traduire(Generateur *os) {
    os->ecrireLigne("for(");
    ((NoeudAffectation*)this->m_assignation)->traduireInline(os);
    os->ecrire("; ");
    this->m_condition->traduire(os);
    os->ecrire("; ");
    ((NoeudAffectation*)this->m_incrementation)->traduireInline(os);
    os->ecrire(")");
    if(typeid(*m_sequence)== typeid(NoeudSeqInst)){
        m_sequence->traduire(os);
    }else{
        os->incNiveau();
        m_sequence->traduire(os);
        os->decNiveau();
    }
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

void NoeudInstEcrire::traduire(Generateur *os) {
    os->ecrireLigne ("std::cout");
    for(Noeud * noeud : this->noeuds){
        os->ecrire("<<");
        noeud->traduire(os);
    }
    os->ecrire(";");
}

////////////////////////////////////////////////////////////////////////////////
// NoeudInstLire
////////////////////////////////////////////////////////////////////////////////

NoeudInstLire::NoeudInstLire(Noeud *symboleValue) : symboleValue(symboleValue) {}

int NoeudInstLire::executer() {
    int i;
    cin>>i;
    ((SymboleValue*)symboleValue)->setValeur(i);
    return i;
}

void NoeudInstLire::traduire(Generateur *os) {
    os->ecrireLigne("std::cin>>");
    this->symboleValue->traduire(os);
    os->ecrire(";");
}

////////////////////////////////////////////////////////////////////////////
// NoeudInstRepeter
////////////////////////////////////////////////////////////////////////////

NoeudInstRepeter::NoeudInstRepeter(Noeud *exp, Noeud *inst) : exp(exp), inst(inst){}

int NoeudInstRepeter::executer() {
    do{
        inst->executer();
    }while (!exp->executer());
    return 0;
}

void NoeudInstRepeter::traduire(Generateur *os) {
    os->ecrireLigne("do");
    this->inst->traduire(os);
    os->ecrire("while");
    this->exp->traduire(os);
    os->ecrire(";");

}

////////////////////////////////////////////////////////////////////////////
// NoeudInc
////////////////////////////////////////////////////////////////////////////

NoeudInc::NoeudInc(Noeud *symboleValue, Noeud *exp) : symboleValue(symboleValue), exp(exp) {}

int NoeudInc::executer() {
    int valeur = exp->executer() + ((SymboleValue*) symboleValue)->getValeur();
    ((SymboleValue*) symboleValue)->setValeur(valeur);
    return valeur;
}

void NoeudInc::traduire(Generateur *os) {
    symboleValue->traduire(os);
    os->ecrire("+=");
    exp->traduire(os);
    os->ecrire(";");
}

////////////////////////////////////////////////////////////////////////////
// NoeudDec
////////////////////////////////////////////////////////////////////////////

NoeudDec::NoeudDec(Noeud *symboleValue, Noeud *exp) : symboleValue(symboleValue), exp(exp) {}

int NoeudDec::executer() {
    int valeur = - exp->executer() + ((SymboleValue*) symboleValue)->getValeur();
    ((SymboleValue*) symboleValue)->setValeur(valeur);
    return valeur;
}

void NoeudDec::traduire(Generateur *os) {
    symboleValue->traduire(os);
    os->ecrire("+=");
    exp->traduire(os);
    os->ecrire(";");
}

////////////////////////////////////////////////////////////////////////////
// NoeudPostInc
////////////////////////////////////////////////////////////////////////////

NoeudPostInc::NoeudPostInc(Noeud *symboleValue) : symboleValue(symboleValue) {}

int NoeudPostInc::executer() {
    int valeur = 1 + ((SymboleValue*) symboleValue)->getValeur();
    ((SymboleValue*) symboleValue)->setValeur(valeur);
    return valeur - 1;
}

void NoeudPostInc::traduire(Generateur *os) {
    symboleValue->traduire(os);
    os->ecrire("++;");
}

////////////////////////////////////////////////////////////////////////////
// NoeudPostDec
////////////////////////////////////////////////////////////////////////////

NoeudPostDec::NoeudPostDec(Noeud *symboleValue) : symboleValue(symboleValue) {}

int NoeudPostDec::executer() {
    int valeur = ((SymboleValue*) symboleValue)->getValeur() - 1;
    ((SymboleValue*) symboleValue)->setValeur(valeur);
    return valeur + 1;
}

void NoeudPostDec::traduire(Generateur *os) {
    symboleValue->traduire(os);
    os->ecrire("--;");
}

////////////////////////////////////////////////////////////////////////////
// NoeudPreInc
////////////////////////////////////////////////////////////////////////////

NoeudPreInc::NoeudPreInc(Noeud *symboleValue) : symboleValue(symboleValue) {}

int NoeudPreInc::executer() {
    int valeur = ((SymboleValue*) symboleValue)->getValeur() + 1;
    ((SymboleValue*) symboleValue)->setValeur(valeur);
    return valeur;
}

void NoeudPreInc::traduire(Generateur *os) {
    os->ecrire("++");
    symboleValue->traduire(os);
    os->ecrire(";");
}

////////////////////////////////////////////////////////////////////////////
// NoeudPreDec
////////////////////////////////////////////////////////////////////////////

NoeudPreDec::NoeudPreDec(Noeud *symboleValue) : symboleValue(symboleValue) {}

int NoeudPreDec::executer() {
    int valeur = ((SymboleValue*) symboleValue)->getValeur() - 1;
    ((SymboleValue*) symboleValue)->setValeur(valeur);
    return valeur;
}

void NoeudPreDec::traduire(Generateur *os) {
    os->ecrire("--");
    symboleValue->traduire(os);
    os->ecrire(";");
}

NoeudInstSelon::NoeudInstSelon(Noeud *exp,
                               const vector<Noeud *> &symboleValue,
                               const vector<Noeud *> &sequinst,
                               const vector<int> &breaks):
                exp(exp),
                symboleValue(symboleValue),
                sequInst(sequinst),
                breaks(breaks){}

int NoeudInstSelon::executer() {
    int i = 0;
    while(i < symboleValue.size() && symboleValue[i]->executer() != exp->executer()){
        i++;
    }
    do{
        sequInst[i++]->executer();
    }while(
        [this,i](){
            for(int j : breaks){
                if(j==i-1){
                    return false;
                }
            }
            return true;
        }()&&
    i < symboleValue.size());
    return 0;
}

void NoeudInstSelon::traduire(Generateur *os) {
    os->ecrireLigne("switch");
    os->ecrire("(");
    exp->traduire(os);
    os->ecrire(")");
    os->ecrire("{");
    os->incNiveau();
    for (int i = 0; i < symboleValue.size(); ++i) {
        os->ecrireLigne("case ");
        symboleValue[i]->traduire(os);
        os->ecrire(":");
        os->incNiveau();
        if(typeid(*sequInst[i])== typeid(NoeudSeqInst)){
            for (int j = 0; j < ((NoeudSeqInst*)sequInst[i])->length(); ++j) {
                ((NoeudSeqInst*)sequInst[i])->getInst(j)->traduire(os);
            }
        } else{
            sequInst[i]->traduire(os);
        }
        if([this,i](){
            for(int j : breaks){
                if(j=i){
                    return false;
                }
            }
            return true;
        }()){
            os->ecrireLigne("break;");
        }
        os->decNiveau();
    }
    os->decNiveau();
    os->ecrireLigne("}");
}
