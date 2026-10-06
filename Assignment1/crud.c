#include <stdio.h>
#include <string.h>

#define FILENAME "users.txt"
#define TEMPFILE "temp.txt"

struct User
{
    int id;
    char name[50];
    int age;
};

/* Read integer safely */
int readInteger(const char *prompt, int *value)
{
    char input[100];
    char extra;

    printf("%s", prompt);

    if (fgets(input, sizeof(input), stdin) == NULL)
        return 0;

    if (sscanf(input, "%d %c", value, &extra) != 1)
    {
        printf("Invalid input. Please enter a number.\n");
        return 0;
    }

    return 1;
}

/* Create file if it does not exist */
void createFile()
{
    FILE *fp = fopen(FILENAME, "a");

    if (fp == NULL)
    {
        printf("Unable to create file.\n");
        return;
    }

    fclose(fp);
}

/* Check whether ID already exists */
int idExists(int id)
{
    FILE *fp = fopen(FILENAME, "r");
    struct User u;

    if (fp == NULL)
        return 0;

    while (fscanf(fp, "%d|%49[^|]|%d",
                  &u.id, u.name, &u.age) == 3)
    {
        if (u.id == id)
        {
            fclose(fp);
            return 1;
        }
    }

    fclose(fp);
    return 0;
}

/* Create */
void createUser()
{
    struct User u;

    if (!readInteger("Enter ID: ", &u.id))
        return;

    if (idExists(u.id))
    {
        printf("ID already exists.\n");
        return;
    }

    printf("Enter Name: ");

    if (fgets(u.name, sizeof(u.name), stdin) == NULL)
        return;

    u.name[strcspn(u.name, "\n")] = '\0';

    if (strlen(u.name) == 0 || strchr(u.name, '|') != NULL)
    {
        printf("Invalid name.\n");
        return;
    }

    if (!readInteger("Enter Age: ", &u.age))
        return;

    if (u.age < 0)
    {
        printf("Age cannot be negative.\n");
        return;
    }

    FILE *fp = fopen(FILENAME, "a");

    if (fp == NULL)
    {
        printf("File error.\n");
        return;
    }

    fprintf(fp, "%d|%s|%d\n", u.id, u.name, u.age);

    fclose(fp);

    printf("User added successfully.\n");
}

/* Read */
void readUsers()
{
    FILE *fp = fopen(FILENAME, "r");
    struct User u;

    if (fp == NULL)
    {
        printf("No records found.\n");
        return;
    }

    printf("\nID\tName\tAge\n");

    while (fscanf(fp, "%d|%49[^|]|%d",
                  &u.id, u.name, &u.age) == 3)
    {
        printf("%d\t%s\t%d\n",
               u.id, u.name, u.age);
    }

    fclose(fp);
}

/* Update */
void updateUser()
{
    FILE *fp = fopen(FILENAME, "r");
    FILE *temp;
    struct User u;
    int id, found = 0;

    if (fp == NULL)
    {
        printf("File error.\n");
        return;
    }

    temp = fopen(TEMPFILE, "w");

    if (temp == NULL)
    {
        printf("Unable to create temporary file.\n");
        fclose(fp);
        return;
    }

    if (!readInteger("Enter ID to update: ", &id))
    {
        fclose(fp);
        fclose(temp);
        remove(TEMPFILE);
        return;
    }

    while (fscanf(fp, "%d|%49[^|]|%d",
                  &u.id, u.name, &u.age) == 3)
    {
        if (u.id == id)
        {
            found = 1;

            printf("Enter new Name: ");

            if (fgets(u.name, sizeof(u.name), stdin) == NULL)
            {
                fclose(fp);
                fclose(temp);
                remove(TEMPFILE);
                return;
            }

            u.name[strcspn(u.name, "\n")] = '\0';

            if (strlen(u.name) == 0 || strchr(u.name, '|') != NULL)
            {
                printf("Invalid name.\n");
                fclose(fp);
                fclose(temp);
                remove(TEMPFILE);
                return;
            }

            if (!readInteger("Enter new Age: ", &u.age) ||
                u.age < 0)
            {
                printf("Invalid age.\n");
                fclose(fp);
                fclose(temp);
                remove(TEMPFILE);
                return;
            }
        }

        fprintf(temp, "%d|%s|%d\n",
                u.id, u.name, u.age);
    }

    fclose(fp);
    fclose(temp);

    if (!found)
    {
        remove(TEMPFILE);
        printf("User not found.\n");
        return;
    }

    if (remove(FILENAME) != 0)
    {
        printf("Error removing old file.\n");
        remove(TEMPFILE);
        return;
    }

    if (rename(TEMPFILE, FILENAME) != 0)
    {
        printf("Error renaming temporary file.\n");
        return;
    }

    printf("User updated successfully.\n");
}

/* Delete */
void deleteUser()
{
    FILE *fp = fopen(FILENAME, "r");
    FILE *temp;
    struct User u;
    int id, found = 0;

    if (fp == NULL)
    {
        printf("File error.\n");
        return;
    }

    temp = fopen(TEMPFILE, "w");

    if (temp == NULL)
    {
        printf("Unable to create temporary file.\n");
        fclose(fp);
        return;
    }

    if (!readInteger("Enter ID to delete: ", &id))
    {
        fclose(fp);
        fclose(temp);
        remove(TEMPFILE);
        return;
    }

    while (fscanf(fp, "%d|%49[^|]|%d",
                  &u.id, u.name, &u.age) == 3)
    {
        if (u.id == id)
        {
            found = 1;
            continue;
        }

        fprintf(temp, "%d|%s|%d\n",
                u.id, u.name, u.age);
    }

    fclose(fp);
    fclose(temp);

    if (!found)
    {
        remove(TEMPFILE);
        printf("User not found.\n");
        return;
    }

    if (remove(FILENAME) != 0)
    {
        printf("Error removing old file.\n");
        remove(TEMPFILE);
        return;
    }

    if (rename(TEMPFILE, FILENAME) != 0)
    {
        printf("Error renaming temporary file.\n");
        return;
    }

    printf("User deleted successfully.\n");
}

int main()
{
    int choice;

    createFile();

    do
    {
        printf("\n1. Create User\n");
        printf("2. Read Users\n");
        printf("3. Update User\n");
        printf("4. Delete User\n");
        printf("5. Exit\n");

        if (!readInteger("Enter choice: ", &choice))
            continue;

        switch (choice)
        {
            case 1:
                createUser();
                break;

            case 2:
                readUsers();
                break;

            case 3:
                updateUser();
                break;

            case 4:
                deleteUser();
                break;

            case 5:
                printf("Exiting...\n");
                break;

            default:
                printf("Invalid choice.\n");
        }

    } while (choice != 5);

    return 0;
}