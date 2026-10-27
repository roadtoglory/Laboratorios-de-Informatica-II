#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "leitor.h"


//Função que remove um comentário de uma linha
void remover_comentario(char linha[]){
    int i = 0;

    while (linha[i] != '\0' && linha[i] != '#'){
        i++;
    }

    linha[i] = '\0';
}

//Função que verifica se uma determinada tem conteúdo
//Percorre a string toda e vê se um determinado char é diferente de tab, new line ou espaço.
int linha_tem_conteudo(char linha[]){
    int i = 0;
    int tem = 0;

    while (linha[i] != '\0'){
        if (linha[i] != ' ' && linha[i] != '\t' && linha[i] != '\n'){
            tem = 1;
        }

        i++;
    }

    return tem;
}


//Adiciona um TipoPilha à definição do jogo.
void ler_tipo(DefinicaoJogo *def, char a[], char b[]){
    int i = def->n_tipos;

    strcpy(def->tipos[i].nome, a);
    strcpy(def->tipos[i].flags, b);
    def->n_tipos++;
}



//Adiciona um InitPilha à definição do jogo.
void ler_init(DefinicaoJogo *def, char a[], char b[]){
    int i = def->n_inits;

    strcpy(def->inits[i].tipo, a);
    def->inits[i].n_cartas = atoi(b);
    def->n_inits++;
}

//Adiciona um movimento MOV à definição do jogo.
void ler_mov(DefinicaoJogo *def, char a[], char b[], char c[]){
    int i = def->n_movimentos;

    strcpy(def->movimentos[i].origem, a);
    strcpy(def->movimentos[i].destino, b);
    strcpy(def->movimentos[i].flags, c);
    def->n_movimentos++;
}

void ler_auto(DefinicaoJogo *def, char a[], char b[], char c[]){
    int i = def->n_automaticos;

    strcpy(def->automaticos[i].origem, a);
    strcpy(def->automaticos[i].destino, b);
    strcpy(def->automaticos[i].flags, c);
    def->n_automaticos++;
}

//Guarda uma nova WIN no array de vitorias[].
void ler_win(DefinicaoJogo *def, char a[], char b[]){
    int i = def->n_vitorias;

    strcpy(def->vitorias[i].tipo, a);
    def->vitorias[i].n_cartas = atoi(b);
    def->n_vitorias++;
}

//Separa uma linha nos seus campos.
void separar_linha(char linha[], char cmd[], char a[], char b[], char c[]){
    sscanf(linha, "%15s %15s %15s %6s", cmd, a, b, c);
}

//Interpreta comandos simples, como JOGO e BARALHOS.
int interpretar_comando_inicial(DefinicaoJogo *def, char cmd[], char a[]){
    int tratado = 1;

    if (strcmp(cmd, "JOGO") == 0) strcpy(def->nome, a);
    else if (strcmp(cmd, "BARALHOS") == 0) def->n_baralhos = atoi(a);
    else tratado = 0;

    return tratado;
}

//Função que limpa completamente as deifnições de um ficheiro, para garantir que não tenha lixo.
void limpar_definicao(DefinicaoJogo *def){
    def->nome[0] = '\0';
    def->n_baralhos = 0;
    def->n_tipos = 0;
    def->n_inits = 0;
    def->n_movimentos = 0;
    def->n_automaticos = 0;
    def->n_vitorias = 0;
}

//Interpreta comandos que criam definições de pilhas.
int interpretar_comando_pilha(DefinicaoJogo *def, char cmd[], char a[], char b[]){
    int tratado = 1;

    if (strcmp(cmd, "TIPO") == 0) ler_tipo(def, a, b);
    else if (strcmp(cmd, "INIT") == 0) ler_init(def, a, b);
    else tratado = 0;

    return tratado;
}

//Interpreta comandos de regras de movimento e vitória.
int interpretar_comando_regra(DefinicaoJogo *def, char cmd[], char a[], char b[], char c[]){
    int tratado = 1;

    if (strcmp(cmd, "MOV") == 0) ler_mov(def, a, b, c);
    else if (strcmp(cmd, "AUTO") == 0) ler_auto(def, a, b, c);
    else if (strcmp(cmd, "WIN") == 0) ler_win(def, a, b);
    else tratado = 0;

    return tratado;
}

//Lê uma linha do ficheiro DSL, identifica o comando e atualiza as definições do jogo.
void interpretar_linha(DefinicaoJogo *def, char linha[]){
    char cmd[TAMANHO_NOME] = "";
    char a[TAMANHO_NOME] = "";
    char b[TAMANHO_NOME] = "";
    char c[TAMANHO_FLAGS] = "";
    int tratado;

    separar_linha(linha, cmd, a, b, c);
    tratado = interpretar_comando_inicial(def, cmd, a);
    if (!tratado) tratado = interpretar_comando_pilha(def, cmd, a, b);
    if (!tratado) interpretar_comando_regra(def, cmd, a, b, c);
}


int ler_definicao(char caminho[], DefinicaoJogo *def){
    FILE *f;
    char linha[TAMANHO_LINHA];

    limpar_definicao(def);
    f = fopen(caminho, "r");

    if (f == NULL) return 0;

    while (fgets(linha, TAMANHO_LINHA, f) != NULL){
        remover_comentario(linha);
        if (linha_tem_conteudo(linha)) interpretar_linha(def, linha);
    }

    fclose(f);
    return 1;
}