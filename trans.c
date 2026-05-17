// Bank-account program reads a random-access file sequentially,
// updates data already written to the file, creates new data to
// be placed in the file, and deletes data previously in the file.
#include <stdio.h>
#include <stdlib.h>
// clientData structure definition
struct clientData
{
    unsigned int acctNum; // account number
    char lastName[15];    // account last name
    char firstName[10];   // account first name
    double balance;       // account balance
};                        // end structure clientData

// prototypes
unsigned int enterChoice(void);
void textFile(FILE *readPtr);
void updateRecord(FILE *fPtr);
void newRecord(FILE *fPtr);
void deleteRecord(FILE *fPtr);
void searchRecord(FILE *fPtr);
void calculateTotalBalance(FILE *fPtr);

int main(int argc, char *argv[])
{
    (void)argc;          // suppress unused parameter warning
    FILE *cfPtr;         // credit.dat file pointer
    unsigned int choice; // user's choice

    // fopen opens the file; attempts to create it if it cannot be opened
    if ((cfPtr = fopen("credit.dat", "rb+")) == NULL)
    {
        // try to create the file
        if ((cfPtr = fopen("credit.dat", "wb+")) == NULL)
        {
            printf("%s: File could not be opened.\n", argv[0]);
            exit(-1);
        }
        else
        {
            // initialize file with 100 blank records
            struct clientData blankClient = {0, "", "", 0.0};
            for (unsigned int i = 1; i <= 100; ++i)
            {
                fwrite(&blankClient, sizeof(struct clientData), 1, cfPtr);
            }
            rewind(cfPtr); // reset pointer to beginning of file
        }
    }

    // enable user to specify action
    while ((choice = enterChoice()) != 7)
    {
        switch (choice)
        {
        // create text file from record file
        case 1:
            textFile(cfPtr);
            break;
        // update record
        case 2:
            updateRecord(cfPtr);
            break;
        // create record
        case 3:
            newRecord(cfPtr);
            break;
        // delete existing record
        case 4:
            deleteRecord(cfPtr);
            break;
        // search for a record
        case 5:
            searchRecord(cfPtr);
            break;
        // calculate total balance
        case 6:
            calculateTotalBalance(cfPtr);
            break;
        // display if user does not select valid choice
        default:
            puts("Incorrect choice");
            break;
        } // end switch
    }     // end while

    fclose(cfPtr); // fclose closes the file
} // end main

// create formatted text file for printing
void textFile(FILE *readPtr)
{
    FILE *writePtr; // accounts.txt file pointer
    int result;     // used to test whether fread read any bytes
    // create clientData with default information
    struct clientData client = {0, "", "", 0.0};

    // fopen opens the file; exits if file cannot be opened
    if ((writePtr = fopen("accounts.txt", "w")) == NULL)
    {
        puts("File could not be opened.");
    } // end if
    else
    {
        rewind(readPtr); // sets pointer to beginning of file
        fprintf(writePtr, "%-6s%-16s%-11s%10s\n", "Acct", "Last Name", "First Name", "Balance");
        printf("\n%-6s%-16s%-11s%10s\n", "Acct", "Last Name", "First Name", "Balance");

        // copy all records from random-access file into text file
        while (!feof(readPtr))
        {
            result = fread(&client, sizeof(struct clientData), 1, readPtr);

            // write single record to text file
            if (result != 0 && client.acctNum != 0)
            {
                fprintf(writePtr, "%-6d%-16s%-11s%10.2f\n", client.acctNum, client.lastName, client.firstName,
                        client.balance);
                printf("%-6d%-16s%-11s%10.2f\n", client.acctNum, client.lastName, client.firstName,
                        client.balance);
            } // end if
        }     // end while

        fclose(writePtr); // fclose closes the file
        printf("\nAccounts successfully exported to \"accounts.txt\"\n");
    }                     // end else
} // end function textFile

