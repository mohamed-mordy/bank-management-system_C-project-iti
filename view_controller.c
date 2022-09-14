#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <errno.h>
#include <unistd.h>
#include "db_access.h"

int code;
int exit_system;
const char *admin_password = "admin";
const char *admin_username = "admin";
unsigned long openedAccountID;

void (*systemStatePtr)(void);

int msleep(long msec);
void main_window();
void admin_sign_in();
void admin_options();
void admin_create_new_account(void);
void admin_open_existing_account(void);
void admin_exit_system(void);
void admin_account_options();
void make_transaction_window(void);
void admin_make_transaction(void);
void admin_change_account_status(void);
void get_cash_window(void);
void admin_get_cash(void);
void deposit_in_account_window(void);
void admin_deposit_in_account(void);
void client_sign_in();
void client_options();
void client_make_transaction(void);
void client_change_account_password(void);
void client_get_cash(void);
void client_deposit_in_account(void);
int main();


/* Following function from: https://stackoverflow.com/questions/1157209/is-there-an-alternative-sleep-function-in-c-to-milliseconds */
int msleep(long msec)
{
    struct timespec ts;
    int res;

    if (msec < 0)
    {
        errno = EINVAL;
        return -1;
    }

    ts.tv_sec = msec / 1000;
    ts.tv_nsec = (msec % 1000) * 1000000;

    do {
        res = nanosleep(&ts, &ts);
    } while (res && errno == EINTR);

    return res;
}

void main_window()
{
    int code;
    int first_time = 1;
    do {
        system("clear");
        printf("********** The National Bank of Egypt **********\n");
        if (!first_time) {
            printf("\nInvalid Input; Pleasy try again\n");
        }
        first_time = 0;
        printf("...\n");
        printf("...\n");
        printf("...\n");
        printf("...To continue as an admin ---> Enter 1\n");
        printf("...To continue as a client ---> Enter 2\n");
        printf("Input: ");
        scanf("%d", &code);
        getchar();
    } while (! (code == 1 || code == 2));
    switch (code) {
        case 1:
            systemStatePtr = admin_sign_in;
            break;
        case 2:
            systemStatePtr = client_sign_in;
            break;
        default:
            /* Error. */
            break;
    }
}

void admin_sign_in()
{
    char password[32];
    char username[32];
    int first_time = 1;
    do {
        system("clear");
        printf("********** The National Bank of Egypt **********\n");
        if (!first_time) {
            printf("\nInvalid Username or Password; Please try again\n");
        }
        first_time = 0;
        printf("Enter Username: ");
        scanf("%s", username);
        getchar();
        printf("Enter Password: ");
        scanf("%s", password);
        getchar();
    } while ( !(strcmp(password, admin_password) == 0 && strcmp(username, admin_username) == 0) );
    systemStatePtr = admin_options;
}

void admin_options()
{
    int first_time = 1;
    int code;
    do {
        system("clear");
        printf("********** The National Bank of Egypt **********\n");
        if (!first_time) {
            printf("\nInvalid Input; Please try again\n");
        }
        first_time = 0;
        printf("Select one of the following options: \n");
        printf("Create New Account    ---> Enter 1\n");
        printf("Open Existing Account ---> Enter 2\n");
        printf("Exit System           ---> Enter 3\n");
        printf("Input: ");
        scanf("%d", &code); /* bug: will result in error if entered character is not digit. */
        getchar(); /* Consume newline character. */
    } while (!(code >= 1 && code <= 3));
    switch (code) {
        case 1:
            systemStatePtr = admin_create_new_account;
            break;
        case 2:
            systemStatePtr = admin_open_existing_account;
            break;
        case 3:
            systemStatePtr = admin_exit_system;
            break;
        default:
            /* Error! */
            break;
    }
}

