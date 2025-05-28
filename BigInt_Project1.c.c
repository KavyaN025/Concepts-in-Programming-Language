#include <stdio.h>
#include <string.h>  //incorporating various libraries to use the in-built functions
#include <ctype.h>
#include <stdlib.h>

#define MAX_DIGITS 310 // Maximum number of digits for a 1024-bit integer
#define BASE 10       // Base used for arithmetic operations

typedef struct 
{
    int digits[MAX_DIGITS]; // Array to store the digits of the number
    int length;             // Number of valid digits
    int sign;               // 1 for positive, -1 for negative
} BigInt_1024;

// Function prototype declarations
void InitializeBigInt_1024(BigInt_1024 *num);
void stringtoBigInt_1024(const char *str, BigInt_1024 *num);
void printBigInt_1024(const BigInt_1024 *num);
int Compare(const BigInt_1024 *a, const BigInt_1024 *b);
void AddBigInt_1024(const BigInt_1024 *a, const BigInt_1024 *b, BigInt_1024 *result);
int SubtractBigInt_1024(const BigInt_1024 *a, const BigInt_1024 *b, BigInt_1024 *result);
void MultiplicationBigInt_1024(const BigInt_1024 *a, const BigInt_1024 *b, BigInt_1024 *result);

// Initializing BigInt_1024
void InitializeBigInt_1024(BigInt_1024 *num) {
    num->length = 1;
    num->sign = 1;
    for (int i = 0; i < MAX_DIGITS; i++) {
        num->digits[i] = 0;
    }
}

// Converting the string to a valid BigInt_1024 integer
void stringtoBigInt_1024(const char *str,BigInt_1024 *num) 
{
    InitializeBigInt_1024(num);
    int start = 0;

    // Check for negative sign
    if (str[0] == '-') {
        num->sign = -1;
        start = 1;
    }
    
    //checking for empty string after checking for the sign
    if(str[start]=='\0')
    {
        printf("The string is empty!");
        return;
    }

    //checking for invalid characters
    for(int i=start;str[i]!='\0';i++)
    {
        if(!isdigit(str[i]))
        {  
            //printing a invalid error message to let the user know
            printf("Error:Invalid character in the string.\n");
            return;
        }
    }
    
    // Check for length exceeding 309 digits
    //throws a error message if the number exceeds 309 digits
    if (strlen(str) - start > 309) 
    {
        printf("Error: The input number exceeds 309 digits!\n");
        return;
    }
    num->length = strlen(str) - start;

    for (int i = 0; i < num->length; i++) {
        num->digits[num->length - i - 1] = str[start + i] - '0';
    }
}

// Printing the BigInt_1024
void printBigInt_1024(const BigInt_1024 *num) 
{
    if(num->length-1> MAX_DIGITS)
    {
        printf("Overflow!!\n"); //checks the overflow condition for multiplication, since the BigInt cannot hold a result which exceeds maximum number of digits
    }
    else
    {
      if (num->sign == -1)  
      {
        printf("-");
      }
      for (int i = num->length-1; i >= 0; i--) 
      {
        printf("%d", num->digits[i]);
      }
      printf("\n");
    }
}

// Comparing two BigInt numbers
//Helper function
int Compare(const BigInt_1024 *a, const BigInt_1024 *b) {
    if (a->length > b->length) {
        return 1; // a > b
    }
    if (a->length < b->length) {
        return -1; // a < b
    }
    for (int i = a->length - 1; i >= 0; i--)
    {
        if (a->digits[i] > b->digits[i]) 
        {
            return 1; // a > b
        }
        if (a->digits[i] < b->digits[i]) 
        {
            return -1; // a < b
        }
    }
    return 0; // a == b
}

// Adding two BigInt numbers
void AddBigInt_1024(const BigInt_1024 *a, const BigInt_1024 *b, BigInt_1024 *result) 
{
    if (a->sign == b->sign) 
    {
        InitializeBigInt_1024(result);
        result->sign = a->sign;
        int carry = 0;

        for (int i = 0; i < a->length || i < b->length || carry; i++) 
        {
            int sum = carry;
            if (i < a->length) 
            {
                sum += a->digits[i];
            }
            if (i < b->length) 
            {
                sum += b->digits[i];
            }
            result->digits[i] = sum % BASE;
            carry = sum / BASE;


            // Update result length incrementally
            result->length = i + 1;
        }

        // If there's a carry after processing all digits, add it
        if (carry) 
        {
            result->digits[result->length++] = carry;
        }
    } 
    else 
    {
        //If the numbers have different signs, the function converts the second number’s sign and calls SubtractBigInt_1024 to subtract the numbers instead

        BigInt_1024 temp_b = *b;
        temp_b.sign = -temp_b.sign;
        SubtractBigInt_1024(a, &temp_b, result);
    }
}



