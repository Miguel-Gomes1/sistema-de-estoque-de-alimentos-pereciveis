#include <stdio.h>
#include <sqlite3.h>

// Função de callback para exibir resultados de consultas
static int callback(void *NotUsed, int argc, char **argv, char **azColName) {
    for (int i = 0; i < argc; i++) {
        printf("%s = %s\n", azColName[i], argv[i] ? argv[i] : "NULL");
    }
    printf("\n");
    return 0;
}

int main() {
    sqlite3 *db;
    char *err_msg = 0;
    int rc;

    // Abre ou cria um banco de dados
    rc = sqlite3_open("alimentos.db", &db);
    if (rc != SQLITE_OK) {
        fprintf(stderr, "Não foi possível abrir o banco de dados: %s\n", sqlite3_errmsg(db));
        return 1;
    }

    // Cria uma tabela para alimentos perecíveis
    const char *sql_create = "CREATE TABLE IF NOT EXISTS alimentos("
                              "id INTEGER PRIMARY KEY AUTOINCREMENT,"
                              "nome TEXT NOT NULL,"
                              "categoria TEXT NOT NULL,"
                              "quantidade INT NOT NULL,"
                              "data_validade TEXT NOT NULL);"; // Formato: DD-MM-AAAA

    rc = sqlite3_exec(db, sql_create, 0, 0, &err_msg);
    if (rc != SQLITE_OK) {
        fprintf(stderr, "Erro ao criar tabela: %s\n", err_msg);
        sqlite3_free(err_msg);
        sqlite3_close(db);
        return 1;
    }

    // Consulta todos os alimentos
    const char *sql_select = "SELECT * FROM alimentos;";
    rc = sqlite3_exec(db, sql_select, callback, 0, &err_msg);
    if (rc != SQLITE_OK) {
        fprintf(stderr, "Erro ao consultar dados: %s\n", err_msg);
        sqlite3_free(err_msg);
    }

    // Consulta alimentos próximos do vencimento (formato de data corrigido)
    const char *sql_select_expiring =
        "SELECT * FROM alimentos "
        "WHERE strftime('%d-%m-%Y', data_validade) <= strftime('%d-%m-%Y', date('now', '+7 days'));";

    printf("\nAlimentos próximos do vencimento (próximos 7 dias):\n");
    rc = sqlite3_exec(db, sql_select_expiring, callback, 0, &err_msg);
    if (rc != SQLITE_OK) {
        fprintf(stderr, "Erro ao consultar alimentos próximos do vencimento: %s\n", err_msg);
        sqlite3_free(err_msg);
    }

    // Fecha o banco de dados
    sqlite3_close(db);
    return 0;
}
