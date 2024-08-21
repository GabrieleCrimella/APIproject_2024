#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#define MAX 255

typedef enum {
    ROSSO,
    NERO
} colore;

typedef struct stoc_ingredienti {
    unsigned int qta;
    unsigned int scadenza;
    struct stoc_ingredienti *prev, *next;
} s_stoccaggio;

typedef struct magazzino {
    char *nome_ingrediente;
    s_stoccaggio *stoccaggio;
    struct magazzino *left, *right, *p;
} s_magazzino;

typedef struct ingrediente {
    s_magazzino *ref_magazzino;
    unsigned int quantita;
    struct ingrediente *next;
} s_ingrediente;

typedef struct ricetta {
    char *nome_ricetta;
    s_ingrediente *ingredienti;
    struct ricetta *left, *right, *p;
    colore c;
} s_ricette;

typedef struct ordini {
    unsigned int tempo;
    s_ricette *ref_ricetta;
    unsigned int numero;
    unsigned int peso_totale;
    struct ordini *next;
} s_ordini;

void acquisisci_comando(char[MAX + 1]);

void aggiungi_ricetta(char[MAX + 1], char[MAX + 1]);

void rimuovi_ricetta(char[MAX + 1]);

void rifornimento(char[MAX + 1]);

unsigned int ordina(char[MAX + 1]);

void corriere();

unsigned int esiste_ricetta(char[MAX + 1]);

s_ricette *esiste_ricetta_ret(char[MAX + 1]);

void non_aggiungi_ricetta(char[MAX + 1]);

void check_ordini();

s_magazzino *cerca_nel_magazzino(s_magazzino *, char[MAX + 1]);

void aggiungi_in_coda(s_ordini *);

int cerca_ordine_in_coda(char[MAX + 1]);

void rimuovi_ricetta_da_ricettario(s_ricette *);

s_ricette *successore_ricettario(s_ricette *);

s_ricette *min_ricetta(s_ricette *);

unsigned int calcola_peso_ordine(char[MAX + 1], unsigned int);

s_ingrediente *cerca_ingredienti(s_ricette *, char[MAX + 1]);

void gestisci_ordine(s_ordini *ordine);

int cerca_ordine_in_lista(char [MAX + 1]);

void inserisci_in_ordine_di_tempo(s_ordini *ordine);

s_magazzino *min_magazzino(s_magazzino *);

s_magazzino *successore_magazzino(s_magazzino *);

void rimuovi_da_magazzino(s_magazzino *);

void rimuovi_ricettario(s_ricette *);

void dealloca_ingredienti(s_ingrediente *);

void rimuovi_magazzino(s_magazzino *);

void dealloca_stoccaggio(s_stoccaggio *);

void rimuovi_ordini_coda();

void rimuovi_ordini_lista();

void aggiungi_rifornimento_a_magazzino(const char *, int, int);

s_magazzino *aggiungi_solo_ricetta_a_magazzino(const char *);

void cambia_ref_ordini(s_ricette *r1, s_ricette *r2);

s_stoccaggio *elimina_stoc_scaduto(s_stoccaggio *stoc, s_magazzino *ingrediente_nel_magazzino);

void riparaRB_inserisci_ricetta(s_ricette *);

void riparaRB_rimozione_ricetta(s_ricette *x);

void togli_scaduti();

s_ordini *ordini_testa = NULL, *ordini_coda = NULL;
// anche questa la gestisco come coda: inserisco in coda e prelevo dalla testa, così ho accesso in O(1) e la tengo intrinsecamente in ordine cronologico
s_ordini *coda_della_coda = NULL, *testa_della_coda = NULL; // gli ultimi due fanno riferimento alla coda. inserisco in coda, prelevo dalla testa
s_ricette *ricettario = NULL;
s_magazzino *magazzino = NULL;
unsigned int stop = 1;
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
                if (esiste_ricetta(ricetta)) {
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
        /*if (tempo % (unsigned int) (periodo / 3) == 0)
            togli_scaduti();
            */
    }

    rimuovi_ricettario(ricettario);
    rimuovi_magazzino(magazzino);
    rimuovi_ordini_lista();
    rimuovi_ordini_coda();

    return 0;
}

