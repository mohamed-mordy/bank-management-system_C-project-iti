#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <errno.h>
#include <unistd.h>

int code;
int exit_system;
const char *admin_password = "admin";
const char *admin_username = "admin";

void (*systemStatePtr)(void);

void client_options();
void client_sign_in();
void admin_account_options();
void admin_exit_system(void);
void admin_open_existing_account(void);
void admin_create_new_account(void);
void admin_options();
void admin_sign_in();
void main_window();

#if 1
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
#endif

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
    char password[6];
    char username[32];
    int i;
    int first_time = 1;
    do {
        system("clear");
        printf("********** The National Bank of Egypt **********\n");
        if (!first_time) {
            printf("\nInvalid Username or Password; Please try again\n");
        }
        first_time = 0;
        printf("Enter Username: ");
        fflush(stdin);
        scanf("%s", username);
        printf("Enter Password: ");
        getchar();
        for (i = 0; i < 5; i++) {
            scanf("%c", &password[i]);
        }
        password[i] = '\0';
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
        scanf("%d", &code);
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
    system("clear");
    printf("********** The National Bank of Egypt **********\n");

}

void admin_open_existing_account(void)
{
    unsigned long accountID;
    int first_time = 0;
    do {
        system("clear");
        printf("********** The National Bank of Egypt **********\n");
        printf("...Opening an Existing Account...\n");
        printf("Enter Account ID: ");
        scanf("%lu", &accountID);
    } while (0);
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
    system("clear");
    printf("********** The National Bank of Egypt **********\n");
    printf("Select from the following options: \n");
    printf("Make Transaction      ---> Enter 1\n");
    printf("Change Account Status ---> Enter 2\n");
    printf("Get Cash              ---> Enter 3\n");
    printf("Deposit in Account    ---> Enter 4\n");
    printf("Return to main menu   ---> Enter 5\n");
    printf("Input: ");
    //scanf("%d", );
}

void client_sign_in()
{
    system("clear");

}

void client_options()
{
    system("clear");
    printf("********** The National Bank of Egypt **********\n");
    printf("Select from the following options: \n");
    printf("Make Transaction        ---> Enter 1\n");
    printf("Change Account Password ---> Enter 2\n");
    printf("Get Cash                ---> Enter 3\n");
    printf("Deposit in Account      ---> Enter 4\n");
    printf("Return to main menu     ---> Enter 5\n");
    printf("Input: ");
    //scanf("%d", );
}

int main()
{
    exit_system = 0;
    systemStatePtr = main_window;
    while (!exit_system) {
        systemStatePtr();
    }
    return 0;
}

