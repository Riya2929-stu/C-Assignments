#include <stdio.h>
#include <string.h>

struct User
{
    int id;
    char name[50];
    int age;
};

void createFile()
{
    FILE *fp = fopen("users.txt", "a");

    if (fp != NULL)
        fclose(fp);
}

int idExists(int id)
{
    FILE *fp = fopen("users.txt", "r");
    struct User u;

    if (fp == NULL)
        return 0;

    while (fscanf(fp, "%d,%49[^,],%d\n",
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

void createUser()
{
    struct User u;

    printf("Enter ID: ");
    scanf("%d", &u.id);

    if (idExists(u.id))
    {
        printf("ID already exists.\n");
        return;
    }

    printf("Enter Name: ");
    scanf(" %49[^\n]", u.name);

    printf("Enter Age: ");
    scanf("%d", &u.age);

    FILE *fp = fopen("users.txt", "a");

    if (fp == NULL)
    {
        printf("File error.\n");
        return;
    }

    fprintf(fp, "%d,%s,%d\n", u.id, u.name, u.age);

    fclose(fp);

    printf("User added successfully.\n");
}

void readUsers()
{
    FILE *fp = fopen("users.txt", "r");
    struct User u;

    if (fp == NULL)
    {
        printf("No records found.\n");
        return;
    }

    printf("\nID\tName\tAge\n");

    while (fscanf(fp, "%d,%49[^,],%d\n",
                  &u.id, u.name, &u.age) == 3)
    {
        printf("%d\t%s\t%d\n",
               u.id, u.name, u.age);
    }

    fclose(fp);
}

void updateUser()
{
    FILE *fp = fopen("users.txt", "r");
    FILE *temp = fopen("temp.txt", "w");
    struct User u;
    int id;
    int found = 0;

    if (fp == NULL || temp == NULL)
    {
        printf("File error.\n");

        if (fp != NULL)
            fclose(fp);

        if (temp != NULL)
            fclose(temp);

        return;
    }

    printf("Enter ID to update: ");
    scanf("%d", &id);

    while (fscanf(fp, "%d,%49[^,],%d\n",
                  &u.id, u.name, &u.age) == 3)
    {
        if (u.id == id)
        {
            found = 1;

            printf("Enter new name: ");
            scanf(" %49[^\n]", u.name);

            printf("Enter new age: ");
            scanf("%d", &u.age);
        }

        fprintf(temp, "%d,%s,%d\n",
                u.id, u.name, u.age);
    }

    fclose(fp);
    fclose(temp);

    remove("users.txt");
    rename("temp.txt", "users.txt");

    if (found)
        printf("User updated successfully.\n");
    else
        printf("User not found.\n");
}

void deleteUser()
{
    FILE *fp = fopen("users.txt", "r");
    FILE *temp = fopen("temp.txt", "w");
    struct User u;
    int id;
    int found = 0;

    if (fp == NULL || temp == NULL)
    {
        printf("File error.\n");

        if (fp != NULL)
            fclose(fp);

        if (temp != NULL)
            fclose(temp);

        return;
    }

    printf("Enter ID to delete: ");
    scanf("%d", &id);

    while (fscanf(fp, "%d,%49[^,],%d\n",
                  &u.id, u.name, &u.age) == 3)
    {
        if (u.id == id)
        {
            found = 1;
            continue;
        }

        fprintf(temp, "%d,%s,%d\n",
                u.id, u.name, u.age);
    }

    fclose(fp);
    fclose(temp);

    remove("users.txt");
    rename("temp.txt", "users.txt");

    if (found)
        printf("User deleted successfully.\n");
    else
        printf("User not found.\n");
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

        printf("Enter choice: ");
        scanf("%d", &choice);

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