#include<stdio.h>
#include<stdlib.h>
#include<string.h>

typedef struct Employee{
    int emp_id;
    char name[50];
    char designation[50];
    struct Employee **teammembers;
    int teamsize;
    int teamCapacity;
    struct Employee *manager;
}Employee;

struct Employee **org = NULL;
int totalEmp =0;
int orgCapacity =0;

Employee *findEmployeeByID(int id){
    for(int i=0;i<totalEmp;i++){
        if(org[i]->emp_id == id){
            return org[i];
        }
    }
    return NULL;
}

int ensureorgCapacity(){
    if(totalEmp<orgCapacity){
        return 1;
    }
    int newCapacity;
    if(orgCapacity ==0){
        newCapacity =2;
    }
    else{
        newCapacity = orgCapacity*2;
    }
    Employee **temp = realloc(org,newCapacity*sizeof(Employee*));
    if(temp==NULL){
        printf("Memory allocation failed\n");
        return 0;
    }
    org = temp;
    orgCapacity = newCapacity;
    return 1;
}

int ensureTeamCapacity(Employee *manager){
    if(manager->teamsize < manager->teamCapacity){
        return 1;
    }
    int newCapacity;
    if(manager->teamCapacity ==0){
        newCapacity =2;
    }
    else{
        newCapacity = manager->teamCapacity*2;
    }
    Employee **temp = realloc(manager->teammembers,newCapacity*sizeof(Employee*));
    if(temp==NULL){
        printf("Memory allocation failed.\n");
        return 0;
    }
    manager->teammembers = temp;
    manager->teamCapacity = newCapacity;
    return 1;
}
int addEmployee(Employee emp,int managerID){
    if(findEmployeeByID(emp.emp_id)!=NULL){
        printf("Employee ID already exists\n");
        return 0;
    }
    Employee *manager = NULL;

    if(managerID !=-1){
        manager =findEmployeeByID(managerID);
        if(manager == NULL){
            printf("Manager not found\n");
            return 0;
        }
        if(managerID == emp.emp_id){
            printf("An employee cannot be their own manager\n");
            return 0;
        }
    }
    if(!ensureorgCapacity()){
        return 0;
    }
    if(manager!=NULL && !ensureTeamCapacity(manager)){
        return 0;
    }
    Employee *newEmployee = malloc(sizeof(Employee));
    if(newEmployee ==NULL){
        printf("Memory allocation failed\n");
        return 0;
    }
    newEmployee->emp_id = emp.emp_id;
    strcpy(newEmployee->name,emp.name);
    strcpy(newEmployee->designation,emp.designation);

    newEmployee->teammembers = NULL;
    newEmployee->teamsize =0;
    newEmployee->teamCapacity =0;
    newEmployee->manager = manager;

    org[totalEmp] = newEmployee;
    totalEmp++;
    if(manager!=NULL){
    manager->teammembers[manager->teamsize]=newEmployee;
    manager->teamsize++;
    }
    printf("Employee added successfully\n");
    return 1;  
}


int removeEmployee(int id) {
    int index = -1;
    for (int i = 0; i < totalEmp; i++) {
        if (org[i]->emp_id == id) {
            index = i;
            break;
        }
    }

    if (index == -1) {
        printf("Employee not found\n");
        return 0;
    }

    Employee *emp = org[index];
    if (emp->manager == NULL ||
        strcmp(emp->designation, "CEO") == 0) {
        printf("The CEO cannot be removed\n");
        return 0;
    }

    Employee *manager = emp->manager;
    Employee *target = manager;

    if (manager->teamsize > 1) {
        target = NULL;

        for (int i = 0; i < manager->teamsize; i++) {
            Employee *sibling = manager->teammembers[i];

            if (sibling == emp) {
                continue;
            }

            if (target == NULL ||
                sibling->teamsize < target->teamsize) {
                target = sibling;
            }
        }
    }

    if (emp->teamsize > 0) {
        int requiredSize = target->teamsize + emp->teamsize;

        while (target->teamCapacity < requiredSize) {
            int newCapacity;

            if (target->teamCapacity == 0) {
                newCapacity = 2;
            } else {
                newCapacity = target->teamCapacity * 2;
            }

            Employee **temp = realloc(
                target->teammembers,
                newCapacity * sizeof(Employee *)
            );

            if (temp == NULL) {
                printf("Memory allocation failed\n");
                return 0;
            }

            target->teammembers = temp;
            target->teamCapacity = newCapacity;
        }
    }

    int teamIndex = -1;

    for (int i = 0; i < manager->teamsize; i++) {
        if (manager->teammembers[i] == emp) {
            teamIndex = i;
            break;
        }
    }

    if (teamIndex != -1) {
        for (int i = teamIndex; i < manager->teamsize - 1; i++) {
            manager->teammembers[i] =
                manager->teammembers[i + 1];
        }

        manager->teamsize--;
    }

    for (int i = 0; i < emp->teamsize; i++) {
        Employee *report = emp->teammembers[i];

        report->manager = target;
        target->teammembers[target->teamsize] = report;
        target->teamsize++;
    }

    for (int i = index; i < totalEmp - 1; i++) {
        org[i] = org[i + 1];
    }

    totalEmp--;

    free(emp->teammembers);
    free(emp);

    printf("Employee removed successfully\n");
    return 1;
}



