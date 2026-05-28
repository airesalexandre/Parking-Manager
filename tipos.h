#ifndef TIPOS_H
#define TIPOS_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#define MAX_PARQUES 20

//Estrutura para armazenar movimentos com nome do parque ordenado
typedef struct movimento_ordenado {
    char *nome_parque;
    char d_entrada[11];
    char h_entrada[6];
    char d_saida[11];
    char h_saida[6];
    int tem_saida;  
    struct movimento_ordenado *next;
} MovimentoOrdenado;

typedef struct facturacao {
    char data[11];
    char hora[6];
    char matricula[9];
    float facturacao;
    struct facturacao *next;
} Facturacao;

typedef struct movimentos {
    char d_entrada[11];
    char d_saida[11];
    char h_entrada[6];
    char h_saida[6];
    float valor_pago;
    struct movimentos *next;
} Movimentos;

typedef struct veiculo {
    char matricula[9];
    int dentro_parque; //1 se dentro e 0 se fora
    Movimentos *movimentos;
    struct veiculo *next;
} Veiculo;

typedef struct parque {
    char *nome;
    int capacidade;
    int lugares_disponiveis;
    float x, y, z;
    Veiculo *veiculos;
    struct parque *next;
} Parque;

//Estrutura para encapsular o estado global
typedef struct sistema_parques {
    Parque *head_parque;
    int contador_parques;
    char ultima_data[11];
    char ultima_hora[6];
} SistemaParques;

#endif