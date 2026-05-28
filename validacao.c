#include "validacao.h"

int validar_matricula(const char *matricula) {
    int letras = 0, digitos = 0;
    if (strlen(matricula) != 8 || matricula[2] != '-' || matricula[5] != '-') {
        return 0;
    }

    for (int i = 0; i < 8; i += 3) {
        char c1 = matricula[i];
        char c2 = matricula[i+1];

        if (isupper(c1) && isupper(c2)) {
            letras++;
        }
        else if (isdigit(c1) && isdigit(c2)) {
            digitos++;
        }
        else {
            return 0;
        }
    }

    return letras >= 1 && digitos >= 1;
}

int verificar_matricula_entrada(SistemaParques *sistema, const char *matricula) {
    Parque *p_atual = sistema->head_parque;
    while (p_atual != NULL) {
        Veiculo *v_atual = p_atual->veiculos;
        while (v_atual != NULL) {
            if ((strcmp(v_atual->matricula, matricula) == 0) && v_atual->dentro_parque == 1) {
                return 0;
            }
            v_atual = v_atual->next;
        }
        p_atual = p_atual->next;
    }
    return 1;
}

int verificar_matricula_saida(SistemaParques *sistema, const char *matricula, const char *nome_parque) {
    Parque *p_atual = sistema->head_parque;
    while (p_atual != NULL) {
        if (strcmp(p_atual->nome, nome_parque) == 0) {
            Veiculo *v_atual = p_atual->veiculos;
            while (v_atual != NULL) {
                if (strcmp(v_atual->matricula, matricula) == 0) {
                    if (v_atual->dentro_parque == 0) {
                        return 0; //veículo já saiu
                    } else {
                        return 1; //veículo ainda está dentro
                    }
                }
                v_atual = v_atual->next;
            }
        }
        p_atual = p_atual->next;
    }
    return 0; //matrícula não encontrada
}

int verificar_matricula_registada(SistemaParques *sistema, const char *matricula) {
    Parque *p_atual = sistema->head_parque;
    while (p_atual != NULL) {
        Veiculo *v_atual = p_atual->veiculos;
        while (v_atual != NULL) {
            if (strcmp(v_atual->matricula, matricula) == 0) {
                return 1;
            }
            v_atual = v_atual->next;
        }
        p_atual = p_atual->next;
    }
    return 0;
}

int validar_data(const char *data, const char *horas) {
    int dia, mes, ano, hora, min;
    
    // Verifica o formato com sscanf
    if (sscanf(data, "%2d-%2d-%4d", &dia, &mes, &ano) != 3) {
        return 0;
    }
    // Verifica se os valores estão dentro dos limites básicos
    if (ano < 0 || mes < 1 || mes > 12 || dia < 1) {
        return 0;
    }
    int dias_mes[] = { 31, 28, 31, 30, 31, 30,
                       31, 31, 30, 31, 30, 31 };

    if (dia > dias_mes[mes - 1]) {
        return 0;
    }

    if (sscanf(horas, "%2d:%2d", &hora, &min) != 2) {
        return 0;
    }

    if (hora < 0 || hora > 23 || min < 0 || min > 59) {
        return 0;
    }

    return 1; //Data válida
}

int verifica_ult_registo(SistemaParques *sistema, const char *data, const char *horas) {
    int ano, mes, dia, hora, min;
    int ult_ano, ult_mes, ult_dia, ult_hora, ult_min;

    sscanf(data, "%2d-%2d-%4d", &dia, &mes, &ano);
    sscanf(horas, "%2d:%2d", &hora, &min);
    sscanf(sistema->ultima_data, "%2d-%2d-%4d", &ult_dia, &ult_mes, &ult_ano);
    sscanf(sistema->ultima_hora, "%2d:%2d", &ult_hora, &ult_min);

    if (ano < ult_ano || (ano == ult_ano && mes < ult_mes) ||
        (ano == ult_ano && mes == ult_mes && dia < ult_dia) ||
        (ano == ult_ano && mes == ult_mes && dia == ult_dia && hora < ult_hora) ||
        (ano == ult_ano && mes == ult_mes && dia == ult_dia && hora == ult_hora && min < ult_min)) {
        return 0; //Data e hora inválidas
    }

    return 1; //Data e hora válidas
}

int verifica_ult_registo_data(SistemaParques *sistema, const char *data) {
    int ano, mes, dia;
    int ult_ano, ult_mes, ult_dia;

    sscanf(data, "%2d-%2d-%4d", &dia, &mes, &ano);
    sscanf(sistema->ultima_data, "%2d-%2d-%4d", &ult_dia, &ult_mes, &ult_ano);

    if (ano > ult_ano || (ano == ult_ano && mes > ult_mes) ||
        (ano == ult_ano && mes == ult_mes && dia > ult_dia)) {
        return 0;
    }

    return 1;
}