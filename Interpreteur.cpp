#include "Interpreteur.h"
#include <stdlib.h>
#include <iostream>
#include <queue>

using namespace std;

#define TESTERTRYCATCH(X) try{\
     tester(#X);\
}catch(SyntaxeException &e){\
    m_nbErreur++;\
    m_lecteur.avancer();\
}\


Interpreteur::Interpreteur(ifstream &fichier, ostream &fsortie) :
                m_lecteur(fichier), m_table(), m_arbre(nullptr), m_generateur(fsortie), m_nbErreur(0) {
}

void Interpreteur::analyse() {
    m_arbre = programme(); // on lance l'analyse de la première règle
}

void Interpreteur::startTranspilation() {
    if (m_arbre != nullptr) {

        m_generateur.ecrire("#include <iostream>\n#include <string>\n");

        for (pair<string, Procedure *> procedure : m_procedures) {
            if (procedure.first != "principale") {
                m_generateur.ecrireLigne("void ");
                m_generateur.ecrire(procedure.first);
                m_generateur.ecrire("(");
                for (int k = 0; k < procedure.second->getParameters().size(); ++k) {
                    const Symbole &param = procedure.second->getParameters()[k];
                    m_generateur.ecrire("int &");
                    m_generateur.ecrire(param.getChaine());
                    if (k < procedure.second->getParameters().size() - 1) {
                        m_generateur.ecrire(", ");
                    }
                }
                m_generateur.ecrire("){");
                m_generateur.incNiveau();
                m_generateur.ecrireLigne("int ");
                for (int i = 0; i < procedure.second->getTableSymboles()->getTaille(); ++i) {
                    if ((*procedure.second->getTableSymboles())[i] == "<VARIABLE>"&&[procedure,i](){
                        for (int j = 0; j < procedure.second->getParameters().size(); ++j) {
                            if((*procedure.second->getTableSymboles())[i].getChaine() == procedure.second->getParameters()[j].getChaine())
                                return false;
                        }
                        return true;
                    }()) {
                        m_generateur.ecrire((*procedure.second->getTableSymboles())[i].getChaine());
                        if (i < procedure.second->getTableSymboles()->getTaille() - 1) {
                            m_generateur.ecrire(",");
                        }
                    }
                }
                m_generateur.ecrire(";");
                for (int j = 0; j < ((NoeudSeqInst *) procedure.second->getArbre())->length(); ++j) {
                    ((NoeudSeqInst *) procedure.second->getArbre())->getInst(j)->traduire(&this->m_generateur);
                }
                m_generateur.decNiveau();
                m_generateur.ecrireLigne("}\n");
            }
        }

        Procedure *principale = m_procedures["principale"];
        m_generateur.ecrireLigne("int main() {");
        m_generateur.incNiveau();
        m_generateur.ecrireLigne("int ");
        for (int i = 0; i < principale->getTableSymboles()->getTaille(); ++i) {
            if ((*principale->getTableSymboles())[i] == "<VARIABLE>") {
                m_generateur.ecrire((*principale->getTableSymboles())[i].getChaine());
                if (i < principale->getTableSymboles()->getTaille() - 1) {
                    m_generateur.ecrire(",");
                }
            }
        }
        m_generateur.ecrire(";");
        for (int j = 0; j < ((NoeudSeqInst *) principale->getArbre())->length(); ++j) {
            ((NoeudSeqInst *) principale->getArbre())->getInst(j)->traduire(&this->m_generateur);
        }

        m_generateur.ecrireLigne("return 0;");
        m_generateur.decNiveau();
        m_generateur.ecrireLigne("}");
    }
}

