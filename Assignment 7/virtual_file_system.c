#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include <limits.h>

#define Blocks_size 512
#define MAX_blocks 5000
#define MAX_name 50

// Defining filenode structure
typedef struct FileNode{
    char name[MAX_name];
    int isDirectory;   // 1= directory 0=fikes
    
    struct FileNode *parent;
    struct FileNode *children;
    struct FileNode *next;

    int *blockPointers;
    int contentSize;
}FileNode;

//Defining freeBlock structure
typedef struct FreeBlock{
    int blockIndex;
    struct FreeBlock *next;
    struct FreeBlock *prev;
}FreeBlock;

char **VirtualDisk;
int numBlocks;

FileNode *root = NULL;
FileNode*cwd = NULL;

FreeBlock *freehead =NULL;
FreeBlock *freetail = NULL;

//Initialize file system
void initializeFileSystem(){
    printf("Enter no of disk blocks: ");
    if(scanf("%d",&numBlocks)!=1){
        printf("Invalid Input\n");
        exit(EXIT_FAILURE);
    }

    if(numBlocks<1 || numBlocks>MAX_blocks){
        printf("Number of blocks must be between 1 and %d\n", MAX_blocks);
        exit(EXIT_FAILURE);
    }
    VirtualDisk = malloc(numBlocks*sizeof(char*));
    if(VirtualDisk==NULL){
        printf("Memory allocation failed\n");
        exit(EXIT_FAILURE);
        }
    for(int i=0;i<numBlocks;i++){
        VirtualDisk[i] = calloc(Blocks_size,sizeof(char));

        if(VirtualDisk[i]==NULL){
            printf("Block allocation failed\n");
            for(int j=0;j<i;j++){
                free(VirtualDisk[j]);
            }
            free(VirtualDisk);
            exit(EXIT_FAILURE);
        }
    }
    // Creating Root
    root = malloc(sizeof(FileNode));
    if(root==NULL){
        printf("Root allocation failed\n");
        for(int i=0;i<numBlocks;i++){
            free(VirtualDisk[i]);
        }
        free(VirtualDisk);
        exit(EXIT_FAILURE);
    }

    strcpy(root->name,"/");
    root->isDirectory=1;
    root->parent = NULL;
    root->children = NULL;
    root->next = NULL;
    root->blockPointers = NULL;
    root->contentSize =0;
    //cwd
    cwd = root;
    printf("File System initialized successfully\n");

}

//initialize free list
void initializeFreeList(){
    for(int i=0;i<numBlocks;i++){
        FreeBlock *newBlock = malloc(sizeof(FreeBlock));
        if(newBlock==NULL){
            printf("Memory allocation failed\n");
            exit(EXIT_FAILURE);
        }
        newBlock->blockIndex =i;
        newBlock->next = NULL;
        newBlock->prev = NULL;

        if(freehead==NULL){
            freehead = newBlock;
            freetail = newBlock;
        }
        else{
            freetail->next = newBlock;
            newBlock->prev = freetail;
            freetail = newBlock;
        }
    }
    printf("Free Block list initialized successfully\n");
}

//search
FileNode *findEntry(FileNode *directory,const char*name){
if(directory==NULL || !directory->isDirectory || directory->children==NULL){
        return NULL;
}
FileNode*temp = directory->children;
do{
if(strcmp(temp->name,name)==0){
    return temp;
}
temp = temp->next;
}while(temp!=directory->children);
return NULL;
}

//insert
void insertEntry(FileNode *directory,FileNode *newNode){
    if(directory==NULL || !directory->isDirectory|| newNode ==NULL){
        printf("Invalid directory or node\n");
        return ;
    }
    newNode->parent = directory;

    if(directory->children==NULL){
        directory->children=newNode;
        newNode->next = newNode;
    }
    else{
        FileNode*temp = directory->children;
        while(temp->next!=directory->children){
            temp = temp->next;
        }
        temp->next = newNode;
        newNode->next = directory->children;
    }
}

//remove
void removeEntry(FileNode *directory,FileNode*target){
    if(directory==NULL || !directory->isDirectory||target==NULL || directory->children==NULL){
        printf("Invalid directory or entry\n");
        return;
    }
    FileNode *curr = directory->children;
    FileNode *prev = NULL;

    do{
        if(curr ==target){
            break;
        }
        prev = curr;
        curr = curr->next;
    }while(curr!=directory->children);

    if(curr!=target){
        printf("Entry not found\n");
        return;
    }

    if(curr->next == curr){
        directory->children = NULL;    // if A->A
    }

    else if(curr == directory->children){
        prev = directory->children;    // removing first node
        while(prev->next!=directory->children){
            prev = prev->next;
        }
        directory->children = curr->next;
        prev->next = directory->children;
    }
    else{
        prev->next = curr->next;
    }
    curr->next =NULL;
    curr->parent=NULL;
}

