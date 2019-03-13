// Pi.cpp : Ce fichier contient la fonction 'main'. L'exécution du programme commence et se termine à cet endroit.
//

#include "pch.h"
#include <thread>
#include <random>
#include <time.h>
#include <mutex>
#include <iostream>
using namespace std;

#define NUM_POINTS 1000000
#define NUM_THREADS 4  // 5 est la meilleure valeur (plus de 3 fois plus rapide qu'un seul thread) sur Begonia.

std::default_random_engine engine;
std::uniform_real_distribution <double> distrib(0, 1);


double global_sum = 0.0f;		// IMPORTANT de ne pas initialiser avec un simple '0'
std::mutex locker;

void calcul_pi_partiel_cercle(int myNum) {
	//Fonction parralélisable
	double local_sum = 0.0f;
	double Un_pas = 1.0 / NUM_POINTS, x,y;
	for (int i = myNum; i < NUM_POINTS; i += NUM_THREADS) {
		x = distrib(engine);
		y = distrib(engine);
		//si le point aléatoire tombe dans le cercle, on ajoute 1 à la somme locale
		if (x*x + y * y < 1) {
			local_sum += Un_pas;
		}
	}
	//on ajoute la somme locale à la globale
	//on multiplie par 4, car on a travaillé sur une aire de 1 et non de 4 pour le cercle unité
	locker.lock();
	global_sum += 4*local_sum;
	locker.unlock();
}
double calcul_pi() {
	//fonction calculant seule pi
	double sum;
	double local_sum = 0.0f;
	double Un_pas = 1.0 / NUM_POINTS, x, y;
	for (int i = 0; i < NUM_POINTS; i += 1) {
		x = distrib(engine);
		y = distrib(engine);
		//voir fonction précédente
		if (x*x + y * y < 1) {
			local_sum += Un_pas;
		}
	}
	//ici local sum renvoie pi
	//on multiplie par 4, car on a travaillé sur une aire de 1 et non de 4 pour le cercle unité
	sum = 4*local_sum;
	return sum;
}
//------------------------------------
int main() {
	//méthode avec threads
	double start = clock();
	std::thread Ids[NUM_THREADS];
	for (int i = 0; i < NUM_THREADS; i++) {
		Ids[i] = thread(calcul_pi_partiel_cercle, i);
	}
	for (int k = 0; k < NUM_THREADS; k++) {
		Ids[k].join();
	}
	double temps_threads = clock()-start;
	cout << "Valeur approchee de Pi (methode cercle unitaire en threads) :  " << global_sum << " en "<<temps_threads<<" ms."<< endl;

	//méthode simple
	start = clock();
	double sum = calcul_pi();
	double temps_simple = clock() - start;
	cout << "Valeur approchee de Pi (methode cercle unitaire simple) :  " << sum << " en " << temps_simple << " ms." << endl;

	//fin
	return 0;
}