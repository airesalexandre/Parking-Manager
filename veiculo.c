#include "veiculo.h"
#include "validacao.h"

void add_veiculo(SistemaParques *sistema ,char *nome_p, const char *matricula, const char *data, const char *horas) {
    Parque *p_atual = sistema->head_parque;
    Veiculo *v_existente = NULL;
    Veiculo *v_novo;
    Movimentos *m_novo;


    int h, min;
    char hora_corrigida[6];

    // Encontrar o parque
    while (p_atual != NULL) {
        if (strcmp(p_atual->nome, nome_p) == 0) {
            break;
        }
        p_atual = p_atual->next;
    }

    if (p_atual == NULL) {
        printf("%s: no such parking.\n", nome_p);
        return;
    }
    if (p_atual->lugares_disponiveis <= 0) {
        printf("%s: parking is full.\n", nome_p);
        return;
    }

    // Validações (mantém as mesmas)
    if (!validar_matricula(matricula)) {
        printf("%s: invalid licence plate.\n", matricula);
        return;
    }
    if (!verificar_matricula_entrada(sistema, matricula)) {
        printf("%s: invalid vehicle entry.\n", matricula);
        return;
    }
    if (!validar_data(data, horas)) {
        printf("invalid date.\n");
        return;
    }
    if (!verifica_ult_registo(sistema, data, horas)) {
        printf("invalid date.\n");
        return;
    }

    sscanf(horas, "%d:%d", &h, &min);
    sprintf(hora_corrigida, "%02d:%02d", h, min);

    //PROCURAR SE O VEÍCULO JÁ EXISTE (independentemente de estar dentro ou fora)
    Veiculo *v_atual = p_atual->veiculos;
    while (v_atual != NULL) {
        if (strcmp(v_atual->matricula, matricula) == 0) {
            v_existente = v_atual;
            break;
        }
        v_atual = v_atual->next;
    }

    if (v_existente != NULL) {
        // VEÍCULO JÁ EXISTE NA LISTA DO PARQUE - REUTILIZAR
        v_existente->dentro_parque = 1;  //Atualizar estado para dentro
        v_novo = v_existente;
    } else {
        //VEÍCULO NÃO EXISTE - criar novo
        v_novo = malloc(sizeof(Veiculo));
        if (!v_novo) {
            printf("Memory allocation error\n");
            return;
        }
        strcpy(v_novo->matricula, matricula);
        v_novo->dentro_parque = 1;
        v_novo->movimentos = NULL;
        v_novo->next = p_atual->veiculos;
        p_atual->veiculos = v_novo;  //Adicionar à lista
    }

    //Criar novo movimento
    m_novo = malloc(sizeof(Movimentos));
    if (!m_novo) {
        if (!v_existente) {
            free(v_novo);  //Só free se foi criado agora
        }
        printf("Memory allocation error\n");
        return;
    }

    strcpy(m_novo->d_entrada, data);
    strcpy(m_novo->h_entrada, hora_corrigida);
    strcpy(m_novo->d_saida, "");
    strcpy(m_novo->h_saida, "");
    m_novo->next = v_novo->movimentos;
    v_novo->movimentos = m_novo;// Adicionar movimento à lista

    p_atual->lugares_disponiveis--;

    strcpy(sistema->ultima_data, data);
    strcpy(sistema->ultima_hora, hora_corrigida);

    printf("%s %d\n", nome_p, p_atual->lugares_disponiveis);
}

