cmake_minimum_required (VERSION 2.6)

project (mcomp)

set(CMAKE_CXX_FLAGS ${CMAKE_CXX_FLAGS} "-pthread")

file(COPY programme.txt motsCle.txt DESTINATION .)

add_executable(mcomp main.cpp ArbreAbstrait.cpp ArbreAbstrait.h Exceptions.h Interpreteur.cpp Interpreteur.h Lecteur.cpp Lecteur.h Symbole.cpp Symbole.h SymboleValue.cpp SymboleValue.h TableSymboles.cpp TableSymboles.h)