// update balance in record
void updateRecord(FILE *fPtr)
{
    unsigned int account; // account number
    double transaction;   // transaction amount
    // create clientData with no information
    struct clientData client = {0, "", "", 0.0};

    // obtain number of account to update
    printf("%s", "Enter account to update ( 1 - 100 ): ");
    if (scanf("%u", &account) != 1) {
        int c;
        while ((c = getchar()) != '\n' && c != EOF);
    }

    if (account < 1 || account > 100)
    {
        printf("Invalid account number. It must be between 1 and 100.\n");
        return;
    }

    // move file pointer to correct record in file
    fseek(fPtr, (account - 1) * sizeof(struct clientData), SEEK_SET);
    // read record from file
    fread(&client, sizeof(struct clientData), 1, fPtr);
    // display error if account does not exist
    if (client.acctNum == 0)
    {
        printf("Account #%d has no information.\n", account);
    }
    else
    { // update record
        printf("%-6d%-16s%-11s%10.2f\n\n", client.acctNum, client.lastName, client.firstName, client.balance);

        int updateChoice = 0;
        printf("\n1 - Update Balance\n");
        printf("2 - Update Name\n");
        printf("Enter choice: ");
        scanf("%d", &updateChoice);

        // clear input buffer
        int c;
        while ((c = getchar()) != '\n' && c != EOF);

        if (updateChoice == 1)
        {
            // request transaction amount from user
            printf("%s", "Enter charge ( + ) or payment ( - ): ");
            scanf("%lf", &transaction);
            client.balance += transaction; // update record balance

            // clear input buffer
            while ((c = getchar()) != '\n' && c != EOF);
        }
        else if (updateChoice == 2)
        {
            printf("Enter new lastname, firstname\n? ");
            scanf("%14s%9s", client.lastName, client.firstName);

            // clear input buffer
            while ((c = getchar()) != '\n' && c != EOF);
        }
        else
        {
            printf("Invalid choice. Update cancelled.\n");
            return;
        }

        printf("%-6d%-16s%-11s%10.2f\n", client.acctNum, client.lastName, client.firstName, client.balance);

        // move file pointer to correct record in file
        // move back by 1 record length
        fseek(fPtr, -(long)sizeof(struct clientData), SEEK_CUR);
        // write updated record over old record in file
        fwrite(&client, sizeof(struct clientData), 1, fPtr);
        printf("Account successfully updated.\n");
    } // end else
} // end function updateRecord

// delete an existing record
void deleteRecord(FILE *fPtr)
{
    struct clientData client;                       // stores record read from file
    struct clientData blankClient = {0, "", "", 0}; // blank client
    unsigned int accountNum;                        // account number

    // obtain number of account to delete
    printf("%s", "Enter account number to delete ( 1 - 100 ): ");
    if (scanf("%u", &accountNum) != 1) {
        int c;
        while ((c = getchar()) != '\n' && c != EOF);
    }

    if (accountNum < 1 || accountNum > 100)
    {
        printf("Invalid account number. It must be between 1 and 100.\n");
        return;
    }

    // move file pointer to correct record in file
    fseek(fPtr, (accountNum - 1) * sizeof(struct clientData), SEEK_SET);
    // read record from file
    fread(&client, sizeof(struct clientData), 1, fPtr);
    // display error if record does not exist
    if (client.acctNum == 0)
    {
        printf("Account %d does not exist.\n", accountNum);
    } // end if
    else
    { // delete record
        char confirm;
        printf("Are you sure you want to delete account %d? (Y/N): ", accountNum);
        scanf(" %c", &confirm);
        
        // clear input buffer
        int c;
        while ((c = getchar()) != '\n' && c != EOF);

        if (confirm == 'Y' || confirm == 'y') 
        {
            // move file pointer to correct record in file
            fseek(fPtr, (accountNum - 1) * sizeof(struct clientData), SEEK_SET);
            // replace existing record with blank record
            fwrite(&blankClient, sizeof(struct clientData), 1, fPtr);
            printf("Account successfully deleted.\n");
        }
        else
        {
            printf("Deletion cancelled.\n");
        }
    } // end else
} // end function deleteRecord