void rem_veiculo(SistemaParques *sistema, char *nome_p, const char *matricula, const char *data, const char *horas) {
    Parque *p_atual = sistema->head_parque;
    Veiculo *v;
    Movimentos *m;
    int h, min;
    char hora_corrigida[6];
    int diferenca_datas;
    float valor_pago;

    while(p_atual != NULL) {
        if (strcmp(p_atual->nome, nome_p) == 0) {
            v = p_atual->veiculos;
            break;
        }
        p_atual = p_atual->next;
    }

    if (p_atual == NULL) {
        printf("%s: no such parking.\n", nome_p);
        return;
    }
    if (!validar_matricula(matricula)) {
        printf("%s: invalid licence plate.\n", matricula);
        return;
    }

    if (!verificar_matricula_saida(sistema, matricula, p_atual->nome)) {
        printf("%s: invalid vehicle exit.\n", matricula);
        return;
    }
    if (!validar_data(data, horas)) {
        printf("invalid date.\n");
        return;
    }
    if (!verifica_ult_registo(sistema, data, horas)) {
        printf("invalid date.\n");
        return;
    }

    while (v != NULL) {
        if (strcmp(v->matricula, matricula) == 0 && v->dentro_parque == 1) {
            break;
        }
        v = v->next;
    }

    if (v == NULL) {
        printf("%s: vehicle not found or already out.\n", matricula);
        return;
    }
    m = v->movimentos;
    if (m == NULL) {
        printf("No entry record found.\n");
        return;
    }

    sscanf(horas, "%d:%d", &h, &min);
    sprintf(hora_corrigida, "%02d:%02d", h, min);

    strcpy(m->d_saida, data);
    strcpy(m->h_saida, hora_corrigida);
    v->dentro_parque = 0;
    p_atual->lugares_disponiveis++;

    diferenca_datas = diferenca_datas_em_minutos(m->d_entrada, m->h_entrada, m->d_saida, m->h_saida);
  
    valor_pago = calcular_valor_a_pagar(diferenca_datas, p_atual->x, p_atual->y, p_atual->z);
    m->valor_pago = valor_pago;

    strcpy(sistema->ultima_data, data);
    strcpy(sistema->ultima_hora, hora_corrigida);

    printf("%s %s %s %s %s %.2f\n", v->matricula, m->d_entrada, m->h_entrada, m->d_saida, m->h_saida, m->valor_pago);
}

void listar_entradas_saidas(SistemaParques *sistema, const char *matricula) {
    Parque *p_atual = sistema->head_parque;
    MovimentoOrdenado *lista_movimentos = NULL;
    MovimentoOrdenado *ultimo = NULL;

    if (!validar_matricula(matricula)) {
        printf("%s: invalid licence plate.\n", matricula);
        return;
    }
    
    if (!verificar_matricula_registada(sistema, matricula)) {
        printf("%s: no entries found in any parking.\n", matricula);
        return;
    }

    // Percorrer todos os parques
    while (p_atual != NULL) {
        // Percorrer todos os veículos do parque atual
        Veiculo *v_atual = p_atual->veiculos;
        while (v_atual != NULL) {
            if (strcmp(v_atual->matricula, matricula) == 0) {
                // Encontrou o veículo, percorrer seus movimentos
                Movimentos *m_atual = v_atual->movimentos;
                while (m_atual != NULL) {
                    // Criar novo movimento na lista
                    MovimentoOrdenado *novo_mov = malloc(sizeof(MovimentoOrdenado));
                    if (!novo_mov) {
                        printf("Memory allocation error\n");
                        while (lista_movimentos != NULL) {
                            MovimentoOrdenado *temp = lista_movimentos;
                            lista_movimentos = lista_movimentos->next;
                            free(temp->nome_parque);
                            free(temp);
                        }
                        return;
                    }
                
                    novo_mov->nome_parque = malloc(strlen(p_atual->nome) + 1);
                    if (!novo_mov->nome_parque) {
                        free(novo_mov);
                        printf("Memory allocation error\n");
                        // Libertar memória já alocada
                        while (lista_movimentos != NULL) {
                            MovimentoOrdenado *temp = lista_movimentos;
                            lista_movimentos = lista_movimentos->next;
                            free(temp->nome_parque);
                            free(temp);
                        }
                        return;
                    }
                    strcpy(novo_mov->nome_parque, p_atual->nome);
                    
                    strcpy(novo_mov->d_entrada, m_atual->d_entrada);
                    strcpy(novo_mov->h_entrada, m_atual->h_entrada);
                    strcpy(novo_mov->d_saida, m_atual->d_saida);
                    strcpy(novo_mov->h_saida, m_atual->h_saida);
                    
                    // Verificar se tem saída (strings vazias significam sem saída)
                    novo_mov->tem_saida = (strlen(m_atual->d_saida) > 0 && strlen(m_atual->h_saida) > 0);
                    novo_mov->next = NULL;

                    // Adicionar à lista
                    if (lista_movimentos == NULL) {
                        lista_movimentos = novo_mov;
                        ultimo = novo_mov;
                    } else {
                        ultimo->next = novo_mov;
                        ultimo = novo_mov;
                    }
                    
                    m_atual = m_atual->next;
                }
            }
            v_atual = v_atual->next;
        }
        p_atual = p_atual->next;
    }

    //CHAMAR A FUNÇÃO DE ORDENAÇÃO E RECEBER A LISTA ORDENADA
    lista_movimentos = ordenacao_lista(lista_movimentos);

    //Imprimir movimentos ordenados
    MovimentoOrdenado *mov = lista_movimentos;
    while (mov != NULL) {
        if (mov->tem_saida) {
            printf("%s %s %s %s %s\n", 
                    mov->nome_parque, mov->d_entrada, mov->h_entrada, 
                    mov->d_saida, mov->h_saida);
        } else {
            printf("%s %s %s\n", 
                    mov->nome_parque, mov->d_entrada, mov->h_entrada);
        }
        mov = mov->next;
    }

    // Libertar memória da lista temporária (incluindo nomes dos parques)
    mov = lista_movimentos;
    while (mov != NULL) {
        MovimentoOrdenado *temp = mov;
        mov = mov->next;
        free(temp->nome_parque);  // Libertar nome alocado dinamicamente
        free(temp);
    }
}

