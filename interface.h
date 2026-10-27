#ifndef INTERFACE_H
#define INTERFACE_H

#include "logica.h"

void imprimir_valor(Carta c);
void imprimir_naipe(Carta c);
void imprimir_carta_visivel(PilhaJogo *p, int i);
void imprimir_pilha(PilhaJogo *p, int indice);
void imprimir_estado(EstadoJogo *estado);
Movimento pedir_movimento(void);
#endif
