#ifndef INTERPRETEUR_H
#define INTERPRETEUR_H

#include "Symbole.h"
#include "Lecteur.h"
#include "Exceptions.h"
#include "TableSymboles.h"
#include "ArbreAbstrait.h"
#include "Generateur.h"

class Interpreteur {
public:
	Interpreteur(ifstream & fichier, ostream &fsortie);   // Construit un interpréteur pour interpreter
	                                    //  le programme dans  fichier 


    void startTranspilation();
	void analyse();                     // Si le contenu du fichier est conforme à la grammaire,
	                                    //   cette méthode se termine normalement et affiche un message "Syntaxe correcte".
                                      //   la table des symboles (ts) et l'arbre abstrait (arbre) auront été construits
	                                    // Sinon, une exception sera levée

	inline const TableSymboles & getTable () const  { return m_table;    } // accesseur	
	inline Noeud* getArbre () const { return m_arbre; }                    // accesseur
    Generateur &getGenerateur();

    int getNbErreur() const;

private:
    Lecteur        m_lecteur;  // Le lecteur de symboles utilisé pour analyser le fichier
    TableSymboles  m_table;    // La table des symboles valués
    Noeud*         m_arbre;    // L'arbre abstrait
    Generateur     m_generateur;

    int m_nbErreur;

    // Implémentation de la grammaire
    Noeud*  programme();   //   <programme> ::= procedure principale() <seqInst> finproc FIN_FICHIER
    Noeud*  seqInst();	   //     <seqInst> ::= <inst> { <inst> }
    Noeud*  inst();	       //        <inst> ::= <affectation> ; | <instSi>
    Noeud*  affectation(); // <affectation> ::= <variable> = <expression> 
    Noeud*  expression();  //  <expression> ::= <expEt> { ou <expEt> }
    Noeud*  expEt();  //  <expEt> ::= <expComp> { et <expComp> }
    Noeud*  expComp();  //<expComp> ::= <expAdd> { == | != | < | <= | > | >= <expAdd> }
    Noeud*  expAdd();  //  <expAdd> ::= <expMult> { + | - <expMult> }
    Noeud*  expMult();  //  <facteur> { * | / <facteur> }
    Noeud*  facteur();     //     <facteur> ::= <entier>  |  <variable>  |  - <facteur>  | non <facteur> | ( <expression> )
                           //   <opBinaire> ::= + | - | *  | / | < | > | <= | >= | == | != | et | ou
    Noeud*  instSi();      //      <instSi> ::= si ( <expression> ) <seqInst> finsi
    Noeud*  instSiRiche(); // <instSiRiche> ::= si ( <expression> ) <seqInst> finsi
    Noeud*  instTantQue(); //
    Noeud*  instPour();    //
    Noeud*  instEcrire();    //
    Noeud*  instLire();    //
    Noeud*  instRepeter(); //
    Noeud*  instPreInc(); //
    Noeud*  instPreDec(); //
    Noeud*  instSelon(); //


    // outils pour simplifier l'analyse syntaxique
    void tester (const string & symboleAttendu) const;   // Si symbole courant != symboleAttendu, on lève une exception
    void testerEtAvancer(const string & symboleAttendu); // Si symbole courant != symboleAttendu, on lève une exception, sinon on avance
    void erreur (const string & mess) const;             // Lève une exception "contenant" le message mess
};

#endif /* INTERPRETEUR_H */
