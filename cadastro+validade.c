#include <stdio.h>
#include <string.h>
#include <time.h>
#include <locale.h>

#define RED     "\x1b[31m"
#define YELLOW  "\x1b[33m"
#define GREEN   "\x1b[32m"
#define RESET   "\x1b[0m"

#define MAX 100
#define LIMITE_CRITICO 3
#define LIMITE_ATENCAO 7

typedef struct {
    int codigo;
    char nome[50];
    char categoria[30];
    float peso;
    int quantidade;
    float preco;
    int dia, mes, ano;
    int dias_restantes;
    int prioridade;
} Alimento;

int calcular_dias(int dia, int mes, int ano) {
    time_t agora = time(NULL);
    struct tm t = {0};
    t.tm_mday = dia;
    t.tm_mon  = mes - 1;
    t.tm_year = ano - 1900;
    time_t validade = mktime(&t);
    double diff = difftime(validade, agora);
    return (int)(diff / 86400);
}

void classificar(Alimento *a) {
    int dias = calcular_dias(a->dia, a->mes, a->ano);
    a->dias_restantes = dias;
    if      (dias <= 0)              a->prioridade = 0;
    else if (dias <= LIMITE_CRITICO) a->prioridade = 1;
    else if (dias <= LIMITE_ATENCAO) a->prioridade = 2;
    else                             a->prioridade = 3;
}

void cadastrar_alimento(Alimento estoque[], int *total) {
    if (*total >= MAX) { printf("\n[ERRO] Estoque cheio!\n"); return; }

    Alimento novo;
    novo.codigo = *total + 1;

    printf("\n--- Cadastro de Alimento (ID: %d) ---\n", novo.codigo);
    printf("Nome: ");                     scanf(" %[^\n]", novo.nome);
    printf("Categoria: ");                scanf(" %[^\n]", novo.categoria);
    printf("Peso (kg): ");                scanf("%f", &novo.peso);
    while (getchar() != '\n');
    printf("Quantidade em estoque: ");    scanf("%d", &novo.quantidade);
    while (getchar() != '\n');
    printf("Preco unitario (R$): ");      scanf("%f", &novo.preco);
    while (getchar() != '\n');
    printf("Data de validade (DD MM AAAA): "); scanf("%d %d %d", &novo.dia, &novo.mes, &novo.ano);
    while (getchar() != '\n');

    classificar(&novo);
    estoque[*total] = novo;
    (*total)++;
    printf("\n[SUCESSO] Produto '%s' adicionado!\n", novo.nome);
}

void listar(Alimento estoque[], int total) {
    if (total == 0) { printf("\n[AVISO] Nenhum alimento cadastrado.\n"); return; }

    char *status[] = {"VENCIDO", "CRITICO", "ATENCAO", "OK"};
    printf("\n%-4s %-15s %-12s %-10s %-12s %-15s %-10s %s\n",
           "ID", "Nome", "Categoria", "Qtd", "Preco(R$)", "Validade", "Status", "Dias");
    printf("--------------------------------------------------------------------------------------------\n");

    for (int i = 0; i < total; i++) {
        Alimento a = estoque[i];
        const char *color;
        switch (a.prioridade) {
            case 0:  color = RED;    break;
            case 1:  color = YELLOW; break;
            case 2:  color = YELLOW; break;
            default: color = GREEN;
        }
        printf("%-4d %-15s %-12s %-10d %-12.2f %02d/%02d/%04d [%s%-7s" RESET "] %d dias\n",
               a.codigo, a.nome, a.categoria, a.quantidade, a.preco,
               a.dia, a.mes, a.ano, color, status[a.prioridade], a.dias_restantes);
    }
}

void buscar_produto(Alimento estoque[], int total) {
    if (total == 0) { printf("\n[AVISO] O estoque esta vazio!\n"); return; }

    int escolha;
    printf("\n--- LOCALIZAR PRODUTO ---\n1. Por CODIGO\n2. Por NOME\nEscolha: ");
    scanf("%d", &escolha);
    while (getchar() != '\n');

    if (escolha == 1) {
        int codigo_busca;
        printf("Codigo: "); scanf("%d", &codigo_busca);
        while (getchar() != '\n');
        for (int i = 0; i < total; i++) {
            if (estoque[i].codigo == codigo_busca) {
                printf("\n[ACHOU!] '%s' (R$ %.2f) - Categoria: [%s]\n",
                       estoque[i].nome, estoque[i].preco, estoque[i].categoria);
                return;
            }
        }
    } else if (escolha == 2) {
        char nome_busca[50];
        printf("Nome exato: "); scanf(" %[^\n]", nome_busca);
        for (int i = 0; i < total; i++) {
            if (strcmp(estoque[i].nome, nome_busca) == 0) {
                printf("\n[ACHOU!] ID %d (R$ %.2f) - Categoria: [%s]\n",
                       estoque[i].codigo, estoque[i].preco, estoque[i].categoria);
                return;
            }
        }
    }
    printf("\n[ERRO] Produto nao localizado.\n");
}

