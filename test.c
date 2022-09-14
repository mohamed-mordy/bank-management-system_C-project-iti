#include <stdio.h>
#include "sqlite3.h"
#include "db_access.h"


sqlite3 *db_connection;

const char *db_name = "Bank.db";

int main(int argc, char **argv)
{
    int rc;
    rc = sqlite3_open(db_name, &db_connection);
    if( rc ){
        fprintf(stderr, "Can't open database: %s\n", sqlite3_errmsg(db_connection));
        sqlite3_close(db_connection);
        return(1);
    }
    /* Start doing real work from here. */

    admin_system_init();
    admin_create_new_account();

    admin_change_account_status(0, closed);
    admin_deposit_in_account(0, 111);
    admin_make_transaction(0, 5);

    admin_print_all_clients(); /* for debugging */

    printf("Amount of cash is: %f\n", admin_get_cash(0));
    printf("status of account is : %d\n", get_status(0));


    /* Your business should be finished here. */
    sqlite3_close(db_connection);
    fprintf(stderr, "Everything seems to be OK\n");
    return 0;
}

#if 0
int main(int argc, char **argv){
    sqlite3 *db;
    char *zErrMsg = 0;
    int rc;

    if( argc!=3 ){
        fprintf(stderr, "Usage: %s DATABASE SQL-STATEMENT\n", argv[0]);
        return(1);
    }
    rc = sqlite3_open(argv[1], &db);
    if( rc ){
        fprintf(stderr, "Can't open database: %s\n", sqlite3_errmsg(db));
        sqlite3_close(db);
        return(1);
    }
    rc = sqlite3_exec(db, argv[2], callback, 0, &zErrMsg);
    if( rc!=SQLITE_OK ){
        fprintf(stderr, "SQL error: %s\n", zErrMsg);
        sqlite3_free(zErrMsg);
    }
    sqlite3_close(db);
    return 0;
}

#endif
