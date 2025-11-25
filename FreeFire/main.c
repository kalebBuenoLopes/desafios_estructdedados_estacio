#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#define MAX_COMPONENTES 20
#define MAX_STR 100

/*
 * Struct Componente
 * - nome
 * - tipo
 * - prioridade
 */
typedef struct {
    char nome[30];
    char tipo[20];
    int prioridade;
} Componente;

/* ---------- Funções utilitárias ---------- */

/* Remove o '\n' gerado por fgets */
void trim_newline(char *s) {
    size_t len = strlen(s);
    if (len > 0 && s[len-1] == '\n') s[len-1] = '\0';
}

/* Cópia segura de string */
void safe_strcpy(char *dest, const char *src, size_t dest_size) {
    strncpy(dest, src, dest_size - 1);
    dest[dest_size - 1] = '\0';
}

/* Mostra todos os componentes em formato tabular */
void mostrarComponentes(Componente itens[], int n) {
    if (n == 0) {
        printf("\nNenhum componente cadastrado.\n");
        return;
    }

    printf("\nLista de Componentes (total: %d):\n", n);
    printf("-----------------------------------------------------------\n");
    printf("| %-3s | %-25s | %-12s | %-8s |\n", "No", "Nome", "Tipo", "Prioridade");
    printf("-----------------------------------------------------------\n");
    for (int i = 0; i < n; i++) {
        printf("| %-3d | %-25s | %-12s | %-8d |\n",
               i + 1, itens[i].nome, itens[i].tipo, itens[i].prioridade);
    }
    printf("-----------------------------------------------------------\n");
}

/* ---------- Algoritmos de ordenação ---------- */

/*
 * Bubble sort por nome (string)
 * Retorna no ponteiro comparacoes o total de comparações realizadas.
 */
void bubbleSortNome(Componente itens[], int n, long *comparacoes) {
    *comparacoes = 0;
    if (n <= 1) return;

    for (int i = 0; i < n - 1; i++) {
        int trocou = 0;
        for (int j = 0; j < n - i - 1; j++) {
            (*comparacoes)++;
            if (strcmp(itens[j].nome, itens[j+1].nome) > 0) {
                Componente tmp = itens[j];
                itens[j] = itens[j+1];
                itens[j+1] = tmp;
                trocou = 1;
            }
        }
        if (!trocou) break;
    }
}

/*
 * Insertion sort por tipo (string)
 * Retorna no ponteiro comparacoes o total de comparações realizadas.
 */
void insertionSortTipo(Componente itens[], int n, long *comparacoes) {
    *comparacoes = 0;
    for (int i = 1; i < n; i++) {
        Componente chave = itens[i];
        int j = i - 1;
        while (j >= 0) {
            (*comparacoes)++;
            if (strcmp(itens[j].tipo, chave.tipo) > 0) {
                itens[j + 1] = itens[j];
                j--;
            } else break;
        }
        itens[j + 1] = chave;
    }
}

/*
 * Selection sort por prioridade (int)
 * Conta comparações em comparacoes.
 */
void selectionSortPrioridade(Componente itens[], int n, long *comparacoes) {
    *comparacoes = 0;
    for (int i = 0; i < n - 1; i++) {
        int min_idx = i;
        for (int j = i + 1; j < n; j++) {
            (*comparacoes)++;
            if (itens[j].prioridade < itens[min_idx].prioridade) {
                min_idx = j;
            }
        }
        if (min_idx != i) {
            Componente tmp = itens[i];
            itens[i] = itens[min_idx];
            itens[min_idx] = tmp;
        }
    }
}

/* ---------- Busca binária por nome ---------- */

/*
 * Busca binária por nome em itens
 * Retorna o índice encontrado (>=0) ou -1 se não encontrado.
 * Conta comparações em comparacoes.
 */
