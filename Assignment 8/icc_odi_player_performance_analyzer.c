#include<stdio.h>
#include<stdlib.h>
#include<string.h>

#include "Players_data.h"

typedef struct{
    int playerId;
    char *name;
    char *teamName;
    char *role;
    int totalRuns;
    float battingAverage;
    float strikeRate;
    int wickets;
    float economyRate;
    float performanceIndex;
}PlayerInfo;

typedef struct Node{
    PlayerInfo player;
    struct Node*next;
}Node;

typedef struct Team{
    int teamID;
    char teamname[51];
    int totalPlayers;
    float averageBattingstrikerate;
    Node * playersHead;
}Team;

Node *createPlayerNode(const Player*source){
    if(source==NULL){
        return NULL;
    }
    Node*newNode = malloc(sizeof(Node));
    if(newNode == NULL){
        printf("Memory allocation failed\n");
        return NULL;
    }

    newNode->player.name = malloc(strlen(source->name)+1);
    newNode->player.teamName = malloc(strlen(source->team)+1);
    newNode->player.role = malloc(strlen(source->role)+1);

    if(newNode->player.name==NULL || newNode->player.teamName==NULL|| newNode->player.role==NULL){
        free(newNode->player.name);
        free(newNode->player.teamName);
        free(newNode->player.role);
        free(newNode);
        printf("Memory allocation failed\n");
        return NULL;
    }

    strcpy(newNode->player.name,source->name);
    strcpy(newNode->player.teamName,source->team);
    strcpy(newNode->player.role,source->role);

    newNode->player.playerId = source->id;
    newNode->player.totalRuns = source->totalRuns;
    newNode->player.battingAverage = source->battingAverage;
    newNode->player.strikeRate = source->strikeRate;
    newNode->player.wickets = source->wickets;
    newNode->player.economyRate = source->economyRate;

    newNode->player.performanceIndex =0.0;
    newNode->next = NULL;
    return newNode;
}

void calculatePerformaneIndex(PlayerInfo*player){
    if(player==NULL){
        return;
    }
    if(strcmp(player->role,"Batsman")==0){
        player->performanceIndex = (player->battingAverage*player->strikeRate)/100.0;
    }
    else if(strcmp(player->role,"Bowler")==0){
        player->performanceIndex =(player->wickets*2)+(100-player->economyRate);
    }
    else if(strcmp(player->role,"All-rounder")==0){
        player->performanceIndex = ((player->battingAverage*player->strikeRate)/100.0)+(player->wickets*2);
    }
    else{
        printf("Invalid player role\n");
        player->performanceIndex=0.0;
    }
}


void initializeTeams(Team teamList[],int count){
    for(int i=0;i<count;i++){
        teamList[i].teamID = i+1;
        strcpy(teamList[i].teamname,teams[i]);
        teamList[i].totalPlayers =0;
        teamList[i].averageBattingstrikerate =.0;
        teamList[i].playersHead=NULL; 
    }
}


int initializePlayers(Team teamList[],int teamCount){
    Node*tail[teamCount];

    for(int i=0;i<teamCount;i++){
        tail[i]=NULL;
    }
    for(int i=0;i<playerCount;i++){
        int teamIndex =-1;
        for(int j=0;j<teamCount;j++){
            if (strcmp(players[i].team, teamList[j].teamname) == 0) {
                teamIndex = j;
                break;
            }
        }
    if(teamIndex==-1){
        printf("Team not found for player: %s\n", players[i].name);
        return 0;
    }
    Node *newNode = createPlayerNode(&players[i]);
    if (newNode == NULL) {
        return 0;
    }

    calculatePerformaneIndex(&newNode->player);
    if (teamList[teamIndex].playersHead == NULL) {
        teamList[teamIndex].playersHead = newNode;
    }
    else{
        tail[teamIndex]->next = newNode;
    }
    tail[teamIndex] = newNode;
    teamList[teamIndex].totalPlayers++;
    }
    return 1;
}