int countDescendants(Employee*emp){
    if(emp==NULL){
        return 0;
    }
    int count =0;
    for(int i=0;i<emp->teamsize;i++){
        count+= 1 + countDescendants(emp->teammembers[i]);
    }
    return count;
}


void displayEmployeeHierarchy(Employee *emp, int level) {
    if (emp == NULL) {
        return;
    }
    for (int i = 0; i < level; i++) {
        printf("    ");
    }

    printf("%d - %s - %s\n",
           emp->emp_id,
           emp->name,
           emp->designation);

    for (int i = 0; i < emp->teamsize; i++) {
        displayEmployeeHierarchy(emp->teammembers[i], level + 1);
    }
}


void showEmployeeHierarchy(){
int id;
printf("Enter Employee ID:");
if(scanf("%d",&id)!=1){
    printf("Invalid Employee ID\n");
    return;
}
Employee *emp = findEmployeeByID(id);
if(emp ==NULL){
    printf("Employee not found\n");
    return;
}
printf("\n===== EMPLOYEE HIERARCHY =====\n");
displayEmployeeHierarchy(emp, 0);
printf("Total Employees under %d:%d\n",id,countDescendants(emp));
}

void displayCompleteHierarchy(void){
    printf("\n===== ORGAIZATION HEIRARCHY =====\n");
    if(totalEmp ==0){
        printf("No employees in organization\n");
        printf("Total Employees:0\n");
        return;
    }
    for(int i=0;i<totalEmp;i++){
        if(org[i]->manager ==NULL){
            displayEmployeeHierarchy(org[i],0);
        }
    }
    printf("Total Employees: %d\n",totalEmp);
}
int main(){
    int choice;
    int id;
    Employee emp;
    int managerID;
    do{
        printf("\n===== EMPLOYEE HIERCHY MANAGEMENT =====\n");
        printf("1. Add Employee\n");
        printf("2. Remove Employee\n");
        printf("3. Display Employee Hierarchy\n");
        printf("4. Display Complete Hierarchy\n");
        printf("5. Exit\n");
        printf("Enter your choice:");

        if(scanf("%d",&choice)!=1){
            printf("Invalid input.Exiting program\n");
            break;
        }
        switch(choice){
            case 1:
                printf("Enter employee ID:");
                if(scanf("%d",&emp.emp_id)!=1){
                    printf("Invalid employee ID\n");
                    break;
                }
                if(findEmployeeByID(emp.emp_id)!=NULL){
                printf("Employee ID already exists\n");
                break;
                }
                printf("Enter employee name: ");
                if (scanf(" %49[^\n]", emp.name) != 1) {
                    printf("Invalid employee name\n");
                    break;
                }

                printf("Enter employee designation: ");
                if (scanf(" %49[^\n]", emp.designation) != 1) {
                    printf("Invalid designation\n");
                    break;
                }
                printf("Enter manager ID(-1 if no manager):");
                if(scanf("%d",&managerID)!=1){
                    printf("Invalid manager id\n");
                 break;
                }
                addEmployee(emp,managerID);
                break;
            case 2:
                printf("Enter employee ID to remove: ");
                if(scanf("%d",&id)!=1){
                    printf("Invalid employee ID\n");
                    break;
                }
                removeEmployee(id);
                break;
            case 3:
                showEmployeeHierarchy();
                break;
            case 4:
                displayCompleteHierarchy();
                break;
            case 5:
                printf("Exiting program");
                break;
            default:
                printf("Invalid choice.Please select 1 to 5\n");
        }
    }while(choice!=5);
    
    return 0;
}