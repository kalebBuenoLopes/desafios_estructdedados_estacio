#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

/*
 * Estrutura de Território
 */
typedef struct {
    char nome[30];
    char cor[10];
    int tropas;
} Territorio;

/*
 * FUNÇÃO: atribuirMissao
 * Sorteia uma missão e copia para o destino
 */
void atribuirMissao(char* destino, char* missoes[], int total) {
    int r = rand() % total;
    strcpy(destino, missoes[r]);
}

/*
 * FUNÇÃO: verificarMissao
 * Lógica simplificada para exemplo
 * (pode ser expandida conforme necessário)
 */
int verificarMissao(char* missao, Territorio* mapa, int tamanho) {

    // Exemplo de verificação simples:
    // Missão: "Conquistar 3 territorios com a cor azul"
    if (strcmp(missao, "Conquistar 3 territorios com a cor azul") == 0) {
        int count = 0;
        for (int i = 0; i < tamanho; i++) {
            if (strcmp(mapa[i].cor, "azul") == 0) {
                count++;
            }
        }
        if (count >= 3) return 1;
    }

    // Missão: "Ter um territorio com mais de 10 tropas"
    if (strcmp(missao, "Ter um territorio com mais de 10 tropas") == 0) {
        for (int i = 0; i < tamanho; i++) {
            if (mapa[i].tropas > 10)
                return 1;
        }
    }

    // Missão: "Eliminar todas as tropas vermelhas"
    if (strcmp(missao, "Eliminar todas as tropas vermelhas") == 0) {
        for (int i = 0; i < tamanho; i++) {
            if (strcmp(mapa[i].cor, "vermelha") == 0 && mapa[i].tropas > 0)
                return 0;
        }
        return 1;
    }

    return 0;
}

/*
 * FUNÇÃO: atacar
 * Simula ataque entre dois territórios
 */
void atacar(Territorio* atacante, Territorio* defensor) {
    if (strcmp(atacante->cor, defensor->cor) == 0) {
        printf("Nao eh possivel atacar um territorio aliado!\n");
        return;
    }

    if (atacante->tropas <= 1) {
        printf("O territorio atacante nao tem tropas suficientes!\n");
        return;
    }

    int dadoA = (rand() % 6) + 1;
    int dadoD = (rand() % 6) + 1;

    printf("\nAtaque!! %s (%d tropas) atacando %s (%d tropas)\n",
           atacante->nome, atacante->tropas, defensor->nome, defensor->tropas);

    printf("Dado atacante: %d | Dado defensor: %d\n", dadoA, dadoD);

    if (dadoA > dadoD) {
        printf("Atacante venceu!\n");
        defensor->tropas /= 2;  
        strcpy(defensor->cor, atacante->cor);
    } else {
        printf("Defensor segurou o ataque!\n");
        atacante->tropas--;
    }
}

/*
 * FUNÇÃO: liberarMemoria
 */
void liberarMemoria(char* m1, char* m2, Territorio* mapa) {
    free(m1);
    free(m2);
    free(mapa);
}

/*
 * FUNÇÃO PRINCIPAL
 */
int main() {
    srand(time(NULL));

    // Vetor dinâmico de territórios
    Territorio* territorios = malloc(5 * sizeof(Territorio));

    // MISSÕES
    char* missoes[] = {
        "Conquistar 3 territorios com a cor azul",
        "Eliminar todas as tropas vermelhas",
        "Ter um territorio com mais de 10 tropas",
        "Conquistar 2 territorios seguidos",
        "Manter um territorio com mais de 8 tropas"
    };
    int totalMissoes = 5;

    // Missão dinâmica para dois jogadores
    char* missaoJogador1 = malloc(100);
    char* missaoJogador2 = malloc(100);

    printf("=== Cadastro de Territorios ===\n");

    for (int i = 0; i < 5; i++) {
        printf("\nTerritorio %d\n", i + 1);

        printf("Nome: ");
        scanf("%s", territorios[i].nome);

        printf("Cor do exercito: ");
        scanf("%s", territorios[i].cor);

        printf("Quantidade de tropas: ");
        scanf("%d", &territorios[i].tropas);
    }

    // Sorteio das missões
    atribuirMissao(missaoJogador1, missoes, totalMissoes);
    atribuirMissao(missaoJogador2, missoes, totalMissoes);

    printf("\nMISSÃO DO JOGADOR 1: %s\n", missaoJogador1);
    printf("MISSÃO DO JOGADOR 2: %s\n\n", missaoJogador2);

    int turno = 1;
    while (1) {
        printf("\n=== TURNO %d ===\n", turno);

        int opcao;
        printf("1 - Atacar\n");
        printf("2 - Ver mapa\n");
        printf("0 - Encerrar jogo\n");
        printf("Escolha: ");
        scanf("%d", &opcao);

        if (opcao == 0) break;

        if (opcao == 2) {
            printf("\n=== MAPA ===\n");
            for (int i = 0; i < 5; i++) {
                printf("Territorio %d | Nome: %s | Cor: %s | Tropas: %d\n",
                       i + 1,
                       territorios[i].nome,
                       territorios[i].cor,
                       territorios[i].tropas);
            }
            continue;
        }

        if (opcao == 1) {
            int a, d;
            printf("Escolha territorio atacante (1-5): ");
            scanf("%d", &a);
            printf("Escolha territorio defensor (1-5): ");
            scanf("%d", &d);

            atacar(&territorios[a - 1], &territorios[d - 1]);
        }

        // Verificação silenciosa
        if (verificarMissao(missaoJogador1, territorios, 5)) {
            printf("\n>>> O JOGADOR 1 CUMPRIU SUA MISSAO! <<<\n");
            break;
        }

        if (verificarMissao(missaoJogador2, territorios, 5)) {
            printf("\n>>> O JOGADOR 2 CUMPRIU SUA MISSAO! <<<\n");
            break;
        }

        turno++;
    }

    liberarMemoria(missaoJogador1, missaoJogador2, territorios);
    printf("\nMemoria liberada. Jogo encerrado!\n");

    return 0;
}
