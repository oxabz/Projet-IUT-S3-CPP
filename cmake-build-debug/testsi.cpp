#include <iostream>
#include <string>

void choixIA(int &nbBaton, int &actionJoueur, int &choix){
	int ;
	if(nbBaton <= 4){
		choix = 3;
	}
	else{
		choix = (4 - actionJoueur);
	}
}

void dessinerPlateau(int &joueur, int &nbBaton, int &actionIA){
	int ij;
	std::cout<<"\27[2J\27[0;0H";
	std::cout<<"Joueur "<<joueur<<"\n";
	for(i = 0; (i < (nbBaton + actionIA)); i++){
		if(nbBaton <= i){
			std::cout<<"\27[31m";
		}
		std::cout<<"A  ";
	}
	std::cout<<"\27[39m\n";
	for(i = 0; (i < 5); i++){
		for(j = 0; (j < (nbBaton + actionIA)); j++){
			if(nbBaton <= j){
				std::cout<<"\27[31m";
			}
			std::cout<<"H  ";
		}
		std::cout<<"\27[39m\n";
	}
	for(i = 0; (i < (nbBaton + actionIA)); i++){
		if(nbBaton <= i){
			std::cout<<"\27[31m";
		}
		std::cout<<"V  ";
	}
	std::cout<<"\27[39m\n";
}

int main() {
	int dernierChoixIa,dernierChoixJoueur,entre,joueur,mode,nbBaton,zero;
	std::cout<<"Bonjour\n";
	std::cout<<"Seron distribués 16 battonnets qui pourron etre retiré tour a tour par les joueur. Chaque joueur pourra retirer entre 1 et 3 batonnet par tour. Le but du jeu est d'etre le dernier à pouvoir jouer\n";
	do{
		std::cout<<"Veuillez choisir entre une partie seul ou en versus (1 : vs | 2 : solo)\n";
		std::cin>>mode;
	}while((mode <= 2) && (mode > 0));
	nbBaton = 16;
	joueur = 2;
	dernierChoixIa = 0;
	dernierChoixJoueur = 0;
	zero = 0;
	while(nbBaton > 0){
		if((joueur == 1) && (mode == 1)){
			joueur = 2;
			dessinerPlateau(joueur,nbBaton,zero);
			entre = (0 - 1);
			do{
				std::cout<<"Saisissez le nombre de baton que vous voulez retirer\n";
				std::cin>>entre;
			}while((entre <= 3) && (entre > 0));
			nbBaton-=entre;
		}else if((joueur == 1) && (mode == 2)){
			joueur = 2;
			choixIA(nbBaton,dernierChoixJoueur,dernierChoixIa);
			nbBaton-=dernierChoixIa;
		}
		else{
			joueur = 1;
			dessinerPlateau(joueur,nbBaton,dernierChoixIa);
			entre = (0 - 1);
			do{
				std::cout<<"Saisissez le nombre de baton que vous voulez retirer\n";
				std::cin>>entre;
			}while((entre <= 3) && (entre > 0));
			nbBaton-=entre;
			dernierChoixJoueur = entre;
		}
	}
	dessinerPlateau(joueur,nbBaton,dernierChoixIa);
	if(mode == 1){
		switch(joueur){
			case 1:
				std::cout<<"Le joueur 1 a triomphé sur le joueur 2";
			case 2:
				std::cout<<"Le joueur 2 a battut le joueur 1";
		}
	}
	else{
		switch(joueur){
			case 1:
				std::cout<<"Le joueur a battut l'IA";
			case 2:
				std::cout<<"L'IA a battut le joueur";
		}
	}
	return 0;
}