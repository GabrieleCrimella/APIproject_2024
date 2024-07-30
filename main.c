#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#define MAX 255

typedef struct _ingrediente {
	char nome_ingrediente[MAX + 1];
	unsigned int quantita;
	struct _ingrediente *next;
} s_ingrediente;

typedef struct _stoc_ingredienti {
	unsigned int qta;
	unsigned int scadenza;
	struct _stoc_ingredienti *prev, *next;
} s_stoccaggio;

typedef struct _magazzino {
	char nome_ingrediente[MAX + 1];
	s_stoccaggio *stoccaggio;
	struct _magazzino *left, *right, *p;
} s_magazzino;

typedef struct _ricetta {
	char nome_ricetta[MAX + 1];
	s_ingrediente *ingredienti;
	struct _ricetta *left, *right, *p;
} s_ricette;

typedef struct _ordini {
	unsigned int tempo;
	char nome_ricetta[MAX + 1];
	unsigned int numero;
	unsigned int peso_totale;
	struct _ordini *next;
} s_ordini;

void acquisisci_comando(char[MAX + 1]);
void aggiungi_ricetta(char[MAX + 1], char[MAX + 1]);
void rimuovi_ricetta(char[MAX + 1]);
void rifornimento(char[MAX + 1]);
unsigned int ordina(char[MAX + 1]);
void corriere();
unsigned int esiste_ricetta(s_ricette *, char[MAX + 1]);
s_ricette *esiste_ricetta_ret(s_ricette *, char[MAX + 1]);
void non_aggiungi_ricetta(char[MAX + 1]);
void check_ordini();
s_ingrediente *get_ricetta(char[MAX + 1]);
s_magazzino *cerca_nel_magazzino(s_magazzino *, char[MAX + 1]);
void aggiungi_in_coda(s_ordini *);
int cerca_ordine_in_coda(char[MAX + 1]);
void rimuovi_ricetta_da_ricettario(s_ricette *, s_ricette *);
s_ricette *successore_ricettario(s_ricette *);
s_ricette *min_ricetta(s_ricette *);
unsigned int calcola_peso_ordine(char[MAX + 1], unsigned int);
s_ingrediente *cerca_ingredienti(s_ricette *, char[MAX + 1]);
void gestisci_ordine(s_ordini *ordine);
int cerca_ordine_in_lista(char [MAX + 1]);
void inserisci_in_ordine_di_tempo(s_ordini *ordine);

s_ordini *ordini_testa = NULL, *ordini_coda = NULL;
// anche questa la gestisco come coda: inserisco in coda e prelevo dalla testa, così ho accesso in O(1) e la tengo intrinsecamente in ordine cronologico
s_ordini *coda_della_coda = NULL, *testa_della_coda = NULL; // gli ultimi due fanno riferimento alla coda. inserisco in coda, prelevo dalla testa
s_ricette *ricettario = NULL;
s_magazzino *magazzino = NULL;
unsigned int stop=1;
unsigned int tempo = 0, periodo;
unsigned int capienza;

int main() {

	char comando[MAX + 1];
	char ricetta[MAX + 1];

	if (scanf("%d %d\n", &periodo, &capienza) <= 0) {
		return -1;
	}

	acquisisci_comando(comando);
	while (stop == 1) {
		if (strcmp(comando, "aggiungi_ricetta") == 0) {
			acquisisci_comando(ricetta);
			acquisisci_comando(comando);
			if (stop == 1) {
				if (esiste_ricetta(ricettario, ricetta)) {
					printf("ignorato\n");
					non_aggiungi_ricetta(comando);
				} else {
					aggiungi_ricetta(ricetta, comando);
					printf("aggiunta\n");
				}
			} else 
				break;
		} else if (strcmp(comando, "rimuovi_ricetta") == 0) {
			acquisisci_comando(comando);
			rimuovi_ricetta(comando);
			acquisisci_comando(comando);
		} else if (strcmp(comando, "rifornimento") == 0) {
			acquisisci_comando(comando);
			rifornimento(comando);
			check_ordini(); // controlla se qualche ordine dalla coda pulò essere gestito
		} else if (strcmp(comando, "ordine") == 0) {
			acquisisci_comando(comando);
			if (ordina(comando) == -1) {
				printf("rifiutato\n");
			} else {
				printf("accettato\n");
			}
			acquisisci_comando(comando);
		}
		tempo++;
		if (tempo % periodo == 0) {
			corriere();
		}
	}
	return 0;
}