// create and insert record
void newRecord(FILE *fPtr)
{
    // create clientData with default information
    struct clientData client = {0, "", "", 0.0};
    unsigned int accountNum; // account number

    // obtain number of account to create
    printf("%s", "Enter new account number ( 1 - 100 ): ");
    if (scanf("%u", &accountNum) != 1) {
        int c;
        while ((c = getchar()) != '\n' && c != EOF);
    }

    if (accountNum < 1 || accountNum > 100)
    {
        printf("Invalid account number. It must be between 1 and 100.\n");
        return;
    }

    // move file pointer to correct record in file
    fseek(fPtr, (accountNum - 1) * sizeof(struct clientData), SEEK_SET);
    // read record from file
    fread(&client, sizeof(struct clientData), 1, fPtr);
    // display error if account already exists
    if (client.acctNum != 0)
    {
        printf("Account #%d already contains information.\n", client.acctNum);
    } // end if
    else
    { // create record
        // user enters last name, first name and balance
        printf("%s", "Enter lastname, firstname, balance\n? ");
        scanf("%14s%9s%lf", client.lastName, client.firstName, &client.balance);
        
        // clear input buffer in case user entered too many characters
        int c;
        while ((c = getchar()) != '\n' && c != EOF);

        client.acctNum = accountNum;
        // move file pointer to correct record in file
        fseek(fPtr, (client.acctNum - 1) * sizeof(struct clientData), SEEK_SET);
        // insert record in file
        fwrite(&client, sizeof(struct clientData), 1, fPtr);
        printf("Account successfully created.\n");
    } // end else
} // end function newRecord

// search for an existing record
void searchRecord(FILE *fPtr)
{
    unsigned int accountNum; // account number
    struct clientData client = {0, "", "", 0.0}; // default client

    // obtain number of account to search
    printf("%s", "Enter account number to search ( 1 - 100 ): ");
    if (scanf("%u", &accountNum) != 1) {
        int c;
        while ((c = getchar()) != '\n' && c != EOF);
    }
    else {
        // clear input buffer if it successfully parsed an int, just to be safe
        int c;
        while ((c = getchar()) != '\n' && c != EOF);
    }

    if (accountNum < 1 || accountNum > 100)
    {
        printf("Invalid account number. It must be between 1 and 100.\n");
        return;
    }

    // move file pointer to correct record in file
    fseek(fPtr, (accountNum - 1) * sizeof(struct clientData), SEEK_SET);
    // read record from file
    fread(&client, sizeof(struct clientData), 1, fPtr);
    
    // display error if record does not exist
    if (client.acctNum == 0)
    {
        printf("Account %d does not exist.\n", accountNum);
    }
    else
    {
        printf("\n%-6s%-16s%-11s%10s\n", "Acct", "Last Name", "First Name", "Balance");
        printf("%-6d%-16s%-11s%10.2f\n\n", client.acctNum, client.lastName, client.firstName, client.balance);
    }
} // end function searchRecord

// calculate and print the total balance of all active accounts
void calculateTotalBalance(FILE *fPtr)
{
    struct clientData client;
    double totalBalance = 0.0;
    int activeAccounts = 0;

    rewind(fPtr); // start from beginning of file

    // read through the entire file
    while (!feof(fPtr))
    {
        int result = fread(&client, sizeof(struct clientData), 1, fPtr);

        // if account is active, add to total
        if (result != 0 && client.acctNum != 0)
        {
            totalBalance += client.balance;
            activeAccounts++;
        }
    }

    printf("\n--- Bank Summary ---\n");
    printf("Total Active Accounts: %d\n", activeAccounts);
    printf("Total Bank Balance: %.2f\n\n", totalBalance);
} // end function calculateTotalBalance

// enable user to input menu choice
unsigned int enterChoice(void)
{
    unsigned int menuChoice = 0; // variable to store user's choice
    // display available options
    printf("%s", "\nEnter your choice\n"
                 "1 - print all accounts to screen and save to\n"
                 "    \"accounts.txt\" for backup\n"
                 "2 - update an account\n"
                 "3 - add a new account\n"
                 "4 - delete an account\n"
                 "5 - search for an account\n"
                 "6 - calculate total bank balance\n"
                 "7 - end program\n? ");

    if (scanf("%u", &menuChoice) != 1)
    {
        // clear input buffer if user entered non-numeric data
        int c;
        while ((c = getchar()) != '\n' && c != EOF);
    }
    return menuChoice;
} // end function enterChoice