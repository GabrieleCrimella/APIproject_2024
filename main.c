#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#define MAX 255

typedef struct _ingrediente {
	char nome_ingrediente[MAX+1];
	unsigned int quantita;
	struct _ingrediente *next;
} s_ingrediente;

typedef struct _stoc_ingredienti {
	unsigned int qta;
	unsigned int scadenza;
	struct _stoc_ingredienti *prev, *next;
} s_stoccaggio;

typedef struct _magazzino {
	char nome_ingrediente[MAX+1];
	s_stoccaggio *stoccaggio;
	struct _magazzino *left, *right, *p;
} s_magazzino;

typedef struct _ricetta {
	char nome_ricetta[MAX+1];
	s_ingrediente *ingredienti;
	struct _ricetta *left, *right, *p;
} s_ricette;

typedef struct _ordini {
	unsigned int tempo;
	char nome_ricetta[MAX+1];
	unsigned int numero;
	unsigned int peso_totale;
	struct _ordini *next;
} s_ordini;

unsigned int acquisisci_comando(char[MAX+1]);
void aggiungi_ricetta(char[MAX+1], char[MAX+1]);
void rimuovi_ricetta(char[MAX+1]);
void rifornimento(char[MAX+1]);
unsigned int ordina(char[MAX+1]);
void corriere();
unsigned int esiste_ricetta(s_ricette*, char[MAX+1]);
s_ricette* esiste_ricetta_ret(s_ricette*, char [MAX+1]);
void non_aggiungi_ricetta(char[MAX+1]);
void check_ordini();
s_ingrediente* get_ricetta(char [MAX+1]);
s_magazzino * cerca_nel_magazzino(s_magazzino *, char [MAX+1]);
void aggiungi_in_coda(s_ordini*);
int cerca_ordine_in_coda(char [MAX+1]);
void rimuovi_ricetta_da_ricettario(s_ricette *, s_ricette *);
s_ricette* successore_ricettario(s_ricette *);
s_ricette* min_ricetta(s_ricette *);
unsigned int calcola_peso_ordine(char [MAX+1],unsigned int);
s_ingrediente* cerca_ingredienti(s_ricette *, char [MAX+1]);
void gestisci_ordine(s_ordini *ordine);

s_ordini *ordini_testa, *ordini_coda;
//anche questa la gestisco come coda: inserisco in coda e prelevo dalla testa, così ho accesso in O(1) e la tengo intrinsecamente in ordine cronologico
s_ordini *coda_della_coda = NULL, *testa_della_coda = NULL;	//gli ultimi due fanno riferimento alla coda. inserisco in coda, prelevo dalla testa
s_ricette *ricettario;
s_magazzino *magazzino;
unsigned int stop;
unsigned int tempo = 0;

int main() {
	
	char comando[MAX+1];
	unsigned int periodo, capienza;
	char ricetta[MAX+1];
	
	if(scanf("%d %d\n", &periodo, &capienza) <= 0) {
		return 1;
	}

	stop=acquisisci_comando(comando);
	while(stop>0) {
		if(strcmp(comando, "aggiungi_ricetta") == 0) {
			stop=acquisisci_comando(ricetta);
			stop=acquisisci_comando(comando);
			if(esiste_ricetta(ricettario, ricetta)) {
				printf("ignorato\n");
				non_aggiungi_ricetta(comando);
			} else {
				aggiungi_ricetta(ricetta, comando);
				printf("aggiunto\n");
			}
		}
		else if(strcmp(comando, "rimuovi_ricetta") == 0) {
			stop=acquisisci_comando(comando);
			rimuovi_ricetta(comando);
			stop=acquisisci_comando(comando);
		}
		else if(strcmp(comando, "rifornimento") == 0) {
			stop=acquisisci_comando(comando);
			rifornimento(comando);
			check_ordini();			//controlla se qualche ordine dalla coda pulò essere gestito
			if(stop<0)
				break;
		}
		else if(strcmp(comando, "ordine") == 0) {
			stop=acquisisci_comando(comando);
			if(ordina(comando) == -1) {
				printf("rifiutato\n");
			} else {
				printf("accettato\n");
			};
			stop=acquisisci_comando(comando);
		}
		tempo ++;
		if(tempo%periodo == 0) {
			corriere();
		}
	}
	//printf("bye\n");
	return 0;
}

unsigned int acquisisci_comando(char stringa[MAX+1]) {
	return scanf("%s",stringa);
}