void acquisisci_comando(char stringa[MAX + 1]) {
	stop = scanf("%s", stringa);
	if(stop != 1)
		stop = -1;
}

unsigned int esiste_ricetta(s_ricette *T, char ricetta[MAX + 1]) {
	// printf("<%s, %s>\n",T->nome_ricetta, ricetta);
	if (T == NULL)
		return 0;
	else if (strcmp(T->nome_ricetta, ricetta) == 0)
		return 1;
	else if (strcmp(T->nome_ricetta, ricetta) < 0)
		return esiste_ricetta(T->right, ricetta);
	else
		return esiste_ricetta(T->left, ricetta);
}

s_ricette *esiste_ricetta_ret(s_ricette *T, char ricetta[MAX + 1]) {
	if (T == NULL || strcmp(T->nome_ricetta, ricetta) == 0)
		return T;
	if (strcmp(T->nome_ricetta, ricetta) < 0)
		return esiste_ricetta_ret(T->right, ricetta);
	return esiste_ricetta_ret(T->left, ricetta);
}

void non_aggiungi_ricetta(char comando[MAX + 1]) {
	int quantita;
	if (scanf("%d", &quantita) <= 0)
		return;
	do {
		acquisisci_comando(comando);
		if (stop != 1)
			return;
		if (strcmp(comando, "aggiungi_ricetta") != 0 && strcmp(comando, "rimuovi_ricetta") != 0 && strcmp(comando, "rifornimento") != 0 && strcmp(comando, "ordine") != 0) {
			if (scanf("%d", &quantita) <= 0)
				return;
		}
	} while (strcmp(comando, "aggiungi_ricetta") != 0 && strcmp(comando, "rimuovi_ricetta") != 0 && strcmp(comando, "rifornimento") != 0 && strcmp(comando, "ordine") != 0);
}

void aggiungi_ricetta(char ricetta[MAX + 1], char ingrediente[MAX + 1]) {
	int quantita;
	s_ricette *pre, *cur, *x;
	x = (s_ricette *)malloc(sizeof(s_ricette));
	if (scanf("%d", &quantita) <= 0) {
		return;
	}

	strcpy(x->nome_ricetta, ricetta);
	x->ingredienti = (s_ingrediente *)malloc(sizeof(s_ingrediente));
	x->left = x->p = x->right = NULL;
	x->ingredienti->next = NULL;
	strcpy(x->ingredienti->nome_ingrediente, ingrediente);
	x->ingredienti->quantita = quantita;

	s_ingrediente *ingredienti;

	pre = NULL;
	cur = ricettario;
	while (cur != NULL) {		//cerca il posto nel ricettario (BST) ove insrire x
		pre = cur;
		if (strcmp(x->nome_ricetta, cur->nome_ricetta) < 0)
			cur = cur->left;
		else
			cur = cur->right;
	}
	x->p = pre;
	if (pre == NULL)
		ricettario = x;
	else if (strcmp(x->nome_ricetta, pre->nome_ricetta) < 0)
		pre->left = x;
	else
		pre->right = x;
	ingredienti = x->ingredienti;
	do {
		acquisisci_comando(ingrediente);
		if (stop != 1)
			return;
		if (strcmp(ingrediente, "aggiungi_ricetta") != 0 && strcmp(ingrediente, "rimuovi_ricetta") != 0 && strcmp(ingrediente, "rifornimento") != 0 && strcmp(ingrediente, "ordine") != 0) {
			ingredienti->next = (s_ingrediente *)malloc(sizeof(s_ingrediente));
			ingredienti = ingredienti->next;
			ingredienti->next = NULL;
			strcpy(ingredienti->nome_ingrediente, ingrediente);
			if (scanf("%d", &(ingredienti->quantita)) <= 0)
				return;
		}
	} while (strcmp(ingrediente, "aggiungi_ricetta") != 0 && strcmp(ingrediente, "rimuovi_ricetta") != 0 && strcmp(ingrediente, "rifornimento") != 0 && strcmp(ingrediente, "ordine") != 0);
}