void admin_create_new_account(void)
{
    BankAccount_t account;
    system("clear");
    printf("********** The National Bank of Egypt **********\n");
    printf("Enter the following data to create a new account: ");
    
    /* name */
    printf("\nEnter the name of the client: ");
    scanf("%s", account.name);
    getchar();

    /* address */
    printf("\nEnter the address of the client: ");
    scanf("%s", account.address);
    getchar();

    /* notional ID */
    printf("\nEnter the notional Id of the client: ");
    scanf("%s", account.nationalID);
    getchar();

    /* age */
    printf("\nEnter the age of the client: ");
    scanf("%d", &account.age);
    getchar();

    /* guardian name */
    if (account.age < 20) {
        printf("\nEnter the name of the guardian: ");
        scanf("%s", account.guardianName);
        getchar();

        printf("\nEnter the national ID of the guardian: ");
        scanf("%s", account.guardianNationalID);
        getchar();
    } else {
        account.guardianName[0] = '\0'; 
        account.guardianNationalID[0] = '\0';
    }
    account.accountStatus = Active;
    printf("\nEnter the start balance of the client: ");
    scanf("%lf", &account.balance);
    getchar();

    /* Bank Account ID */
    generate_unique_id(&account.bankAccountID);

    /* Bank Account Password */
    generate_password(account.password);

    /* Store account data in the database */
    create_new_account(&account);

    printf("\nThe ID of your account is: %ld\n", account.bankAccountID);
    printf("\nThe password of your account is %s\n", account.password);
    
    int code;

    printf("Create New Account      ---> Enter 1\n");
    printf("Back to admin menu      ---> Enter 2\n");
    printf("Input: ");
    scanf("%d", &code);
    getchar();

    switch (code) {
        case 1:
            /* systemStatePtr already points to this function. */
            break;
        case 2:
            systemStatePtr = admin_options;
            break;
        default:
            /* should be in Error. */
            /* but anyway goto admin_options() */
            systemStatePtr = admin_options;
            break;
    }
}

void admin_open_existing_account(void)
{
    unsigned long accountID;
    int first_time = 1;
    do {
        system("clear");
        printf("********** The National Bank of Egypt **********\n");
        if (!first_time) {
            printf("\n...Invalid Account ID; Enter a valid Account ID.\n");
        }
        first_time = 0;
        printf("...Opening an Existing Account...\n");
        printf("Enter Account ID: ");
        scanf("%lu", &accountID);
        getchar();
    } while ( is_valid_accountID(accountID) != 1 );
    openedAccountID = accountID;
    systemStatePtr = admin_account_options;
}

void admin_exit_system(void)
{
    system("clear");
    printf("********** The National Bank of Egypt **********\n");
    printf(".\n.\n.\n.\n");
    printf("        The System is halting..\n");
    for (int i = 0; i < 32; i++) {
        printf(".");
        fflush(stdout);
        msleep(100);
    }
    printf("\n");
    exit_system = 1;
}

void admin_account_options()
{
    int code;
    int first_time = 1;
    do {
        system("clear");
        printf("********** The National Bank of Egypt **********\n");
        if (!first_time) {
            printf("\nInvalid input; Try again\n\n");
        }
        printf("Select from the following options: \n");
        printf("Make Transaction      ---> Enter 1\n");
        printf("Change Account Status ---> Enter 2\n");
        printf("Get Cash              ---> Enter 3\n");
        printf("Deposit in Account    ---> Enter 4\n");
        printf("Return to main menu   ---> Enter 5\n");
        printf("Input: ");
        scanf("%d", &code);
        getchar();
    } while ( !(code >= 1 && code <= 5) );
    switch (code) {
        case 1:
            systemStatePtr = admin_make_transaction;
            break;
        case 2:
            systemStatePtr = admin_change_account_status;
            break;
        case 3:
            systemStatePtr = admin_get_cash;
            break;
        case 4:
            systemStatePtr = admin_deposit_in_account;
            break;
        case 5:
            systemStatePtr = main_window;
            break;
        default:
            /* Error. */
            break;
    }
}

void make_transaction_window(void)
{
    double available_cash;
    double transaction_amount;
    unsigned long accountID;
    int first_time = 1;

    do {
        system("clear");
        printf("********** The National Bank of Egypt **********\n");
        if (!first_time) {
            printf("Invalid Account ID; try again\n");
        }
        first_time = 0;
        printf("Enter the Account ID:");
        scanf("%lu", &accountID);
        getchar(); // consume newline character
    } while (is_valid_accountID(accountID));

    if (get_status(openedAccountID) == Active &&
            get_status(accountID) == Active) {
        available_cash = get_balance(openedAccountID);
        if (available_cash > 0) {
            first_time = 1;
            do {
                system("clear");
                printf("********** The National Bank of Egypt **********\n");
                if (!first_time) {
                    printf("Transaction amount is greater than available cash\n");
                    printf("Enter a valid amount of cash to transfer\n");
                }
                first_time = 0;
                printf("Enter the Amount of Cash to transfer:");
                scanf("%lf", &transaction_amount);
                getchar(); // consume newline character
            } while (transaction_amount > available_cash);
            make_transaction(openedAccountID, accountID, transaction_amount);
            printf("\n Transaction done sccessfully \n");
            msleep(2500);
        } else {
            printf(" this account doesn't have any money\n");
            msleep(2500);
        }
    } else {
        printf("\n can not complete transaction process\n");
        printf("\n One of the accounts or both seems to be in-active\n");
        msleep(2500);
    }
}

void admin_make_transaction(void)
{
    make_transaction_window();
    systemStatePtr = admin_account_options;
}

