#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#define MAX_PISTA 100
#define HASH_SIZE 101   // tamanho simples para a hash table (primo)

typedef struct Sala {
    char nome[50];
    char pista[MAX_PISTA]; // pista associada à sala (pode ser string vazia)
    struct Sala *esquerda;
    struct Sala *direita;
} Sala;

/* Nó da BST que armazena pistas coletadas (strings) */
typedef struct PistaNode {
    char texto[MAX_PISTA];
    struct PistaNode *esquerda;
    struct PistaNode *direita;
} PistaNode;

/* Nó para lista na tabela hash (cadeia) */
typedef struct HashNode {
    char chave[MAX_PISTA];      // texto da pista (chave)
    char suspeito[50];         // nome do suspeito associado
    struct HashNode *proximo;
} HashNode;

// -------------------------------
// Funções para salas (árvore binária)
// -------------------------------

Sala* criarSala(const char *nome, const char *pista) {
    Sala *s = (Sala*) malloc(sizeof(Sala));
    if (!s) { perror("malloc"); exit(EXIT_FAILURE); }
    strncpy(s->nome, nome, sizeof(s->nome)-1);
    s->nome[sizeof(s->nome)-1] = '\0';
    if (pista) strncpy(s->pista, pista, sizeof(s->pista)-1);
    else s->pista[0] = '\0';
    s->pista[sizeof(s->pista)-1] = '\0';
    s->esquerda = s->direita = NULL;
    return s;
}

// -------------------------------
// Funções para BST de pistas
// -------------------------------

PistaNode* criarPistaNode(const char *texto) {
    PistaNode *n = (PistaNode*) malloc(sizeof(PistaNode));
    if (!n) { perror("malloc"); exit(EXIT_FAILURE); }
    strncpy(n->texto, texto, sizeof(n->texto)-1);
    n->texto[sizeof(n->texto)-1] = '\0';
    n->esquerda = n->direita = NULL;
    return n;
}

/* busca se já existe a pista na BST (retorna 1 se existe) */
int existePista(PistaNode *raiz, const char *texto) {
    if (raiz == NULL) return 0;
    int cmp = strcmp(texto, raiz->texto);
    if (cmp == 0) return 1;
    else if (cmp < 0) return existePista(raiz->esquerda, texto);
    else return existePista(raiz->direita, texto);
}

/* insere se não existir (mantém únicas) */
PistaNode* inserirPista(PistaNode *raiz, const char *texto) {
    if (raiz == NULL) {
        return criarPistaNode(texto);
    }
    int cmp = strcmp(texto, raiz->texto);
    if (cmp < 0) raiz->esquerda = inserirPista(raiz->esquerda, texto);
    else if (cmp > 0) raiz->direita = inserirPista(raiz->direita, texto);
    // se igual, não insere duplicata
    return raiz;
}

/* imprime em ordem (alfabético) */
void exibirPistasOrdenadas(PistaNode *raiz) {
    if (!raiz) return;
    exibirPistasOrdenadas(raiz->esquerda);
    printf(" - %s\n", raiz->texto);
    exibirPistasOrdenadas(raiz->direita);
}

/* libera BST */
void liberarPistas(PistaNode *raiz) {
    if (!raiz) return;
    liberarPistas(raiz->esquerda);
    liberarPistas(raiz->direita);
    free(raiz);
}

// -------------------------------
// Funções para tabela hash (chave: pista -> valor: suspeito)
// -------------------------------

unsigned long hash_djb2(const char *str) {
    unsigned long hash = 5381;
    int c;
    while ((c = (unsigned char)*str++))
        hash = ((hash << 5) + hash) + c; /* hash * 33 + c */
    return hash % HASH_SIZE;
}

void inicializarHash(HashNode *tabela[]) {
    for (int i = 0; i < HASH_SIZE; i++) tabela[i] = NULL;
}

void inserirNaHash(HashNode *tabela[], const char *pista, const char *suspeito) {
    unsigned long h = hash_djb2(pista);
    // não checamos duplicatas, apenas inserimos no início
    HashNode *n = (HashNode*) malloc(sizeof(HashNode));
    if (!n) { perror("malloc"); exit(EXIT_FAILURE); }
    strncpy(n->chave, pista, sizeof(n->chave)-1);
    n->chave[sizeof(n->chave)-1] = '\0';
    strncpy(n->suspeito, suspeito, sizeof(n->suspeito)-1);
    n->suspeito[sizeof(n->suspeito)-1] = '\0';
    n->proximo = tabela[h];
    tabela[h] = n;
}

