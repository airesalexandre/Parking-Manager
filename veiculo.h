#ifndef VEICULOS_H
#define VEICULOS_H

#include "tipos.h"
#include "parque.h"
#include "validacao.h"

void add_veiculo(SistemaParques *sistema, char *nome_p, const char *matricula, const char *data, const char *horas);
void rem_veiculo(SistemaParques *sistema, char *nome_p, const char *matricula, const char *data, const char *horas);
void listar_entradas_saidas(SistemaParques *sistema, const char *matricula);
MovimentoOrdenado* ordenacao_lista(MovimentoOrdenado *lista_movimentos);

#endif