void admin_change_account_status(void)
{
    int code;
    int first_time = 1;
    do {
        system("clear");
        printf("********** The National Bank of Egypt **********\n");
        if (!first_time) {
            printf("\nInvalid input; Try again\n\n");
        }
        printf("Select from the following options: \n");
        printf("Set status to Active     ---> Enter 1\n");
        printf("Set status to Restricted ---> Enter 2\n");
        printf("Set status to Closed     ---> Enter 3\n");
        printf("Input: ");
        scanf("%d", &code);
        getchar();
    } while ( !(code >= 1 && code <= 3) );
    set_status(openedAccountID, code - 1);
    printf("Account Status is set successfully\n");
    msleep(2500);
    systemStatePtr = admin_account_options;
}

void get_cash_window(void)
{
    double available_cash;
    double transaction_amount;
    int first_time = 1;
    available_cash = get_balance(openedAccountID);
    do {
        system("clear");
        printf("********** The National Bank of Egypt **********\n");
        if (!first_time) {
            printf("Transaction amount is greater than available cash\n");
            printf("Enter a valid amount of cash to transfer\n");
        }
        first_time = 0;
        printf("Enter the Amount of Cash to transfer:");
        scanf("%lf", &transaction_amount);
        getchar(); // consume newline character
    } while (transaction_amount > available_cash);
    get_cash(openedAccountID, transaction_amount);
    printf("\n Transaction done sccessfully \n");
    msleep(2500);
}

void admin_get_cash(void)
{
    get_cash_window();
    systemStatePtr = admin_account_options;
}

void deposit_in_account_window(void)
{
    double transaction_amount = 0;
    system("clear");
    printf("********** The National Bank of Egypt **********\n");
    printf("Enter the Amount of Cash to doposit:");
    scanf("%lf", &transaction_amount);
    deposit_in_account(openedAccountID, transaction_amount);
    printf("\n Deposition done sccessfully \n");
    msleep(2500);
}

void admin_deposit_in_account(void)
{
    deposit_in_account_window();
    systemStatePtr = admin_account_options;
}

void client_sign_in()
{
    char password[32];
    unsigned long accountID;
    char stored_password[32];
    int first_time = 1;
    do {
        system("clear");
        printf("********** The National Bank of Egypt **********\n");
        if (!first_time) {
            printf("\nInvalid Account ID or Password; Please try again\n");
        }
        first_time = 0;
        printf("Enter accountID: ");
        scanf("%ld", &accountID);
        getchar();
        printf("Enter Password: ");
        scanf("%s", password);
        getchar();
        if (is_valid_accountID(accountID)) {
            get_password(accountID, stored_password);
        }
    } while ( !(is_valid_accountID(accountID) && 
                strcmp(password, stored_password) == 0));
    openedAccountID = accountID;
    systemStatePtr = client_options;
}

void client_options()
{
    int code;
    int first_time = 1;
    do {
        system("clear");
        printf("********** The National Bank of Egypt **********\n");
        if (!first_time) {
            printf("\nInvalid input; Try again\n\n");
        }
        printf("Select from the following options: \n");
        printf("Make Transaction        ---> Enter 1\n");
        printf("Change Account Password ---> Enter 2\n");
        printf("Get Cash                ---> Enter 3\n");
        printf("Deposit in Account      ---> Enter 4\n");
        printf("Return to main menu     ---> Enter 5\n");
        printf("Input: ");
        scanf("%d", &code);
        getchar();
    } while ( !(code >= 1 && code <= 5) );
    switch (code) {
        case 1:
            systemStatePtr = client_make_transaction;
            break;
        case 2:
            systemStatePtr = client_change_account_password;
            break;
        case 3:
            systemStatePtr = client_get_cash;
            break;
        case 4:
            systemStatePtr = client_deposit_in_account;
            break;
        case 5:
            systemStatePtr = main_window;
            break;
        default:
            /* Error. */
            break;
    }
}

void client_make_transaction(void)
{
    make_transaction_window();
    systemStatePtr = client_options;
}

void client_change_account_password(void)
{
    char password[32];
    system("clear");
    printf("********** The National Bank of Egypt **********\n");
    printf("Enter new password: ");
    scanf("%s", password);
    // getchar(); /* Consume newline character. */
    set_password(openedAccountID, password);
    printf("\nAccount password is changed successfully\n");
    msleep(2500);
    systemStatePtr = client_options;
}

void client_get_cash(void)
{
    get_cash_window();
    systemStatePtr = client_options;
}

void client_deposit_in_account(void)
{
    deposit_in_account_window();
    systemStatePtr = client_options;
}

int main()
{
    id_list_init();
    system_init();
    exit_system = 0;
    systemStatePtr = main_window;
    while (!exit_system) {
        systemStatePtr();
    }
    return 0;
}

