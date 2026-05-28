#ifndef VALIDACAO_H
#define VALIDACAO_H

#include "tipos.h"

int validar_matricula(const char *matricula);
int verificar_matricula_entrada(SistemaParques *sistema, const char *matricula);
int verificar_matricula_saida(SistemaParques *sistema, const char *matricula, const char *nome_parque);
int verificar_matricula_registada(SistemaParques *sistema, const char *matricula);
int validar_data(const char *data, const char *horas);
int verifica_ult_registo(SistemaParques *sistema, const char *data, const char *horas);
int verifica_ult_registo_data(SistemaParques *sistema, const char *data);

#endif