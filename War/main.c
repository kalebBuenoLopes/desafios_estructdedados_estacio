#include <stdio.h>
#include <string.h>

/*
 * Criando a estruct Territorio:
 * Armazena informações sobre um território:
 * nom, cor e tropas
 */
struct Territorio {
    char nome[30];
    char cor[10];
    int tropas;
};

int main() {
    // Vetor que guarda 5 territórios
    struct Territorio territorios[5];

    // Entrada dos territórios
    printf("Cadastro de Territorios \n");

    for (int i = 0; i < 5; i++) {
        printf("\n Territorio %d \n", i + 1);

        // inserção e leitura do nome
        printf("Digite o nome do territorio: ");
        scanf("%s", territorios[i].nome);

        //da cor
        printf("Digite a cor do exercito: ");
        scanf("%s", territorios[i].cor);

        // e das tropas
        printf("Digite a quantidade de tropas: ");
        scanf("%d", &territorios[i].tropas);
    }

    // Exibição dos dados após todo o cadastro
    printf("\n Territorios Cadastrados \n");

    for (int i = 0; i < 5; i++) {
        printf("\nTerritorio %d:\n", i + 1);
        printf("Nome: %s\n", territorios[i].nome);
        printf("Cor do exercito: %s\n", territorios[i].cor);
        printf("Tropas: %d\n", territorios[i].tropas);
    }

    return 0;
}


