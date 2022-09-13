#ifndef BANK_ACCOUNT_H_
#define BANK_ACCOUNT_H_

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


#endif /* BANK_ACCOUNT_H_ */

