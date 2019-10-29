#include <iostream>
using namespace std;
#include "Interpreteur.h"
#include "Exceptions.h"

int main(int argc, char* argv[]) {
    string nomFEntree;
    string nomFSortie;
    if (argc != 3) {
        cout << "Usage : " << argv[0] << " nom_fichier_source nom_fichier_cpp" << endl << endl;
        cout << "Entrez le nom du fichier que voulez-vous interpréter : ";
        getline(cin, nomFEntree);
        cout << "Entrez le nom du fichier de sortie";
        getline(cin, nomFSortie);
    } else
        nomFEntree = argv[1];
    ifstream fEntree(nomFEntree);
    ofstream fSortie(nomFSortie);

    try {
        if (fEntree.fail()) throw FichierException();
        if (fSortie.fail()) throw FichierException();
        Interpreteur interpreteur(fEntree,fSortie);
        interpreteur.analyse();
        // Si pas d'exception levée, l'analyse syntaxique a réussi
        cout << endl << "================ Syntaxe Correcte" << endl;
        // On affiche le contenu de la table des symboles avant d'exécuter le programme
        cout << endl << "================ Table des symboles avant exécution : " << interpreteur.getTable();
        cout << endl << "================ Execution de l'arbre" << endl;
        // On exécute le programme si l'arbre n'est pas vide
        if (interpreteur.getArbre() != nullptr) interpreteur.getArbre()->executer();
        // Et on vérifie qu'il a fonctionné en regardant comment il a modifié la table des symboles
        cout << endl << "================ Table des symboles apres exécution : " << interpreteur.getTable();
        cout << endl << "================ Start transpilation : "<<endl;
        interpreteur.startTranspilation();
        fSortie.close();
        cout << endl << "================ Transpilation completed : "<<endl;

    } catch (InterpreteurException & e) {
        cout << e.what() << endl;
    }
    return 0;
}