int buscaBinariaPorNome(Componente itens[], int n, const char *nomeChave, long *comparacoes) {
    int esquerda = 0, direita = n - 1;
    *comparacoes = 0;
    while (esquerda <= direita) {
        int meio = esquerda + (direita - esquerda) / 2;
        (*comparacoes)++;
        int cmp = strcmp(itens[meio].nome, nomeChave);
        if (cmp == 0) return meio;
        else if (cmp < 0) esquerda = meio + 1;
        else direita = meio - 1;
    }
    return -1;
}

/* ---------- Medidor de tempo ---------- */

/*
 * Tipo de função para ordenadores:
 * void algoritmo(Componente[], int, long*)
 *
 * medirTempo recebe um ponteiro para função de ordenação e executa,
 * retornando tempo em segundos (double) e comparacoes via ponteiro.
 */
double medirTempo(void (*algoritmo)(Componente[], int, long*),
                  Componente itens[], int n, long *comparacoes) {
    clock_t inicio = clock();
    algoritmo(itens, n, comparacoes);
    clock_t fim = clock();
    return ((double)(fim - inicio)) / CLOCKS_PER_SEC;
}

/* ---------- Função para clonar vetor ---------- */
void clonarVetor(Componente origem[], Componente destino[], int n) {
    for (int i = 0; i < n; i++) destino[i] = origem[i];
}

/* ---------- Interface para cadastro e controle ---------- */

/* Lê um componente via fgets */
void lerComponente(Componente *c) {
    char buf[MAX_STR];

    printf("Nome (ex: chip central): ");
    fgets(buf, sizeof(buf), stdin);
    trim_newline(buf);
    safe_strcpy(c->nome, buf, sizeof(c->nome));

    printf("Tipo (controle/suporte/propulsao): ");
    fgets(buf, sizeof(buf), stdin);
    trim_newline(buf);
    safe_strcpy(c->tipo, buf, sizeof(c->tipo));

    do {
        printf("Prioridade (1 a 10): ");
        if (fgets(buf, sizeof(buf), stdin) == NULL) { buf[0] = '\0'; }
        trim_newline(buf);
        c->prioridade = atoi(buf);
        if (c->prioridade < 1 || c->prioridade > 10) {
            printf("Prioridade invalida. Informe um valor entre 1 e 10.\n");
        } else break;
    } while (1);
}

/* ---------- Programa principal com menu ---------- */