void salvar_estoque(Alimento estoque[], int total) {
    FILE *arquivo = fopen("estoque.txt", "w");
    if (arquivo == NULL) { printf("\n[Erro] Nao foi possivel abrir o arquivo.\n"); return; }

    fprintf(arquivo, "%-4s %-15s %-12s %-10s %-12s %-15s %-10s %s\n",
            "ID", "Nome", "Categoria", "Qtd", "Preco(R$)", "Validade", "Status", "Dias");
    fprintf(arquivo, "--------------------------------------------------------------------------------------------\n");

    char *status[] = {"VENCIDO", "CRITICO", "ATENCAO", "OK"};
    for (int i = 0; i < total; i++) {
        Alimento a = estoque[i];
        fprintf(arquivo, "%-4d %-15s %-12s %-10d %-12.2f %02d/%02d/%04d %-10s %d\n",
                a.codigo, a.nome, a.categoria, a.quantidade, a.preco,
                a.dia, a.mes, a.ano, status[a.prioridade], a.dias_restantes);
    }
    fclose(arquivo);
    printf("\n[SUCESSO] Salvo em 'estoque.txt'.\n");
}

void excluir_produtos(Alimento estoque[], int *total) {
    if (*total == 0) { printf("\nEstoque vazio! Nada para excluir.\n"); return; }

    int codigo;
    printf("\nDigite o codigo do produto a excluir: ");
    scanf("%d", &codigo);
    while (getchar() != '\n');

    for (int i = 0; i < *total; i++) {
        if (estoque[i].codigo == codigo) {
            for (int j = i; j < *total - 1; j++)
                estoque[j] = estoque[j + 1];
            (*total)--;
            for (int k = 0; k < *total; k++)
                estoque[k].codigo = k + 1;
            printf("\nProduto com ID %d excluido com sucesso.\n", codigo);
            return;
        }
    }
    printf("\nProduto com ID %d nao encontrado.\n", codigo);
}
void carregar_estoque(Alimento estoque[], int *total) {
    FILE *arquivo = fopen("estoque.txt", "r");
    if (arquivo == NULL) {
        printf("\n[INFO] Nenhum estoque anterior encontrado. Iniciando vazio.\n");
        return;
    }

    char linha[256];
    fgets(linha, sizeof(linha), arquivo); // pula cabeçalho
    fgets(linha, sizeof(linha), arquivo); // pula separador

    while (*total < MAX && fgets(linha, sizeof(linha), arquivo)) {
        Alimento a;
        char status_str[10];
        int lidos = sscanf(linha, "%d %49s %29s %d %f %d/%d/%d %9s %d",
               &a.codigo, a.nome, a.categoria, &a.quantidade, &a.preco,
               &a.dia, &a.mes, &a.ano, status_str, &a.dias_restantes);
        if (lidos == 10) {
            classificar(&a);
            estoque[(*total)++] = a;
        }
    }
    fclose(arquivo);
    printf("\n[INFO] %d produto(s) carregado(s).\n", *total);
}

int main() {
    setlocale(LC_ALL, "portuguese");
    Alimento lista_estoque[MAX];
    int total_produtos = 0;

    carregar_estoque(lista_estoque, &total_produtos);

    int opcao;
    do {
        printf("\n===== CONTROLE DE ESTOQUE MENU =====\n");
        printf("1. Cadastrar Produto\n");
        printf("2. Listar Todos os Produtos\n");
        printf("3. Localizar Produto (Busca)\n");
        printf("4. Salvar arquivo em .TXT\n");
        printf("5. Excluir Produtos\n");
        printf("0. Sair\n");
        printf("Opcao: ");
        scanf("%d", &opcao);
        while (getchar() != '\n');

        if      (opcao == 1) cadastrar_alimento(lista_estoque, &total_produtos);
        else if (opcao == 2) listar(lista_estoque, total_produtos);
        else if (opcao == 3) buscar_produto(lista_estoque, total_produtos);
        else if (opcao == 4) salvar_estoque(lista_estoque, total_produtos);
        else if (opcao == 5) excluir_produtos(lista_estoque, &total_produtos);
        else if (opcao != 0) printf("\n[ERRO] Opcao invalida!\n");

    } while (opcao != 0);

    printf("\nEncerrando. Voce tem %d produto(s) no estoque.\n", total_produtos);
    return 0;
}
