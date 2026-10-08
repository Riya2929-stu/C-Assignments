#include <stdio.h>
#include <ctype.h>
#include <string.h>
#include <limits.h>

#define MAX_LENGTH    200
#define MAX_NUMBERS   50
#define MAX_OPERATORS 50

#define SUCCESS      0
#define ERR_INVALID  1
#define ERR_DIV_ZERO 2
#define ERR_OVERFLOW 3
#define ERR_TOO_LONG 4
#define ERR_TOO_MANY 5
#define ERR_INPUT    6

int isOperator(char ch)
{
    return ch == '+' || ch == '-' || ch == '*' || ch == '/';
}

int readExpression(char expression[], int size)
{
    if (fgets(expression, size, stdin) == NULL)
    {
        return ERR_INPUT;
    }

    if (strchr(expression, '\n') == NULL)
    {
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

        if (isspace((unsigned char)ch))
        {
            i++;
            continue;
        }

        if (expectingNumber)
        {
            int isNegative = 0;
            long long value = 0;

            if (ch == '-')
            {
                isNegative = 1;
                i++;
            }

            if (!isdigit((unsigned char)expression[i]))
            {
                return ERR_INVALID;
            }

            while (isdigit((unsigned char)expression[i]))
            {
                int digit = expression[i] - '0';

                value = value * 10 + digit;

                if ((!isNegative && value > INT_MAX) ||
                    (isNegative && value > (long long)INT_MAX + 1))
                {
                    return ERR_OVERFLOW;
                }

                i++;
            }

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
            expectingNumber = 0;
        }
        else
        {
            if (!isOperator(ch))
            {
                return ERR_INVALID;
            }
            if (*operatorCount >= MAX_OPERATORS)
            {
                return ERR_TOO_MANY;
            }

            operators[*operatorCount] = ch;
            (*operatorCount)++;

            i++;
            expectingNumber = 1;
        }
    }

    if (expectingNumber)
    {
        return ERR_INVALID;
    }

    return SUCCESS;
}
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
    if (answer > INT_MAX || answer < INT_MIN)
    {
        return ERR_OVERFLOW;
    }

    *result = (int)answer;

    return SUCCESS;
}

int evaluate(int numbers[], char operators[], int numberCount,
             int operatorCount, int *result)
{
    int status;
    for (int i = 0; i < operatorCount; i++)
    {
        if (operators[i] == '*' || operators[i] == '/')
        {
            status = applyOperator(numbers[i],operators[i],numbers[i + 1],&numbers[i]);

            if (status != SUCCESS)
            {
                return status;
            }
            for (int j = i; j < operatorCount - 1; j++)
            {
                operators[j] = operators[j + 1];
            }

            operatorCount--;
            for (int j = i + 1; j < numberCount - 1; j++)
            {
                numbers[j] = numbers[j + 1];
            }

            numberCount--;
            i--;
        }
    }
    *result = numbers[0];

    for (int i = 0; i < operatorCount; i++)
    {
        status = applyOperator(*result,operators[i],numbers[i + 1],result);

        if (status != SUCCESS)
        {
            return status;
        }
    }

    return SUCCESS;
}

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
    status = readExpression(expression, MAX_LENGTH);

    if (status == SUCCESS)
    {
        status = parseExpression(expression,numbers,operators,&numberCount,&operatorCount);
    }

    if (status == SUCCESS)
    {
        status = evaluate(numbers,operators,numberCount,operatorCount,&result);
    }

    if (status != SUCCESS)
    {
        printError(status);
        return 1;
    }

    printf("Result = %d\n", result);

    return 0;
}