//allocating file node
FileNode *allocateFileNode(const char *name,int isDirectory){
    FileNode *newNode = malloc(sizeof(FileNode));
    if(newNode==NULL){
        printf("Memory allocation failed\n");
        return NULL;
    }
    strncpy(newNode->name,name,MAX_name-1);
    newNode->name[MAX_name-1] = '\0';
    newNode->isDirectory = isDirectory;
    newNode->parent = NULL;
    newNode->children = NULL;
    newNode->next = NULL;
    newNode->blockPointers =NULL;
    newNode->contentSize =0;

    return newNode;
}

//mkdir
void mkdir(const char*name){
    if(name ==NULL || strlen(name)==0 || strlen(name)>=MAX_name||strchr(name,'/')!=NULL){
        printf("Invalid directory name\n");
        return;
    }
    if (cwd == NULL || !cwd->isDirectory) {
        printf("Invalid current directory\n");
        return;
    }
    if (findEntry(cwd, name) != NULL) {
        printf("An entry with this name already exists!\n");
        return;
    }
    FileNode *newDir = allocateFileNode(name, 1);
    if (newDir == NULL) {
        printf("Could not create directory!\n");
        return;
    }
    insertEntry(cwd, newDir);

    printf("Directory '%s' created successfully!\n", name);
}

//create
void create(const char *name) {
    if (name == NULL || strlen(name) == 0 ||
    strlen(name) >= MAX_name || strchr(name, '/') != NULL) {

    printf("Invalid file name!\n");
    return;
}
if (cwd == NULL || !cwd->isDirectory) {
    printf("Invalid current directory!\n");
    return;
}
if (findEntry(cwd, name) != NULL) {
    printf("An entry with this name already exists!\n");
    return;
}
FileNode *newFile = allocateFileNode(name, 0);
if (newFile == NULL) {
    printf("Could not create file!\n");
    return;
}
insertEntry(cwd, newFile);
printf("File '%s' created successfully!\n", name);
}

//allocate block
int *allocateBlocks(int requiredBlocks) {
if (requiredBlocks <= 0) {
    return NULL;
}
int availableBlocks = 0;
FreeBlock *temp = freehead;
while (temp != NULL) {
    availableBlocks++;
    temp = temp->next;
}
if (availableBlocks < requiredBlocks) {
    printf("Not enough free disk space!\n");
    return NULL;
}
int *allocated = malloc(requiredBlocks * sizeof(int));
if (allocated == NULL) {
    printf("Memory allocation failed!\n");
    return NULL;
}
for (int i = 0; i < requiredBlocks; i++) {
    FreeBlock *allocatedBlock = freehead;
    allocated[i] = allocatedBlock->blockIndex;
    freehead = freehead->next;
    if (freehead != NULL) {
        freehead->prev = NULL;
    }
    else {
        freetail = NULL;
    }
    free(allocatedBlock);
    }
    return allocated;
}

// free blocks
int freeBlocks(int *blockPointers, int count) {
if (count < 0 || (count > 0 && blockPointers == NULL)) {
    return 0;
}
FreeBlock *newHead = NULL;
FreeBlock *newTail = NULL;
for (int i = 0; i < count; i++) {
    FreeBlock *newBlock = malloc(sizeof(FreeBlock));
    if (newBlock == NULL) {
        FreeBlock *temp = newHead;
        while (temp != NULL) {
            FreeBlock *next = temp->next;
            free(temp);
            temp = next;
        }
        return 0;  //because of fail
    }
newBlock->blockIndex = blockPointers[i];
newBlock->next = NULL;
newBlock->prev = newTail;
if (newHead == NULL) {
    newHead = newBlock;
}
else {
    newTail->next = newBlock;
}
newTail = newBlock;
}
if (freehead == NULL) {
    freehead = newHead;
    freetail = newTail;
    }
else {
freetail->next = newHead;
newHead->prev = freetail;
freetail = newTail;
}
return 1; //because of success
}

