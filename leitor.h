#ifndef LEITOR_H
#define LEITOR_H

#define TAMANHO_NOME 256 //SimpleSimon 11 + o barra zero é o maior, 16 deve chegar
#define TAMANHO_FLAGS 256 //a maior flag tem 6 caracteres + o barra zero
#define MAX_TIPOS 256 //número máximo de tipos de pilha por jogo 
#define MAX_INITS 256 //Número máximo de pilhas no tabuleiro (simple simon tem 14)
#define MAX_REGRAS 256 //Número máximo de regras MOV ou AUTO (o único que tem +12 é o klondike)
#define MAX_VITORIAS 256 //Número máximo de condições WIN
#define TAMANHO_LINHA 256 //Tamanho máximo de uma linha do ficheiro DSL

//Cada uma das seguintes structs guardam o que é necessário para cada comando ser lido da forma correta.

//Guarda um comando TIPO. Contém nome e flag.
typedef struct {
    char nome[TAMANHO_NOME]; //ex: TAB
    char flags[TAMANHO_FLAGS];//ex: =, _
} TipoPilha;

//Guarda um comando INIT. Contém nome e o número de cartas.
typedef struct {
    char tipo[TAMANHO_NOME];
    int n_cartas;
} InitPilha;

//Guarda os comandos MOV e AUTO. Contêm uma pilha do tipo origem, uma pilha do tipo destino e as flags.
typedef struct {
    char origem[TAMANHO_NOME];
    char destino[TAMANHO_NOME];
    char flags[TAMANHO_FLAGS];
} RegraMovimento;

//Guarda o comando WIN. Contêm o número de cartas em uma pilha vitoriosa.
typedef struct {
    char tipo[TAMANHO_NOME];
    int n_cartas;
} RegraVitoria;

//Engloba todos os structs
typedef struct {
    char nome[TAMANHO_NOME];
    int n_baralhos; //<- está aqui o comando dos baralhos

    TipoPilha tipos[MAX_TIPOS];
    int n_tipos;

    InitPilha inits[MAX_INITS];
    int n_inits; // quantos tipos INIT existem

    RegraMovimento movimentos[MAX_REGRAS];
    int n_movimentos; //quantas regras de MOV existem

    RegraMovimento automaticos[MAX_REGRAS];
    int n_automaticos; //quantas regras de AUTO existem

    RegraVitoria vitorias[MAX_VITORIAS];
    int n_vitorias; //quantas regras de WIN existem
} DefinicaoJogo;


void limpar_definicao(DefinicaoJogo *def);
void remover_comentario(char linha[]);
int linha_tem_conteudo(char linha[]);
void ler_tipo(DefinicaoJogo *def, char a[], char b[]);
void ler_init(DefinicaoJogo *def, char a[], char b[]);
void ler_mov(DefinicaoJogo *def, char a[], char b[], char c[]);
void ler_auto(DefinicaoJogo *def, char a[], char b[], char c[]);
void ler_win(DefinicaoJogo *def, char a[], char b[]);
void separar_linha(char linha[], char cmd[], char a[], char b[], char c[]);
int interpretar_comando_inicial(DefinicaoJogo *def, char cmd[], char a[]);
int interpretar_comando_pilha(DefinicaoJogo *def, char cmd[], char a[], char b[]);
int interpretar_comando_regra(DefinicaoJogo *def, char cmd[], char a[], char b[], char c[]);
void interpretar_linha(DefinicaoJogo *def, char linha[]);
int ler_definicao(char caminho[], DefinicaoJogo *def);

#endif