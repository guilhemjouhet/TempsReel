// QSort.cpp  Ce fichier contient la fonction 'main'. L'exécution du programme commence et se termine à cet endroit.
//

#include "pch.h"
#include <thread>
#include <random>
#include <time.h>
#include <iostream>
using namespace std;

#define TAILLE 10000
#define NUM_THREADS 4  // 5 est la meilleure valeur (plus de 3 fois plus rapide qu'un seul thread) sur Begonia.


bool is_constant(std::vector<int> &Tableau) {
	//fonction qui renvoie true si un tableau a touts ses éléments égaux
	int init = Tableau[0];
	for (int k = 1; k < Tableau.size(); k++) {
		if (Tableau[k] != init) {
			return false;
		}
	}
	return true;
}


void sort_simple(std::vector<int> &Tableau,bool tothread=false) {
	//algorithme de tri par split and merge
	//permet un multi threading si tothread=true. deux thread créés le cas échéant
	std::vector<int> Tmin, Tmax;
	if (Tableau.size() > 1 && !is_constant(Tableau)) {
		int pivot = Tableau[0];
		for (int j = 1; j < Tableau.size(); j++) {
			if (Tableau[j] <= pivot) {
				Tmin.push_back(Tableau[j]);
			}
			else {
				Tmax.push_back(Tableau[j]);
			}
		}
		Tmin.push_back(Tableau[0]);
		if (tothread) {
			std::thread t1(sort_simple, std::ref(Tmin),false);
			std::thread t2(sort_simple, std::ref(Tmax),false);
			t1.join();
			t2.join();
		}
		else {
			sort_simple(Tmin);
			sort_simple(Tmax);
		}
		Tmin.insert(Tmin.end(), Tmax.begin(), Tmax.end());
		Tableau = Tmin;
	}
}
//------------------------------------
int main() {
	std::vector<int> Tableau;
	std::vector<int> Tableau_thread;
	int current_val;
	int max = 10000;
	Tableau.push_back(max/2);
	Tableau_thread.push_back(max/2);
	for (int i = 1; i < TAILLE; i++) {
		current_val = rand() % max + 1; //entier aléatoire entre 0 et 1000
		Tableau.push_back(current_val);
		Tableau_thread.push_back(current_val);
	}
	//simple
	double start = clock();
	sort_simple(Tableau);
	double temps_simple = clock() - start;
	bool check = true;
	int current_max = Tableau[0];
	//check
	cout << "Vecteur classé : [";
	for (int i = 1; i < Tableau.size(); i++) {
		if (i < 20) {
			cout << Tableau[i] << " ";
		}
		if (current_max > Tableau[i]) {
			check = false;
		}
		current_max = Tableau[i];
	}
	//output
	if (check) {
		cout <<"...]"<< endl << "tri de " << TAILLE << " entiers avec succes en " << temps_simple << " ms" << endl;
	}
	else {
		cout << "fail" << endl;
	}

	//méthode avec threads
	start = clock();
	sort_simple(Tableau_thread,true);
	double temps_thread = clock() - start;
	check = true;
	current_max = Tableau_thread[0];
	//check
	cout << "Vecteur classé : [";
	for (int i = 1; i < Tableau_thread.size(); i++) {
		if (i < 20) {
			cout << Tableau_thread[i] << " ";
		}
		if (current_max > Tableau_thread[i]) {
			check = false;
		}
		current_max = Tableau_thread[i];
	}
	//output
	if (check) {
		cout << "...]" << endl << "tri de " << TAILLE << " entiers avec succes en " << temps_thread << " ms" << endl;
	}
	else {
		cout << "fail" << endl;
	}
}