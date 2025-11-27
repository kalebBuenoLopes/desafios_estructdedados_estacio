#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#define TAM_FILA 5      // Tamanho fixo da fila circular
#define TAM_PILHA 3     // Capacidade máxima da pilha de reserva

// Representação de uma peça do jogo
typedef struct {
    char nome;   
    int id;      
} Peca;

/* -------------------------- VARIÁVEIS GLOBAIS --------------------------- */
Peca fila[TAM_FILA];
Peca pilha[TAM_PILHA];

int frente = 0;      // remoção da fila
int tras = 0;        // inserção da fila
int qtd_fila = 0;    // quantidade atual

int topo = -1;       // topo da pilha
int proximoID = 0;   // contador de IDs para gerar peças únicas

/* ---------------------- FUNCÃO PARA GERAR PEÇA ------------------------- */
Peca gerarPeca() {
    char tipos[4] = {'I', 'O', 'T', 'L'};
    Peca p;
    p.nome = tipos[rand() % 4];
    p.id = proximoID++;
    return p;
}

/* ----------------------- OPERAÇÕES ----------------------------- */

// Insere uma nova peça
void enqueue(Peca p) {
    if (qtd_fila < TAM_FILA) {
        fila[tras] = p;
        tras = (tras + 1) % TAM_FILA;
        qtd_fila++;
    }
}

// Remove e retorna a peça da frente
Peca dequeue() {
    Peca removida = fila[frente];
    frente = (frente + 1) % TAM_FILA;
    qtd_fila--;
    return removida;
}

/* ----------------------- OPERAÇÕES DA PILHA ---------------------------- */

// Adiciona peça
int push(Peca p) {
    if (topo == TAM_PILHA - 1) {
        printf("A pilha de reserva está cheia. Não é possível reservar.\n");
        return 0;
    }
    pilha[++topo] = p;
    return 1;
}

// Remove peça
Peca pop() {
    return pilha[topo--];
}

/* -------------------------- EXIBIR ESTADO ------------------------------ */
void exibirEstado() {
    int i;

    printf("\nEstado atual:\n");

    // Exibição da fila
    printf("Fila de peças\t");
    int idx = frente;
    for (i = 0; i < qtd_fila; i++) {
        printf("[%c %d] ", fila[idx].nome, fila[idx].id);
        idx = (idx + 1) % TAM_FILA;
    }
    printf("\n");

    // Exibição da pilha
    printf("Pilha de reserva \t(Topo -> Base): ");
    for (i = topo; i >= 0; i--) {
        printf("[%c %d] ", pilha[i].nome, pilha[i].id);
    }
    printf("\n");
}

int main() {
    srand(time(NULL));

    // Inicializar fila com 5 peças
    for (int i = 0; i < TAM_FILA; i++) {
        enqueue(gerarPeca());
    }

    int opcao;

    do {
        exibirEstado();

        printf("\nOpções de Ação:\n");
        printf("1 - Jogar peça\n");
        printf("2 - Reservar peça\n");
        printf("3 - Usar peça reservada\n");
        printf("0 - Sair\n");
        printf("Opção: ");
        scanf("%d", &opcao);

        switch (opcao) {

        case 1: {
            // Jogar peça (remove da fila)
            Peca jogada = dequeue();
            printf("A peça [%c %d] foi jogada.\n", jogada.nome, jogada.id);

            // Sempre gerar nova peça
            enqueue(gerarPeca());
            break;
        }

        case 2: {
            // Reservar peça (fila -> pilha)
            if (topo == TAM_PILHA - 1) {
                printf("A pilha está cheia. Não é possível reservar.\n");
            } else {
                Peca reservada = dequeue();
                push(reservada);
                printf("A peça [%c %d] foi reservada.\n", reservada.nome, reservada.id);

                enqueue(gerarPeca());
            }
            break;
        }

        case 3:
            // Usar peça reservada (pop)
            if (topo == -1) {
                printf("A pilha está vazia.\n");
            } else {
                Peca usada = pop();
                printf("A peça reservada [%c %d] foi utilizada.\n", usada.nome, usada.id);
            }
            break;

        case 0:
            printf("Encerrando o programa.\n");
            break;

        default:
            printf("Opção inválida.\n");
        }

    } while (opcao != 0);

    return 0;
}