void Interpreteur::tester(const string &symboleAttendu) const {
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

void Interpreteur::testerEtAvancer(const string &symboleAttendu) {
    // Teste si le symbole courant est égal au symboleAttendu... Si oui, avance, Sinon, lève une exception
    try {
        tester(symboleAttendu);
        m_lecteur.avancer();
    } catch (SyntaxeException &e) {
        cerr << e.what() << endl;
        m_nbErreur++;
        m_lecteur.avancer();
    }

}

void Interpreteur::erreur(const string &message) const {
    // Lève une exception contenant le message et le symbole courant trouvé
    // Utilisé lorsqu'il y a plusieurs symboles attendus possibles...
    static char messageWhat[256];
    sprintf(messageWhat,
            "Ligne %d, Colonne %d - Erreur de syntaxe - %s - Symbole trouvé : %s",
            m_lecteur.getLigne(), m_lecteur.getColonne(), message.c_str(), m_lecteur.getSymbole().getChaine().c_str());
    throw SyntaxeException(messageWhat);
}

Noeud *Interpreteur::programme() {
    // <programme> ::= procedure principale() <seqInst> finproc FIN_FICHIER
    while (m_lecteur.getSymbole() == "procedure") {
        testerEtAvancer("procedure");
        string nom = m_lecteur.getSymbole().getChaine();
        m_lecteur.avancer();
        testerEtAvancer("(");
        vector<Symbole> parameters;
        while (m_lecteur.getSymbole() == "<VARIABLE>") {
            parameters.push_back(m_lecteur.getSymbole());
            m_lecteur.avancer();
            if (m_lecteur.getSymbole() != ")") {
                testerEtAvancer(",");
            }
        }
        testerEtAvancer(")");
        TableSymboles *tableSymboles = new TableSymboles();
        m_table = tableSymboles;
        Noeud *sequence = seqInst();
        testerEtAvancer("finproc");
        m_procedures.insert({nom, new Procedure(parameters, tableSymboles, sequence)});
    }
    tester("<FINDEFICHIER>");
    if (m_nbErreur) {
        static char messageWhat[256];
        sprintf(messageWhat,
                "%d erreurs de syntaxe",m_nbErreur);
        throw SyntaxeException(messageWhat);
        return nullptr;
    } else {
        return m_procedures["principale"]->getArbre();
    }
}

Noeud *Interpreteur::seqInst() {
    // <seqInst> ::= <inst> { <inst> }
    NoeudSeqInst *sequence = new NoeudSeqInst();

    do {
        sequence->ajoute(inst());
    } while (m_lecteur.getSymbole() == "<VARIABLE>" || m_lecteur.getSymbole() == "si" ||
             m_lecteur.getSymbole() == "tantque" || m_lecteur.getSymbole() == "pour" ||
             m_lecteur.getSymbole() == "ecrire" || m_lecteur.getSymbole() == "lire" ||
             m_lecteur.getSymbole() == "repeter" || m_lecteur.getSymbole() == "++" || m_lecteur.getSymbole() == "--" ||
             m_lecteur.getSymbole() == "selon");

    // Tant que le symbole courant est un début possible d'instruction...
    // Il faut compléter cette condition chaque fois qu'on rajoute une nouvelle instruction
    return sequence;
}

Noeud *Interpreteur::inst() {
    // <inst> ::= <affectation>  ; | <instSi>
    try {
        if (m_lecteur.getSymbole() == "<VARIABLE>" && m_procedures.find(m_lecteur.getSymbole().getChaine()) != m_procedures.end()) {
            return instAppel();
        } else if (m_lecteur.getSymbole() == "<VARIABLE>") {
            Noeud *affect = affectation();
            testerEtAvancer(";");
            return affect;
        } else if (m_lecteur.getSymbole() == "si") {
            return instSiRiche();

            // Compléter les alternatives chaque fois qu'on rajoute une nouvelle instruction
        } else if (m_lecteur.getSymbole() == "tantque") {
            return instTantQue();
        } else if (m_lecteur.getSymbole() == "ecrire") {
            return instEcrire();
        } else if (m_lecteur.getSymbole() == "lire") {
            return instLire();
        } else if (m_lecteur.getSymbole() == "pour") {
            return instPour();
        } else if (m_lecteur.getSymbole() == "repeter") {
            return instRepeter();
        } else if (m_lecteur.getSymbole() == "++") {
            return instPreInc();
        } else if (m_lecteur.getSymbole() == "--") {
            return instPreDec();
        } else if (m_lecteur.getSymbole() == "selon") {
            return instSelon();
        } else {
            erreur("Instruction incorrecte");
            return nullptr;
        }
    } catch (SyntaxeException &e) {
        m_nbErreur++;
        cerr << e.what() << endl;
        do {
            m_lecteur.avancer();
        } while (m_lecteur.getSymbole() != "<VARIABLE>" && m_lecteur.getSymbole() != "si" &&
                 m_lecteur.getSymbole() != "tantque" && m_lecteur.getSymbole() != "ecrire" &&
                 m_lecteur.getSymbole() != "lire" && m_lecteur.getSymbole() != "pour" &&
                 m_lecteur.getSymbole() != "++" && m_lecteur.getSymbole() != "--" &&
                 m_lecteur.getSymbole() != "selon" && m_lecteur.getSymbole() != "<FINDEFICHIER>");
        return nullptr;
    }

}

Noeud *Interpreteur::affectation() {
    // <affectation> ::= <variable> = <expression>
    tester("<VARIABLE>");
    Noeud *var = m_table->chercheAjoute(m_lecteur.getSymbole()); // La variable est ajoutée à la table eton la mémorise
    m_lecteur.avancer();
    if (m_lecteur.getSymbole() == "=") {
        testerEtAvancer("=");
        Noeud *exp = expression();             // On mémorise l'expression trouvée
        return new NoeudAffectation(var, exp); // On renvoie un noeud affectation
    } else if (m_lecteur.getSymbole() == "++") {
        testerEtAvancer("++");
        return new NoeudPostInc(var);
    } else if (m_lecteur.getSymbole() == "--") {
        testerEtAvancer("--");
        return new NoeudPostDec(var);
    } else if (m_lecteur.getSymbole() == "+=") {
        testerEtAvancer("+=");
        Noeud *exp = expression();
        return new NoeudInc(var, exp);
    } else if (m_lecteur.getSymbole() == "-=") {
        testerEtAvancer("-=");
        Noeud *exp = expression();
        return new NoeudDec(var, exp);
    } else {
        erreur("Symbole Attendu : =,++,--,+=,-=");

        return nullptr;
    }
}


Noeud *Interpreteur::expression() {
    Noeud *fact = expEt();
    while (m_lecteur.getSymbole() == "ou") {
        Symbole operateur = m_lecteur.getSymbole(); // On mémorise le symbole de l'opérateur
        m_lecteur.avancer();
        Noeud *factDroit = expEt(); // On mémorise l'opérande droit
        fact = new NoeudOperateurBinaire(operateur, fact, factDroit); // Et on construuit un noeud opérateur binaire
    }
    return fact;
}


Noeud *Interpreteur::expEt() {
    Noeud *fact = expComp();
    while (m_lecteur.getSymbole() == "et") {

        Symbole operateur = m_lecteur.getSymbole(); // On mémorise le symbole de l'opérateur
        m_lecteur.avancer();
        Noeud *factDroit = expComp(); // On mémorise l'opérande droit
        fact = new NoeudOperateurBinaire(operateur, fact, factDroit); // Et on construuit un noeud opérateur binaire
    }
    return fact;
}

Noeud *Interpreteur::expComp() {
    Noeud *fact = expAdd();
    while (m_lecteur.getSymbole() == "<" || m_lecteur.getSymbole() == "<=" ||
           m_lecteur.getSymbole() == ">" || m_lecteur.getSymbole() == ">=" ||
           m_lecteur.getSymbole() == "==" || m_lecteur.getSymbole() == "!=") {
        Symbole operateur = m_lecteur.getSymbole(); // On mémorise le symbole de l'opérateur
        m_lecteur.avancer();
        Noeud *factDroit = expAdd(); // On mémorise l'opérande droit
        fact = new NoeudOperateurBinaire(operateur, fact, factDroit); // Et on construuit un noeud opérateur binaire
    }
    return fact;
}

Noeud *Interpreteur::expAdd() {
    Noeud *fact = expMult();
    while (m_lecteur.getSymbole() == "+" || m_lecteur.getSymbole() == "-") {
        Symbole operateur = m_lecteur.getSymbole(); // On mémorise le symbole de l'opérateur
        m_lecteur.avancer();
        Noeud *factDroit = expMult(); // On mémorise l'opérande droit
        fact = new NoeudOperateurBinaire(operateur, fact, factDroit); // Et on construuit un noeud opérateur binaire
    }
    return fact;
}

Noeud *Interpreteur::expMult() {
    Noeud *fact = facteur();
    while (m_lecteur.getSymbole() == "*" || m_lecteur.getSymbole() == "/") {
        Symbole operateur = m_lecteur.getSymbole(); // On mémorise le symbole de l'opérateur
        m_lecteur.avancer();
        Noeud *factDroit = facteur(); // On mémorise l'opérande droit
        fact = new NoeudOperateurBinaire(operateur, fact, factDroit); // Et on construuit un noeud opérateur binaire
    }
    return fact;
}


Noeud *Interpreteur::facteur() {
    // <facteur> ::= <entier> | <variable> | - <facteur> | non <facteur> | ( <expression> )
    Noeud *fact = nullptr;
    if (m_lecteur.getSymbole() == "<VARIABLE>" || m_lecteur.getSymbole() == "<ENTIER>") {
        fact = m_table->chercheAjoute(m_lecteur.getSymbole()); // on ajoute la variable ou l'entier à la table
        m_lecteur.avancer();
    } else if (m_lecteur.getSymbole() == "-") { // - <facteur>
        m_lecteur.avancer();
        // on représente le moins unaire (- facteur) par une soustraction binaire (0 - facteur)
        fact = new NoeudOperateurBinaire(Symbole("-"), m_table->chercheAjoute(Symbole("0")), facteur());
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

Noeud *Interpreteur::instSi() {
    // <instSi> ::= si ( <expression> ) <seqInst> finsi
    testerEtAvancer("si");
    testerEtAvancer("(");
    Noeud *condition = expression(); // On mémorise la condition
    testerEtAvancer(")");
    Noeud *sequence = seqInst();     // On mémorise la séquence d'instruction
    testerEtAvancer("finsi");
    return new NoeudInstSi(condition, sequence); // Et on renvoie un noeud Instruction Si
}

Noeud *Interpreteur::instSiRiche() {
    // <instSiRiche> ::= si ( <expression> ) <seqInst finsi
    vector<Noeud *> conditions;
    vector<Noeud *> sequences;
    testerEtAvancer("si");
    testerEtAvancer("(");


    conditions.push_back(expression());
    testerEtAvancer(")");
    sequences.push_back(seqInst());

    while (m_lecteur.getSymbole() == "sinonsi") {
        testerEtAvancer("sinonsi");
        testerEtAvancer("(");
        conditions.push_back(expression());
        testerEtAvancer(")");
        sequences.push_back(seqInst());
    }
    if (m_lecteur.getSymbole() == "sinon") {
        testerEtAvancer("sinon");
        sequences.push_back(seqInst());
    }
    testerEtAvancer("finsi");
    return new NoeudInstSiRiche(conditions, sequences);
}

Noeud *Interpreteur::instTantQue() {
    testerEtAvancer("tantque");
    testerEtAvancer("(");
    Noeud *condition = expression();
    testerEtAvancer(")");
    Noeud *sequence = seqInst();
    testerEtAvancer("fintantque");
    return new NoeudInstTantQue(condition, sequence);
}

Noeud *Interpreteur::instPour() {
    testerEtAvancer("pour");
    testerEtAvancer("(");
    Noeud *assignation = affectation();
    testerEtAvancer(";");
    Noeud *condition = expression();
    testerEtAvancer(";");
    Noeud *incrementation = affectation();
    testerEtAvancer(")");
    Noeud *sequence = seqInst();
    testerEtAvancer("finpour");
    return new NoeudInstPour(assignation, incrementation, condition, sequence);
}

Noeud *Interpreteur::instEcrire() {
    vector<Noeud *> noeuds;
    testerEtAvancer("ecrire");
    testerEtAvancer("(");
    if (m_lecteur.getSymbole() == "<CHAINE>") {
        noeuds.push_back(m_table->chercheAjoute(m_lecteur.getSymbole()));
        m_lecteur.avancer();
    } else
        noeuds.push_back(expression());
    while (m_lecteur.getSymbole() == ",") {
        testerEtAvancer(",");
        if (m_lecteur.getSymbole() == "<CHAINE>") {
            noeuds.push_back(m_table->chercheAjoute(m_lecteur.getSymbole()));
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
    Noeud *var = m_table->chercheAjoute(m_lecteur.getSymbole());
    m_lecteur.avancer();
    testerEtAvancer(")");
    testerEtAvancer(";");
    return new NoeudInstLire(var);
}

Noeud *Interpreteur::instRepeter() {
    testerEtAvancer("repeter");
    Noeud *sesquInst = seqInst();
    testerEtAvancer("jusqua");
    testerEtAvancer("(");
    Noeud *exp = expression();
    testerEtAvancer(")");
    testerEtAvancer(";");
    return new NoeudInstRepeter(exp, sesquInst);
}

Noeud *Interpreteur::instPreInc() {
    testerEtAvancer("++");
    Noeud *fact = m_table->chercheAjoute(m_lecteur.getSymbole()); // on ajoute la variable ou l'entier à la table
    m_lecteur.avancer();
    testerEtAvancer(";");
    return new NoeudPreInc(fact);
}

Noeud *Interpreteur::instPreDec() {
    testerEtAvancer("--");
    Noeud *fact = m_table->chercheAjoute(m_lecteur.getSymbole()); // on ajoute la variable ou l'entier à la table
    m_lecteur.avancer();
    testerEtAvancer(";");
    return new NoeudPreDec(fact);
}


Generateur &Interpreteur::getGenerateur() {
    return m_generateur;
}

Noeud *Interpreteur::instSelon() {
    testerEtAvancer("selon");
    testerEtAvancer("(");
    Noeud *exp = expression();
    testerEtAvancer(")");
    vector<Noeud *> ind;
    vector<Noeud *> instructions;
    vector<int> breaks;
    int i = 0;
    while (m_lecteur.getSymbole() == "cas") {
        m_lecteur.avancer();
        try{
            tester("<ENTIER>");
        }catch(SyntaxeException &e){
            m_nbErreur++;
            m_lecteur.avancer();
        }ind.emplace_back(m_table->chercheAjoute(m_lecteur.getSymbole()));
        m_lecteur.avancer();
        testerEtAvancer(":");
        instructions.emplace_back(inst());
        if (m_lecteur.getSymbole() == "stop") {
            testerEtAvancer("stop");
            breaks.emplace_back(i);
        }
        i++;
    }
    testerEtAvancer("finselon");
    return new NoeudInstSelon(exp, ind, instructions, breaks);
}


Noeud *Interpreteur::instAppel() {
    tester("<VARIABLE>");
    string procedureName = m_lecteur.getSymbole().getChaine();
    m_lecteur.avancer();
    testerEtAvancer("(");
    Procedure *procedure = m_procedures[procedureName];
    vector<Noeud *> arguments;
    for (int i = 0; i < procedure->getParameters().size(); ++i) {
        tester("<VARIABLE>");
        arguments.push_back(m_table->chercheAjoute(m_lecteur.getSymbole()));
        m_lecteur.avancer();
        if (i < procedure->getParameters().size() - 1) {
            testerEtAvancer(",");
        }
    }
    testerEtAvancer(")");
    testerEtAvancer(";");

    return new NoeudInstAppel(procedureName,procedure,arguments);
}

int Interpreteur::getNbErreur() const {
    return m_nbErreur;
}