unsigned int esiste_ricetta(s_ricette *T, char ricetta[MAX+1]) {
	if(T == NULL)
		return 0;
	if (strcmp(T->nome_ricetta,ricetta) == 0) 
		return 1;
	if (strcmp(T->nome_ricetta, ricetta) < 0)
		return esiste_ricetta(T->right, ricetta);
	return esiste_ricetta(T->left, ricetta);
}

s_ricette* esiste_ricetta_ret(s_ricette *T, char ricetta[MAX+1]) {
	if(T == NULL || strcmp(T->nome_ricetta,ricetta) == 0)
		return T;
	if (strcmp(T->nome_ricetta, ricetta) < 0)
		return esiste_ricetta_ret(T->right, ricetta);
	return esiste_ricetta_ret(T->left, ricetta);
}

void non_aggiungi_ricetta(char comando[MAX+1]) {
	int quantita;
	scanf("%d",&quantita);
	do{
		stop=acquisisci_comando(comando);
		if(stop<0)
			return;
		if(strcmp(comando, "aggiungi_ricetta") != 0 && strcmp(comando, "rimuovi_ricetta") != 0 && strcmp(comando, "rifornimento") != 0 && strcmp(comando, "ordine") != 0){
			scanf("%d", &quantita);
		}
	}while(strcmp(comando, "aggiungi_ricetta") != 0 && strcmp(comando, "rimuovi_ricetta") != 0 && strcmp(comando, "rifornimento") != 0 && strcmp(comando, "ordine") != 0);
}

void aggiungi_ricetta(char ricetta[MAX+1], char ingrediente[MAX+1]) {
	int quantita;
	s_ricette *pre,*cur,*x;
	x = (s_ricette*) malloc(sizeof(s_ricette));
	scanf("%d", &quantita);

	strcpy(x->nome_ricetta, ricetta);
	x->ingredienti = (s_ingrediente*) malloc(sizeof(s_ingrediente));
	strcpy(x->ingredienti->nome_ingrediente, ingrediente);
	x->ingredienti->quantita = quantita;

	s_ingrediente *ingredienti;

	pre = NULL;
	cur = ricettario;
	while(cur != NULL) {
		pre = cur;
		if (strcmp(x->nome_ricetta, cur->nome_ricetta) < 0)
			cur = cur->left;
		else
			cur = cur->right;
	}
	x->p = pre;
	if (pre == NULL)
		ricettario = x;
	else if (strcmp(x->nome_ricetta, pre->nome_ricetta)<0)
		pre->left = x;
	else
		pre->right = x;
	ingredienti = x->ingredienti;
	do {
		stop=acquisisci_comando(ingrediente);
		if(stop<0)
			return;
		if(strcmp(ingrediente, "aggiungi_ricetta") != 0 && strcmp(ingrediente, "rimuovi_ricetta") != 0 && strcmp(ingrediente, "rifornimento") != 0 && strcmp(ingrediente, "ordine") != 0) {
			ingredienti->next = (s_ingrediente*) malloc(sizeof(s_ingrediente));
			ingredienti = ingredienti->next;
			strcpy(ingredienti -> nome_ingrediente, ingrediente);
			scanf("%d",&(ingredienti->quantita));
		}
	} while(strcmp(ingrediente, "aggiungi_ricetta") != 0 && strcmp(ingrediente, "rimuovi_ricetta") != 0 && strcmp(ingrediente, "rifornimento") != 0 && strcmp(ingrediente, "ordine") != 0);
}

void rimuovi_ricetta(char ricetta[MAX+1]) {
	if(cerca_ordine_in_coda(ricetta) == 1) {
		printf("ordini in sospeso\n");
	} else {	//cerca nel ricettario: se la trovi stampi 'rimossa', altrimenti ' non presente'
		s_ricette *x = esiste_ricetta_ret(ricettario, ricetta);
		if(x == NULL) {
			printf("non presente\n");
		} else {
			rimuovi_ricetta_da_ricettario(ricettario, x);
			printf("rimossa\n");
		}
	}
}

int cerca_ordine_in_coda(char ricetta[MAX+1]) {
	s_ordini *x;
	x = testa_della_coda;
	while(x != NULL) {
		if(strcmp(x->nome_ricetta, ricetta) == 0) {
			return 1;
		}
		x = x ->next;
	}
	return 0;
}

