#include "tipos.h"
#include "parque.h"
#include "veiculo.h"
#include "facturacao.h"


void free_tudo(SistemaParques *sistema) {
    while (sistema->head_parque != NULL) {
        Parque *next = sistema->head_parque->next;
        Veiculo *v = sistema->head_parque->veiculos;
        while (v != NULL) {
            Veiculo *next_v = v->next;
            Movimentos *m = v->movimentos;
            while (m != NULL) {
                Movimentos *m_next = m->next;
                free(m);
                m = m_next;
            }
            free(v);
            v = next_v;
        }
        sistema->head_parque->veiculos = NULL;
        free(sistema->head_parque->nome);
        free(sistema->head_parque);
        sistema->head_parque = next;
    }
    sistema->head_parque = NULL;
}

void processar_comandos(SistemaParques *sistema) {
    char input[BUFSIZ];
    char nome_extraido[BUFSIZ];
    char matricula[9], data[11], hora[6];
    int capacidade;
    float custo_15_1h, custo_15_apos_1h, custo_max;

    while (fgets(input, BUFSIZ, stdin)) {
        if (input[0] == '\n' || input[0] == '\0') continue;
        char c = input[0];

        switch(c) {
            case 'q':
                //Liberta os parques e os veiculos como os movimentos associados
                free_tudo(sistema);
                return;
            case 'p':
                //Se receber apenas p
                if (input[1] == '\n' || input[1] == '\0') {
                    listar_parques(sistema);
                    break;
                }

                //Extrair o nome (com ou sem aspas)
                extrair_nome(input, nome_extraido);

                //Procurar onde o nome terminou, strstr pega na string grande e numa substring e verifica onde está e coloca na variavel
                char *pos = strstr(input, nome_extraido);
                if (pos == NULL) {
                    fprintf(stderr, "Erro ao localizar nome no input\n");
                    continue;
                }

                pos += strlen(nome_extraido);

                //Se o nome tinha aspas, saltar também a aspa final
                if (*(pos) == '"') pos++;

                //Saltar espaços
                while (*pos == ' ' || *pos == '\t') pos++;

                if (sscanf(pos, "%d %f %f %f", &capacidade, &custo_15_1h, &custo_15_apos_1h, &custo_max) == 4) {
                    add_parque(sistema, nome_extraido, capacidade, custo_15_1h, custo_15_apos_1h, custo_max);
                } else {
                    fprintf(stderr, "Argumentos inseridos incorretamente\n");
                }
                continue;

            case 'e':
            
                extrair_nome(input, nome_extraido);

                pos = strstr(input, nome_extraido);
                if (pos == NULL) {
                    fprintf(stderr, "Erro ao localizar nome no input\n");
                    continue;
                }

                pos += strlen(nome_extraido);

                if (*(pos) == '"') pos++;

                while (*pos == ' ' || *pos == '\t') pos++;

                if (sscanf(pos, "%s %s %s", matricula, data, hora) == 3) {
                    add_veiculo(sistema, nome_extraido, matricula, data, hora);
                }
                else {
                    fprintf(stderr, "Argumentos inseridos incorretamente\n");
                }
        
                continue;

            case 's':

                extrair_nome(input, nome_extraido);

                pos = strstr(input, nome_extraido);
                if (pos == NULL) {
                    fprintf(stderr, "Erro ao localizar nome no input\n");
                    continue;
                }

                pos += strlen(nome_extraido);

                if (*(pos) == '"') pos++;

                while (*pos == ' ' || *pos == '\t') pos++;

                if (sscanf(pos, "%s %s %s", matricula, data, hora) == 3) {
                    rem_veiculo(sistema, nome_extraido, matricula, data, hora);
                }
                else {
                    fprintf(stderr, "Argumentos inseridos incorretamente\n");
                }
                continue;
            case 'v':
                if (sscanf(input, "v %s", matricula) == 1) {
                    listar_entradas_saidas(sistema, matricula);
                }
                else {
                    fprintf(stderr, "Argumentos inseridos incorretamente\n");
                }
                continue;
            case 'f':
                extrair_nome(input, nome_extraido);

                pos = strstr(input, nome_extraido);
                if (pos == NULL) {
                    fprintf(stderr, "Erro ao localizar nome no input\n");
                    continue;
                }

                pos += strlen(nome_extraido);

                if (*(pos) == '"') pos++;

                while (*pos == ' ' || *pos == '\t') pos++;

                if (*pos == '\n' || *pos == '\0') {
                    facturacao_um_arg(sistema, nome_extraido);
                }
                else if (sscanf(pos, "%s", data) == 1) {
                    facturacao_dois_args(sistema, nome_extraido, data);
                }
                else {
                    fprintf(stderr, "Argumentos inseridos incorretamente\n");
                }
                continue;
            case 'r':
                if (sscanf(input, "r %s", nome_extraido) == 1) {
                    extrair_nome(input, nome_extraido);

                    pos = strstr(input, nome_extraido);
                    if (pos == NULL) {
                        fprintf(stderr, "Erro ao localizar nome no input\n");
                    }
                    rem_parque(sistema, nome_extraido);
                } 
                else {
                    fprintf(stderr, "Argumentos inseridos incorretamente\n");
                }
                continue;
            default:
                fprintf(stderr, "Comando desconhecido: %c\n", c);
                break;
        }
    }
}

int main() {
    //Inicialização do sistema
    SistemaParques sistema = {0};
    strcpy(sistema.ultima_data, "01-01-0001");
    strcpy(sistema.ultima_hora, "00:00");

    processar_comandos(&sistema);

    return 0;
}