void rimuovi_ordini_coda() {
    while (testa_della_coda != NULL) {
        s_ordini *temp = testa_della_coda;
        testa_della_coda = testa_della_coda->next;
        free(temp);
    }
    coda_della_coda = NULL;
}

void rimuovi_ordini_lista() {
    while (ordini_testa != NULL) {
        s_ordini *temp = ordini_testa;
        ordini_testa = ordini_testa->next;
        free(temp);
    }
    ordini_coda = NULL;
}

void rimuovi_magazzino(s_magazzino *M) {
    if (M == NULL)
        return;
    rimuovi_magazzino(M->left);
    rimuovi_magazzino(M->right);
    dealloca_stoccaggio(M->stoccaggio);
    free(M->nome_ingrediente);
    free(M);
}

void dealloca_stoccaggio(s_stoccaggio *stoc) {
    s_stoccaggio *temp;
    while (stoc != NULL) {
        temp = stoc;
        stoc = stoc->next;
        free(temp);
    }
}

void rimuovi_ricettario(s_ricette *R) {
    if (R == NULL) {
        return;
    }
    rimuovi_ricettario(R->left);
    rimuovi_ricettario(R->right);
    dealloca_ingredienti(R->ingredienti);
    free(R->nome_ricetta);
    free(R);
}

void dealloca_ingredienti(s_ingrediente *ingredienti) {
    s_ingrediente *temp;
    while (ingredienti != NULL) {
        temp = ingredienti;
        ingredienti = ingredienti->next;
        free(temp);
    }
}

void acquisisci_comando(char stringa[MAX + 1]) {
    stop = scanf("%s", stringa);
    if (stop != 1)
        stop = -1;
}

unsigned int esiste_ricetta(char ricetta[MAX + 1]) {
    s_ricette *T = ricettario;
    while (T != NULL) {
        int cmp = strcmp(T->nome_ricetta, ricetta);
        if (cmp == 0)
            return 1;
        else if (cmp < 0)
            T = T->right;
        else
            T = T->left;
    }
    return 0;
}

s_ricette *esiste_ricetta_ret(char ricetta[MAX + 1]) {
    s_ricette *T = ricettario;
    while (T != NULL) {
        int cmp = strcmp(T->nome_ricetta, ricetta);
        if (cmp == 0)
            return T;
        else if (cmp < 0)
            T = T->right;
        else
            T = T->left;
    }
    return NULL;
}

void non_aggiungi_ricetta(char comando[MAX + 1]) {
    int quantita;
    if (scanf("%d", &quantita) <= 0)
        return;
    do {
        acquisisci_comando(comando);
        if (stop != 1)
            return;
        if (strcmp(comando, "aggiungi_ricetta") != 0 && strcmp(comando, "rimuovi_ricetta") != 0 &&
            strcmp(comando, "rifornimento") != 0 && strcmp(comando, "ordine") != 0) {
            if (scanf("%d", &quantita) <= 0)
                return;
        }
    } while (strcmp(comando, "aggiungi_ricetta") != 0 && strcmp(comando, "rimuovi_ricetta") != 0 &&
             strcmp(comando, "rifornimento") != 0 && strcmp(comando, "ordine") != 0);
}

