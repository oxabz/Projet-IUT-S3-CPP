/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/*
 * File:   newtestclass1.cpp
 * Author: oxabz
 *
 * Created on 2 nov. 2019, 18:46:19
 */

#include "newtestclass1.h"
#include "Interpreteur.h"


CPPUNIT_TEST_SUITE_REGISTRATION(newtestclass1);

newtestclass1::newtestclass1() {
}

newtestclass1::~newtestclass1() {
}

void newtestclass1::setUp() {
}

void newtestclass1::tearDown() {
}

void newtestclass1::testInterpreteur() {
    ifstream& fichier;
    ostream& fsortie;
    Interpreteur interpreteur(fichier, fsortie);
    if (true /*check result*/) {
        CPPUNIT_ASSERT(false);
    }
}

void newtestclass1::testStartTranspilation() {
    Interpreteur interpreteur;
    interpreteur.startTranspilation();
    if (true /*check result*/) {
        CPPUNIT_ASSERT(false);
    }
}

void newtestclass1::testAnalyse() {
    Interpreteur interpreteur;
    interpreteur.analyse();
    if (true /*check result*/) {
        CPPUNIT_ASSERT(false);
    }
}