//write
void writeFile(const char *name, const char *content){
    if (content == NULL) {
        printf("Invalid content\n");
        return;
    }
    FileNode *file = findEntry(cwd, name);
    if (file == NULL) {
        printf("File not found\n");
        return;
    }
    if (file->isDirectory == 1) {
        printf("Cannot write to a directory\n");
        return;
    }

    int size = strlen(content);
    int requiredBlocks = (size + Blocks_size - 1) / Blocks_size;

    if (file->blockPointers != NULL) {
        int oldBlocks = (file->contentSize + Blocks_size - 1) / Blocks_size;
        if (freeBlocks(file->blockPointers, oldBlocks) == 0) {
            printf("Could not release old blocks\n");
            return;
        }
        free(file->blockPointers);
        file->blockPointers = NULL;
    }

    if (requiredBlocks == 0) {
        file->contentSize = 0;
        printf("File written successfully\n");
        return;
    }

    file->blockPointers = allocateBlocks(requiredBlocks);

    if (file->blockPointers == NULL) {
        printf("Not enough free blocks\n");
        file->contentSize = 0;
        return;
    }

    int copied = 0;
    for (int i = 0; i < requiredBlocks; i++) {
        int bytes;
        if (size - copied >= Blocks_size) {
            bytes = Blocks_size;
        }
        else {
            bytes = size - copied;
        }
        memcpy(VirtualDisk[file->blockPointers[i]],content + copied,bytes);
        copied = copied + bytes;
    }
    file->contentSize = size;
    printf("File written successfully\n");
}

//readfile
void readFile(const char *name){
    FileNode *file = findEntry(cwd, name);

    if (file == NULL) {
        printf("File not found\n");
        return;
    }
    if (file->isDirectory == 1) {
        printf("This is a directory, not a file\n");
        return;
    }
    if (file->contentSize == 0) {
        printf("File is empty\n");
        return;
    }
    if (file->blockPointers == NULL) {
        printf("File data is not available\n");
        return;
    }
    int requiredBlocks = (file->contentSize + Blocks_size - 1) / Blocks_size;
    int remaining = file->contentSize;
    for (int i = 0; i < requiredBlocks; i++) {
        int blockIndex = file->blockPointers[i];
        int bytes;
        if (remaining >= Blocks_size) {
            bytes = Blocks_size;
        }
        else {
            bytes = remaining;
        }
        printf("%.*s", bytes, VirtualDisk[blockIndex]);
        remaining = remaining - bytes;
    }
    printf("\n");
}

//delete
void deleteFile(const char *name){
    FileNode *file = findEntry(cwd, name);
    if (file == NULL) {
        printf("File not found\n");
        return;
    }
    if (file->isDirectory == 1) {
        printf("This is a directory. Cannot delete it as a file.\n");
        return;
    }
    int requiredBlocks = (file->contentSize + Blocks_size - 1) / Blocks_size;

    if (requiredBlocks > 0) {
        if (freeBlocks(file->blockPointers, requiredBlocks) == 0) {
            printf("Could not release disk blocks\n");
            return;
        }
    }
    free(file->blockPointers);
    removeEntry(cwd, file);
    free(file);

    printf("File deleted successfully\n");
}

//rmdir
void rmdir(const char *name){

    FileNode *dir = findEntry(cwd, name);
    if (dir == NULL) {
        printf("Directory not found\n");
        return;
    }
    if (dir->isDirectory == 0) {
        printf("This is a file, not a directory\n");
        return;
    }

    if (dir->children != NULL) {
        printf("Directory is not empty\n");
        return;
    }
    removeEntry(cwd, dir);

    free(dir);
    printf("Directory removed successfully\n");
}

//ls
void ls(){
    if (cwd->children == NULL) {
        printf("Directory is empty\n");
        return;
    }

    FileNode *temp = cwd->children;
    printf("\nContents of current directory:\n");

    while (1) {
        if (temp->isDirectory == 1) {
            printf("[DIR]  %s\n", temp->name);
        }
        else {
            printf("[FILE] %s\n", temp->name);
        }
        temp = temp->next;
        if (temp == cwd->children) {
            break;
        }
    }
}

//cd
void cd(const char *name){

    if (strcmp(name, "/") == 0) {
        cwd = root;
        return;
    }
    if (strcmp(name, "..") == 0) {
        if (cwd->parent != NULL) {
            cwd = cwd->parent;
        }
        else {
            printf("Already in root directory\n");
        }
        return;
    }
    FileNode *dir = findEntry(cwd, name);

    if (dir == NULL) {
        printf("Directory not found\n");
        return;
    }
    if (dir->isDirectory == 0) {
        printf("Cannot enter a file\n");
        return;
    }
    cwd = dir;
    printf("Directory changed successfully\n");
}

