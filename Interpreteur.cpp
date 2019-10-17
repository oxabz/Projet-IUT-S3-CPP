#include "Interpreteur.h"
#include <stdlib.h>
#include <iostream>
#include <queue>
using namespace std;

Interpreteur::Interpreteur(ifstream & fichier) :
m_lecteur(fichier), m_table(), m_arbre(nullptr) {
}

void Interpreteur::analyse() {
  m_arbre = programme(); // on lance l'analyse de la première règle
}

void Interpreteur::tester(const string & symboleAttendu) const {
  // Teste si le symbole courant est égal au symboleAttendu... Si non, lève une exception
  static char messageWhat[256];
  if (m_lecteur.getSymbole() != symboleAttendu) {
    sprintf(messageWhat,
            "Ligne %d, Colonne %d - Erreur de syntaxe - Symbole attendu : %s - Symbole trouvé : %s",
            m_lecteur.getLigne(), m_lecteur.getColonne(),
            symboleAttendu.c_str(), m_lecteur.getSymbole().getChaine().c_str());
    throw SyntaxeException(messageWhat);
  }
}

void Interpreteur::testerEtAvancer(const string & symboleAttendu) {
  // Teste si le symbole courant est égal au symboleAttendu... Si oui, avance, Sinon, lève une exception
    try {
        tester(symboleAttendu);
        m_lecteur.avancer();
    }catch (SyntaxeException &e){
        Symbole * symboleThrow = new Symbole(m_lecteur.getSymbole());
        m_lecteur.avancer();
        if(m_lecteur.getSymbole()!=symboleAttendu){
            throw SyntaxeException(e.what(), symboleThrow);
        }
    }

}

void Interpreteur::erreur(const string & message) const {
  // Lève une exception contenant le message et le symbole courant trouvé
  // Utilisé lorsqu'il y a plusieurs symboles attendus possibles...
  static char messageWhat[256];
  sprintf(messageWhat,
          "Ligne %d, Colonne %d - Erreur de syntaxe - %s - Symbole trouvé : %s",
          m_lecteur.getLigne(), m_lecteur.getColonne(), message.c_str(), m_lecteur.getSymbole().getChaine().c_str());
  throw SyntaxeException(messageWhat);
}

Noeud* Interpreteur::programme() {
  // <programme> ::= procedure principale() <seqInst> finproc FIN_FICHIER
  testerEtAvancer("procedure");
  testerEtAvancer("principale");
  testerEtAvancer("(");
  testerEtAvancer(")");
  Noeud* sequence = seqInst();
  testerEtAvancer("finproc");
  tester("<FINDEFICHIER>");
  return sequence;
}

Noeud* Interpreteur::seqInst() {
  // <seqInst> ::= <inst> { <inst> }
  NoeudSeqInst* sequence = new NoeudSeqInst();
  do {
    sequence->ajoute(inst());
  } while (m_lecteur.getSymbole() == "<VARIABLE>" || m_lecteur.getSymbole() == "si"|| m_lecteur.getSymbole() == "tantque"|| m_lecteur.getSymbole() == "pour"||m_lecteur.getSymbole() == "ecrire"||m_lecteur.getSymbole() == "lire"||m_lecteur.getSymbole() == "repeter");
  // Tant que le symbole courant est un début possible d'instruction...
  // Il faut compléter cette condition chaque fois qu'on rajoute une nouvelle instruction
  return sequence;
}

Noeud* Interpreteur::inst() {
    // <inst> ::= <affectation>  ; | <instSi>
    if (m_lecteur.getSymbole() == "<VARIABLE>") {
        Noeud *affect = affectation();
        testerEtAvancer(";");
        return affect;
    } else if (m_lecteur.getSymbole() == "si") {
        return instSiRiche();

        // Compléter les alternatives chaque fois qu'on rajoute une nouvelle instruction
    }else if (m_lecteur.getSymbole() == "tantque"){
        return instTantQue();
    }else if (m_lecteur.getSymbole() == "ecrire"){
        return instEcrire();
    }else if (m_lecteur.getSymbole() == "lire"){
        return instLire();
    }else if (m_lecteur.getSymbole() == "repeter"){
        return instRepeter();
    }else if (m_lecteur.getSymbole() == "pour"){
        return instPour();
    }
  else {
      erreur("Instruction incorrecte");
      return nullptr;
  }
}

