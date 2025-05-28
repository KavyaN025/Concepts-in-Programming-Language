#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define TABLE_SIZE 10

// Structure for (key-value pair)
typedef struct Symbol {
    char name[50];  // Variable name
    int value;      // Variable value
    struct Symbol *next; // Chaining for handling collision
} Symbol;

// Structure for a scope (linked list of hash tables)
typedef struct Scope {
    Symbol *table[TABLE_SIZE]; // Hash table
    struct Scope *parent; // Pointer to the parent scope
} Scope;

Scope *current = NULL; // Global pointer to keep track of the current scope

// Hash function
int hash(char *name) {
    int hash = 0;
    while (*name) {
        hash = (hash + *name) % TABLE_SIZE;
        name++;
    }
    return hash; // Returns the index
}

// Creating a new scope
void begin() {
    Scope *newscope = (Scope *)malloc(sizeof(Scope));
    if (!newscope) {
        printf("Memory allocation failed!\n");
        return;
    }

    for (int i = 0; i < TABLE_SIZE; i++) {
        newscope->table[i] = NULL; // Initialize hash table
    }

    newscope->parent = current; // Link the new scope to the parent scope
    current = newscope; // Update current scope pointer
}

// Insert or update a variable in the current scope
void assign(char *name, int value) {
    if (current == NULL) {
        printf("No active scope!\n");
        return;
    }

    int index = hash(name);
    Symbol *symbol = current->table[index];

    // Check if the variable already exists in the current scope
    while (symbol) {
        if (strcmp(symbol->name, name) == 0) {
            symbol->value = value; // Update the existing variable value
            return;
        }
        symbol = symbol->next;
    }

    // If not found, insert a new variable
    Symbol *newsymbol = (Symbol *)malloc(sizeof(Symbol));
    if (!newsymbol) {
        printf("Memory allocation failed!\n");
        return;
    }

    strcpy(newsymbol->name, name);
    newsymbol->value = value;
    newsymbol->next = current->table[index]; // Insert at head (chaining)
    current->table[index] = newsymbol;
}

// Print the value of a variable (with error handling)
void printVariable(char *name) {
    if (current == NULL) {
        printf("No active scope!\n");
        return;
    }

    Scope *scope = current;
    while (scope) {
        int index = hash(name);
        Symbol *symbol = scope->table[index];

        while (symbol) {
            if (strcmp(symbol->name, name) == 0) {
                printf("%d\n", symbol->value);
                return;
            }
            symbol = symbol->next;
        }
        scope = scope->parent; // Move to parent scope
    }

    printf("Variable %s not found\n", name);
}

// End the current scope
void endscope() {
    if (current == NULL) {
        printf("No active scope to end.\n");
        return;
    }

    // Free all symbols in the current scope
    for (int i = 0; i < TABLE_SIZE; i++) {
        Symbol *symbol = current->table[i];
        while (symbol) {
            Symbol *temp = symbol;
            symbol = symbol->next;
            free(temp);
        }
    }

    // Move to parent scope before freeing the current scope
    Scope *tempScope = current;
    current = current->parent;
    free(tempScope);
}

// Driver Code
int main() {
    begin();
    assign("a", 1);
    assign("b", 2);

    begin();
    assign("a", 3);
    assign("c", 4);
    printVariable("b"); // Prints: 2

    begin();
    assign("c", 5);
    printVariable("b"); // Prints: 2
    printVariable("a"); // Prints: 3
    printVariable("c"); // Prinst: 5

    endscope();
    printVariable("a"); // Prints: 3
    printVariable("c"); // Prints: 4

    endscope();
    printVariable("a"); // Prints: 1

    endscope();

    // Edge case: Printing a variable after all scopes are closed
    printVariable("a"); // Expected Output: "No active scope!"

    // Edge case: Ending an already ended scope
    endscope(); // Expected Output: "No active scope to end."

    return 0;
}
