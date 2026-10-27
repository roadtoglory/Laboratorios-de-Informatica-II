#ifndef LOGICA_H
#define LOGICA_H

#include <stdio.h>

#include "leitor.h"

#define CARTAS_POR_BARALHO 52 //numero de cartas num baralho
#define MAX_CARTAS_TOTAL 104 //numero de cartas em dois baralhos (caso exista algum jogo no paciencias com baralho > 1)

typedef enum {
    COPAS, ESPADAS, OUROS, PAUS
} Naipe;

typedef enum {
    A, DOIS, TRES, QUATRO, CINCO, SEIS, SETE, OITO, NOVE, DEZ, J ,Q, K
} ValorCarta;

typedef struct {
    Naipe naipe;
    ValorCarta valor;
} Carta;

Carta cria_carta(int valor, int naipe);
void cria_baralho(Carta baralho[], int n_baralhos);
void embaralha_cartas(Carta baralho[], int n_cartas);


// Guarda as cartas de uma pilha e o número atual de cartas.
typedef struct {
    Carta cartas[MAX_CARTAS_TOTAL];
    int n_cartas; //n_cartas é o tamanho real do array de cartas

} PilhaCartas;

// Representa uma pilha real do jogo.
typedef struct {
    char tipo[TAMANHO_NOME];
    char flags[TAMANHO_FLAGS];
    PilhaCartas pilha;

} PilhaJogo;

// Guarda o estado atual da partida.
typedef struct {
    DefinicaoJogo definicao;
    PilhaJogo pilhas[MAX_INITS];
    int n_pilhas;

} EstadoJogo;

// Representa uma jogada feita pelo jogador.
typedef struct {
    int origem;
    int destino;
    int n_cartas;

} Movimento;
Carta cria_carta(int valor, int naipe);
void cria_baralho(Carta baralho[], int n_baralhos);
void embaralha_cartas(Carta baralho[], int n_cartas);


void limpar_pilha(PilhaCartas *pilha);
void adicionar_carta(PilhaCartas *pilha, Carta c);
Carta remover_carta(PilhaCartas *pilha);


int procurar_tipo(DefinicaoJogo *def, char tipo[]);
void copiar_tipo_pilha(EstadoJogo *estado, int indice);
void criar_pilhas_vazias(EstadoJogo *estado);
void distribuir_cartas(EstadoJogo *estado, Carta baralho[]);
void iniciar_estado(DefinicaoJogo *def, EstadoJogo *estado);


int indice_valido(EstadoJogo *estado, int indice);
int verificacao_basica_movimento(EstadoJogo *estado, Movimento m);
int flag_existe(char flags[], char flag);
int regra_tipos_compativeis(RegraMovimento *r, PilhaJogo *origem, PilhaJogo *destino);
int pode_mover_varias(RegraMovimento *r, Movimento m);
Carta carta_topo(PilhaCartas *pilha);
Carta carta_base_movimento(PilhaCartas *pilha, int n_cartas);
int valores_adjacentes(Carta a, Carta b);
int valor_inferior(Carta a, Carta b);
int valor_superior(Carta a, Carta b);


int sequencia_decrescente(PilhaCartas *pilha, int n_cartas);
int sequencia_mesmo_naipe(PilhaCartas *pilha, int n_cartas);
int sequencia_crescente(PilhaCartas *pilha, int n_cartas);
int sequencia_naipes_alternados(PilhaCartas *pilha, int n_cartas);
int mesma_cor(Carta a, Carta b);
int cores_alternadas(PilhaCartas *pilha, int n_cartas);


int tem_as_limite(PilhaCartas *pilha, int n_cartas);
int tem_rei_limite(PilhaCartas *pilha, int n_cartas);
int verificar_ordem_sequencia(PilhaCartas *origem, RegraMovimento *r, int n_cartas);
int verificar_tipo_sequencia(PilhaCartas *origem, RegraMovimento *r, int n_cartas);
int verificar_sequencias(PilhaCartas *origem, RegraMovimento *r, int n_cartas);
int verificar_limites(PilhaCartas *origem, RegraMovimento *r, int n_cartas);
int regras_grupo_movido(PilhaCartas *origem, RegraMovimento *r, int n_cartas);
int destino_vazio_valido(PilhaCartas *destino, RegraMovimento *r);
int destino_naipe_valido(PilhaCartas *destino, RegraMovimento *r, Carta base);
int destino_cor_valida(PilhaCartas *destino, RegraMovimento *r, Carta base);
int verificar_flags_destino(PilhaCartas *destino, RegraMovimento *r, Carta base);
int exige_destino_com_carta(RegraMovimento *r);
int comparar_valor_destino(PilhaCartas *destino, RegraMovimento *r, Carta base);
int verificar_flags_valor(PilhaCartas *destino, RegraMovimento *r, Carta base);
int regra_flags_validas(EstadoJogo *estado, RegraMovimento *r, Movimento m);


int existe_regra_valida(EstadoJogo *estado, Movimento m);
int movimento_e_valido(EstadoJogo *estado, Movimento m);
int descobrir_numero_cartas(EstadoJogo *estado, Movimento *m);
void mover_cartas(EstadoJogo *estado, Movimento m);
void executar_movimento(EstadoJogo *estado, Movimento m);
int regra_vitoria_cumprida(EstadoJogo *estado, RegraVitoria *r);
int jogo_ganho(EstadoJogo *estado);

void preparar_movimento_auto(Movimento *m, int origem, int destino);
int tentar_destinos_auto(EstadoJogo *estado, int regra, int origem);
int tentar_origens_auto(EstadoJogo *estado, int regra);
int tentar_movimento_automatico(EstadoJogo *estado);

void valor_para_str(int valor, char str[]);
int str_para_valor(char str[]);
char naipe_para_char(int naipe);
int char_para_naipe(char c);
void guardar_carta(FILE *f, Carta c);
void guardar_pilha(FILE *f, PilhaJogo *p);
void guardar_todas_pilhas(FILE *f, EstadoJogo *estado);
int guardar_estado(EstadoJogo *estado, char nome_ficheiro[]);
int ler_carta_str(char cartinhaSeparada[], Carta *c);
void ler_pilha_linha(char linha[], PilhaJogo *p);
int carregar_estado(EstadoJogo *estado, char nome_ficheiro[]);

void executar_jogada_utilizador(EstadoJogo *estado, Movimento m);
void tratar_comando_especial(EstadoJogo *estado, Movimento m);
int tratar_undo(EstadoJogo *estado, EstadoJogo *estado_anterior, int *tem_estado_anterior, Movimento m);
int tratar_movimento_normal(EstadoJogo *estado, EstadoJogo *estado_anterior, int *tem_estado_anterior, Movimento m);
int passo_ciclo_jogo(EstadoJogo *estado, EstadoJogo *estado_anterior, int *tem_estado_anterior);

void ciclo_jogo(EstadoJogo *estado);

#endif
