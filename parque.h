#ifndef PARQUE_H
#define PARQUE_H

#include "tipos.h"
#include "validacao.h"

void listar_parques(SistemaParques *sistema);
void add_parque(SistemaParques *sistema, const char *nome, int capacidade, float custo_15_1h, float custo_15_apos_1h, float custo_max);
void rem_parque(SistemaParques *sistema, const char *nome);
void print_parques_ordenados(SistemaParques *sistema);
void extrair_nome(const char *input, char *nome_extraido);
int diferenca_datas_em_minutos(char *data_entrada, char *horas_entrada, char *data_saida, char *horas_saida);
float calcular_valor_a_pagar(int diferenca_data, float custo_1h, float custo_apos_1h, float custo_max);

#endif