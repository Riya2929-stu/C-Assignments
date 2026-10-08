#include <stdio.h>
#include <ctype.h>
#include <string.h>
#include <limits.h>

#define MAX_LENGTH    200
#define MAX_NUMBERS   50
#define MAX_OPERATORS 50

/* Status codes */
#define SUCCESS      0
#define ERR_INVALID  1
#define ERR_DIV_ZERO 2
#define ERR_OVERFLOW 3
#define ERR_TOO_LONG 4
#define ERR_TOO_MANY 5
#define ERR_INPUT    6

/* Returns 1 if ch is +, -, * or / */
int isOperator(char ch)
{
    return ch == '+' || ch == '-' || ch == '*' || ch == '/';
}

/* Reads one line safely */
int readExpression(char expression[], int size)
{
    if (fgets(expression, size, stdin) == NULL)
    {
        return ERR_INPUT;
    }

    if (strchr(expression, '\n') == NULL)
    {
        /*
         * No newline means the complete input may not fit
         * inside the array.
         */
        int next = getchar();

        if (next != EOF)
        {
            return ERR_TOO_LONG;
        }
    }
    else
    {
        expression[strcspn(expression, "\n")] = '\0';
    }

    return SUCCESS;
}

/* Splits expression into numbers and operators */
int parseExpression(const char expression[], int numbers[], char operators[],
                    int *numberCount, int *operatorCount)
{
    int i = 0;
    int expectingNumber = 1;

    *numberCount = 0;
    *operatorCount = 0;

    while (expression[i] != '\0')
    {
        char ch = expression[i];

        /* Ignore spaces between numbers and operators */
        if (isspace((unsigned char)ch))
        {
            i++;
            continue;
        }

        /* We are expecting a number */
        if (expectingNumber)
        {
            int isNegative = 0;
            long long value = 0;

            /* '-' before a number means negative number */
            if (ch == '-')
            {
                isNegative = 1;
                i++;
            }

            /* There must be at least one digit */
            if (!isdigit((unsigned char)expression[i]))
            {
                return ERR_INVALID;
            }

            /* Read all digits */
            while (isdigit((unsigned char)expression[i]))
            {
                int digit = expression[i] - '0';

                /*
                 * Use long long while building the number.
                 * Positive numbers can go up to INT_MAX.
                 * Negative numbers can go up to INT_MAX + 1
                 * because INT_MIN is -2147483648.
                 */
                value = value * 10 + digit;

                if ((!isNegative && value > INT_MAX) ||
                    (isNegative && value > (long long)INT_MAX + 1))
                {
                    return ERR_OVERFLOW;
                }

                i++;
            }

            /* Check array limit */
            if (*numberCount >= MAX_NUMBERS)
            {
                return ERR_TOO_MANY;
            }

            if (isNegative)
            {
                value = -value;
            }

            numbers[*numberCount] = (int)value;
            (*numberCount)++;

            /* Now we need an operator */
            expectingNumber = 0;
        }
        else
        {
            /*
             * We are expecting an operator.
             * Therefore "12 34" is invalid.
             */
            if (!isOperator(ch))
            {
                return ERR_INVALID;
            }

            /* Check operator array limit */
            if (*operatorCount >= MAX_OPERATORS)
            {
                return ERR_TOO_MANY;
            }

            operators[*operatorCount] = ch;
            (*operatorCount)++;

            i++;

            /* Now we need another number */
            expectingNumber = 1;
        }
    }

    /*
     * If we are still expecting a number,
     * the expression was empty or ended with an operator.
     */
    if (expectingNumber)
    {
        return ERR_INVALID;
    }

    return SUCCESS;
}

/* Performs one arithmetic operation */
int applyOperator(int left, char op, int right, int *result)
{
    long long a = left;
    long long b = right;
    long long answer;

    if (op == '+')
    {
        answer = a + b;
    }
    else if (op == '-')
    {
        answer = a - b;
    }
    else if (op == '*')
    {
        answer = a * b;
    }
    else
    {
        if (right == 0)
        {
            return ERR_DIV_ZERO;
        }

        answer = a / b;
    }

    /* Check whether result fits inside int */
    if (answer > INT_MAX || answer < INT_MIN)
    {
        return ERR_OVERFLOW;
    }

    *result = (int)answer;

    return SUCCESS;
}

/* Evaluates * and / first, then + and - */
int evaluate(int numbers[], char operators[], int numberCount,
             int operatorCount, int *result)
{
    int status;

    /*
     * First handle multiplication and division.
     * This gives * and / higher precedence.
     */
    for (int i = 0; i < operatorCount; i++)
    {
        if (operators[i] == '*' || operators[i] == '/')
        {
            status = applyOperator(numbers[i],
                                   operators[i],
                                   numbers[i + 1],
                                   &numbers[i]);

            if (status != SUCCESS)
            {
                return status;
            }

            /* Remove the used operator */
            for (int j = i; j < operatorCount - 1; j++)
            {
                operators[j] = operators[j + 1];
            }

            operatorCount--;

            /* Remove the used number */
            for (int j = i + 1; j < numberCount - 1; j++)
            {
                numbers[j] = numbers[j + 1];
            }

            numberCount--;

            /* Check the same position again */
            i--;
        }
    }

    /*
     * Now only + and - remain.
     * Evaluate them from left to right.
     */
    *result = numbers[0];

    for (int i = 0; i < operatorCount; i++)
    {
        status = applyOperator(*result,
                               operators[i],
                               numbers[i + 1],
                               result);

        if (status != SUCCESS)
        {
            return status;
        }
    }

    return SUCCESS;
}

/* Prints error message */
void printError(int status)
{
    if (status == ERR_INVALID)
    {
        printf("Error: Invalid expression.\n");
    }
    else if (status == ERR_DIV_ZERO)
    {
        printf("Error: Division by zero.\n");
    }
    else if (status == ERR_OVERFLOW)
    {
        printf("Error: Integer overflow.\n");
    }
    else if (status == ERR_TOO_LONG)
    {
        printf("Error: Expression is too long.\n");
    }
    else if (status == ERR_TOO_MANY)
    {
        printf("Error: Too many numbers or operators.\n");
    }
    else if (status == ERR_INPUT)
    {
        printf("Error: Could not read input.\n");
    }
}

int main(void)
{
    char expression[MAX_LENGTH];
    int numbers[MAX_NUMBERS];
    char operators[MAX_OPERATORS];

    int numberCount = 0;
    int operatorCount = 0;
    int result = 0;
    int status;

    printf("Enter expression: ");

    /* Step 1: Read input */
    status = readExpression(expression, MAX_LENGTH);

    /* Step 2: Parse input */
    if (status == SUCCESS)
    {
        status = parseExpression(expression,
                                 numbers,
                                 operators,
                                 &numberCount,
                                 &operatorCount);
    }

    /* Step 3: Evaluate expression */
    if (status == SUCCESS)
    {
        status = evaluate(numbers,
                          operators,
                          numberCount,
                          operatorCount,
                          &result);
    }

    /* Handle errors */
    if (status != SUCCESS)
    {
        printError(status);
        return 1;
    }

    printf("Result = %d\n", result);

    return 0;
}