void calculateTeamAverageStrikeRate(Team teamList[], int teamCount) {
    for (int i = 0; i < teamCount; i++) {
        float totalStrikeRate = 0.0;
        int battingPlayerCount = 0;

        Node *current = teamList[i].playersHead;

        while (current != NULL) {
            if (strcmp(current->player.role, "Batsman") == 0 ||strcmp(current->player.role, "All-rounder") == 0) {
                totalStrikeRate += current->player.strikeRate;
                battingPlayerCount++;
            }
            current = current->next;
        }

        if (battingPlayerCount > 0) {
            teamList[i].averageBattingstrikerate =
                totalStrikeRate / battingPlayerCount;
        }
        else {
            teamList[i].averageBattingstrikerate = 0.0;
        }
    }
}


int findTeamByName(Team teamList[], int teamCount, const char *searchName) {
    int low = 0;
    int high = teamCount - 1;

    while (low <= high) {
        int mid = low + (high - low) / 2;

        int result = strcmp(searchName, teamList[mid].teamname);
        if (result == 0) {
            return mid;
        }
        else if (result < 0) {
            high = mid - 1;
        }
        else {
            low = mid + 1;
        }
    }
    return -1;
}


int addPlayerToTeam(Team teamList[], int teamCount, PlayerInfo newPlayer) {
    int teamIndex = findTeamByName(teamList, teamCount, newPlayer.teamName);

    if (teamIndex == -1) {
        printf("Team not found!\n");
        return 0;
    }
    Node *newNode = malloc(sizeof(Node));
    if (newNode == NULL) {
        printf("Memory allocation failed!\n");
        return 0;
    }

    newNode->player = newPlayer;
    newNode->next = NULL;

    calculatePerformaneIndex(&newNode->player);

    if (teamList[teamIndex].playersHead == NULL) {
        teamList[teamIndex].playersHead = newNode;
    }
    else {
        Node *current = teamList[teamIndex].playersHead;

        while (current->next != NULL) {
            current = current->next;
        }
        current->next = newNode;
    }
    teamList[teamIndex].totalPlayers++;
    return 1;
}


void displayPlayersByTeam(Team teamList[], int teamCount, const char *teamName) {

    int teamIndex = findTeamByName(teamList, teamCount, teamName);
    if (teamIndex == -1) {
        printf("Team not found!\n");
        return;
    }

    Node *current = teamList[teamIndex].playersHead;
    printf("\nPlayers of Team %s:\n\n", teamList[teamIndex].teamname);

    printf("================================================================================\n");
    printf("%-5s %-22s %-15s %-8s %-8s %-8s %-7s %-7s %-12s\n","ID", "Name", "Role", "Runs", "Avg", "SR", "Wkts", "ER", "Perf. Index");

    printf("================================================================================\n");

    while (current != NULL) {

        printf("%-5d %-22s %-15s %-8d %-8.1f %-8.1f %-7d %-7.1f %-12.2f\n",current->player.playerId,
            current->player.name,current->player.role,current->player.totalRuns,
            current->player.battingAverage,current->player.strikeRate,current->player.wickets,
            current->player.economyRate,current->player.performanceIndex);

        current = current->next;
    }
    printf("================================================================================\n");

    printf("\nTotal Players: %d\n", teamList[teamIndex].totalPlayers);

    printf("\nAverage Batting Strike Rate: %.2f\n",
           teamList[teamIndex].averageBattingstrikerate);
}


void displayTeamsSortedByStrikeRate(Team teamList[], int teamCount) {

    Team *sortedTeams[teamCount];
    for (int i = 0; i < teamCount; i++) {
        sortedTeams[i] = &teamList[i];
    }

    for (int i = 0; i < teamCount - 1; i++) {
        for (int j = 0; j < teamCount - i - 1; j++) {
        if (sortedTeams[j]->averageBattingstrikerate <sortedTeams[j + 1]->averageBattingstrikerate) {
                Team *temp = sortedTeams[j];
                sortedTeams[j] = sortedTeams[j + 1];
                sortedTeams[j + 1] = temp;
            }
        }
    }

    printf("\nTeams Sorted by Average Batting Strike Rate:\n\n");

    printf("============================================================\n");

    printf("%-10s %-22s %-18s %-15s\n","Team ID", "Team Name", "Total Players", "Average SR");

    printf("============================================================\n");

    for (int i = 0; i < teamCount; i++) {

        printf("%-10d %-22s %-18d %-15.2f\n",sortedTeams[i]->teamID,
        sortedTeams[i]->teamname,sortedTeams[i]->totalPlayers,sortedTeams[i]->averageBattingstrikerate);
    }
    printf("============================================================\n");
}