MovimentoOrdenado* ordenacao_lista(MovimentoOrdenado *lista_movimentos) {
    // Ordenar lista por nome do parque, depois por data/hora de entrada
    if (lista_movimentos == NULL) {
        return NULL;
    }
    
    int trocou = 1;
    while (trocou) {
        trocou = 0;
        MovimentoOrdenado *atual = lista_movimentos;
        MovimentoOrdenado *proximo = atual->next;
        MovimentoOrdenado *anterior = NULL;
        
        while (proximo != NULL) {
            int precisa_trocar = 0;
            
            // Comparar primeiro por nome do parque
            int cmp_parque = strcmp(atual->nome_parque, proximo->nome_parque);
            if (cmp_parque > 0) {
                precisa_trocar = 1;
            } else if (cmp_parque == 0) {
                // Mesmo parque, comparar por data/hora de entrada
                int ano1, mes1, dia1, hora1, min1;
                int ano2, mes2, dia2, hora2, min2;
                
                sscanf(atual->d_entrada, "%2d-%2d-%4d", &dia1, &mes1, &ano1);
                sscanf(atual->h_entrada, "%2d:%2d", &hora1, &min1);
                sscanf(proximo->d_entrada, "%2d-%2d-%4d", &dia2, &mes2, &ano2);
                sscanf(proximo->h_entrada, "%2d:%2d", &hora2, &min2);
                
                if (ano1 > ano2 || (ano1 == ano2 && mes1 > mes2) ||
                    (ano1 == ano2 && mes1 == mes2 && dia1 > dia2) ||
                    (ano1 == ano2 && mes1 == mes2 && dia1 == dia2 && hora1 > hora2) ||
                    (ano1 == ano2 && mes1 == mes2 && dia1 == dia2 && hora1 == hora2 && min1 > min2)) {
                    precisa_trocar = 1;
                }
            }
            
            if (precisa_trocar) {
                // Trocar nós
                if (anterior == NULL) {
                    lista_movimentos = proximo;
                } else {
                    anterior->next = proximo;
                }
                atual->next = proximo->next;
                proximo->next = atual;
                
                trocou = 1;
                anterior = proximo;
                proximo = atual->next;
            } else {
                anterior = atual;
                atual = proximo;
                proximo = proximo->next;
            }
        }
    }
    return lista_movimentos;
}