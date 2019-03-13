// GoL.cpp : Ce fichier contient la fonction 'main'. L'exécution du programme commence et se termine à cet endroit.

#include "pch.h"
#include <thread>
#include <random>
#include <time.h>
#include <mutex>
#include <windows.h>
#include <algorithm> 
#include <iostream>
using namespace std;


//variables globales partagées
std::vector<int> tableau;
int alive = 0;
std::mutex lockAlive;

//variables globales
int thread_dim = 1000;

//create seed
void Seed() {
	int val;
	for (int i = 0; i < 2 * thread_dim; i++) {
		for (int j = 0; j < 2 * thread_dim; j++) {
			val = rand() % 2;
			tableau.push_back(val);
			if (val == 1) {
				alive++;
			}
		}
	}
	cout << "Seeded with " << alive << " living people" << endl;
}

void affiche() {
	for (int i = 0; i < 2 * thread_dim; i++) {
		for (int j = 0; j < 2 * thread_dim; j++) {
			cout << tableau[i * 2 * thread_dim + j] << " ";
		}
		cout << endl;
	}
	cout << "---------------";
}

void gereThread(std::vector<int> &newtableau,int position) {
	//pour chaque case (i = num ligne, j = num col)
	int offseti = (position / 2) * thread_dim;
	int offsetj = (position % 2) * thread_dim;
	for (int i = offseti; i < offseti + thread_dim; i++) {
		for (int j = offsetj ; j < offsetj + thread_dim; j++) {
			//i et j index dans le grand tableau
			int voisinsVivants = 0;
			//on itère sur les voisins
			int minI = max(0, i - 1), maxI = min(i + 2, thread_dim*2), minJ = max(0, j - 1), maxJ = min(j + 2, thread_dim*2);
			for (int ii = minI; ii < maxI; ii++) {
				for (int jj = minJ; jj < maxJ; jj++) {
					if (!(ii==i && jj==j) && (tableau[ii* 2*thread_dim + jj] == 1)) { voisinsVivants += 1; }
				}
			}
			//on détermine le nouvel état de la case
			if (tableau[i*2*thread_dim + j] == 1) {
				if (voisinsVivants < 2 || voisinsVivants > 3) {
					newtableau.push_back(0);
				}
				else {
					newtableau.push_back(1);
					lockAlive.lock();
					alive++;
					lockAlive.unlock();
				}
			}
			else {
				if (voisinsVivants == 3) {
					newtableau.push_back(1);
					lockAlive.lock();
					alive++;
					lockAlive.unlock();
				}
				else {
					newtableau.push_back(0);
				}
			}

		}
	}
}

void geresimple(std::vector<int> &newtableau) {
	//pour chaque case (i = num ligne, j = num col)
	for (int i = 0; i < 2*thread_dim; i++) {
		for (int j = 0; j < 2*thread_dim; j++) {
			//i et j index dans le grand tableau
			int voisinsVivants = 0;
			//on itère sur les voisins
			int minI = max(0, i - 1), maxI = min(i + 2, thread_dim * 2), minJ = max(0, j - 1), maxJ = min(j + 2, thread_dim * 2);
			for (int ii = minI; ii < maxI; ii++) {
				for (int jj = minJ; jj < maxJ; jj++) {
					if (!(ii == i && jj == j) && (tableau[ii * 2 * thread_dim + jj] == 1)) { voisinsVivants += 1; }
				}
			}
			//on détermine le nouvel état de la case
			if (tableau[i * 2 * thread_dim + j] == 1) {
				if (voisinsVivants < 2 || voisinsVivants > 3) {
					newtableau.push_back(0);
				}
				else {
					newtableau.push_back(1);
					alive++;
				}
			}
			else {
				if (voisinsVivants == 3) {
					newtableau.push_back(1);
					alive++;
				}
				else {
					newtableau.push_back(0);
				}
			}

		}
	}
}

//controleur
int main() {
	Seed();
	for (int e = 0; e < 4; e++) {
		//WITHOUT THREAD
		double start = clock();
		alive = 0;
		std::vector<int> newT;
		geresimple(newT);
		//on modifie le tableau
		for (int i = 0; i < 2 * thread_dim; i++) {
			for (int j = 0; j < 2 * thread_dim; j++) {
				tableau[i * 2 * thread_dim + j] = newT[i * 2 * thread_dim + j];
			}
		}
		cout << "time elapsed without threads: " << clock() - start << " and they are " << alive << " living people" << endl;

		//THREADS
		start = clock();
		alive = 0;
		std::vector<int> newT0, newT1, newT2, newT3;
		std::thread T0(gereThread, std::ref(newT0), 0), T1(gereThread, std::ref(newT1), 1), T2(gereThread, std::ref(newT2), 2), T3(gereThread, std::ref(newT3), 3);
		T0.join(); T1.join(); T2.join(); T3.join();
		//on modifie le tableau
		for (int i = 0; i < thread_dim; i++) {
			for (int j = 0; j < thread_dim; j++) {
				tableau[i * 2 * thread_dim + j] = newT0[i*thread_dim + j];
			}
			for (int j = 0; j < thread_dim; j++) {
				tableau[i * 2 * thread_dim + j + thread_dim] = newT1[i*thread_dim + j];
			}
		}
		for (int i = 0; i < thread_dim; i++) {
			for (int j = 0; j < thread_dim; j++) {
				tableau[2 * thread_dim*thread_dim + i * 2 * thread_dim + j] = newT2[i*thread_dim + j];
			}
			for (int j = 0; j < thread_dim; j++) {
				tableau[2 * thread_dim*thread_dim + i * 2 * thread_dim + j + thread_dim] = newT3[i*thread_dim + j];
			}
		}
		cout << "time elapsed with threads: " << clock() - start << " and they are " <<alive<<" living people"<< endl;
	}
}