void displayTopKPlayers(Team teamList[], int teamCount,const char *teamName, const char *role, int k) {

    int teamIndex = findTeamByName(teamList, teamCount, teamName);

    if (teamIndex == -1) {
        printf("Team not found!\n");
        return;
    }
    if (k <= 0) {
        printf("K must be greater than zero!\n");
        return;
    }

    int count = 0;
    Node *current = teamList[teamIndex].playersHead;

    while (current != NULL) {
        if (strcmp(current->player.role, role) == 0) {
            count++;
        }
        current = current->next;
    }

    if (count == 0) {
        printf("No players found for this role!\n");
        return;
    }

    PlayerInfo **matchingPlayers = malloc(count * sizeof(PlayerInfo *));
    if (matchingPlayers == NULL) {
        printf("Memory allocation failed!\n");
        return;
    }
    current = teamList[teamIndex].playersHead;
    int index = 0;

    while (current != NULL) {
        if (strcmp(current->player.role, role) == 0) {
            matchingPlayers[index] = &current->player;
            index++;
        }
        current = current->next;
    }

    for (int i = 0; i < count - 1; i++) {
        for (int j = 0; j < count - i - 1; j++) {
            if (matchingPlayers[j]->performanceIndex <matchingPlayers[j + 1]->performanceIndex) {
                PlayerInfo *temp = matchingPlayers[j];
                matchingPlayers[j] = matchingPlayers[j + 1];
                matchingPlayers[j + 1] = temp;
            }
        }
    }
    
    if (k > count) {
        k = count;
    }

    printf("\nTop %d %s Players of %s:\n", k, role, teamName);

    printf("===============================================================\n");
    printf("%-5s %-22s %-12s %-12s\n","ID", "Name", "Perf. Index", "Strike Rate");
    printf("===============================================================\n");

    for (int i = 0; i < k; i++) {
        printf("%-5d %-22s %-12.2f %-12.2f\n",matchingPlayers[i]->playerId,matchingPlayers[i]->name,
        matchingPlayers[i]->performanceIndex,matchingPlayers[i]->strikeRate);
    }

    printf("===============================================================\n");
    free(matchingPlayers);
}


void mergePlayers(PlayerInfo *arr[], PlayerInfo *temp[],int left, int mid, int right) {
   
    int i = left;
    int j = mid + 1;
    int k = left;

    while (i <= mid && j <= right) {
        if (arr[i]->performanceIndex >= arr[j]->performanceIndex) {
            temp[k] = arr[i];
            i++;
        }
        else {
            temp[k] = arr[j];
            j++;
        }

        k++;
    }
    while (i <= mid) {
        temp[k] = arr[i];
        i++;
        k++;
    }
    while (j <= right) {
        temp[k] = arr[j];
        j++;
        k++;
    }
    for (int p = left; p <= right; p++) {
        arr[p] = temp[p];
    }
}

void mergeSortPlayers(PlayerInfo *arr[], PlayerInfo *temp[], int left, int right) {
    if (left < right) {
        int mid = left + (right - left) / 2;

        mergeSortPlayers(arr, temp, left, mid);
        mergeSortPlayers(arr, temp, mid + 1, right);
        mergePlayers(arr, temp, left, mid, right);
    }
}


