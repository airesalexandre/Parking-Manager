#include "parque.h"

void listar_parques(SistemaParques *sistema) {
    Parque *p_atual = sistema->head_parque;
    while (p_atual != NULL) {
        printf("%s %d %d\n", p_atual->nome, p_atual->capacidade, p_atual->lugares_disponiveis);
        p_atual = p_atual->next;
    }
}

void add_parque(SistemaParques *sistema, const char *nome, int capacidade, float custo_15_1h, float custo_15_apos_1h, float custo_max) {
    Parque *p_atual = sistema->head_parque;
    Parque *p_novo;
    float x = custo_15_1h;
    float y = custo_15_apos_1h;
    float z = custo_max;

    while (p_atual != NULL) {
        if (strcmp(p_atual->nome, nome) == 0) {
            printf("%s: parking already exists.\n", nome);
            return;
        }
        p_atual = p_atual->next;
    }

    if (capacidade <= 0) {
        printf("%d: invalid capacity.\n", capacidade);
        return;
    }

    if (x <= 0 || y <= 0 || z <= 0) {
        printf("invalid cost.\n");
        return;
    }
    if (x > y || y > z) {
        printf("invalid cost.\n");
        return;
    }

    if (sistema->contador_parques >= MAX_PARQUES) {
        printf("too many parks.\n");
        return;
    }

    p_novo = malloc(sizeof(Parque));
    if (!p_novo) {
        printf("Memory allocation error\n");
        return;
    }

    p_novo->nome = malloc(strlen(nome) + 1);
    if (!p_novo->nome) {
        free(p_novo);
        printf("Memory allocation error\n");
        return;
    }
    strcpy(p_novo->nome, nome);
    p_novo->capacidade = capacidade;
    p_novo->lugares_disponiveis = capacidade;
    p_novo->x = x;
    p_novo->y = y;
    p_novo->z = z;
    p_novo->veiculos = NULL;
    p_novo->next = NULL;

    if (sistema->head_parque == NULL) {
        sistema->head_parque = p_novo;
    }
    else {
        Parque *temp = sistema->head_parque;
        while (temp->next != NULL) {
            temp = temp->next;
        }
        temp->next = p_novo;
    }
    sistema->contador_parques++;
}

void rem_parque(SistemaParques *sistema,  const char *nome) {
    Parque *p_atual = sistema->head_parque;
    Parque *p_anterior = NULL;
    Veiculo *v_atual;
    Movimentos *m_atual;
    while (p_atual != NULL && strcmp(p_atual->nome, nome) != 0) {
        p_anterior = p_atual;
        p_atual = p_atual->next;
    }

    if (p_atual == NULL) {
        printf("%s: no such parking.\n", nome);
        return;
    }

    v_atual = p_atual->veiculos;
    while (v_atual != NULL) {
        Veiculo *v_next = v_atual->next;
        m_atual = v_atual->movimentos;
        while (m_atual != NULL) {
            Movimentos *m_next = m_atual->next;
            free(m_atual);
            m_atual = m_next;
        }
        free(v_atual);
        v_atual = v_next;
    }

    if (p_anterior == NULL) {
        sistema->head_parque = p_atual->next;
    }
    else {
        p_anterior->next = p_atual->next;
    }
    free(p_atual->nome);
    free(p_atual);

    print_parques_ordenados(sistema);
    sistema->contador_parques--;
}

//Funcao auxiliar para ajudar a extrair nomes de parques tenham eles aspas ou nao
void extrair_nome(const char *input, char *nome_extraido) {
    const char *p = input;
    
    // Avançar até ao primeiro espaço após comando 'p'
    while (*p != ' ' && *p != '\t' && *p != '\0') p++;
    while (*p == ' ' || *p == '\t') p++;  // Saltar espaços após 'p'

    if (*p == '"') {
        // Nome entre aspas
        p++;  // Saltar aspas
        const char *start = p;
        while (*p != '"' && *p != '\0') p++;
        if (*p == '\0') {
            nome_extraido[0] = '\0';
            return;
        }
        size_t len = p - start;
        strncpy(nome_extraido, start, len);
        nome_extraido[len] = '\0';
    } else {
        // Nome sem aspas
        const char *start = p;
        while (*p != ' ' && *p != '\t' && *p != '\n' && *p != '\0') p++;
        size_t len = p - start;
        strncpy(nome_extraido, start, len);
        nome_extraido[len] = '\0';
    }
}