Noeud* Interpreteur::affectation() {
  // <affectation> ::= <variable> = <expression> 
  tester("<VARIABLE>");
  Noeud* var = m_table.chercheAjoute(m_lecteur.getSymbole()); // La variable est ajoutée à la table eton la mémorise
  m_lecteur.avancer();
  testerEtAvancer("=");
  Noeud* exp = expression();             // On mémorise l'expression trouvée
  return new NoeudAffectation(var, exp); // On renvoie un noeud affectation
}

Noeud* Interpreteur::expression() {
  // <expression> ::= <facteur> { <opBinaire> <facteur> }
  //  <opBinaire> ::= + | - | *  | / | < | > | <= | >= | == | != | et | ou
  Noeud* fact = facteur();
  while ( m_lecteur.getSymbole() == "+"  || m_lecteur.getSymbole() == "-"  ||
          m_lecteur.getSymbole() == "*"  || m_lecteur.getSymbole() == "/"  ||
          m_lecteur.getSymbole() == "<"  || m_lecteur.getSymbole() == "<=" ||
          m_lecteur.getSymbole() == ">"  || m_lecteur.getSymbole() == ">=" ||
          m_lecteur.getSymbole() == "==" || m_lecteur.getSymbole() == "!=" ||
          m_lecteur.getSymbole() == "et" || m_lecteur.getSymbole() == "ou"   ) {
    Symbole operateur = m_lecteur.getSymbole(); // On mémorise le symbole de l'opérateur
    m_lecteur.avancer();
    Noeud* factDroit = facteur(); // On mémorise l'opérande droit
    fact = new NoeudOperateurBinaire(operateur, fact, factDroit); // Et on construuit un noeud opérateur binaire
  }
  return fact; // On renvoie fact qui pointe sur la racine de l'expression
}

Noeud* Interpreteur::facteur() {
  // <facteur> ::= <entier> | <variable> | - <facteur> | non <facteur> | ( <expression> )
  Noeud* fact = nullptr;
  if (m_lecteur.getSymbole() == "<VARIABLE>" || m_lecteur.getSymbole() == "<ENTIER>") {
    fact = m_table.chercheAjoute(m_lecteur.getSymbole()); // on ajoute la variable ou l'entier à la table
    m_lecteur.avancer();
  } else if (m_lecteur.getSymbole() == "-") { // - <facteur>
    m_lecteur.avancer();
    // on représente le moins unaire (- facteur) par une soustraction binaire (0 - facteur)
    fact = new NoeudOperateurBinaire(Symbole("-"), m_table.chercheAjoute(Symbole("0")), facteur());
  } else if (m_lecteur.getSymbole() == "non") { // non <facteur>
    m_lecteur.avancer();
    // on représente le moins unaire (- facteur) par une soustractin binaire (0 - facteur)
    fact = new NoeudOperateurBinaire(Symbole("non"), facteur(), nullptr);
  } else if (m_lecteur.getSymbole() == "(") { // expression parenthésée
    m_lecteur.avancer();
    fact = expression();
    testerEtAvancer(")");
  } else
    erreur("Facteur incorrect");
  return fact;
}

Noeud* Interpreteur::instSi() {
  // <instSi> ::= si ( <expression> ) <seqInst> finsi
  testerEtAvancer("si");
  testerEtAvancer("(");
  Noeud* condition = expression(); // On mémorise la condition
  testerEtAvancer(")");
  Noeud* sequence = seqInst();     // On mémorise la séquence d'instruction
  testerEtAvancer("finsi");
  return new NoeudInstSi(condition, sequence); // Et on renvoie un noeud Instruction Si
}

