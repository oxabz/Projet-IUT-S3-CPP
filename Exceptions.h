/* 
 * File:   Exceptions.h
 * Author: martin
 *
 * Created on 7 décembre 2014, 19:08
 */

#ifndef EXCEPTIONS_H
#define	EXCEPTIONS_H
#include <exception>
#include <string>
#include "Symbole.h"
using namespace std;

// Classe mère de toutes les exceptions de l'interpréteur
class InterpreteurException : public exception {
public:
    const char * what() const throw() override {
        return "Exception Interpreteur";
    }
};

class FichierException : public InterpreteurException {
public:
    const char * what() const throw() override {
        return "Ouverture Fichier Impossible";
    }
};

class SyntaxeException : public InterpreteurException {
public:
    SyntaxeException(const char * message = NULL, Symbole* s = nullptr) : m_message(message), symbole(s) {}
    const char * what() const throw() override {
        return m_message;
    }

    Symbole *getSymbole() const {
        return symbole;
    }

private :
    const char* m_message;
    Symbole *symbole;
};


class IndefiniException : public InterpreteurException {
public:
    const char * what() const throw() override {
        return "Valeur Indéfinie";
    }
};


class DivParZeroException : public InterpreteurException {
public:
    const char * what() const throw() override {
        return "Division par 0";
    }
};

class OperationInterditeException : public InterpreteurException {
public:
    const char * what() const throw() override {
        return "Operation Interdite sur un noeud";
    }
};

#endif	/* EXCEPTIONS_H */

