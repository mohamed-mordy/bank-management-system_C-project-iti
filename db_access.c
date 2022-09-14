#include "sqlite3.h"
#include <stdlib.h>
#include <assert.h>
#include <stdio.h>
#include <time.h>
#include "db_access.h"


#define MAX_NUMBER_OF_CLIENTS  1000

unsigned long id_list[MAX_NUMBER_OF_CLIENTS];
static char sql_statement[1000];
static sqlite3 *db_connection;
const char *db_name = "Bank.db";

/******************************************************************************************************/
/*************************************** SQLITE3 QUERIES **********************************************/
const char *create_table = "CREATE TABLE IF NOT EXISTS clients(name VARCHAR[128],"
                                                             "address VARCHAR[128],"
                                                             "notionalID VARCHAR[15],"
                                                             "age INTEGER,"
                                                             "bankAccountID INTEGER PRIMARY KEY,"
                                                             "guardianName VARCHAR[128],"
                                                             "guardianNationID VARCHAR[15],"
                                                             "accountStatus INTEGER,"
                                                             "balance REAL,"
                                                             "password VARCHAR[5])";

const char *create_account = "INSERT INTO clients VALUES(\"%s\"," /* name */
                                                       "\"%s\","  /* address */
                                                       "\"%s\","  /* notionalID */
                                                       "%d,"      /* age */
                                                       "%lu,"      /* bankAccountID */
                                                       "\"%s\","  /* guardianName */
                                                       "\"%s\","  /* guardianNationID */
                                                       "%d,"  /* accountStatus */
                                                       "%f,"      /* balance */
                                                       "\"%s\")"; /* password */

const char *select_row = "SELECT * FROM clients WHERE bankAccountID = %lu";

const char *select_all = "SELECT * FROM clients";

const char *update_status = "UPDATE clients SET accountStatus = %d WHERE bankAccountID = %lu";

const char *update_password = "UPDATE clients SET password = %s WHERE bankAccountID = %lu";

const char *update_cash = "UPDATE clients SET balance = balance + %lf WHERE bankAccountID = %lu";

const char *drop_table = "DROP TABLE IF EXISTS clients";
/******************************************************************************************************/

/* Admin Window. */

static int status_callback(void *status, int argc, char **argv, char **azColName)
{
    assert(status);
    /* accountStatus is column 7 in the table */
    sscanf(argv[7], "%d", (int *)status);
    return 0;
}

static int password_callback(void *password, int argc, char **argv, char **azColName)
{
    assert(password);
    /* password is column 7 in the table */
    sscanf(argv[9], "%s", (char *)password);
    return 0;
}

static int balance_callback(void *balance, int argc, char **argv, char **azColName)
{
    assert(balance);
    /* balance is column 8 in the table */
    sscanf(argv[8], "%lf", (double *)balance);
    return 0;
}

static int select_all_callback(void *, int argc, char **argv, char **azColName)
{
    int i;
    for(i=0; i<argc; i++) {
        printf("%d: %s = %s\n",i, azColName[i], argv[i] ? argv[i] : "NULL");
    }
    printf("\n");
    return 0;
}


static int validate_id_callback(void *count, int argc, char **argv, char **azColName)
{
    *(int *)count = *(int *)count + 1;
    return 0;
}

void generate_password(char *pass)
{
    int i;
    for (i = 0; i < 5; i++) {
        pass[i] = rand()%10 + '0';
    }
    pass[i] = '\0';
}

void generate_unique_id(unsigned long *id)
{
    static unsigned current  = 0;
    assert(current < MAX_NUMBER_OF_CLIENTS);
    *id = id_list[current++];
}

void id_list_init(void)
{
    unsigned long start = 1568969615;
    for (int i = 0; i < MAX_NUMBER_OF_CLIENTS; i++) {
        id_list[i] = start++;
    }
    srand(time(0));
    int j = 3;
    do {
        for (int i = 0; i < MAX_NUMBER_OF_CLIENTS; i++) {
            int temp = id_list[i];
            int rnd = rand()%MAX_NUMBER_OF_CLIENTS;
            id_list[i] = id_list[rnd];
            id_list[rnd] = temp;
        }
        j--;
    } while (j > 0); // do it three times
}

void id_list_print(void)
{
    for (int i = 0; i < MAX_NUMBER_OF_CLIENTS; i++) {
        fprintf(stdout, "%ld\n", id_list[i]);
    }
}

void system_init(void)
{
    char *zErrMsg = NULL;
    int rc;

    rc = sqlite3_open(db_name, &db_connection);
    if( rc ){
        fprintf(stderr, "Can't open database: %s\n", sqlite3_errmsg(db_connection));
        sqlite3_close(db_connection);
    }

    rc = sqlite3_exec(db_connection, drop_table, 0, 0, &zErrMsg);
    if ( rc != SQLITE_OK ) {
        fprintf(stderr, "SQL error: %s\n", zErrMsg);
        fprintf(stderr, "%s\n", drop_table);
        sqlite3_free(zErrMsg);
    }

    rc = sqlite3_exec(db_connection, create_table, 0, 0, &zErrMsg);
    if ( rc != SQLITE_OK ) {
        fprintf(stderr, "SQL error: %s\n", zErrMsg);
        fprintf(stderr, "%s\n", create_table);
        sqlite3_free(zErrMsg);
    }
}

void create_new_account(BankAccount_t *account)
{
    int rc;
    char *zErrMsg = NULL;

    sprintf(sql_statement, create_account, account->name,
                                           account->address,
                                           account->nationalID,
                                           account->age,
                                           account->bankAccountID,
                                           account->guardianName,
                                           account->guardianNationalID,
                                           account->accountStatus,
                                           account->balance,
                                           account->password);
    rc = sqlite3_exec(db_connection, sql_statement, select_all_callback, 0, &zErrMsg);
    if ( rc != SQLITE_OK ) {
        fprintf(stderr, "SQL error: %s\n", zErrMsg);
        fprintf(stderr, "%s\n", sql_statement);
        sqlite3_free(zErrMsg);
    }
}