void rimuovi_ricetta(char ricetta[MAX + 1]) {
	if (cerca_ordine_in_coda(ricetta) == 1 || cerca_ordine_in_lista(ricetta) == 1) {
		printf("ordini in sospeso\n");
	}
	else { // cerca nel ricettario: se la trovi stampi 'rimossa', altrimenti ' non presente'
		s_ricette *x = esiste_ricetta_ret(ricettario, ricetta);
		if (x == NULL) {
			printf("non presente\n");
		}
		else {
			rimuovi_ricetta_da_ricettario(ricettario, x);
			printf("rimossa\n");
		}
	}
}

int cerca_ordine_in_lista(char ricetta[MAX + 1]) {
	s_ordini *x;
	x = ordini_testa;
	while (x != NULL) {
		if (strcmp(x->nome_ricetta, ricetta) == 0) {
			return 1;
		}
		x = x->next;
	}
	return 0;
}

int cerca_ordine_in_coda(char ricetta[MAX + 1]) {
	s_ordini *x;
	x = testa_della_coda;
	while (x != NULL) {
		if (strcmp(x->nome_ricetta, ricetta) == 0) {
			return 1;
		}
		x = x->next;
	}
	return 0;
}

void rimuovi_ricetta_da_ricettario(s_ricette *T, s_ricette *x) {
	s_ricette *da_canc, *sottoa;
	if (x->left == NULL || x->right == NULL) {
		da_canc = x;
	} else {
		da_canc = successore_ricettario(x);
	}
	if (da_canc->left != NULL)
		sottoa = da_canc->left;
	else
		sottoa = da_canc->right;
	if (sottoa != NULL)
		sottoa->p = da_canc->p;
	if (da_canc->p == NULL)
		ricettario = sottoa;
	else if (da_canc == da_canc->p->left)
		da_canc->p->left = sottoa;
	else
		da_canc->p->right = sottoa;
	if (da_canc != x) {
		strcpy(x->nome_ricetta, da_canc->nome_ricetta);
		x->ingredienti = da_canc -> ingredienti;
	}
	free(da_canc);
}

s_ricette *successore_ricettario(s_ricette *x) {
	s_ricette *y;
	if (x->right != NULL)
		return min_ricetta(x->right);
	y = x->p;
	while (y != NULL && y->right == x) {
		x = y;
		y = y->p;
	}
	return y;
}

s_ricette *min_ricetta(s_ricette *x) {
	s_ricette *cur = x;
	while (cur->left != NULL)
		cur = cur->left;
	return cur;
}

