#ifndef FACTURACAO_H
#define FACTURACAO_H

#include "tipos.h"
#include "validacao.h"

void facturacao_um_arg(SistemaParques *sistema, const char *nome_parque);
void facturacao_dois_args(SistemaParques *sistema, const char *nome_parque, const char *data);
void atualizar_facturacao(Facturacao **head, const char *data, float valor);
Facturacao* ordenar_facturacao_por_data(Facturacao *head);
Facturacao* ordenar_facturacao_por_hora(Facturacao *lista_facturacao);

#endif