// Subtract two BigInt numbers
int SubtractBigInt_1024(const BigInt_1024 *a, const BigInt_1024 *b, BigInt_1024 *result) 
{
    if (a->sign != b->sign) {
        BigInt_1024 temp_b = *b;
        temp_b.sign = -temp_b.sign;
        AddBigInt_1024(a, &temp_b, result);
        return 0;
    }

    const BigInt_1024 *greater = a;
    const BigInt_1024 *lesser = b;
    int cmp = Compare(a, b);

    if (cmp < 0) //if a < b
    {
        greater = b;
        lesser = a;
    }

    InitializeBigInt_1024(result);
    result->sign = cmp >= 0 ? a->sign : -a->sign; //setting the sign of the result based on the comparison 

    int borrow = 0;
    for (int i = 0; i < greater->length; i++) 
    {
        int diff = greater->digits[i] - borrow;
        if (i < lesser->length) 
        {
            diff -= lesser->digits[i];
        }
        if (diff < 0) 
        {
            diff += BASE;
            borrow = 1;
        } 
        else 
        {
            borrow = 0;
        }
        result->digits[i] = diff;
    }

    result->length = greater->length;
    while (result->length> 1 && result->digits[result->length-1] == 0) 
    {
        result->length--; //Removing the leading zeros
    }
    return 0;
}

// Multiply two BigInt numbers
void MultiplicationBigInt_1024(const BigInt_1024 *a, const BigInt_1024 *b, BigInt_1024 *result) 
{
    InitializeBigInt_1024(result);
    result->sign = a->sign * b->sign;

    for (int i = 0; i < a->length; i++) 
    {
        int carry = 0;
        for (int j = 0; j < b->length || carry; j++) 
        {
            int product = result->digits[i + j] + a->digits[i] * (j < b->length ? b->digits[j] : 0) + carry;

            result->digits[i + j] = product % BASE;
            carry = product / BASE;
        }
    }

    result->length = a->length + b->length;
    while (result->length > 1 && result->digits[result->length-1] == 0) //Removing all the leading zeros
    {
        result->length--; //Removing the leading zeros
    }
}

// Main function
int main() 
{
    BigInt_1024 num1, num2, sum, difference, product;
    int choice;

    char input1[MAX_DIGITS + 2]; // Buffer for user input (with space for sign and null terminator)
    char input2[MAX_DIGITS + 2];

    // Initialize numbers (converting two 309 digit integer string to integer)
    //giving this input as a reference for checking
    printf("Enter the first BigInt number: ");
    scanf("%s", input1); // Read first number as a string
    stringtoBigInt_1024(input1, &num1);
    
    printf("Enter the second BigInt number: ");
    scanf("%s", input2); // Read first number as a string
    stringtoBigInt_1024(input2, &num2);

    //Creating a Menu
    printf("Options:\n");
    printf("1. Perform Addition\n");
    printf("2. Perform Subtraction\n");
    printf("3. Perform Multiplication\n");
    printf("4. Exit\n");

    do {
        printf("Enter your choice: ");
        scanf("%d", &choice);

        switch (choice) {
        case 1:
            AddBigInt_1024(&num1, &num2, &sum);
            printf("Addition Result: ");
            printBigInt_1024(&sum);
            break;

        case 2:
            SubtractBigInt_1024(&num1, &num2, &difference);
            printf("Subtraction Result: ");
            printBigInt_1024(&difference);
            break;

        case 3:
            MultiplicationBigInt_1024(&num1, &num2, &product);
            printf("Multiplication Result: ");
            printBigInt_1024(&product);
            break;

        case 4:
            printf("Exiting the BigInt_1024 calculator.\n");
            break;

        default:
            printf("Invalid choice! Try again.\n");
        }
    } while (choice != 4);

    return 0;
}