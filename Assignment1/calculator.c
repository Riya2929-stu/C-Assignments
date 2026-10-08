#include <stdio.h>
#include <ctype.h>
#include <string.h>

int main()
{
    char expression[100];
    int numbers[50];
    char operators[50];
    int numbercount = 0;
    int operatorcount = 0;
    int result = 0;
    int number = 0;
    printf("Enter expression: ");
    fgets(expression, sizeof(expression), stdin);
    expression[strcspn(expression, "\n")] = '\0';
    int length = strlen(expression);

    if (length == 0)
    {
        printf("Error: Invalid expression.");
        return 0;
    }

    int first = 0;
    while (first < length && isspace(expression[first]))
    {
        first++;
    }

    if (first == length)
    {
        printf("Error: Invalid expression.");
        return 0;
    }

    if (expression[first] == '+' ||
        expression[first] == '-' ||
        expression[first] == '*' ||
        expression[first] == '/')
    {
        printf("Error: Invalid expression.");
        return 0;
    }
    int expectingNumber = 1;
    for (int i = first; i < length; i++)
    {
        if (isdigit(expression[i]))
        {
            number = number * 10 + (expression[i] - '0');
            expectingNumber = 0;
        }

        else if (isspace(expression[i]))
        { continue;
        }

        else if (expression[i] == '+' ||
                 expression[i] == '-' ||
                 expression[i] == '*' ||
                 expression[i] == '/')
        {
            if (expectingNumber)
            {
                printf("Error: Invalid expression.");
                return 0;
            }

            numbers[numbercount] = number;
            numbercount++;

            operators[operatorcount] = expression[i];
            operatorcount++;

            number = 0;
            expectingNumber = 1;
        }

        else
        {
            printf("Error: Invalid expression.");
            return 0;
        }
    }
    if (expectingNumber)
    {
        printf("Error: Invalid expression.");
        return 0;
    }
    numbers[numbercount] = number;
    numbercount++;
    for (int i = 0; i < operatorcount; i++)
    {
        if (operators[i] == '*' || operators[i] == '/')
        {
            if (operators[i] == '*')
            {
                numbers[i] = numbers[i] * numbers[i + 1];
            }

            else if (operators[i] == '/')
            {
                if (numbers[i + 1] == 0)
                {
                    printf("Error: Division by zero.");
                    return 0;
                }

                numbers[i] = numbers[i] / numbers[i + 1];
            }

            for (int j = i; j < operatorcount - 1; j++)
            {
                operators[j] = operators[j + 1];
            }

            operatorcount--;
            for (int j = i + 1; j < numbercount - 1; j++)
            {
                numbers[j] = numbers[j + 1];
            }

            numbercount--;

            i--;
        }
    }
    result = numbers[0];

    for (int i = 0; i < operatorcount; i++)
    {
        if (operators[i] == '+')
        {
            result = result + numbers[i + 1];
        }

        else if (operators[i] == '-')
        {
            result = result - numbers[i + 1];
        }
    }

    printf("Result = %d", result);

    return 0;
}