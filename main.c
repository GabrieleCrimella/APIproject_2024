#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#define MAX 23

typedef struct stoc_ingredienti {
    unsigned int qta;
    unsigned int scadenza;
    struct stoc_ingredienti *prev, *next;
} s_stoccaggio;

typedef struct magazzino {
    char nome_ingrediente[MAX + 1];
    s_stoccaggio *stoccaggio;
    int somma_qta;
    struct magazzino *left, *right;
} s_magazzino;

typedef struct ingrediente {
    s_magazzino *ref_magazzino;
    unsigned int quantita;
    struct ingrediente *next;
} s_ingrediente;

typedef struct ricetta {
    char nome_ricetta[MAX + 1];
    s_ingrediente *ingredienti;
    int peso;
    struct ricetta *left, *right, *p;
} s_ricette;

typedef struct ordini {
    unsigned int tempo;
    s_ricette *ref_ricetta;
    unsigned int numero;
    unsigned int peso_totale;
    struct ordini *next;
    s_magazzino *mancante;
    int da_ricetta_ne_servono;
} s_ordini;

void acquisisci_comando(char[MAX + 1]);

void aggiungi_ricetta(char[MAX + 1], char[MAX + 1]);

void rimuovi_ricetta(char[MAX + 1]);

void rifornimento();

unsigned int ordina(char[MAX + 1]);

void corriere();

unsigned int esiste_ricetta(s_ricette *, char[MAX + 1]);

s_ricette *esiste_ricetta_ret(s_ricette *, char[MAX + 1]);

void non_aggiungi_ricetta(char[MAX + 1]);

void check_ordini();

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

void rimuovi_ricettario(s_ricette *);

void dealloca_ingredienti(s_ingrediente *);

void rimuovi_magazzino(s_magazzino *);

void dealloca_stoccaggio(s_stoccaggio *);

void rimuovi_ordini_coda();

void rimuovi_ordini_lista();

void aggiungi_rifornimento_a_magazzino(const char *, int, int);

s_magazzino *aggiungi_solo_ricetta_a_magazzino(const char *);

void cambia_ref_ordini(s_ricette *r1, s_ricette *r2);

void butta_scaduti(s_magazzino *pMagazzino);

s_ordini *ordini_testa = NULL;
// anche questa la gestisco come coda: inserisco in coda e prelevo dalla testa, così ho accesso in O(1) e la tengo intrinsecamente in ordine cronologico
s_ordini *coda_della_coda = NULL, *testa_della_coda = NULL; // gli ultimi due fanno riferimento alla coda. inserisco in coda, prelevo dalla testa
s_ricette *ricettario = NULL;
s_magazzino *magazzino = NULL;
int stop = 1;
unsigned int tempo = 0, periodo;
int scelta;
unsigned int capienza;

