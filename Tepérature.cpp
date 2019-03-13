
#include "pch.h"
#include <thread>
#include <random>
#include <time.h>
#include <mutex>
#include <windows.h>
#include <iostream>
using namespace std;

#define TAILLE 10000
#define NUM_THREADS 4

//variables globales partagées
bool go_chauffage = false, go_pompe=false;
double temp=10;
double press = 2000;

//variables globales
double seuilT=15;
double seuilP = 1800;

//chauffage
void Fchauffage(std::mutex &lock) {
	while (1) {
		if (go_chauffage) {
			lock.lock();
			temp += 1; //chauffage en route : la température augmente
			lock.unlock();
		}
		else {
			lock.lock();
			temp -= 1; // chauffage éteint, la température décroit
			lock.unlock();
		}
		//attente 1sec
		Sleep(1000);
	}
}

//pompe
void Fpompe(std::mutex &lock) {
	while (1) {
		if (go_pompe) {
			lock.lock();
			press -= 100; // pompe en route : la pression diminue
			lock.unlock();
		}
		else {
			lock.lock();
			press += 250; // pompe arrété : P augmente
			lock.unlock();
		}
		Sleep(235);
	}
}

//capteur de température
void temperature(std::mutex &lock, double &tempCPT) {
	bool chauffe;
	while (1) {
		lock.lock();
		tempCPT = temp;
		lock.unlock();
		Sleep(500);
	}
}

//capteur de pression
void pression(std::mutex &lock, double &pressionCPT) {
	bool chauffe;
	while (1) {
		lock.lock();
		pressionCPT = press;
		lock.unlock();
		Sleep(700);
	}
}

//gestion de l'affichage
void screen(std::mutex &lock,double &tempCPT,double &pressCPT) {
	double P, T;
	while (1) {
		lock.lock();
		P = pressCPT;
		T = tempCPT;
		lock.unlock();
		cout << "pression : " << P << " et temperature : " << T << endl;
		Sleep(1000);
	}
}

//controleur
int main() {
	//variables P,T locale partagées avec Scrren et les capteurs
	double tempCPT;
	double pressionCPT;
	//un seul verrou pour toutes les variables : peut être amélioré par plusieurs verrours
	std::mutex lock;
	//création de sthreads
	std::thread chauffage(Fchauffage, std::ref(lock));
	std::thread pompage(Fpompe, std::ref(lock));
	std::thread capteurT(temperature, std::ref(lock), std::ref(tempCPT));
	std::thread capteurP(pression, std::ref(lock), std::ref(pressionCPT));
	std::thread Screen(screen, std::ref(lock), std::ref(tempCPT), std::ref(pressionCPT));
	//et le gestionnaire embarquant les règles de contrôle
	while (1) {
		if (tempCPT > seuilT) {
			go_chauffage = false;
			if (pressionCPT > seuilP) {
				go_pompe = true;
			}
			else {
				go_pompe = false;
			}
		}
		else {
			go_chauffage = true;
			go_pompe = true;
		}
		Sleep(0);
	}
}