void displayAllPlayersByRole(Team teamList[], int teamCount, const char *role) {
    int count = 0;

    for (int i = 0; i < teamCount; i++) {
        Node *current = teamList[i].playersHead;

        while (current != NULL) {
            if (strcmp(current->player.role, role) == 0) {
                count++;
            }
            current = current->next;
        }
    }

    if (count == 0) {
        printf("No players found for this role!\n");
        return;
    }

    PlayerInfo **matchingPlayers = malloc(count * sizeof(PlayerInfo *));
    PlayerInfo **temp = malloc(count * sizeof(PlayerInfo *));

    if (matchingPlayers == NULL || temp == NULL) {
        printf("Memory allocation failed!\n");
        free(matchingPlayers);
        free(temp);
        return;
    }

    int index = 0;

    for (int i = 0; i < teamCount; i++) {
        Node *current = teamList[i].playersHead;

        while (current != NULL) {
            if (strcmp(current->player.role, role) == 0) {
                matchingPlayers[index] = &current->player;
                index++;
            }
            current = current->next;
        }
    }

    mergeSortPlayers(matchingPlayers, temp, 0, count - 1);

    printf("\nAll %s Players Across Teams:\n\n", role);

    printf("================================================================================\n");
    printf("%-5s %-22s %-18s %-12s %-12s\n", "ID", "Name", "Team", "Role", "Perf. Index");
    printf("================================================================================\n");

    for (int i = 0; i < count; i++) {
        printf("%-5d %-22s %-18s %-12s %-12.2f\n",
               matchingPlayers[i]->playerId,
               matchingPlayers[i]->name,
               matchingPlayers[i]->teamName,
               matchingPlayers[i]->role,
               matchingPlayers[i]->performanceIndex);
    }
    printf("================================================================================\n");

    free(matchingPlayers);
    free(temp);
}


int inputPlayerDetails(PlayerInfo *newPlayer) {
    char name[51];
    char teamName[51];
    char role[30];

    printf("Enter Player ID: ");
    if (scanf("%d", &newPlayer->playerId) != 1) return 0;
    getchar();

    printf("Enter Player Name: ");
    if (fgets(name, sizeof(name), stdin) == NULL) return 0;
    name[strcspn(name, "\n")] = '\0';

    printf("Enter Team Name: ");
    if (fgets(teamName, sizeof(teamName), stdin) == NULL) return 0;
    teamName[strcspn(teamName, "\n")] = '\0';

    printf("Enter Role: ");
    if (fgets(role, sizeof(role), stdin) == NULL) return 0;
    role[strcspn(role, "\n")] = '\0';

    printf("Enter Total Runs: ");
    if (scanf("%d", &newPlayer->totalRuns) != 1) return 0;

    printf("Enter Batting Average: ");
    if (scanf("%f", &newPlayer->battingAverage) != 1) return 0;

    printf("Enter Strike Rate: ");
    if (scanf("%f", &newPlayer->strikeRate) != 1) return 0;

    printf("Enter Wickets: ");
    if (scanf("%d", &newPlayer->wickets) != 1) return 0;

    printf("Enter Economy Rate: ");
    if (scanf("%f", &newPlayer->economyRate) != 1) return 0;

    newPlayer->name = malloc(strlen(name) + 1);
    newPlayer->teamName = malloc(strlen(teamName) + 1);
    newPlayer->role = malloc(strlen(role) + 1);

    if (newPlayer->name == NULL || newPlayer->teamName == NULL || newPlayer->role == NULL) {
        free(newPlayer->name);
        free(newPlayer->teamName);
        free(newPlayer->role);
        return 0;
    }

    strcpy(newPlayer->name, name);
    strcpy(newPlayer->teamName, teamName);
    strcpy(newPlayer->role, role);

    newPlayer->performanceIndex = 0.0;

    return 1;
}


int validatePlayerDetails(Team teamList[], int teamCount, PlayerInfo *newPlayer) {

    if (newPlayer == NULL) {
        return 0;
    }

    if (newPlayer->playerId <= 0) {
        printf("Invalid Player ID!\n");
        return 0;
    }

    for (int i = 0; i < teamCount; i++) {
        Node *current = teamList[i].playersHead;

        while (current != NULL) {
            if (current->player.playerId == newPlayer->playerId) {
                printf("Player ID already exists!\n");
                return 0;
            }
            current = current->next;
        }
    }

    if (findTeamByName(teamList, teamCount, newPlayer->teamName) == -1) {
        printf("Invalid Team Name!\n");
        return 0;
    }

    if (strcmp(newPlayer->role, "Batsman") != 0 &&
        strcmp(newPlayer->role, "Bowler") != 0 &&
        strcmp(newPlayer->role, "All-rounder") != 0) {
        printf("Invalid Player Role!\n");
        return 0;
    }

    if (newPlayer->totalRuns < 0 || newPlayer->battingAverage < 0 ||
        newPlayer->strikeRate < 0 || newPlayer->wickets < 0 ||
        newPlayer->economyRate < 0) {
        printf("Player statistics cannot be negative!\n");
        return 0;
    }

    return 1;
}