void rifornimento(char ingrediente[MAX + 1]) {
	int quantita, scadenza;
	do {
		if (scanf("%d", &quantita) <= 0 || scanf("%d", &scadenza) <= 0)
			return;

		s_magazzino *pre = NULL, *cur = magazzino;
		s_stoccaggio *precedente, *attuale, *da_inserire;

		while (cur != NULL && strcmp(cur->nome_ingrediente, ingrediente) != 0) {
			pre = cur;
			if (strcmp(cur->nome_ingrediente, ingrediente) < 0)
				cur = cur->right;
			else
				cur = cur->left;
		}
		if (cur == NULL) { // se l'ingrediente è nuovo, lo creo nell'albero
			cur = (s_magazzino *)malloc(sizeof(s_magazzino));
			strcpy(cur->nome_ingrediente, ingrediente);
			cur->left = cur->right = NULL;
			cur->stoccaggio = NULL;
			cur->p = pre;
			if (pre == NULL) // se l'albero è vuoto, il nouvo nodo è la radice
				magazzino = cur;
			else if (strcmp(pre->nome_ingrediente, cur->nome_ingrediente) < 0) // lo aggancio a sinistra/destra del predecessore
				pre->right = cur;
			else
				pre->left = cur;
		}

		// gestisco le qta degli ingredienti

		da_inserire = (s_stoccaggio *)malloc(sizeof(s_stoccaggio));
		da_inserire->qta = quantita;
		da_inserire->scadenza = scadenza;
		da_inserire->next = da_inserire->prev = NULL;

		if (cur->stoccaggio == NULL) { // lista vuota
			cur->stoccaggio = da_inserire;
		} else {
			attuale = cur->stoccaggio;
			precedente = NULL;

			while (attuale != NULL && attuale->scadenza < scadenza) { // cerco dove inserire
				precedente = attuale;
				attuale = attuale->next;
			}

			if (attuale != NULL && attuale->scadenza == scadenza) {
				attuale->qta += quantita;
				free(da_inserire);
			} else if (precedente == NULL) { // inserisco in testa
				da_inserire->next = cur->stoccaggio;
				cur->stoccaggio->prev = da_inserire;
				cur->stoccaggio = da_inserire;
			} else {
				da_inserire->next = attuale;
				da_inserire->prev = precedente;
				precedente->next = da_inserire;

				if (attuale != NULL) { // se non sto inserendo in coda
					attuale->prev = da_inserire;
				}
			}
		}
		acquisisci_comando(ingrediente);
		if (stop != 1) {
			break;
		}
	} while (strcmp(ingrediente, "aggiungi_ricetta") != 0 && strcmp(ingrediente, "rimuovi_ricetta") != 0 && strcmp(ingrediente, "rifornimento") != 0 && strcmp(ingrediente, "ordine") != 0 && stop > 0);
	printf("rifornito\n");
}

unsigned int calcola_peso_ordine(char ricetta[MAX + 1], unsigned int numero) {
	unsigned int accumulatore = 0;
	s_ingrediente *comanda = cerca_ingredienti(ricettario, ricetta);
	while (comanda != NULL) {
		accumulatore += comanda->quantita;
		comanda = comanda->next;
	}
	return accumulatore * numero;
}

unsigned int ordina(char ricetta[MAX + 1]) {
	s_ingrediente *ric;
	s_stoccaggio *stoc;
	unsigned int numero, accumulatore;
	s_ordini *ordine = (s_ordini *)malloc(sizeof(s_ordini));
	ordine->next = NULL;
	s_magazzino *ingrediente_nel_magazzino;

	// acquisisci il numero di ricette che si vogliono ordinare
	if (scanf("%d", &numero) <= 0)
		return -1;

	strcpy(ordine->nome_ricetta, ricetta);
	ordine->numero = numero;
	ordine->tempo = tempo;
	
	ric = cerca_ingredienti(ricettario, ricetta);
	if (ric == NULL)
		return -1;
	// calcola peso della ricetta e moltiplicalo per il numero

	ordine->peso_totale = calcola_peso_ordine(ricetta, numero);

	while (ric != NULL) {
		accumulatore = 0;
		ingrediente_nel_magazzino = cerca_nel_magazzino(magazzino, ric->nome_ingrediente);
		if(ingrediente_nel_magazzino == NULL) {
			aggiungi_in_coda(ordine);
			return 0;
		}
		stoc = ingrediente_nel_magazzino->stoccaggio;
		while (accumulatore < ric->quantita * numero && stoc != NULL) {
			if (stoc->scadenza <= tempo) { // ingrediente scaduto: va rimosso
				s_stoccaggio *da_eliminare = stoc;
				stoc = stoc->next;
				ingrediente_nel_magazzino->stoccaggio = stoc;
				if(stoc != NULL)
					stoc->prev = NULL;
				free(da_eliminare);
			} else {
				accumulatore += stoc->qta;
				stoc = stoc -> next;
			}
		}
		if (accumulatore < ric->quantita * numero) {
			aggiungi_in_coda(ordine); // lo aggiunge semplicemente in coda
			return 0;
		}
		ric = ric->next;
	}
	//inserisci_in_ordine_di_tempo(ordine);
	
	gestisci_ordine(ordine); // lo aggiunge agli ordini da gestire, elimina gli ingredienti usati dallo stoccaggio.
	inserisci_in_ordine_di_tempo(ordine);
	return 0;
}

