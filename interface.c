#include <stdio.h>
#include "interface.h"

void imprimir_valor(Carta c) {
    if (c.valor == A) printf("A");
    else if (c.valor == J) printf("J");
    else if (c.valor == Q) printf("Q");
    else if (c.valor == K) printf("K");
    else printf("%d", c.valor + 1); //+1 por causa do enum que começa em 0.
}

void imprimir_naipe(Carta c) {
    if (c.naipe == COPAS) printf("H");
    else if (c.naipe == ESPADAS) printf("S");
    else if (c.naipe == OUROS) printf("D");
    else printf("C");
}

// Imprime a carta real ou XX, dependendo da visibilidade da pilha.
// Se a carta não for a última ou não tiver flag de visível, então não pode ser impressa.
void imprimir_carta_visivel(PilhaJogo *p, int i) {
    if (flag_existe(p->flags, '_')) printf("XX");
    else if (flag_existe(p->flags, '^') && i < p->pilha.n_cartas - 1) printf("XX");

    else {
        imprimir_valor(p->pilha.cartas[i]);
        imprimir_naipe(p->pilha.cartas[i]);
    }
}

// Printa uma pilha. Recebe o índice da pilha (array começa em 0) e recebe o tipo que queremos printar.
void imprimir_pilha(PilhaJogo *p, int indice) {
    int i;

    printf("Pilha %d (%s): ", indice + 1, p->tipo);
    for (i = 0; i < p->pilha.n_cartas; i++) {
        imprimir_carta_visivel(p, i);
        printf(" ");
    }
    printf("\n");
}

// Imprime todas as pilhas do estado de jogo.
void imprimir_estado(EstadoJogo *estado) {
    int i;

    printf("\nEstado atual:\n");
    for (i = 0; i < estado->n_pilhas; i++) {
        imprimir_pilha(&estado->pilhas[i], i);
    }
}

// Pede ao jogador apenas a origem e o destino.
// O número de cartas é decidido automaticamente pelo programa.
Movimento pedir_movimento(void){
    Movimento m;

    printf("\nOrigem (-1 = undo, -2 = save, -3 = load): ");
    scanf("%d", &m.origem);

    if (m.origem < 0) return m;

    printf("Destino: ");
    scanf("%d", &m.destino);
    m.origem--;
    m.destino--;
    m.n_cartas = 1;

    return m;
}