int main() {
    char comando[MAX + 1];
    char ricetta[MAX + 1];

    if (scanf("%d %d\n", &periodo, &capienza) <= 0) {
        return -1;
    }

    acquisisci_comando(comando);
    while (stop > 0) {
        if (scelta == 1) {
            acquisisci_comando(ricetta);
            acquisisci_comando(comando);
            if (stop == 1) {
                if (esiste_ricetta(ricettario, ricetta)) {
                    printf("ignorato\n");
                    non_aggiungi_ricetta(comando);
                } else {
                    aggiungi_ricetta(ricetta, comando);
                    printf("aggiunta\n");
                    acquisisci_comando(comando);
                }
            } else
                break;
        } else if (scelta == 2) {
            acquisisci_comando(comando);
            rimuovi_ricetta(comando);
            acquisisci_comando(comando);
        } else if (scelta == 3) {
            rifornimento();
            check_ordini(); // controlla se qualche ordine dalla coda pulò essere gestito
            acquisisci_comando(comando);
        } else if (scelta == 4) {
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
}

void rimuovi_magazzino(s_magazzino *M) {
    if (M == NULL)
        return;
    rimuovi_magazzino(M->left);
    rimuovi_magazzino(M->right);
    dealloca_stoccaggio(M->stoccaggio);
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
    if (strcmp(stringa, "aggiungi_ricetta") == 0)
        scelta = 1;
    else if (strcmp(stringa, "rimuovi_ricetta") == 0)
        scelta = 2;
    else if (strcmp(stringa, "rifornimento") == 0)
        scelta = 3;
    else if (strcmp(stringa, "ordine") == 0)
        scelta = 4;
    else
        scelta = 0;

}

unsigned int esiste_ricetta(s_ricette *T, char ricetta[MAX + 1]) {
    while (T != NULL) {
        int risultato = strcmp(T->nome_ricetta, ricetta);
        if (risultato == 0) {
            return 1;
        } else if (risultato < 0) {
            T = T->right;
        } else {
            T = T->left;
        }
    }
    return 0;
}

s_ricette *esiste_ricetta_ret(s_ricette *T, char ricetta[MAX + 1]) {
    while (T != NULL) {
        int risultato = strcmp(T->nome_ricetta, ricetta);
        if (risultato == 0) {
            return T;
        } else if (risultato < 0) {
            T = T->right;
        } else {
            T = T->left;
        }
    }
    return NULL;
}

void non_aggiungi_ricetta(char comando[MAX + 1]) {
    char temp[1000];
    if (fgets(temp, sizeof(temp), stdin) != NULL)
        acquisisci_comando(comando);
}

void aggiungi_ricetta(char ricetta[MAX + 1], char ingrediente[MAX + 1]) {
    int quantita;
    s_ricette *pre, *cur, *x;
    x = (s_ricette *) malloc(sizeof(s_ricette));
    if (x == NULL || scanf("%d", &quantita) <= 0) {
        free(x);
        return;
    }
    strcpy(x->nome_ricetta, ricetta);
    x->ingredienti = (s_ingrediente *) malloc(sizeof(s_ingrediente));
    if (x->ingredienti == NULL) {
        free(x);
        return;
    }
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


    char *linea = NULL;
    size_t lunghezza = 0;
    ssize_t letto;
    letto = getline(&linea, &lunghezza, stdin);
    if (letto > 0 && linea[letto - 1] == '\n') {
        linea[letto - 1] = '\0';
    }
    char *token = strtok(linea, " ");
    while (token != NULL) {
        ingrediente = token;
        token = strtok(NULL, " ");

        ingredienti->next = (s_ingrediente *) malloc(sizeof(s_ingrediente));
        if (ingredienti->next == NULL) {
            dealloca_ingredienti(x->ingredienti);
            free(x);
            return;
        }
        ingredienti = ingredienti->next;
        ingredienti->ref_magazzino = aggiungi_solo_ricetta_a_magazzino(ingrediente);
        ingredienti->next = NULL;
        ingredienti->quantita = atoi(token);

        token = strtok(NULL, " ");
    }

    x->peso = calcola_peso_ordine(ricetta, 1);
    free(linea);
}

void rimuovi_ricetta(char ricetta[MAX + 1]) {
    if (cerca_ordine_in_coda(ricetta) == 1 || cerca_ordine_in_lista(ricetta) == 1) {
        printf("ordini in sospeso\n");
    } else { // cerca nel ricettario: se la trovi stampi 'rimossa', altrimenti 'non presente'
        s_ricette *x = esiste_ricetta_ret(ricettario, ricetta);
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
        cambia_ref_ordini(da_canc, x);
        strcpy(x->nome_ricetta, da_canc->nome_ricetta);
        x->ingredienti = da_canc->ingredienti;
        x->peso = da_canc->peso;
    } else {
        dealloca_ingredienti(da_canc->ingredienti);
    }
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

void rifornimento() {
    int quantita, scadenza;
    char *linea = NULL, *ingrediente;
    size_t lunghezza = 0;
    ssize_t letto;

    letto = getline(&linea, &lunghezza, stdin);
    if (letto > 0 && linea[letto - 1] == '\n') {
        linea[letto - 1] = '\0';
    }

    char *token = strtok(linea, " ");

    while (token != NULL) {
        ingrediente = token;
        token = strtok(NULL, " ");
        quantita = atoi(token);

        token = strtok(NULL, " ");
        scadenza = atoi(token);

        aggiungi_rifornimento_a_magazzino(ingrediente, quantita, scadenza);

        token = strtok(NULL, " ");

    }
    printf("rifornito\n");
    free(linea);
}

void aggiungi_rifornimento_a_magazzino(const char *ingrediente, int quantita, int scadenza) {
    s_stoccaggio *precedente, *attuale, *da_inserire;
    s_magazzino *cur = aggiungi_solo_ricetta_a_magazzino(ingrediente);

    // gestisco le qta degli ingredienti
    da_inserire = (s_stoccaggio *) malloc(sizeof(s_stoccaggio));
    da_inserire->qta = quantita;
    da_inserire->scadenza = scadenza;
    da_inserire->next = da_inserire->prev = NULL;

    cur->somma_qta += quantita;

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
        strcpy(cur->nome_ingrediente, ingrediente);
        cur->left = cur->right = NULL;
        cur->stoccaggio = NULL;
        cur->somma_qta = 0;
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
    //s_stoccaggio *stoc;
    unsigned int numero;
    //int accumulatore;
    s_ordini *ordine = (s_ordini *) malloc(sizeof(s_ordini));
    ordine->next = NULL;
    s_magazzino *ingrediente_nel_magazzino;

    // acquisisci il numero di ricette che si vogliono ordinare
    if (scanf("%d", &numero) <= 0) {
        free(ordine);
        return -1;
    }

    ordine->ref_ricetta = esiste_ricetta_ret(ricettario, ricetta);
    if (ordine->ref_ricetta == NULL) {
        free(ordine);
        return -1;
    }
    ordine->numero = numero;
    ordine->tempo = tempo;
    ordine->mancante = NULL;
    ordine->da_ricetta_ne_servono = 0;

    ric = ordine->ref_ricetta->ingredienti;
    if (ric == NULL) {
        free(ordine);
        return -1;
    }

    // calcola peso della ricetta e moltiplicalo per il numero

    ordine->peso_totale = ordine->ref_ricetta->peso * numero;

    while (ric != NULL) {
        //accumulatore = 0;
        ingrediente_nel_magazzino = ric->ref_magazzino;
        if (ingrediente_nel_magazzino == NULL) {
            aggiungi_in_coda(ordine);
            return 0;
        }

        butta_scaduti(ingrediente_nel_magazzino);

        if (ingrediente_nel_magazzino->somma_qta < ric->quantita * numero) {
            ordine->mancante = ingrediente_nel_magazzino;
            ordine->da_ricetta_ne_servono = ric->quantita;
            aggiungi_in_coda(ordine);
            return 0;
        }
        /*
        stoc = ingrediente_nel_magazzino->stoccaggio;
        if (stoc == NULL) {
            aggiungi_in_coda(ordine);
            return 0;
        }
        while (accumulatore < ric->quantita * numero && stoc != NULL) {
            if (stoc->scadenza <= tempo) { // ingrediente scaduto: va rimosso
                ingrediente_nel_magazzino->somma_qta -= stoc->qta;
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
         */
        ric = ric->next;
    }

    gestisci_ordine(ordine); // lo aggiunge agli ordini da gestire, elimina gli ingredienti usati dallo stoccaggio.
    inserisci_in_ordine_di_tempo(ordine);
    return 0;
}

void butta_scaduti(s_magazzino *pMagazzino) {
    s_stoccaggio *cur = pMagazzino->stoccaggio;
    while (cur != NULL) {
        if (cur->scadenza <= tempo) {
            pMagazzino->somma_qta -= cur->qta;
            s_stoccaggio *da_eliminare = cur;
            cur = cur->next;
            pMagazzino->stoccaggio = cur;
            if (cur != NULL)
                cur->prev = NULL;
            free(da_eliminare);
        } else {
            return;
        }
    }

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
            if (accumulatore + stoc->qta <= ric->quantita * ordine->numero) {
                ingrediente_nel_magazzino->somma_qta -= stoc->qta;
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
                ingrediente_nel_magazzino->somma_qta -= ric->quantita * ordine->numero - accumulatore;
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
    while (R != NULL) {
        int risultato = strcmp(R->nome_ricetta, ricetta);
        if (risultato == 0) {
            return R->ingredienti;
        } else if (risultato > 0) {
            R = R->left;
        } else {
            R = R->right;
        }
    }
    return NULL;
}

void corriere() {
    s_ordini *testa = NULL;
    unsigned int accumulatore = 0;

    s_ordini *temp = ordini_testa;
    s_ordini *prev = NULL;
    while (temp != NULL && accumulatore + temp->peso_totale <= capienza) {
        accumulatore += temp->peso_totale;
        if (prev != NULL) {
            prev->next = temp->next; // tolgo da una parte, metto nell'altra
        } else {
            ordini_testa = temp->next;
        }
        temp->next = NULL;

        if (testa == NULL ||
            (testa->peso_totale < temp->peso_totale) ||
            (testa->peso_totale == temp->peso_totale && testa->tempo > temp->tempo)) {
            temp->next = testa;
            testa = temp;
        } else {
            s_ordini *curr = testa;
            while (curr->next != NULL &&
                   (curr->next->peso_totale > temp->peso_totale ||
                    (curr->next->peso_totale == temp->peso_totale && curr->next->tempo <= temp->tempo))) {
                curr = curr->next;
            }
            temp->next = curr->next;
            curr->next = temp;
        }

        // procedo
        temp = (prev != NULL) ? prev->next : ordini_testa;
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
    //s_stoccaggio *stoc;
    //unsigned int accumulatore;
    s_ordini *ordine = testa_della_coda, *prec = NULL, *cur;
    //s_ordini *da_eliminare;

    s_magazzino *ingrediente_nel_magazzino;
    unsigned int sentinella = 0;

    while (ordine != NULL) {
        int qti = ordine->da_ricetta_ne_servono;
        if (ordine->mancante->somma_qta >= qti * ordine->numero) {
            ordine->mancante = NULL;

            //ric = cerca_ingredienti(ricettario, ordine->nome_ricetta);
            ric = ordine->ref_ricetta->ingredienti;
            while (ric != NULL) {
                //accumulatore = 0;
                ingrediente_nel_magazzino = ric->ref_magazzino;

                if (ingrediente_nel_magazzino != NULL) {
                    butta_scaduti(ingrediente_nel_magazzino);
                    if (ingrediente_nel_magazzino->somma_qta < ric->quantita * ordine->numero) {
                        ordine->mancante = ingrediente_nel_magazzino;
                        ordine->da_ricetta_ne_servono = ric->quantita;
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
        } else {
            prec = ordine;
            ordine = ordine->next;
            sentinella = 0;
            continue;
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


void inserisci_in_ordine_di_tempo(s_ordini *ordine) {
    s_ordini *cur;
    if (ordini_testa == NULL || ordini_testa->tempo >= ordine->tempo) { //caso di inserimento in testa
        ordine->next = ordini_testa;
        ordini_testa = ordine;
    } else {//caso di inserimento in mezzo
        cur = ordini_testa;
        while (cur->next != NULL && cur->next->tempo < ordine->tempo) {
            cur = cur->next;
        }
        ordine->next = cur->next;
        cur->next = ordine;
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