int is_valid_accountID(unsigned long accountID)
{
    int count = 0;
    char *zErrMsg = NULL;
    int rc;
    sprintf(sql_statement, select_row, accountID);
    rc = sqlite3_exec(db_connection, sql_statement, validate_id_callback, &count, &zErrMsg);
    if ( rc != SQLITE_OK ) {
        fprintf(stderr, "SQL error: %s\n", zErrMsg);
        fprintf(stderr, "%s\n", sql_statement);
        sqlite3_free(zErrMsg);
    }
    return count == 1 ? 1 : 0;
}

void set_password(unsigned long accountID, char *password)
{
    char *zErrMsg = NULL;
    int rc;
    sprintf(sql_statement, update_password, password, accountID);
    rc = sqlite3_exec(db_connection, sql_statement, 0, 0, &zErrMsg);
    if ( rc != SQLITE_OK ) {
        fprintf(stderr, "SQL error: %s\n", zErrMsg);
        fprintf(stderr, "%s\n", sql_statement);
        sqlite3_free(zErrMsg);
    }
}

void get_password(unsigned long accountID, char *password)
{
    char *zErrMsg = NULL;
    int rc;
    sprintf(sql_statement, select_row, accountID);
    rc = sqlite3_exec(db_connection, sql_statement, password_callback, password, &zErrMsg);
    if ( rc != SQLITE_OK ) {
        fprintf(stderr, "SQL error: %s\n", zErrMsg);
        fprintf(stderr, "%s\n", sql_statement);
        sqlite3_free(zErrMsg);
    }
}

void print_all_clients(void) /* for debugging */
{
    char *zErrMsg = NULL;
    int rc;
    rc = sqlite3_exec(db_connection, select_all, select_all_callback, 0, &zErrMsg);
    if ( rc != SQLITE_OK ) {
        fprintf(stderr, "SQL error: %s\n", zErrMsg);
        fprintf(stderr, "%s\n", select_all);
        sqlite3_free(zErrMsg);
    }
}

void make_transaction(unsigned long from, unsigned long to, double amount)
{
    char *zErrMsg = NULL;
    int rc;
    sprintf(sql_statement, update_cash, -amount, from);
    rc = sqlite3_exec(db_connection, sql_statement, 0, 0, &zErrMsg);
    if ( rc != SQLITE_OK ) {
        fprintf(stderr, "SQL error: %s\n", zErrMsg);
        fprintf(stderr, "%s\n", sql_statement);
        sqlite3_free(zErrMsg);
    }

    sprintf(sql_statement, update_cash, amount, to);
    rc = sqlite3_exec(db_connection, sql_statement, 0, 0, &zErrMsg);
    if ( rc != SQLITE_OK ) {
        fprintf(stderr, "SQL error: %s\n", zErrMsg);
        fprintf(stderr, "%s\n", sql_statement);
        sqlite3_free(zErrMsg);
    }
}

void get_cash(unsigned long accountID, double amount)
{
    char *zErrMsg = NULL;
    int rc;
    sprintf(sql_statement, update_cash, -amount, accountID);
    rc = sqlite3_exec(db_connection, sql_statement, 0, 0, &zErrMsg);
    if ( rc != SQLITE_OK ) {
        fprintf(stderr, "SQL error: %s\n", zErrMsg);
        fprintf(stderr, "%s\n", sql_statement);
        sqlite3_free(zErrMsg);
    }
}

void set_status(unsigned long accountID, Account_Status_t status)
{
    char *zErrMsg = NULL;
    int rc;
    sprintf(sql_statement, update_status, status, accountID);
    rc = sqlite3_exec(db_connection, sql_statement, 0, 0, &zErrMsg);
    if ( rc != SQLITE_OK ) {
        fprintf(stderr, "SQL error: %s\n", zErrMsg);
        fprintf(stderr, "%s\n", sql_statement);
        sqlite3_free(zErrMsg);
    }
}

int get_status(unsigned long accountID)
{
    int status = -1;

    char *zErrMsg = NULL;
    int rc;
    sprintf(sql_statement, select_row, accountID);
    rc = sqlite3_exec(db_connection, sql_statement, status_callback, &status, &zErrMsg);
    if ( rc != SQLITE_OK ) {
        fprintf(stderr, "SQL error: %s\n", zErrMsg);
        fprintf(stderr, "%s\n", sql_statement);
        sqlite3_free(zErrMsg);
    }
    return status;
}

double get_balance(unsigned long accountID)
{
    double cash = 0;

    char *zErrMsg = NULL;
    int rc;
    sprintf(sql_statement, select_row, accountID);
    rc = sqlite3_exec(db_connection, sql_statement, balance_callback, &cash, &zErrMsg);
    if ( rc != SQLITE_OK ) {
        fprintf(stderr, "SQL error: %s\n", zErrMsg);
        fprintf(stderr, "%s\n", sql_statement);
        sqlite3_free(zErrMsg);
    }
    return cash;
}

void deposit_in_account(unsigned long accountID, double amount)
{
    char *zErrMsg = NULL;
    int rc;
    sprintf(sql_statement, update_cash, amount, accountID);
    rc = sqlite3_exec(db_connection, sql_statement, NULL, NULL, &zErrMsg);
    if ( rc != SQLITE_OK ) {
        fprintf(stderr, "SQL error: %s\n", zErrMsg);
        fprintf(stderr, "%s\n", sql_statement);
        sqlite3_free(zErrMsg);
    }
}

