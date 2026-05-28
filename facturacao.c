#include "facturacao.h"

void facturacao_um_arg(SistemaParques *sistema, const char *nome_parque) {
    Parque *p_atual = sistema->head_parque;
    while(p_atual != NULL) {
        if (strcmp(p_atual->nome, nome_parque) == 0) {
            break;
        }
        p_atual = p_atual->next;
    }

    if (p_atual == NULL) {
        printf("%s: no such parking.\n", nome_parque);
        return;
    }

    Facturacao *facturacao = NULL;
    Veiculo *v_atual = p_atual->veiculos;
    while (v_atual != NULL) {
        Movimentos *m_atual = v_atual->movimentos;
        while (m_atual != NULL) {
            if (strlen(m_atual->d_saida) > 0) {
                atualizar_facturacao(&facturacao, m_atual->d_saida, m_atual->valor_pago);
            }
            m_atual = m_atual->next;
        }
        v_atual = v_atual->next;
    }
    
    facturacao = ordenar_facturacao_por_data(facturacao);

    Facturacao *f_atual = facturacao;
    while (f_atual != NULL) {
        printf("%s %.2f\n", f_atual->data, f_atual->facturacao);
        Facturacao *temp = f_atual->next;
        free(f_atual);
        f_atual = temp;
    }
}

void facturacao_dois_args(SistemaParques *sistema, const char *nome_parque, const char *data) {
    Parque *p_atual = sistema->head_parque;
    Facturacao *lista_facturacao = NULL;

    while(p_atual != NULL) {
        if (strcmp(p_atual->nome, nome_parque) == 0) {
            break;
        }
        p_atual = p_atual->next;
    }
    if (p_atual == NULL) {
        printf("%s: no such parking.\n", nome_parque);
        return;
    }
    if (!validar_data(data, "12:12")) { //Hora random so para poder reutilizar a funcao, sendo que 12:12 é sempre aceite
        printf("invalid date.\n");
        return;
    }
    if (!verifica_ult_registo_data(sistema, data)) {
        printf("invalid date.\n");
        return;
    }

    //Percorrer todos os veículos do parque
    Veiculo *v = p_atual->veiculos;
    while (v != NULL) {
        Movimentos *m = v->movimentos;
        while (m != NULL) {
            //Verificar se saiu na data especificada
            if (strcmp(m->d_saida, data) == 0) {
                // Criar novo registo de faturação
                Facturacao *nova_facturacao = malloc(sizeof(Facturacao));
                if (!nova_facturacao) {
                    printf("Memory allocation error\n");
                    // Libertar lista já criada
                    while (lista_facturacao != NULL) {
                        Facturacao *temp = lista_facturacao;
                        lista_facturacao = lista_facturacao->next;
                        free(temp);
                    }
                    return;
                }

                strcpy(nova_facturacao->data, m->d_saida);
                strcpy(nova_facturacao->hora, m->h_saida);
                strcpy(nova_facturacao->matricula, v->matricula);
                nova_facturacao->facturacao = m->valor_pago;
                nova_facturacao->next = lista_facturacao;
                lista_facturacao = nova_facturacao;
            }
            m = m->next;
        }
        v = v->next;
    }

    // Ordenar por hora de saída
    lista_facturacao = ordenar_facturacao_por_hora(lista_facturacao);

    // Imprimir resultados ordenados
    Facturacao *f_atual_ordenado = lista_facturacao;
    while (f_atual_ordenado != NULL) {
        printf("%s %s %.2f\n", f_atual_ordenado->matricula, f_atual_ordenado->hora, f_atual_ordenado->facturacao);
        f_atual_ordenado = f_atual_ordenado->next;
    }

    // Libertar memória
    while (lista_facturacao != NULL) {
        Facturacao *temp = lista_facturacao;
        lista_facturacao = lista_facturacao->next;
        free(temp);
    }
}

void atualizar_facturacao(Facturacao **head, const char *data, float valor) {
    Facturacao *atual = *head;
    Facturacao *anterior = NULL;

    while (atual != NULL) {
        if (strcmp(atual->data, data) == 0) {
            atual->facturacao += valor;
            return;
        }
        anterior = atual;
        atual = atual->next;
    }

    Facturacao *novo = malloc(sizeof(Facturacao));
    strcpy(novo->data, data);
    novo->facturacao = valor;
    novo->next = NULL;

    if (anterior == NULL) {
        *head = novo;
    } else {
        anterior->next = novo;
    }
}

Facturacao* ordenar_facturacao_por_data(Facturacao *head) {
    if (head == NULL) {
        return NULL;
    }

    int trocou;
    do {
        trocou = 0;
        Facturacao **p = &head;  //Ponteiro para ponteiro local
        while ((*p) && (*p)->next) {
            Facturacao *a = *p;
            Facturacao *b = a->next;

            int d1, m1, y1, d2, m2, y2;
            sscanf(a->data, "%2d-%2d-%4d", &d1, &m1, &y1);
            sscanf(b->data, "%2d-%2d-%4d", &d2, &m2, &y2);

            if ((y1 > y2) || (y1 == y2 && m1 > m2) || (y1 == y2 && m1 == m2 && d1 > d2)) {
                //Trocar os nós
                a->next = b->next;
                b->next = a;
                *p = b;
                trocou = 1;
            }
            p = &((*p)->next);
        }
    } while (trocou);
    
    return head;  //Retornar a lista ordenada
}

Facturacao* ordenar_facturacao_por_hora(Facturacao *lista_facturacao) {
    if (lista_facturacao == NULL) {
        return NULL;
    }

    int trocou = 1;
    while (trocou) {
        trocou = 0;
        Facturacao *atual = lista_facturacao;
        Facturacao *proximo = atual->next;
        Facturacao *anterior = NULL;

        while (proximo != NULL) {
            int precisa_trocar = 0;
            
            // Extrair hora e minuto para comparação
            int hora1, min1, hora2, min2;
            sscanf(atual->hora, "%2d:%2d", &hora1, &min1);
            sscanf(proximo->hora, "%2d:%2d", &hora2, &min2);
            
            // Comparar: primeiro por hora, depois por minuto
            if (hora1 > hora2 || (hora1 == hora2 && min1 > min2)) {
                precisa_trocar = 1;
            }

            if (precisa_trocar) {
                // Trocar nós
                if (anterior == NULL) {
                    lista_facturacao = proximo;
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
    return lista_facturacao;
}