int main() {
    Componente componentes[MAX_COMPONENTES];
    int n = 0;
    int opcao;

    printf("Módulo de Prioritização e Montagem da Torre de Fuga\n");
    printf("Maximo de componentes: %d\n", MAX_COMPONENTES);

    do {
        printf("\n=========================================\n");
        printf("1 - Cadastrar componente\n");
        printf("2 - Listar componentes\n");
        printf("3 - Ordenar e medir algoritmos\n");
        printf("4 - Busca binaria por nome (após ordenar por nome)\n");
        printf("5 - Limpar componentes\n");
        printf("0 - Sair deste módulo\n");
        printf("Escolha: ");
        if (scanf("%d", &opcao) != 1) {
            while (getchar() != '\n'); // limpar
            opcao = -1;
        }
        while (getchar() != '\n'); // limpar newline após escolha

        if (opcao == 1) {
            if (n >= MAX_COMPONENTES) {
                printf("Limite atingido. Não é possível cadastrar mais componentes.\n");
            } else {
                printf("\nCadastro do componente %d\n", n + 1);
                lerComponente(&componentes[n]);
                n++;
                printf("Componente cadastrado.\n");
            }
        }
        else if (opcao == 2) {
            mostrarComponentes(componentes, n);
        }
        else if (opcao == 3) {
            if (n == 0) {
                printf("Nenhum componente para ordenar.\n");
                continue;
            }

            // Clonar para rodar cada algoritmo com os mesmos dados
            Componente copia[MAX_COMPONENTES];

            long comp_bubble = 0, comp_insertion = 0, comp_selection = 0;
            double t_bubble = 0.0, t_insertion = 0.0, t_selection = 0.0;

            // Bubble sort por nome
            clonarVetor(componentes, copia, n);
            t_bubble = medirTempo(bubbleSortNome, copia, n, &comp_bubble);

            // Insertion sort por tipo
            clonarVetor(componentes, copia, n);
            t_insertion = medirTempo(insertionSortTipo, copia, n, &comp_insertion);

            // Selection sort por prioridade
            clonarVetor(componentes, copia, n);
            t_selection = medirTempo(selectionSortPrioridade, copia, n, &comp_selection);

            printf("\nResultados das ordenacoes (cada algoritmo recebeu o mesmo conjunto de dados):\n");
            printf("Bubble Sort (por nome): comparacoes = %ld, tempo = %.6f segundos\n", comp_bubble, t_bubble);
            printf("Insertion Sort (por tipo): comparacoes = %ld, tempo = %.6f segundos\n", comp_insertion, t_insertion);
            printf("Selection Sort (por prioridade): comparacoes = %ld, tempo = %.6f segundos\n", comp_selection, t_selection);

            // Perguntar qual ordenacao aplicar de fato ao vetor principal
            int escolha;
            printf("\nEscolha qual ordenacao aplicar ao vetor principal:\n");
            printf("1 - Bubble (por nome)\n");
            printf("2 - Insertion (por tipo)\n");
            printf("3 - Selection (por prioridade)\n");
            printf("0 - Nao ordenar\n");
            printf("Escolha: ");
            if (scanf("%d", &escolha) != 1) { while (getchar() != '\n'); escolha = -1; }
            while (getchar() != '\n');

            long comps = 0;
            double tempo = 0.0;
            if (escolha == 1) {
                tempo = medirTempo(bubbleSortNome, componentes, n, &comps);
                printf("\nAplicado Bubble Sort por nome: comparacoes=%ld, tempo=%.6f s\n", comps, tempo);
            } else if (escolha == 2) {
                tempo = medirTempo(insertionSortTipo, componentes, n, &comps);
                printf("\nAplicado Insertion Sort por tipo: comparacoes=%ld, tempo=%.6f s\n", comps, tempo);
            } else if (escolha == 3) {
                tempo = medirTempo(selectionSortPrioridade, componentes, n, &comps);
                printf("\nAplicado Selection Sort por prioridade: comparacoes=%ld, tempo=%.6f s\n", comps, tempo);
            } else {
                printf("Nenhuma ordenacao aplicada.\n");
            }

            // Mostrar vetor atual
            mostrarComponentes(componentes, n);
        }
        else if (opcao == 4) {
            if (n == 0) {
                printf("Nenhum componente cadastrado.\n");
                continue;
            }

            char chave[MAX_STR];
            printf("Informe o nome do componente-chave para buscar (busca binaria): ");
            fgets(chave, sizeof(chave), stdin);
            trim_newline(chave);

            // Busca binária requer vetor ordenado por nome; aconselhar o usuário
            // não há verificação formal se está ordenado
            long comp_bin = 0;
            int idx = buscaBinariaPorNome(componentes, n, chave, &comp_bin);
            if (idx >= 0) {
                printf("\nComponente encontrado no indice %d (0-based):\n", idx);
                printf("Nome: %s | Tipo: %s | Prioridade: %d\n", componentes[idx].nome, componentes[idx].tipo, componentes[idx].prioridade);
            } else {
                printf("\nComponente nao encontrado com busca binaria.\n");
            }
            printf("Comparacoes na busca binaria: %ld\n", comp_bin);
        }
        else if (opcao == 5) {
            n = 0;
            printf("Lista de componentes limpa.\n");
        }
        else if (opcao == 0) {
            printf("Saindo do modulo de montagem da torre.\n");
        }
        else {
            printf("Opcao invalida.\n");
        }

    } while (opcao != 0);

    return 0;
}