/* busca o suspeito associado a uma pista (NULL se não achar) */
const char* encontrarSuspeito(HashNode *tabela[], const char *pista) {
    unsigned long h = hash_djb2(pista);
    for (HashNode *it = tabela[h]; it; it = it->proximo) {
        if (strcmp(it->chave, pista) == 0)
            return it->suspeito;
    }
    return NULL;
}

/* libera toda a tabela hash */
void liberarHash(HashNode *tabela[]) {
    for (int i = 0; i < HASH_SIZE; i++) {
        HashNode *it = tabela[i];
        while (it) {
            HashNode *tmp = it->proximo;
            free(it);
            it = tmp;
        }
        tabela[i] = NULL;
    }
}

// -------------------------------
// Utilitários de input/strings
// -------------------------------

/* remove newline final se existir */
void trim_newline(char *s) {
    size_t L = strlen(s);
    if (L>0 && s[L-1]=='\n') s[L-1] = '\0';
}

/* lê uma linha do stdin em buffer, com fgets seguro */
void lerLinha(char *buf, int size) {
    if (!fgets(buf, size, stdin)) { buf[0]='\0'; return; }
    trim_newline(buf);
}

// -------------------------------
// Exploração: coleta e inserção na BST
// -------------------------------

/* quando entra em uma sala, coleta (se existir) e insere na BST (se ainda não coletada),
   e informa o jogador; também retorna 1 se coletou nova pista, 0 caso contrário. */
int coletarPistaSeExistir(PistaNode **raiz, const char *pista) {
    if (pista == NULL || pista[0] == '\0') return 0;
    if (existePista(*raiz, pista)) {
        printf("Pista já coletada anteriormente: \"%s\"\n", pista);
        return 0;
    } else {
        *raiz = inserirPista(*raiz, pista);
        printf("Pista coletada: \"%s\"\n", pista);
        return 1;
    }
}

/* Exploração interativa a partir da sala atual.
   O ponteiro para pistasBST é atualizado conforme coleta. */
void explorarSalas(Sala *atual, PistaNode **pistasBST) {
    char escolha[10];

    while (1) {
        printf("\nVocê está na sala: %s\n", atual->nome);
        if (strlen(atual->pista) > 0) {
            coletarPistaSeExistir(pistasBST, atual->pista);
        } else {
            printf("Nenhuma pista nesta sala.\n");
        }

        printf("\nEscolha o próximo passo:\n");
        printf(" e - ir à esquerda\n");
        printf(" d - ir à direita\n");
        printf(" s - sair da exploração\n");
        printf("Opção: ");
        if (!fgets(escolha, sizeof(escolha), stdin)) { printf("Entrada inválida.\n"); continue; }
        trim_newline(escolha);

        if (strcmp(escolha, "e") == 0) {
            if (atual->esquerda) atual = atual->esquerda;
            else printf("Não há caminho à esquerda.\n");
        } else if (strcmp(escolha, "d") == 0) {
            if (atual->direita) atual = atual->direita;
            else printf("Não há caminho à direita.\n");
        } else if (strcmp(escolha, "s") == 0) {
            printf("Exploração encerrada pelo jogador.\n");
            return;
        } else {
            printf("Opção inválida. Use 'e', 'd' ou 's'.\n");
        }
    }
}

// -------------------------------
// Função para contar pistas que apontam para um suspeito
// percorre a BST e para cada pista consulta a hash; se o suspeito associado
// for igual ao procurado, incrementa contador.
// -------------------------------

int contadorPistasParaSuspeitoRec(PistaNode *raiz, HashNode *tabela[], const char *suspeito) {
    if (!raiz) return 0;
    int count = 0;
    // esquerda
    count += contadorPistasParaSuspeitoRec(raiz->esquerda, tabela, suspeito);
    // este nó
    const char *s = encontrarSuspeito(tabela, raiz->texto);
    if (s && strcmp(s, suspeito) == 0) count++;
    // direita
    count += contadorPistasParaSuspeitoRec(raiz->direita, tabela, suspeito);
    return count;
}

int contarPistasParaSuspeito(PistaNode *raiz, HashNode *tabela[], const char *suspeito) {
    return contadorPistasParaSuspeitoRec(raiz, tabela, suspeito);
}

// -------------------------------
// MAIN: monta mansão, tabela hash de pistas->suspeitos,
// permite explorar, listar pistas e acusar.
// -------------------------------