void gestisci_ordine(s_ordini *ordine) { // inserisco in coda, prelevo dalla testa
	s_ingrediente *ric;
	s_stoccaggio *stoc;
	unsigned int accumulatore;
	s_magazzino *ingrediente_nel_magazzino;

	ric = cerca_ingredienti(ricettario, ordine->nome_ricetta);
	if (ric == NULL)
		return;

	while (ric != NULL) {
		accumulatore = 0;
		ingrediente_nel_magazzino = cerca_nel_magazzino(magazzino, ric->nome_ingrediente);
		if(ingrediente_nel_magazzino==NULL)
			return;
		stoc = ingrediente_nel_magazzino->stoccaggio;
		while (stoc != NULL && accumulatore < ric->quantita * ordine->numero) {
			if (stoc->scadenza <= tempo) { // ingrediente scaduto: va rimosso
				s_stoccaggio *da_eliminare = stoc;
				stoc = stoc->next;
				
				if (da_eliminare->prev != NULL) {
					da_eliminare->prev->next = stoc;
				} else {
					ingrediente_nel_magazzino->stoccaggio = stoc;
				}

				if (stoc != NULL) {
					stoc->prev = da_eliminare->prev;
				}

				free(da_eliminare);
			} else if (accumulatore + stoc->qta <= ric->quantita * ordine->numero) {
				accumulatore += stoc->qta;
				s_stoccaggio *da_eliminare = stoc;
				stoc = stoc->next;

				// 
				if (da_eliminare->prev != NULL) {
					da_eliminare->prev->next = stoc;
				} else {
					ingrediente_nel_magazzino->stoccaggio = stoc;
				}

				if (stoc != NULL) {
					stoc->prev = da_eliminare->prev;
				}

				free(da_eliminare);
			} else { // caso in cui devo solo modificare la quantità dello stoccaggio
				stoc->qta -= ric->quantita * ordine->numero - accumulatore;
				break;
			}
		}
		ric = ric->next;
	}
	return;
}
/*
 * ATTENZIONE NELLE RIGHE QUI SOPRA!!!!!!!!
 * ATTENZIONE!!!!!
 */

s_ingrediente *cerca_ingredienti(s_ricette *R, char ricetta[MAX + 1]) {
	if (R == NULL)
		return NULL;
	else if (strcmp(R->nome_ricetta, ricetta) == 0)
		return R->ingredienti;
	else if (strcmp(R->nome_ricetta, ricetta) > 0)
		return cerca_ingredienti(R->left, ricetta);
	else
		return cerca_ingredienti(R->right, ricetta);
}

s_magazzino *cerca_nel_magazzino(s_magazzino *mag, char ingrediente[MAX + 1]) {
    if (mag == NULL || strcmp(mag->nome_ingrediente, ingrediente) == 0)
        return mag;
    else if (strcmp(mag->nome_ingrediente, ingrediente) < 0)
        return cerca_nel_magazzino(mag->right, ingrediente);
    else
        return cerca_nel_magazzino(mag->left, ingrediente);
}

void corriere() {
	s_ordini *testa = NULL; // ora inserisco direttamente in ordine di scadenza decrescente, ordinando però in modo STABILE
	unsigned int accumulatore = 0;
	while (ordini_testa != NULL && accumulatore + ordini_testa->peso_totale <= capienza) {
		accumulatore += ordini_testa->peso_totale;

		s_ordini *ordine = ordini_testa;
        ordini_testa = ordini_testa->next;
		if(ordini_testa==NULL)
			ordini_coda=NULL;
        ordine->next = NULL;
		if (testa == NULL) {
			testa = ordine;
		} else {
			s_ordini *j = testa, *prec = NULL;
			while (j != NULL && (j->peso_totale > ordine->peso_totale || (j->peso_totale == ordine->peso_totale && j->tempo < ordine->tempo))) {
				prec = j;
				j = j->next;
			}
			if(prec == NULL) {//inserisci in testa
				ordine->next = testa;
				testa=ordine;
			} else {
				ordine->next=j;
				prec->next=ordine;
			}
		}
	}
	if(testa == NULL) {
		printf("camioncino vuoto\n");
		return;
	}
	s_ordini *x;
	while (testa != NULL) {
		printf("%d %s %d\n", testa->tempo, testa->nome_ricetta, testa->numero);
		x = testa;
		testa = testa->next;
		free(x);
	}
}