void aggiungi_ricetta(char ricetta[MAX + 1], char ingrediente[MAX + 1]) {
    int quantita;
    s_ricette *pre, *cur, *x;
    x = (s_ricette *) malloc(sizeof(s_ricette));
    if (scanf("%d", &quantita) <= 0) {
        free(x);
        return;
    }
    x->nome_ricetta = (char *) malloc(sizeof(char) * (strlen(ricetta) + 1));
    strcpy(x->nome_ricetta, ricetta);
    x->ingredienti = (s_ingrediente *) malloc(sizeof(s_ingrediente));
    x->left = x->p = x->right = NULL;
    x->ingredienti->next = NULL;
    x->ingredienti->ref_magazzino = aggiungi_solo_ricetta_a_magazzino(ingrediente);
    x->ingredienti->quantita = quantita;

    s_ingrediente *ingredienti;

    pre = NULL;
    cur = ricettario;
    while (cur != NULL) {        //cerca il posto nel ricettario (BST) ove insrire x
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
    riparaRB_inserisci_ricetta(x);
    do {
        acquisisci_comando(ingrediente);
        if (stop != 1)
            return;
        if (strcmp(ingrediente, "aggiungi_ricetta") != 0 && strcmp(ingrediente, "rimuovi_ricetta") != 0 &&
            strcmp(ingrediente, "rifornimento") != 0 && strcmp(ingrediente, "ordine") != 0) {
            ingredienti->next = (s_ingrediente *) malloc(sizeof(s_ingrediente));
            if (ingredienti->next == NULL) {
                free(x->nome_ricetta);
                free(x);
                return;
            }
            ingredienti = ingredienti->next;
            ingredienti->ref_magazzino = aggiungi_solo_ricetta_a_magazzino(ingrediente);
            ingredienti->next = NULL;
            if (scanf("%d", &(ingredienti->quantita)) <= 0) {
                dealloca_ingredienti(x->ingredienti);
                free(x->nome_ricetta);
                free(x);
                return;
            }

        }
    } while (strcmp(ingrediente, "aggiungi_ricetta") != 0 && strcmp(ingrediente, "rimuovi_ricetta") != 0 &&
             strcmp(ingrediente, "rifornimento") != 0 && strcmp(ingrediente, "ordine") != 0);
}

void rimuovi_ricetta(char ricetta[MAX + 1]) {
    if (cerca_ordine_in_coda(ricetta) == 1 || cerca_ordine_in_lista(ricetta) == 1) {
        printf("ordini in sospeso\n");
    } else { // cerca nel ricettario: se la trovi stampi 'rimossa', altrimenti 'non presente'
        s_ricette *x = esiste_ricetta_ret(ricetta);
        if (x == NULL) {
            printf("non presente\n");
        } else {
            rimuovi_ricetta_da_ricettario(x);
            printf("rimossa\n");
        }
    }
}

int cerca_ordine_in_lista(char ricetta[MAX + 1]) {
    s_ordini *x = ordini_testa;
    while (x != NULL) {
        if (strcmp(x->ref_ricetta->nome_ricetta, ricetta) == 0) {
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
        if (strcmp(x->ref_ricetta->nome_ricetta, ricetta) == 0) {
            return 1;
        }
        x = x->next;
    }
    return 0;
}

void rimuovi_ricetta_da_ricettario(s_ricette *x) {
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
        free(x->nome_ricetta);
        cambia_ref_ordini(da_canc, x);
        x->nome_ricetta = da_canc->nome_ricetta;
        x->ingredienti = da_canc->ingredienti;
    } else {
        dealloca_ingredienti(da_canc->ingredienti);
        free(da_canc->nome_ricetta);
    }

    if (da_canc->c == NERO)
        riparaRB_rimozione_ricetta(sottoa);

    free(da_canc);
}

void cambia_ref_ordini(s_ricette *r1, s_ricette *r2) {
    s_ordini *temp;
    temp = ordini_testa;
    while (temp != NULL) {
        if (temp->ref_ricetta == r1)
            temp->ref_ricetta = r2;
        temp = temp->next;
    }
    temp = testa_della_coda;
    while (temp != NULL) {
        if (temp->ref_ricetta == r1)
            temp->ref_ricetta = r2;
        temp = temp->next;
    }
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
    while (x->left != NULL)
        x = x->left;
    return x;
}

void rifornimento(char ingrediente[MAX + 1]) {
    int quantita, scadenza;
    do {
        if (scanf("%d", &quantita) <= 0 || scanf("%d", &scadenza) <= 0)
            return;

        aggiungi_rifornimento_a_magazzino(ingrediente, quantita, scadenza);

        acquisisci_comando(ingrediente);
        if (stop != 1) {
            break;
        }
    } while (strcmp(ingrediente, "aggiungi_ricetta") != 0 && strcmp(ingrediente, "rimuovi_ricetta") != 0 &&
             strcmp(ingrediente, "rifornimento") != 0 && strcmp(ingrediente, "ordine") != 0 && stop == 1);
    printf("rifornito\n");
}

void aggiungi_rifornimento_a_magazzino(const char *ingrediente, int quantita, int scadenza) {
    s_stoccaggio *precedente, *attuale, *da_inserire;
    s_magazzino *cur = aggiungi_solo_ricetta_a_magazzino(ingrediente);

    // gestisco le qta degli ingredienti
    da_inserire = (s_stoccaggio *) malloc(sizeof(s_stoccaggio));
    da_inserire->qta = quantita;
    da_inserire->scadenza = scadenza;
    da_inserire->next = da_inserire->prev = NULL;

    if (cur->stoccaggio == NULL) { // lista vuota
        cur->stoccaggio = da_inserire;
    } else {
        attuale = cur->stoccaggio;
        precedente = NULL;

        while (attuale != NULL && attuale->scadenza < scadenza) { // cerco dove inserire
            if (attuale->scadenza <= tempo) {
                attuale = elimina_stoc_scaduto(attuale, cur);
            } else {
                precedente = attuale;
                attuale = attuale->next;
            }
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
}

s_magazzino *aggiungi_solo_ricetta_a_magazzino(const char *ingrediente) {
    s_magazzino *pre = NULL, *cur = magazzino;
    while (cur != NULL && strcmp(cur->nome_ingrediente, ingrediente) != 0) {
        pre = cur;
        if (strcmp(cur->nome_ingrediente, ingrediente) < 0)
            cur = cur->right;
        else
            cur = cur->left;
    }
    if (cur == NULL) { // se l'ingrediente è nuovo, lo creo nell'albero
        cur = (s_magazzino *) malloc(sizeof(s_magazzino));
        cur->nome_ingrediente = (char *) malloc(sizeof(char) * (strlen(ingrediente) + 1));
        strcpy(cur->nome_ingrediente, ingrediente);
        cur->left = cur->right = NULL;
        cur->stoccaggio = NULL;
        cur->p = pre;
        if (pre == NULL) // se l'albero è vuoto, il nouvo nodo è la radice
            magazzino = cur;
        else if (strcmp(pre->nome_ingrediente, cur->nome_ingrediente) <
                 0) // lo aggancio a sinistra/destra del predecessore
            pre->right = cur;
        else
            pre->left = cur;
    }
    return cur;
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
    s_ordini *ordine = (s_ordini *) malloc(sizeof(s_ordini));
    ordine->next = NULL;
    s_magazzino *ingrediente_nel_magazzino;

    // acquisisci il numero di ricette che si vogliono ordinare
    if (scanf("%d", &numero) <= 0) {
        free(ordine);
        return -1;
    }

    ordine->ref_ricetta = esiste_ricetta_ret(ricetta);
    if (ordine->ref_ricetta == NULL) {
        free(ordine);
        return -1;
    }
    ordine->numero = numero;
    ordine->tempo = tempo;

    ric = ordine->ref_ricetta->ingredienti;
    if (ric == NULL) {
        free(ordine);
        return -1;
    }

    // calcola peso della ricetta e moltiplicalo per il numero

    ordine->peso_totale = calcola_peso_ordine(ricetta, numero);

    while (ric != NULL) {
        accumulatore = 0;
        ingrediente_nel_magazzino = ric->ref_magazzino;
        if (ingrediente_nel_magazzino == NULL) {
            aggiungi_in_coda(ordine);
            return 0;
        }
        stoc = ingrediente_nel_magazzino->stoccaggio;
        if (stoc == NULL) {
            aggiungi_in_coda(ordine);
            return 0;
        }
        while (accumulatore < ric->quantita * numero && stoc != NULL) {
            if (stoc->scadenza <= tempo) { // ingrediente scaduto: va rimosso
                s_stoccaggio *da_eliminare = stoc;
                stoc = stoc->next;
                ingrediente_nel_magazzino->stoccaggio = stoc;
                if (stoc != NULL)
                    stoc->prev = NULL;
                free(da_eliminare);
            } else {
                accumulatore += stoc->qta;
                stoc = stoc->next;
            }
        }
        if (accumulatore < ric->quantita * numero) {
            aggiungi_in_coda(ordine); // lo aggiunge semplicemente in coda
            return 0;
        }
        ric = ric->next;
    }

    gestisci_ordine(ordine); // lo aggiunge agli ordini da gestire, elimina gli ingredienti usati dallo stoccaggio.
    inserisci_in_ordine_di_tempo(ordine);
    return 0;
}

void gestisci_ordine(s_ordini *ordine) { // inserisco in coda, prelevo dalla testa
    s_ingrediente *ric;
    s_stoccaggio *stoc;
    unsigned int accumulatore;
    s_magazzino *ingrediente_nel_magazzino;

    ric = ordine->ref_ricetta->ingredienti;
    if (ric == NULL) {
        free(ordine);
        return;
    }

    //while(ric!=NULL){
    while (ric != NULL && ordine->ref_ricetta != NULL) {
        accumulatore = 0;
        ingrediente_nel_magazzino = ric->ref_magazzino;
        if (ingrediente_nel_magazzino == NULL)
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
            /*
            if (ingrediente_nel_magazzino->stoccaggio == NULL)
                rimuovi_da_magazzino(ingrediente_nel_magazzino);*/
        }
        ric = ric->next;
    }

    return;
}

/*
 * ATTENZIONE NELLE RIGHE QUI SOPRA!!!!!!!!
 * ATTENZIONE!!!!!
 */

s_magazzino *successore_magazzino(s_magazzino *elemento) {
    s_magazzino *y;
    if (elemento->right != NULL)
        return min_magazzino(elemento->right);
    y = elemento->p;
    while (y != NULL && y->right == elemento) {
        elemento = y;
        y = y->p;
    }
    return y;
}

s_magazzino *min_magazzino(s_magazzino *x) {
    s_magazzino *cur = x;
    while (cur->left != NULL)
        cur = cur->left;
    return cur;
}

void rimuovi_da_magazzino(s_magazzino *x) {
    s_magazzino *da_canc, *sottoa;

    s_stoccaggio *stoc = x->stoccaggio;
    while (stoc != NULL) {
        s_stoccaggio *da_eliminare = stoc;
        stoc = stoc->next;
        free(da_eliminare);
    }

    if (x->left == NULL || x->right == NULL) {
        da_canc = x;
    } else {
        da_canc = successore_magazzino(x);
    }
    if (da_canc->left != NULL)
        sottoa = da_canc->left;
    else
        sottoa = da_canc->right;
    if (sottoa != NULL)
        sottoa->p = da_canc->p;
    if (da_canc->p == NULL)
        magazzino = sottoa;
    else if (da_canc == da_canc->p->left)
        da_canc->p->left = sottoa;
    else
        da_canc->p->right = sottoa;
    if (da_canc != x) {
        x->nome_ingrediente = (char *) realloc(x->nome_ingrediente,
                                               sizeof(char) * (strlen(da_canc->nome_ingrediente) + 1));
        strcpy(x->nome_ingrediente, da_canc->nome_ingrediente);
        x->stoccaggio = da_canc->stoccaggio;
    }
    free(da_canc->nome_ingrediente);
    free(da_canc);
}


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
        if (ordini_testa == NULL)
            ordini_coda = NULL;
        ordine->next = NULL;
        if (testa == NULL) {
            testa = ordine;
        } else {
            s_ordini *j = testa, *prec = NULL;
            while (j != NULL && (j->peso_totale > ordine->peso_totale ||
                                 (j->peso_totale == ordine->peso_totale && j->tempo < ordine->tempo))) {
                prec = j;
                j = j->next;
            }
            if (prec == NULL) {//inserisci in testa
                ordine->next = testa;
                testa = ordine;
            } else {
                ordine->next = j;
                prec->next = ordine;
            }
        }
    }
    if (testa == NULL) {
        printf("camioncino vuoto\n");
        return;
    }
    s_ordini *x;
    while (testa != NULL) {
        printf("%d %s %d\n", testa->tempo, testa->ref_ricetta->nome_ricetta, testa->numero);
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

    s_magazzino *ingrediente_nel_magazzino;
    unsigned int sentinella = 0;

    while (ordine != NULL) {
        //ric = cerca_ingredienti(ricettario, ordine->nome_ricetta);
        ric = ordine->ref_ricetta->ingredienti;
        while (ric != NULL) {
            accumulatore = 0;
            ingrediente_nel_magazzino = ric->ref_magazzino;
            if (ingrediente_nel_magazzino != NULL) {
                stoc = ingrediente_nel_magazzino->stoccaggio;
                unsigned int richiesta_totale = ric->quantita * ordine->numero;
                while (stoc != NULL && accumulatore < richiesta_totale) {
                    if (stoc->scadenza > tempo)
                        accumulatore += stoc->qta;
                    stoc = stoc->next;
                }
                if (accumulatore < ric->quantita * ordine->numero) { // non posso ancora gestire l'ordine
                    prec = ordine;
                    ordine = ordine->next;
                    sentinella = 1;
                    break;
                }
            } else {
                prec = ordine;
                ordine = ordine->next;
                sentinella = 1;
                break;
            }
            ric = ric->next;
        }
        if (sentinella == 1) {
            sentinella = 0;
            continue;
        }
        cur = ordine;
        ordine = ordine->next;
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

void togli_scaduti() {
    s_ingrediente *ric;
    s_stoccaggio *stoc;
    s_ordini *ordine = testa_della_coda;
    s_magazzino *ingrediente_nel_magazzino;
    while (ordine != NULL) {
        ric = ordine->ref_ricetta->ingredienti;
        while (ric != NULL) {
            ingrediente_nel_magazzino = ric->ref_magazzino;
            if (ingrediente_nel_magazzino != NULL) {
                stoc = ingrediente_nel_magazzino->stoccaggio;
                while (stoc != NULL) {
                    if (stoc->scadenza <= tempo) { // ingrediente scaduto: va rimosso
                        stoc = elimina_stoc_scaduto(stoc, ingrediente_nel_magazzino);
                    } else {
                        break;
                    }
                }
            }
            ric = ric->next;
        }
        ordine = ordine->next;
    }
}

s_stoccaggio *elimina_stoc_scaduto(s_stoccaggio *stoc, s_magazzino *ingrediente_nel_magazzino) {
    s_stoccaggio *da_eliminare = stoc;
    stoc = stoc->next;
    ingrediente_nel_magazzino->stoccaggio = stoc;
    if (stoc != NULL)
        stoc->prev = NULL;
    free(da_eliminare);
    return stoc;
}

void inserisci_in_ordine_di_tempo(s_ordini *ordine) {
    s_ordini *cur;
    if (ordini_testa == NULL || ordini_testa->tempo >= ordine->tempo) { //caso di inserimento in testa
        ordine->next = ordini_testa;
        ordini_testa = ordine;
        if (ordini_coda == NULL)
            ordini_coda = ordine;
    } else {//caso di inserimento in mezzo
        cur = ordini_testa;
        while (cur->next != NULL && cur->next->tempo < ordine->tempo) {
            cur = cur->next;
        }
        ordine->next = cur->next;
        cur->next = ordine;
        if (ordine->next == NULL)
            ordini_coda = ordine;
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

void rotazione_sx_ricetta(s_ricette *x) {
    s_ricette *y = x->right;
    x->right = y->left;

    if (x->right != NULL)
        x->right->p = x;

    y->p = x->p;

    if (x->p == NULL)
        ricettario = y;
    else if (x == x->p->left)
        x->p->left = y;
    else x->p->right = y;

    y->left = x;
    x->p = y;
}

void rotazione_dx_ricetta(s_ricette *y) {
    s_ricette *x = y->left;
    y->left = x->right;

    if (x->right != NULL)
        x->right->p = y;

    x->p = y->p;

    if (x->p == NULL)
        ricettario = x;
    else if (y == y->p->left)
        y->p->left = x;
    else y->p->right = x;

    x->right = y;
    y->p = x;
}

void riparaRB_inserisci_ricetta(s_ricette *x) {
    s_ricette *y;

    while (x != ricettario && x->p->c == ROSSO) {
        if (x->p == x->p->p->left) {
            y = x->p->p->right;
            if (y != NULL && y->c == ROSSO) {
                x->p->c = NERO;
                y->c = NERO;
                x->p->p->c = ROSSO;
                x = x->p->p;
            } else {
                if (x == x->p->right) {
                    x = x->p;
                    rotazione_sx_ricetta(x);
                }
                x->p->c = NERO;
                x->p->p->c = ROSSO;
                rotazione_dx_ricetta(x->p->p);
            }
        } else {
            y = x->p->p->left;
            if (y != NULL && y->c == ROSSO) {
                x->p->c = NERO;
                y->c = NERO;
                x->p->p->c = ROSSO;
                x = x->p->p;
            } else {
                if (x == x->p->left) {
                    x = x->p;
                    rotazione_dx_ricetta(x);
                }
                x->p->c = NERO;
                x->p->p->c = ROSSO;
                rotazione_sx_ricetta(x->p->p);
            }
        }
    }
    ricettario->c = NERO;
}

void riparaRB_rimozione_ricetta(s_ricette *x) {
    s_ricette *w = NULL; // Inizializza w a NULL

    while (x != ricettario && x != NULL && x->c == NERO) {
        if (x->p != NULL && x == x->p->left) {
            w = x->p->right;
            if (w != NULL && w->c == ROSSO) {
                w->c = NERO;
                x->p->c = ROSSO;
                rotazione_sx_ricetta(x->p);
                w = x->p->right;
            }
            if ((w == NULL || (w->left == NULL || w->left->c == NERO)) &&
                (w == NULL || (w->right == NULL || w->right->c == NERO))) {
                if (w != NULL)
                    w->c = ROSSO;
                x = (x->p != NULL) ? x->p : NULL;
            } else {
                if (w != NULL && (w->right == NULL || w->right->c == NERO)) {
                    if (w->left != NULL)
                        w->left->c = NERO;
                    w->c = ROSSO;
                    rotazione_dx_ricetta(w);
                    w = x->p->right;
                }
                if (w != NULL)
                    w->c = x->p->c;
                x->p->c = NERO;
                if (w->right != NULL)
                    w->right->c = NERO;
                rotazione_sx_ricetta(x->p);
                x = ricettario;
            }
        } else {
            if (x->p != NULL) w = x->p->left;
            if (w != NULL && w->c == ROSSO) {
                w->c = NERO;
                x->p->c = ROSSO;
                rotazione_dx_ricetta(x->p);
                w = x->p->left;
            }
            if ((w == NULL || (w->right == NULL || w->right->c == NERO)) &&
                (w == NULL || (w->left == NULL || w->left->c == NERO))) {
                if (w != NULL)
                    w->c = ROSSO;
                x = (x->p != NULL) ? x->p : NULL;
            } else {
                if (w != NULL && (w->left == NULL || w->left->c == NERO)) {
                    if (w->right != NULL)
                        w->right->c = NERO;
                    w->c = ROSSO;
                    rotazione_sx_ricetta(w);
                    w = x->p->left;
                }
                if (w != NULL)
                    w->c = x->p->c;
                x->p->c = NERO;
                if (w->left != NULL)
                    w->left->c = NERO;
                rotazione_dx_ricetta(x->p);
                x = ricettario;
            }
        }
    }
    if (x != NULL) x->c = NERO;
}