void rimuovi_ricetta_da_ricettario(s_ricette *T, s_ricette *x) {
	s_ricette *da_canc, *sottoa;
	if(x->left == NULL || x->right == NULL) 
		da_canc = x;
	else
		da_canc = successore_ricettario(x);
	if (da_canc->left != NULL)
		sottoa = da_canc->left;
	else
		sottoa = da_canc->right;
	if (sottoa != NULL)
		sottoa->p = da_canc->p;
	if(da_canc->p == NULL)
		ricettario = sottoa;
	else if(da_canc == da_canc->p->left)
		da_canc->p->left = sottoa;
	else da_canc->p->right = sottoa;
	if(da_canc != x)
		strcpy(x->nome_ricetta, da_canc->nome_ricetta);
	free(da_canc);
}

s_ricette* successore_ricettario(s_ricette *x) {
	s_ricette *y;
	if(x->right == NULL)
		return min_ricetta(x->right);
	y = x->p;
	while (y != NULL && y->right == x) {
		x = y;
		y = y->p;
	}
	return y;
}

s_ricette* min_ricetta(s_ricette *x) {
	s_ricette *cur = x;
	while(ricettario->left != NULL)
		cur = cur -> left;
	return cur;
}

void rifornimento(char ingrediente[MAX+1]) {
	int quantita, scadenza;
	do{
		scanf("%d", &quantita);
		scanf("%d", &scadenza);

		s_magazzino *pre = NULL, *cur = magazzino;
		s_stoccaggio *precedente, *attuale, *da_inserire;

		while (cur != NULL && strcmp(cur -> nome_ingrediente, ingrediente) != 0) {
			pre = cur;
			if (strcmp(cur -> nome_ingrediente, ingrediente) < 0) 
				cur = cur->right;
			else
				cur = cur->left;
		}
		if(cur == NULL) {		//se l'ingrediente è nuovo, lo creo nell'albero
			cur = (s_magazzino*) malloc(sizeof(s_magazzino));
			strcpy(cur -> nome_ingrediente, ingrediente);
			cur -> left = cur -> right = NULL;
			cur -> p = pre;
			if (pre == NULL)	//se l'albero è vuoto, il nouvo nodo è la radice
				magazzino = cur;
			else if (strcmp(pre->nome_ingrediente, cur->nome_ingrediente) < 0)	//lo aggancio a sinistra/destra del predecessore
				pre -> right = cur;
			else
				pre -> left = cur;
		}


		//gestisco le qta degli ingredienti

		da_inserire = (s_stoccaggio*) malloc(sizeof(s_stoccaggio));
		da_inserire -> qta = quantita;
		da_inserire -> scadenza = scadenza;
		da_inserire -> next = da_inserire -> prev = NULL;

		if(cur -> stoccaggio == NULL) {		//lista vuota
			cur -> stoccaggio = da_inserire;
		} else {		
			attuale = cur -> stoccaggio;
			precedente = NULL;

			while (attuale != NULL && attuale -> scadenza < scadenza) {		//cerco dove inserire
				precedente = attuale;
				attuale = attuale -> next;
			}

			if(attuale != NULL && attuale -> scadenza == scadenza) {
				attuale -> qta += quantita;
				free (da_inserire);
			} else if (precedente == NULL) {	//inserisco in testa
				da_inserire -> next = cur -> stoccaggio;
				cur -> stoccaggio -> prev = da_inserire;
				cur -> stoccaggio = da_inserire;
			} else {
				da_inserire -> next = attuale;
				da_inserire -> prev = precedente;
				precedente -> next = da_inserire;

				if (attuale != NULL) {		//se non sto inserendo in coda
					attuale->prev = da_inserire;
				}
			}
		}
		stop = acquisisci_comando(ingrediente);
		if (stop < 0) {
			printf("rifornito\n");
			return;
		}
	} while(strcmp(ingrediente, "aggiungi_ricetta") != 0 && strcmp(ingrediente, "rimuovi_ricetta") != 0 && strcmp(ingrediente, "rifornimento") != 0 && strcmp(ingrediente, "ordine") != 0);
	printf("rifornito\n");
}

unsigned int calcola_peso_ordine(char ricetta[MAX+1],unsigned int numero) {
	unsigned int accumulatore = 0;
	s_ingrediente *comanda = cerca_ingredienti(ricettario, ricetta);
	while(comanda != NULL) {
		accumulatore += comanda -> quantita;
	}
	return accumulatore * numero;
}