void check_ordini() {
	s_ingrediente *ric;
	s_stoccaggio *stoc;
	unsigned int accumulatore;
	s_ordini *ordine = testa_della_coda, *prec = NULL, *cur;
	//s_ordini *da_eliminare;
	
	s_magazzino *ingrediente_nel_magazzino;
	unsigned int sentinella=0;

	while (ordine != NULL) {
		ric = cerca_ingredienti(ricettario, ordine->nome_ricetta);
		while (ric != NULL) {
			accumulatore = 0;
			ingrediente_nel_magazzino = cerca_nel_magazzino(magazzino, ric->nome_ingrediente);
			if(ingrediente_nel_magazzino != NULL){
				stoc = ingrediente_nel_magazzino->stoccaggio;
				while (accumulatore < ric->quantita * ordine->numero && stoc != NULL) {
					if (stoc->scadenza <= tempo) { // ingrediente scaduto: va rimosso
						s_stoccaggio *da_eliminare = stoc;
						stoc = stoc->next;
						ingrediente_nel_magazzino->stoccaggio = stoc;
						if(stoc != NULL)
							stoc->prev = NULL;
						free(da_eliminare);
					} else {
						accumulatore += stoc->qta;
						stoc = stoc->next;
					}
				}
				if (accumulatore < ric->quantita * ordine->numero) { // non posso ancora gestire l'ordine
					prec = ordine;
					ordine = ordine -> next;
					sentinella=1;
					break;
				}
			} else {
				prec = ordine;
				ordine = ordine -> next;
				sentinella=1;
				break;
			}
			ric = ric->next;
		}
		if(sentinella==1){
			sentinella=0;
			continue;
		}
		cur = ordine;
		ordine = ordine -> next;
		// Caso 1: ordine è la testa della lista
		if (prec == NULL) {
			testa_della_coda = cur->next;
			// Se la testa è anche la coda, sistemo la coda
			if (coda_della_coda == cur) {
				coda_della_coda = NULL;
			}
		} else {
			// Caso 2: `ordine` non è la testa della lista
			prec->next = cur->next;
			// Caso 3: `ordine` è la coda della lista
			if (coda_della_coda == cur) {
				coda_della_coda = prec;
			}
		}

		gestisci_ordine(cur); // lo aggiunge agli ordini da gestire, elimina gli ingredienti usati dallo stoccaggio.
		
		inserisci_in_ordine_di_tempo(cur);
	}
}

void 	inserisci_in_ordine_di_tempo(s_ordini *ordine) {
	s_ordini *cur;
	if (ordini_testa == NULL || ordini_testa->tempo >= ordine->tempo) { //caso di inserimento in testa
        ordine->next = ordini_testa;
        ordini_testa = ordine;
		if(ordini_coda == NULL)
			ordini_coda = ordine;
    } else {//caso di inserimento in mezzo
        cur = ordini_testa;
        while (cur->next != NULL && cur->next->tempo < ordine->tempo) {
            cur = cur->next;
        }
        ordine->next = cur->next;
        cur->next = ordine;
		if(ordine->next == NULL)
			ordini_coda=ordine;
    }
}

void aggiungi_in_coda(s_ordini *ordine) {
	if (testa_della_coda == NULL) {
		testa_della_coda = ordine;
		coda_della_coda = ordine;
		testa_della_coda->next = NULL;
	} else {
		coda_della_coda->next = ordine;
		coda_della_coda = ordine;
	}
}