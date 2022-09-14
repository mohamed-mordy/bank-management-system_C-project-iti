#ifndef BANK_ACCOUNT_H_
#define BANK_ACCOUNT_H_

/*
 


*/
typedef enum {
    Active,
    Restricted,
    closed,
} Account_Status_t;

typedef struct {
    char name[128];
    char address[128];
    char nationalID[15];
    int age;
    unsigned long bankAccountID;
    char guardianName[128];
    char guardianNationalID[15];
    /* Note: if account is Restricted or Closed you can't perform any action on it. */
    Account_Status_t accountStatus;
    double balance;
    char password[5];
} BankAccount_t;


void generate_password(char *pass);
void generate_unique_id(unsigned long *id);
void id_list_init(void);
void id_list_print(void);
void system_init(void);
void create_new_account(BankAccount_t *account);
int is_valid_accountID(unsigned long accountID);
void set_password(unsigned long accountID, char *password);
void get_password(unsigned long accountID, char *password);
void make_transaction(unsigned long from, unsigned long to, double amount);
void get_cash(unsigned long accountID, double amount);
void change_account_status(unsigned long accountID, Account_Status_t newStatus);
void set_status(unsigned long accountID, Account_Status_t status);
int get_status(unsigned long accountID);
double get_balance(unsigned long accountID);
void deposit_in_account(unsigned long accountID, double amount);


#endif /* BANK_ACCOUNT_H_ */

