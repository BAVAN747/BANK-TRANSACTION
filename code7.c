#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h> // For isdigit, etc.
#include <limits.h> // For INT_MAX and INT_MIN

#define MAX 100
#define MAX_NAME_LENGTH 49 // Leave space for null terminator
#define FILENAME "customer_details.txt"

typedef struct {
    char name[50];
    int accountNumber;
    float balance;
    float depositAmount;
    float withdrawAmount;
} Customer;

Customer queue[MAX];
int front = -1, rear = -1;

// Queue operations
int isFull() {
    return rear == MAX - 1;
}

int isEmpty() {
    return front == -1 || front > rear;
}

void enqueue(Customer c) {
    if (isFull()) {
        fprintf(stderr, "Queue is full. Cannot add more customers.\n");
        return;
    }
    if (isEmpty()) {
        front = 0;
    }
    rear++;
    queue[rear] = c;
    printf("Customer '%s' added to the queue.\n", c.name);
}

Customer dequeue() {
    Customer dummy = {"", 0, 0.0, 0.0, 0.0};
    if (isEmpty()) {
        fprintf(stderr, "Queue is empty.\n");
        return dummy;
    }
    Customer c = queue[front];
    front++;
    if (front > rear) {
        front = rear = -1; // Reset queue
    }
    return c;
}

Customer* peek() {
    if (isEmpty()) {
        fprintf(stderr, "Queue is empty.\n");
        return NULL;
    }
    return &queue[front];
}

// Bank operations
void deposit(Customer* c, float amount) {
    if (amount <= 0) {
        fprintf(stderr, "Invalid amount.\n");
        return;
    }
    c->balance += amount;
    c->depositAmount += amount;
    printf("Deposited %.2f. New Balance: %.2f\n", amount, c->balance);
}

void withdraw(Customer* c, float amount) {
    if (amount <= 0 || amount > c->balance) {
        fprintf(stderr, "Invalid or insufficient balance.\n");
        return;
    }
    c->balance -= amount;
    c->withdrawAmount += amount;
    printf("Withdrawn %.2f. New Balance: %.2f\n", amount, c->balance);
}

void viewBalance(Customer* c) {
    printf("Customer: %s, Account Number: %d, Balance: %.2f\n", c->name, c->accountNumber, c->balance);
}

// Function to save customer details to a file
void saveCustomerDetails(Customer c) {
    FILE *fp = fopen(FILENAME, "a"); // Open file in append mode

    if (fp == NULL) {
        perror("Error opening file");
        return;
    }

    if (fprintf(fp, "Name: %s, Account Number: %d, Balance: %.2f, Deposit: %.2f, Withdraw: %.2f\n",
                c.name, c.accountNumber, c.balance, c.depositAmount, c.withdrawAmount) < 0) {
        perror("Error writing to file");
        fclose(fp);
        return;
    }
    fclose(fp);
    printf("Customer details saved to %s\n", FILENAME);
}

// Function to safely get a line of input
int getLine(char *buffer, size_t bufferSize, const char *prompt) {
    printf("%s", prompt);
    if (fgets(buffer, bufferSize, stdin) == NULL) {
        return 0; // Indicate failure (e.g., EOF)
    }
    buffer[strcspn(buffer, "\n")] = '\0'; // Remove trailing newline
    return 1; // Indicate success
}

// Main interaction
int main() {
    int choice;
    Customer current;
    char name[50];
    int accountNumber;
    float amount;

    while (1) {
        printf("\nBank Transaction Menu:\n");
        printf("1. Add Customer to Queue\n");
        printf("2. Process Next Customer\n");
        printf("3. Exit\n");
        printf("Choose option: ");

        if (scanf("%d", &choice) != 1) {
            fprintf(stderr, "Invalid input. Please enter a number.\n");
            while (getchar() != '\n'); // Clear input buffer
            continue;
        }
        getchar(); // consume newline

        switch (choice) {
            case 1:
                if (isFull()) {
                    fprintf(stderr, "Queue is full. Cannot add more customers.\n");
                    break;
                }

                int result = getLine(name, sizeof(name), "Enter customer name: ");
                if (result == 0) {
                    fprintf(stderr, "Error reading input.\n");
                    break;
                }

                printf("Enter account number: ");
                if (scanf("%d", &accountNumber) != 1) {
                    fprintf(stderr, "Invalid account number format.\n");
                    while (getchar() != '\n');
                    break;
                }
                getchar(); // Consume newline character


                Customer newCustomer = {.balance = 0.0, .depositAmount = 0.0, .withdrawAmount = 0.0};
                strncpy(newCustomer.name, name, sizeof(newCustomer.name) - 1);
                newCustomer.name[sizeof(newCustomer.name) - 1] = '\0';
                newCustomer.accountNumber = accountNumber;
                enqueue(newCustomer);
                break;

            case 2:
                if (isEmpty()) {
                    fprintf(stderr, "No customers in queue.\n");
                    break;
                }
                current = dequeue();
                printf("\nProcessing Customer: %s, Account Number: %d\n", current.name, current.accountNumber);

                int tChoice;
                do {
                    printf("\n1. Deposit\n2. Withdraw\n3. View Balance\n4. Finish\nChoose: ");
                    if (scanf("%d", &tChoice) != 1) {
                        fprintf(stderr, "Invalid input. Please enter a number.\n");
                        while (getchar() != '\n'); // Clear input buffer
                        tChoice = -1; // Force another loop iteration
                        continue;
                    }
                    getchar(); // Consume newline character

                    switch (tChoice) {
                        case 1:
                            printf("Enter amount to deposit: ");
                            if (scanf("%f", &amount) != 1) {
                                fprintf(stderr, "Invalid amount format.\n");
                                while (getchar() != '\n');
                                break;
                            }
                            getchar(); // Consume newline character
                            if (amount < 500) {
                                fprintf(stderr, "Amount must be at least 500.\n");
                                break;
                            }
                            deposit(&current, amount);
                            break;
                        case 2:
                            printf("Enter amount to withdraw: ");
                            if (scanf("%f", &amount) != 1) {
                                fprintf(stderr, "Invalid amount format.\n");
                                while (getchar() != '\n');
                                break;
                            }
                            getchar(); // Consume newline character
                            if (amount <= 0) {
                                fprintf(stderr, "Amount must be positive.\n");
                                break;
                            }
                            if (amount > current.balance) {
                                fprintf(stderr, "Insufficient balance.\n");
                                break;
                            }
                            withdraw(&current, amount);
                            break;
                        case 3:
                            viewBalance(&current);
                            break;
                        case 4:
                            printf("Finished processing %s.\n", current.name);
                            saveCustomerDetails(current);
                            break;
                        default:
                            fprintf(stderr, "Invalid option.\n");
                    }
                } while (tChoice != 4);
                break;

            case 3:
                printf("Exiting system. Goodbye!\n");
                exit(0);
            default:
                fprintf(stderr, "Invalid option.\n");
        }
    }

    return 0;
}