int main() {
    srand((unsigned) time(NULL));

    // --- Montagem fixa da mansão (árvore binária) ---
    // cada sala tem pista fixa (ou "") de acordo com as regras codificadas
    Sala *hall       = criarSala("Hall de Entrada",          "Poeira de lama no sapato");
    Sala *salaEstar  = criarSala("Sala de Estar",           "");
    Sala *cozinha    = criarSala("Cozinha",                 "Pegada com sola estreita");
    Sala *biblioteca = criarSala("Biblioteca",              "Página rasgada com iniciais 'J.'");
    Sala *jardim     = criarSala("Jardim",                  "Cigarro apagado no canteiro");
    Sala *porão      = criarSala("Porão",                   "Chave metálica encontrada");
    Sala *escritorio = criarSala("Escritório",              "Bilhete com nome 'Joana'");
    Sala *quarto     = criarSala("Quarto Principal",        "Loção com perfume floral");
    Sala *salaMusica = criarSala("Sala de Música",         "Partitura com marcas de tinta");
    Sala *cochicho   = criarSala("Closet",                  "Fio de tecido azul");

    /*
            Hall
           /    \
      Estar      Cozinha
      /  \       /    \
  Biblioteca Jardim Porão Escritório
   /  \
 Quarto SalaMusica
 (etc)
    (mapa pequeno, manual)
    */

    hall->esquerda = salaEstar;
    hall->direita  = cozinha;

    salaEstar->esquerda = biblioteca;
    salaEstar->direita  = jardim;

    biblioteca->esquerda = quarto;
    library: ; // no-op label to avoid unused variable warning if needed

    cozinha->esquerda = porão;
    cozinha->direita  = escritorio;

    quarto->esquerda = salaMusica;
    salaMusica->direita = cochicho;

    // --- Montagem da tabela hash associando pistas a suspeitos ---
    HashNode *tabela[HASH_SIZE];
    inicializarHash(tabela);

    // Regras codificadas: pista -> suspeito
    // (estas associações são definidas a priori)
    inserirNaHash(tabela, "Poeira de lama no sapato", "Ricardo");
    inserirNaHash(tabela, "Pegada com sola estreita", "Joana");
    inserirNaHash(tabela, "Página rasgada com iniciais 'J.'", "Joana");
    inserirNaHash(tabela, "Cigarro apagado no canteiro", "Marcos");
    inserirNaHash(tabela, "Chave metálica encontrada", "Ricardo");
    inserirNaHash(tabela, "Bilhete com nome 'Joana'", "Joana");
    inserirNaHash(tabela, "Loção com perfume floral", "Mariana");
    inserirNaHash(tabela, "Partitura com marcas de tinta", "Marcos");
    inserirNaHash(tabela, "Fio de tecido azul", "Mariana");

    // --- BST vazia para pistas coletadas ---
    PistaNode *pistasColetadas = NULL;

    printf("=== Detective Quest: Investigação Final ===\n");
    printf("Explore a mansão a partir do Hall de Entrada.\n");

    // limpamos buffer e chamamos exploração
    fflush(stdout);
    explorarSalas(hall, &pistasColetadas);

    // Ao final, exibir pistas coletadas em ordem
    printf("\n===== Pistas coletadas (ordem alfabética) =====\n");
    if (!pistasColetadas) {
        printf("Nenhuma pista foi coletada.\n");
    } else {
        exibirPistasOrdenadas(pistasColetadas);
    }

    // Fase de acusação: jogador indica um suspeito
    char acusacao[50];
    printf("\nQuem você acusa? Digite o nome do suspeito: ");
    if (!fgets(acusacao, sizeof(acusacao), stdin)) acusacao[0] = '\0';
    trim_newline(acusacao);

    if (strlen(acusacao) == 0) {
        printf("Nenhuma acusação realizada. Encerrando.\n");
    } else {
        // contamos quantas pistas (únicas) apontam para esse suspeito
        int n = contarPistasParaSuspeito(pistasColetadas, tabela, acusacao);
        printf("Pistas que associam '%s' ao caso: %d\n", acusacao, n);
        if (n >= 2) {
            printf("Acusação sustentada: %s é provavelmente o culpado.\n", acusacao);
        } else {
            printf("Acusação fraca: menos de 2 pistas ligam %s ao crime.\n", acusacao);
        }
    }

    // liberar memória
    liberarPistas(pistasColetadas);
    liberarHash(tabela);

    // liberar salas (árvore fixa): percorrer e free
    // para simplicidade, liberamos manualmente todos os nós criados
    free(hall);
    free(salaEstar);
    free(cozinha);
    free(biblioteca);
    free(jardim);
    free(porão);
    free(escritorio);
    free(quarto);
    free(salaMusica);
    free(cochicho);

    printf("\nFim da investigação.\n");
    return 0;
}
