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

s_ordini *ordini, *coda;
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
			check_ordini();
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
	printf("sto rimuovendo ricetta chiamata <%s>\n",ricetta);
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
			printf("rifornito");
			return;
		}
	} while(strcmp(ingrediente, "aggiungi_ricetta") != 0 && strcmp(ingrediente, "rimuovi_ricetta") != 0 && strcmp(ingrediente, "rifornimento") != 0 && strcmp(ingrediente, "ordine") != 0);
	printf("rifornito");
}

unsigned int ordina(char ricetta[MAX+1]) {
	return 0;
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
	printf("corriere");
}

void check_ordini(){
	printf("check_ordini");
	
}