Noeud *Interpreteur::instSiRiche() {
    // <instSiRiche> ::= si ( <expression> ) <seqInst finsi
    vector<Noeud * > conditions;
    vector<Noeud * > sequences;
    testerEtAvancer("si");

    testerEtAvancer("(");
    conditions.push_back(expression());
    testerEtAvancer(")");
    sequences.push_back(seqInst());

    while (m_lecteur.getSymbole()=="sinonsi"){
        testerEtAvancer("sinonsi");
        testerEtAvancer("(");
        conditions.push_back(expression());
        testerEtAvancer(")");
        sequences.push_back(seqInst());
    }
    if(m_lecteur.getSymbole()=="sinon"){
        testerEtAvancer("sinon");
        sequences.push_back(seqInst());
    }
    testerEtAvancer("finsi");
    return new NoeudInstSiRiche(conditions,sequences);
}

Noeud *Interpreteur::instTantQue() {
    Noeud *condition;
    Noeud *sequence;
    int restartPoint = -1;
    restart:
    try {
        switch (restartPoint){
            default:
                testerEtAvancer("tantque");
                testerEtAvancer("(");
                condition = expression();
            case 0:
                testerEtAvancer(")");
                sequence = seqInst();
            case 1:
                testerEtAvancer("fintantque");
        }
        return new NoeudInstTantQue(condition, sequence);
    }catch(SyntaxeException &e) {
        string fallback_symbole[] = {")","fintantque"};

        while (m_lecteur.getSymbole()!=";"){
            restartPoint = 0;
            while(restartPoint<2&&m_lecteur.getSymbole()!=fallback_symbole[restartPoint]){
                restartPoint++;
            }
            if(restartPoint<2){
                goto restart;
            }else{
                m_lecteur.avancer();
            }
        }
        throw e;
    }

}

Noeud *Interpreteur::instPour() {
    Noeud *assignation;
    Noeud *condition;
    Noeud *incrementation;
    Noeud *sequence;
    testerEtAvancer("pour");
    testerEtAvancer("(");
    assignation = affectation();
    testerEtAvancer(";");
    condition = expression();
    testerEtAvancer(";");
    incrementation = affectation();
    testerEtAvancer(")");
    sequence = seqInst();
    testerEtAvancer("finpour");
    return new NoeudInstPour(assignation,incrementation,condition,sequence);
    restart :
    int restartPoint = -1;
    try{
        switch (restartPoint){

        }
    }catch(SyntaxeException &e) {
        string fallback_symbole[] = {")","fintantque"};

        while (m_lecteur.getSymbole()!=";"){
            restartPoint = 0;
            while(restartPoint<2&&m_lecteur.getSymbole()!=fallback_symbole[restartPoint]){
                restartPoint++;
            }
            if(restartPoint<2){
                goto restart;
            }else{
                m_lecteur.avancer();
            }
        }
        throw e;
    }
}

Noeud *Interpreteur::instEcrire() {
    vector<Noeud * > noeuds;
    testerEtAvancer("ecrire");
    testerEtAvancer("(");
    if (m_lecteur.getSymbole()=="<CHAINE>"){
        noeuds.push_back(m_table.chercheAjoute(m_lecteur.getSymbole()));
        m_lecteur.avancer();
    } else
        noeuds.push_back(expression());
    while(m_lecteur.getSymbole()==","){
        testerEtAvancer(",");
        if (m_lecteur.getSymbole()=="<CHAINE>"){
            noeuds.push_back(m_table.chercheAjoute(m_lecteur.getSymbole()));
            m_lecteur.avancer();
        } else
            noeuds.push_back(expression());
    }
    testerEtAvancer(")");
    testerEtAvancer(";");
    return new NoeudInstEcrire(noeuds);
}

Noeud *Interpreteur::instLire() {
    testerEtAvancer("lire");
    testerEtAvancer("(");
    Noeud * var = m_table.chercheAjoute(m_lecteur.getSymbole());
    m_lecteur.avancer();
    testerEtAvancer(")");
    testerEtAvancer(";");
    return new NoeudInstLire(var);
}

Noeud *Interpreteur::instRepeter() {
    testerEtAvancer("repeter");
    Noeud * sesquInst = seqInst();
    testerEtAvancer("jusqua");
    testerEtAvancer("(");
    Noeud * exp = expression();
    testerEtAvancer(")");
    return new NoeudInstRepeter(exp,sesquInst);
}