//helper function
void printPath(FileNode *dir){
    if (dir == NULL || dir==root) {
        return;
    }
    if (dir->parent != NULL) {
        printPath(dir->parent);
    }
    printf("/%s", dir->name);
}

//pwd
void pwd(){
    if (cwd == root) {
        printf("/\n");
        return;
    }
    printPath(cwd);
    printf("\n");
}

//df
void df(){
    int freeCount = 0;
    FreeBlock *temp = freehead;
    while (temp != NULL) {
        freeCount++;
        temp = temp->next;
    }
    int usedBlocks = numBlocks - freeCount;
    float diskUsage = ((float)usedBlocks / numBlocks) * 100;

    printf("Total blocks : %d\n", numBlocks);
    printf("Used Blocks: %d\n", usedBlocks);
    printf("Free Blocks: %d\n", freeCount);
    printf("Disk Usage: %.2f%%\n", diskUsage);
}

//free directory
void freeDirectory(FileNode *dir){
    if (dir == NULL) {
        return;
    }
    if (dir->children != NULL) {
        FileNode *current = dir->children;
        FileNode *next;

        do {
            next = current->next;
            freeDirectory(current);

            if (current->blockPointers != NULL) {
                free(current->blockPointers);
            }
            free(current);
            current = next;
        } while (current != dir->children);
    }
}

//cleanup
void cleanup(){

    freeDirectory(root);
    free(root);
    FreeBlock *temp = freehead;

    while (temp != NULL) {
        FreeBlock *next = temp->next;
        free(temp);
        temp = next;
    }
    for (int i = 0; i < numBlocks; i++) {
        free(VirtualDisk[i]);
    }
    free(VirtualDisk);
    printf("File system memory released successfully\n");
}


void displayPrompt(){
    if (cwd == root) {
        printf("/> ");
    }
    else {
        printf("%s > ", cwd->name);
    }
}

//main
int main(){
    char input[1200];
    char command[20];
    char name[MAX_name];
    char content[1000];

    initializeFileSystem();
    initializeFreeList();
    getchar();
    printf("\nCompact VFS - ready. Type 'exit' to quit.\n");

    while (1){
        displayPrompt();
        if (fgets(input, sizeof(input), stdin) == NULL) {
            cleanup();
            break;
        }
        input[strcspn(input, "\n")] = '\0';
        if (sscanf(input, "%19s", command) != 1) {
            continue;
        }
        if (strcmp(command, "mkdir") == 0) {
            if (sscanf(input, "%*s %49s", name) == 1) {
                mkdir(name);
            }
            else {
                printf("Please enter directory name\n");
            }
        }
        else if (strcmp(command, "create") == 0) {
            if (sscanf(input, "%*s %49s", name) == 1) {
                create(name);
            }
            else {
                printf("Please enter file name\n");
            }
        }
        else if (strcmp(command, "write") == 0) {
            if (sscanf(input, "write %49s \"%999[^\"]\"", name, content) == 2) {
                writeFile(name, content);
            }
            else {
                printf("Use: write filename \"content\"\n");
            }
        }
        else if (strcmp(command, "read") == 0) {
            if (sscanf(input, "%*s %49s", name) == 1) {
                readFile(name);
            }
            else {
                printf("Please enter file name\n");
            }
        }
        else if (strcmp(command, "delete") == 0) {
            if (sscanf(input, "%*s %49s", name) == 1) {
                deleteFile(name);
            }
            else {
                printf("Please enter file name\n");
            }
        }
        else if (strcmp(command, "rmdir") == 0) {
            if (sscanf(input, "%*s %49s", name) == 1) {
                rmdir(name);
            }
            else {
                printf("Please enter directory name\n");
            }
        }

        else if (strcmp(command, "ls") == 0) {
            ls();
        }
        else if (strcmp(command, "cd") == 0) {
            if (sscanf(input, "%*s %49s", name) == 1) {
                cd(name);
            }
            else {
                printf("Please enter directory name\n");
            }
        }
        else if (strcmp(command, "pwd") == 0) {
            pwd();
        }
        else if (strcmp(command, "df") == 0) {
            df();
        }
        else if (strcmp(command, "exit") == 0) {
            cleanup();
            printf("Memory released. Exiting program...\n");
            break;
        }
        else {
            printf("Invalid command\n");
        }
    }
    return 0;
}