unsigned int ordina(char ricetta[MAX+1]) {
	s_ingrediente *ric;
	s_stoccaggio *stoc;
	unsigned int numero, accumulatore;
	s_ordini *ordine = (s_ordini*) malloc(sizeof(s_ordini));
	s_magazzino *ingrediente_nel_magazzino;

	//acquisisci il numero di ricette che si vogliono ordinare
	scanf("%d",&numero);
	
	strcpy(ordine->nome_ricetta, ricetta);
	ordine->numero = numero;
	ordine -> tempo = tempo;
	ordine -> peso_totale = calcola_peso_ordine(ricetta, numero);
	
	ric = cerca_ingredienti(ricettario, ricetta);
	if(ric == NULL)
		return -1;
	//calcola peso della ricetta e moltiplicalo per il numero

	while(ric != NULL) {
		accumulatore = 0;
		ingrediente_nel_magazzino = cerca_nel_magazzino(magazzino, ric->nome_ingrediente);
		stoc = ingrediente_nel_magazzino -> stoccaggio;
		while(accumulatore < ric -> quantita * numero || stoc -> next != NULL) {
			if(stoc -> scadenza < tempo) { //ingrediente scaduto: va rimosso
				s_stoccaggio *da_eliminare = stoc;
				stoc = stoc -> next;
				ingrediente_nel_magazzino->stoccaggio = stoc;
				free(da_eliminare);
			} else {
				accumulatore += stoc -> qta;
			}
		}
		if (accumulatore < ric -> quantita * numero) {
			aggiungi_in_coda(ordine);	//lo aggiunge semplicemente in coda
			return 0;
		}
		ric = ric -> next;
	}
	gestisci_ordine(ordine);	//lo aggiunge agli ordini da gestire, elimina gli ingredienti usati dallo stoccaggio.
	
	return 0;
}

void gestisci_ordine(s_ordini *ordine) {	//inserisco in coda, prelevo dalla testa
	if(ordini_testa == NULL) {
		ordini_testa = ordine;
		ordini_coda = ordine;
	} else {
		ordine -> next = NULL;
		ordini_coda -> next = ordine;
		ordini_coda = ordine;
	}

	s_ingrediente *ric;
	s_stoccaggio *stoc;
	unsigned int accumulatore;
	s_magazzino *ingrediente_nel_magazzino;
	
	ric = cerca_ingredienti(ricettario, ordine -> nome_ricetta);
	if(ric == NULL)
		return;

	while(ric != NULL) {
		accumulatore = 0;
		ingrediente_nel_magazzino = cerca_nel_magazzino(magazzino, ric->nome_ingrediente);
		stoc = ingrediente_nel_magazzino -> stoccaggio;
		while(accumulatore < ric -> quantita * ordine -> numero || stoc -> next != NULL) {
			if(stoc -> scadenza < tempo) { //ingrediente scaduto: va rimosso
				s_stoccaggio *da_eliminare = stoc;
				stoc = stoc -> next;
				ingrediente_nel_magazzino->stoccaggio = stoc;
				free(da_eliminare);
			} else if(accumulatore + stoc -> qta <= ric -> quantita * ordine -> numero) {
				accumulatore += stoc -> qta;
				s_stoccaggio *da_eliminare;
				da_eliminare = stoc;
				stoc -> prev -> next = stoc -> next;
				stoc -> next -> prev = stoc -> prev;
				stoc = stoc -> next;
				free(da_eliminare);
			} else { //caso in cui devo solo modificare la quantità dello stoccaggio
				stoc -> qta -= ric -> quantita * ordine -> numero - accumulatore;
				continue;
			}
		}
		ric = ric -> next;
	}
	return;
}

s_ingrediente* cerca_ingredienti(s_ricette *R, char ricetta[MAX+1]) {
	if(ricettario == NULL){
		return NULL;
	}
	else if(strcmp(R->nome_ricetta, ricetta) == 0)
		return R->ingredienti;
	else if (strcmp(R->nome_ricetta, ricetta)<0)
		return cerca_ingredienti(R->left, ricetta);
	else
		return cerca_ingredienti(R->right, ricetta);
}

s_magazzino * cerca_nel_magazzino(s_magazzino *magazzino, char ingrediente[MAX+1]) {
	if (magazzino == NULL || strcmp(magazzino ->nome_ingrediente, ingrediente) == 0)
		return magazzino;
	else if(strcmp(magazzino -> nome_ingrediente, ingrediente) < 0) 
		return cerca_nel_magazzino(magazzino->right, ingrediente);
	else
		return cerca_nel_magazzino(magazzino->left, ingrediente);

}


void corriere() {

	printf("corriere\n");
}

void check_ordini(){
	//printf("check_ordini\n");

}

void aggiungi_in_coda(s_ordini *ordine) {
	if(testa_della_coda == NULL) {
		testa_della_coda = ordine;
		coda_della_coda = ordine;
	} else {
		coda_della_coda -> next = ordine;
		coda_della_coda = ordine;
	}
}