void print_parques_ordenados(SistemaParques *sistema) {
    int count = 0;
    Parque *p = sistema->head_parque;

    // 1. Contar parques
    while (p != NULL) {
        count++;
        p = p->next;
    }

    if (count == 0) return;

    // 2. Guardar ponteiros para os parques num array
    Parque **array = malloc(count * sizeof(Parque *));
    p = sistema->head_parque;
    for (int i = 0; i < count; i++) {
        array[i] = p;
        p = p->next;
    }

    // 3. Bubble sort por nome
    for (int i = 0; i < count - 1; i++) {
        for (int j = 0; j < count - i - 1; j++) {
            if (strcmp(array[j]->nome, array[j + 1]->nome) > 0) {
                Parque *tmp = array[j];
                array[j] = array[j + 1];
                array[j + 1] = tmp;
            }
        }
    }

    // 4. Imprimir
    for (int i = 0; i < count; i++) {
        printf("%s\n", array[i]->nome);
    }

    free(array);
}

int diferenca_datas_em_minutos(char *data_entrada, char *horas_entrada, char *data_saida, char *horas_saida) {
    int dia_entrada, mes_entrada, ano_entrada;
    int dia_saida, mes_saida, ano_saida;
    int hora_entrada, minuto_entrada, hora_saida, minuto_saida;

    int dias_mes[] = { 31, 28, 31, 30, 31, 30,
                       31, 31, 30, 31, 30, 31 };

    sscanf(data_entrada, "%2d-%2d-%4d", &dia_entrada, &mes_entrada, &ano_entrada);
    sscanf(data_saida, "%2d-%2d-%4d", &dia_saida, &mes_saida, &ano_saida);
    sscanf(horas_entrada, "%2d:%2d", &hora_entrada, &minuto_entrada);
    sscanf(horas_saida, "%2d:%2d", &hora_saida, &minuto_saida);

    int total_dias_entrada = ano_entrada * 365 + (mes_entrada - 1) * 30 + dia_entrada;
    int total_dias_saida = ano_saida * 365 + (mes_saida - 1) * 30 + dia_saida;

    for (int m = 0; m < mes_entrada - 1; m++) {
        total_dias_entrada += (dias_mes[m] - 30);
    }
    for (int m = 0; m < mes_saida - 1; m++) {
        total_dias_saida += (dias_mes[m] - 30);
    }

    int total_minutos_entrada = total_dias_entrada * 24 * 60 + hora_entrada * 60 + minuto_entrada;
    int total_minutos_saida = total_dias_saida * 24 * 60 + hora_saida * 60 + minuto_saida;

    return total_minutos_saida - total_minutos_entrada;
}


float calcular_valor_a_pagar(int diferenca_data, float custo_1h, float custo_apos_1h, float custo_max) {
    float x = custo_1h, y = custo_apos_1h, z = custo_max;
    int um_bloco = 15, conta_blocos = 0;
    int data_atual = diferenca_data;
    float valor_z = 0.0, valor_xy = 0.0;
    int um_dia = 24 * 60;
    
    while (data_atual > 0) {
        if (data_atual >= um_dia) {
            valor_z += z;
            data_atual -= um_dia;
        }
        else if (data_atual < um_dia) {
            if (conta_blocos < 4) {
                valor_xy += x;
                data_atual -= um_bloco;
                conta_blocos++;
            }
            else {
                valor_xy += y;
                data_atual -= um_bloco;
            }
        }
    }
    //Se a soma dos valores de X e Y for maior ou igual a Z, o valor a pagar é Z + valor_Z
    if (valor_xy >= z)
        return z + valor_z;
    //Caso contrário, o valor a pagar é X + Y + valor_Z
    else
        return valor_xy + valor_z;
}