void freeMemory(Team teamList[], int teamCount) {
    for (int i = 0; i < teamCount; i++) {
        Node *current = teamList[i].playersHead;

        while (current != NULL) {
            Node *temp = current;
            current = current->next;
            free(temp->player.name);
            free(temp->player.teamName);
            free(temp->player.role);
            free(temp);
        }
        teamList[i].playersHead = NULL;
        teamList[i].totalPlayers = 0;
    }
}

int main() {

    Team teamList[teamCount];
    initializeTeams(teamList, teamCount);

    if (!initializePlayers(teamList, teamCount)) {
        freeMemory(teamList, teamCount);
        return 1;
    }

    calculateTeamAverageStrikeRate(teamList, teamCount);
    int choice;

    do {
        printf("\n================================================================================\n");
        printf("\tICC ODI Player Performance Analyzer\n");
        printf("================================================================================\n\n");

        printf("1. Add Player to Team\n\n");
        printf("2. Display Players of a Specific Team\n\n");
        printf("3. Display Teams by Average Batting Strike Rate\n\n");
        printf("4. Display Top K Players of a Specific Team by Role\n\n");
        printf("5. Display all Players of Specific Role Across All Teams by Performance Index\n\n");
        printf("6. Exit\n\n");

        printf("================================================================================\n");
        printf("Enter your choice: ");

        if (scanf("%d", &choice) != 1) {
            printf("Invalid input!\n");
            freeMemory(teamList, teamCount);
            return 1;
        }

        switch (choice) {
            case 1: {
                PlayerInfo newPlayer;

                if (inputPlayerDetails(&newPlayer)) {
                    if (validatePlayerDetails(teamList, teamCount, &newPlayer)) {
                        if (addPlayerToTeam(teamList, teamCount, newPlayer)) {
                            calculateTeamAverageStrikeRate(teamList, teamCount);
                            printf("Player added successfully!\n");
                        }
                        else {
                            free(newPlayer.name);
                            free(newPlayer.teamName);
                            free(newPlayer.role);
                        }
                    }
                    else {
                        free(newPlayer.name);
                        free(newPlayer.teamName);
                        free(newPlayer.role);
                    }
                }
                else {
                    printf("Failed to input player details!\n");
                }

                break;
            }

            case 2: {
                char teamName[51];

                getchar();
                printf("Enter Team Name: ");
                fgets(teamName, sizeof(teamName), stdin);
                teamName[strcspn(teamName, "\n")] = '\0';

                displayPlayersByTeam(teamList, teamCount, teamName);
                break;
            }
            case 3:
                displayTeamsSortedByStrikeRate(teamList, teamCount);
                break;

            case 4: {
                char teamName[51];
                char role[30];
                int k;

                getchar();

                printf("Enter Team Name: ");
                fgets(teamName, sizeof(teamName), stdin);
                teamName[strcspn(teamName, "\n")] = '\0';

                printf("Enter Role: ");
                fgets(role, sizeof(role), stdin);
                role[strcspn(role, "\n")] = '\0';

                printf("Enter K: ");
                if (scanf("%d", &k) != 1) {
                    printf("Invalid K!\n");
                    freeMemory(teamList, teamCount);
                    return 1;
                }
                displayTopKPlayers(teamList, teamCount, teamName, role, k);
                break;
            }
            case 5: {
                char role[30];

                getchar();

                printf("Enter Role: ");
                fgets(role, sizeof(role), stdin);
                role[strcspn(role, "\n")] = '\0';

                displayAllPlayersByRole(teamList, teamCount, role);
                break;
            }
            case 6:
                printf("Exiting program...\n");
                break;

            default:
                printf("Invalid choice! Please try again.\n");
        }

    } while (choice != 6);

    freeMemory(teamList, teamCount);
    return 0;
}