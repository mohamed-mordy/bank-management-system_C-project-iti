#include "sqlite3.h"
#include <stdlib.h>
#include <assert.h>
#include <stdio.h>
#include "db_access.h"

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

const char *update_password = "UPDATE clients SET password = %s WHERE bankAccountID = %lu";

const char *update_cash = "UPDATE clients SET balance = balance + %lf WHERE bankAccountID = %lu";

const char *create_account = "INSERT INTO clients VALUES(\"%s\"," /* name */
                                                       "\"%s\","  /* address */
                                                       "\"%s\","  /* notionalID */
                                                       "%d,"      /* age */
                                                       "%lu,"      /* bankAccountID */
                                                       "\"%s\","  /* guardianName */
                                                       "\"%s\","  /* guardianNationID */
                                                       "\"%d\","  /* accountStatus */
                                                       "%f,"      /* balance */
                                                       "\"%s\")"; /* password */

const char *select_row = "SELECT * FROM clients WHERE bankAccountID = %lu";

const char *select_all = "SELECT * FROM clients";

const char *update_status = "UPDATE clients SET accountStatus = %d WHERE bankAccountID = %lu";

const char *drop_table = "DROB TABLE clients";
/******************************************************************************************************/

static char sql_statement[1000];


extern sqlite3 *db_connection;

/* Admin Window. */

static int status_callback(void *status, int argc, char **argv, char **azColName)
{
    assert(status);
    /* accountStatus is column 7 in the table */
    sscanf(argv[7], "%d", (int *)status);
    return 0;
}

static int balance_callback(void *balance, int argc, char **argv, char **azColName)
{
    assert(balance);
    /* balance is column 8 in the table */
    sscanf(argv[8], "%lf", (double *)balance);
    return 0;
}

static int callback(void *NotUsed, int argc, char **argv, char **azColName){
    int i;
    for(i=0; i<argc; i++){
        printf("%d: %s = %s\n",i, azColName[i], argv[i] ? argv[i] : "NULL");
    }
    printf("\n");
    return 0;
}

static void generate_password(char *pass)
{
    *pass = '1';
    pass++;
    *pass = '\0';
}

static void generate_unique_id(unsigned long *id)
{
    static unsigned current  = 0;
    *id = current++;
}

void system_init(void)
{
    char *zErrMsg = NULL;
    int rc;
    rc = sqlite3_exec(db_connection, create_table, callback, 0, &zErrMsg);
    if( rc != SQLITE_OK ){
        fprintf(stderr, "SQL error: %s\n", zErrMsg);
        fprintf(stderr, "%s\n", create_table);
        sqlite3_free(zErrMsg);
    }
}

void create_new_account(void)
{
    int rc;
    char *zErrMsg = NULL;
    BankAccount_t account;

    printf("You are about to create a new account!\n");
    printf("Enter the following data to create a new account: ");
    
    /* name */
    printf("\nEnter the name of the client: ");
    scanf("%s", account.name);

    /* address */
    printf("\nEnter the address of the client: ");
    scanf("%s", account.address);

    /* notional ID */
    printf("\nEnter the notional Id of the client: ");
    scanf("%s", account.nationalID);

    /* age */
    printf("\nEnter the age of the client: ");
    scanf("%d", &account.age);

    /* Bank Account ID */
     generate_unique_id(&account.bankAccountID);

    /* guardian name */
    if (account.age < 20) {
        printf("\nEnter the name of the guardian: ");
        scanf("%s", account.guardianName);

        printf("\nEnter the national ID of the guardian: ");
        scanf("%s", account.guardianNationalID);
    } else {
        account.guardianName[0] = '\0'; 
        account.guardianNationalID[0] = '\0';
    }
    account.accountStatus = Active;
    printf("\nEnter the start balance of the client: ");
    scanf("%lf", &account.balance);

    generate_password(account.password);
    printf("\nThe password of the newly create account is %s\n", account.password);

    sprintf(sql_statement, create_account, account.name,
                                           account.address,
                                           account.nationalID,
                                           account.age,
                                           account.bankAccountID,
                                           account.guardianName,
                                           account.guardianNationalID,
                                           account.accountStatus,
                                           account.balance,
                                           account.password);
    rc = sqlite3_exec(db_connection, sql_statement, callback, 0, &zErrMsg);
    if( rc != SQLITE_OK ){
        fprintf(stderr, "SQL error: %s\n", zErrMsg);
        fprintf(stderr, "%s\n", sql_statement);
        sqlite3_free(zErrMsg);
    }
}


void print_all_clients(void) /* for debugging */
{
    char *zErrMsg = NULL;
    int rc;
    rc = sqlite3_exec(db_connection, select_all, callback, 0, &zErrMsg);
    if( rc != SQLITE_OK ){
        fprintf(stderr, "SQL error: %s\n", zErrMsg);
        fprintf(stderr, "%s\n", select_all);
        sqlite3_free(zErrMsg);
    }
}

void make_transaction(unsigned long accountID, double amount)
{
    char *zErrMsg = NULL;
    int rc;
    sprintf(sql_statement, update_cash, -amount, accountID);
    rc = sqlite3_exec(db_connection, sql_statement, callback, 0, &zErrMsg);
    if( rc != SQLITE_OK ){
        fprintf(stderr, "SQL error: %s\n", zErrMsg);
        fprintf(stderr, "%s\n", sql_statement);
        sqlite3_free(zErrMsg);
    }
}

void change_account_status(unsigned long accountID, Account_Status_t newStatus)
{
    char *zErrMsg = NULL;
    int rc;
    //char *stmt = "UPDATE clients SET accountStatus = 1 WHERE bankAccountID = 0";
    sprintf(sql_statement, update_status, newStatus, accountID);
    rc = sqlite3_exec(db_connection, sql_statement, callback, 0, &zErrMsg);
    if( rc != SQLITE_OK ) {
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
    if( rc != SQLITE_OK ) {
        fprintf(stderr, "SQL error: %s\n", zErrMsg);
        fprintf(stderr, "%s\n", sql_statement);
        sqlite3_free(zErrMsg);
    }
    return status;
}

double get_cash(unsigned long accountID)
{
    double cash = 0;

    char *zErrMsg = NULL;
    int rc;
    sprintf(sql_statement, select_row, accountID);
    rc = sqlite3_exec(db_connection, sql_statement, balance_callback, &cash, &zErrMsg);
    if( rc != SQLITE_OK ) {
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
    if( rc != SQLITE_OK ){
        fprintf(stderr, "SQL error: %s\n", zErrMsg);
        fprintf(stderr, "%s\n", sql_statement);
        sqlite3_free(zErrMsg);
    }
}

