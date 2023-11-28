#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#define V1 0;
#define V2 1;
#define V3 2;
#define OUT1 13;
#define OUT2 12;
#define OUT3 11;
#define prezzoE 0,44;
#define TAGDAY 287; 


void *leggi(int p1,int p2,int p3,t_lettura dati);
void *aggiorna(t_lettura);
int accendi();
int spegni();
int stampa();

typede struct {
  float potenzaP;
  float totaleP;
  float giornalieroP;
  float potenzaC;
  float totaleC;
  float giornalieroC;
  float bilancio;
  float guadagno;

} t_dati;

//definizione della struct Item
typedef struct {
  int id;
  float v1;
  float v2;
  float v3;
  struct t_dati;
  int timestamp;
} Item;

//definizione di un nodo della lista dati
typedef struct ElemNodo{
	Item Valore;
	struct ElemNodo* Successore;
} t_nodo;

//definizione di una lista della spesa
typedef t_nodo* ListaNodi;



int main(){
  int exit=0;
  t_lettura dati1;
  while(exit=0){ //apertura ciclo ogni 5 minuti
    leggi(V1,V2,V3,dati1);
    aggiorna();
  }
  return 0;
}
