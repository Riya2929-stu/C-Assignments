#include <stdio.h>

struct Student {
    int roll;
    char name[50];
    int marks1;
    int marks2;
    int marks3;
};

int totalStudents = 0;

int calculateTotal(struct Student s) {
    return s.marks1 + s.marks2 + s.marks3;
}

float calculateAverage(int total) {
    return total / 3.0;
}

char calculateGrade(float average) {
    if (average >= 85) return 'A';
    else if (average >= 70) return 'B';
    else if (average >= 50) return 'C';
    else if (average >= 35) return 'D';
    else return 'F';
}

void printPerformance(char grade) {
    int stars = 0;
    switch (grade) {
        case 'A': stars = 5; break;
        case 'B': stars = 4; break;
        case 'C': stars = 3; break;
        case 'D': stars = 2; break;
    }
    printf("Performance: ");
    for (int i = 0; i < stars; i++) {
        printf("*");
    }
    printf("\n");
}

void printRollNumbers(struct Student students[], int n, int index) {
    if (index == n) return;
    printf("%d ", students[index].roll);
    printRollNumbers(students, n, index + 1);
}

int main() {
    int n;
    struct Student students[100];

    scanf("%d", &n);
    if (n < 1 || n > 100) {
        printf("Invalid number of students (must be 1 to 100)\n");
        return 1;
    }
    totalStudents = n;

    for (int i = 0; i < totalStudents; i++) {
        scanf("%d %49s %d %d %d",
              &students[i].roll,
              students[i].name,
              &students[i].marks1,
              &students[i].marks2,
              &students[i].marks3);
    }

    for (int i = 0; i < totalStudents; i++) {
        int total = calculateTotal(students[i]);
        float average = calculateAverage(total);
        char grade = calculateGrade(average);

        printf("Roll: %d\n", students[i].roll);
        printf("Name: %s\n", students[i].name);
        printf("Total: %d\n", total);
        printf("Average: %.2f\n", average);
        printf("Grade: %c\n", grade);

        if (average < 35) {
            continue;
        }
        printPerformance(grade);
    }
    printf("List of Roll Numbers (via recursion): ");
    printRollNumbers(students, totalStudents, 0);
    printf("\n");

    return 0;
}