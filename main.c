#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <locale.h>
#include <string.h>
#include <dirent.h>
#include "leitor.h"
#include "logica.h"



int nome_ignorado(char nome[]) {
    return strcmp(nome, ".") == 0 || strcmp(nome, "..") == 0;
}

// Lê a pasta paciencias e guarda os ficheiros encontrados.
int carregar_ficheiros(char ficheiros[20][100]) {
    DIR *dir;
    struct dirent *entrada;
    int numero_ficheiro = 0;

    dir = opendir("paciencias");
    
    while ((entrada = readdir(dir)) != NULL) {
        if (numero_ficheiro < 20 && !nome_ignorado(entrada->d_name)) {
            strcpy(ficheiros[numero_ficheiro], entrada->d_name);
            numero_ficheiro++;
        }
    }

    closedir(dir);
    return numero_ficheiro;
}

// Mostra ao utilizador as paciências disponíveis.
void imprimir_ficheiros(char ficheiros[20][100], int n) {
    int i;

    printf("Paciências disponíveis:\n");
    for (i = 0; i < n; i++) {
        printf("%d - %s\n", i + 1, ficheiros[i]);
    }
}

// Pede ao utilizador qual paciência quer jogar.
int pedir_escolha(int n) {
    int escolha;

    printf("Escolha uma paciência: ");
    scanf("%d", &escolha);

    if (escolha < 1 || escolha > n) escolha = 1;

    return escolha;
}

// Cria o caminho completo para o ficheiro escolhido.
void criar_caminho(char caminho[], char ficheiro[]) {
    strcpy(caminho, "paciencias/");
    strcat(caminho, ficheiro);
}

// Junta todos os passos necessários para escolher uma paciência.
int escolher_paciencia(char caminho[]) {
    char ficheiros[20][100];
    int n, escolha;

    n = carregar_ficheiros(ficheiros);
    if (n == 0) return 0;

    imprimir_ficheiros(ficheiros, n);
    escolha = pedir_escolha(n);
    criar_caminho(caminho, ficheiros[escolha - 1]);

    return 1;
}

int main(void) {
    DefinicaoJogo def;
    EstadoJogo estado;
    char caminho[100];

    setlocale(LC_ALL, "");
    srand(time(NULL));

    if (!escolher_paciencia(caminho)) {
        printf("Erro ao abrir a pasta paciencias.\n");
        return EXIT_FAILURE;
    }

    if (!ler_definicao(caminho, &def)) {
        printf("Erro ao ler %s\n", caminho);
        return EXIT_FAILURE;
    }

    iniciar_estado(&def, &estado);
    ciclo_jogo(&estado);

    return EXIT_SUCCESS;
}
