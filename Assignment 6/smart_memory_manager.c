#include<stdio.h>
#include<stdlib.h>

struct MemoryBlock{
    int start;
    int size;
    int isAllocated;
    int allocationID;
}; 
#define first_fit 1
#define best_fit 2
#define worst_fit 3

int *memoryPool = NULL;
struct MemoryBlock *blocks = NULL;
int totalMemory = 0;
int blockcount =0;
int blockCapacity =0;
int nextAllocation_id = 1;
int lastFailedRequest =0;
int allocationStrategy = first_fit;

void initializeMemoryPool(){
    printf("Enter memory pool size in bytes: ");
    if(scanf("%d",&totalMemory)!=1){
        printf("Invalid input\n");
        return;
    }
    if(totalMemory < (int)sizeof(int) || 
    totalMemory >100000 || totalMemory %sizeof(int)!=0){
        printf("Invalid memory pool size\n");
        totalMemory =0;
        return;
    }
    int numberofElements = totalMemory /sizeof(int);
    memoryPool = malloc(numberofElements *sizeof(int));
    if( memoryPool == NULL){
        printf("Memory allocation failed\n");
        totalMemory =0;
        return;
    }
    blockCapacity =1;
    blocks = malloc(blockCapacity * sizeof(struct MemoryBlock));
    if(blocks == NULL){
        printf("Metadata allocation failed\n");
        free(memoryPool);
        memoryPool = NULL;
        totalMemory =0;
        blockCapacity =0;
        return;
    }
    blocks[0].start =0;
    blocks[0].size =totalMemory;
    blocks[0].isAllocated =0;
    blocks[0].allocationID =0;
    blockcount =1;
    nextAllocation_id =1;
    printf("Memory pool initialized successfully\n");
}

void displayMemoryMap(){
    if(memoryPool == NULL){
        printf("Memory pool is not initialized\n");
        return;
    }
    printf("Memory Map(%d bytes):\n",totalMemory);
    for(int i=0;i<blockcount;i++){
        if(blocks[i].isAllocated ==1){
            printf("[Allocated #%d: %d bytes,Start:%d]\n",blocks[i].allocationID,blocks[i].size,blocks[i].start);
        }
        else{
            printf("[Free: %d bytes,Start:%d]\n",blocks[i].size,blocks[i].start);
        }
    }
}

int findFreeBlock(int requestedSize , int strategy){
    if(requestedSize <=0){
        return -1;
    }

    if(strategy!= first_fit && strategy!=best_fit && strategy!= worst_fit){
        return -1;
    }
    int selectedIndex =-1;
    if(strategy==first_fit){
        for(int i=0;i<blockcount;i++){
            if(blocks[i].isAllocated ==0 && blocks[i].size >= requestedSize){
                return i;
            }
        }  }
        else if(strategy == best_fit) {
    int smallestSize = totalMemory + 1;
    for(int i = 0; i < blockcount; i++) {
        if(blocks[i].isAllocated == 0 &&
           blocks[i].size >= requestedSize) {
            if(blocks[i].size < smallestSize) {
                smallestSize = blocks[i].size;
                selectedIndex = i;
            }
        }}}
    else if(strategy == worst_fit){
        int largestSize =-1;
        for(int i=0;i<blockcount;i++){
            if(blocks[i].isAllocated ==0 && blocks[i].size >= requestedSize){
                if(blocks[i].size > largestSize){
                    largestSize = blocks[i].size;
                    selectedIndex =i;
                }
            } 
        }
    }
    return selectedIndex;
}

int increaseBlockCapacity(){
    blockCapacity = blockCapacity*2;
    struct MemoryBlock *temp = realloc(blocks,blockCapacity *sizeof(struct MemoryBlock));
    if(temp == NULL){
        blockCapacity = blockCapacity/2;
        return 0;
    }
    blocks = temp;
    return 1;
}

void allocateMemory(){
    if(memoryPool == NULL){
        printf("Memory pool is not initialized \n");
        return ;
    }
    int requestedSize;
    printf("Enter memory size to allocate(in bytes): ");
    scanf("%d",&requestedSize);
    if(requestedSize <=0){
        printf("Invalid allocation size\n");
        return;
    }
    
    int index = findFreeBlock(requestedSize, allocationStrategy);
    if (index == -1) {
    int totalFree = 0;
    int largestFree = 0;
    for (int i = 0; i < blockcount; i++) {
        if (blocks[i].isAllocated == 0) {
            totalFree += blocks[i].size;
            if (blocks[i].size > largestFree) {
                largestFree = blocks[i].size;
            }}}

    if (totalFree >= requestedSize &&
        largestFree < requestedSize) {

        lastFailedRequest = requestedSize;

        printf("Allocation failed due to external fragmentation.\n");
    }
    else {
        printf("Allocation failed: insufficient free memory.\n");
    }
    return;
}
    int originalSize = blocks[index].size;
    int originalStart = blocks[index].start;

    if(originalSize > requestedSize){
        if(blockcount == blockCapacity){
            if(increaseBlockCapacity()==0){
                printf("Unable to increase metadata capacity\n");
                return;
            }
        }
        for(int i=blockcount;i>index+1;i--){
            blocks[i] = blocks[i-1];
        }
        blocks[index+1].start = originalStart + requestedSize;
        blocks[index +1].size = originalSize - requestedSize;
        blocks[index +1].isAllocated =0;
        blocks[index+1].allocationID =0;
        blockcount++;
    }
    blocks[index].start = originalStart;
    blocks[index].size = requestedSize;
    blocks[index].isAllocated =1;
    blocks[index].allocationID = nextAllocation_id;
    printf("Memory allocated successfully\n");
    printf("Allocation ID :%d\n",nextAllocation_id);
    printf("Allocated Size: %d bytes\n",requestedSize);
    nextAllocation_id++;
}

void mergeFreeBlocks(){
    int i=0;
    while(i<blockcount-1){
        if(blocks[i].isAllocated==0 && blocks[i+1].isAllocated==0){
            blocks[i].size += blocks[i+1].size;
            for(int j=i+1;j<blockcount-1;j++){
                blocks[j] = blocks[j+1];
            }
            blockcount--;
        }
        else{
            i++;
        }}
}

void releaseMemory(){
    if(memoryPool == NULL){
        printf("Memory pool is not initialized\n");
        return;
    }
    int id;
    printf("Enter Allocation id to release: ");
    scanf("%d",&id);
    if(id<=0){
        printf("Invalid allocation ID\n");
        return;
    }
    int found =0;
    for(int i=0;i<blockcount;i++){
        if(blocks[i].isAllocated ==1 && blocks[i].allocationID == id){
            blocks[i].isAllocated =0;
            blocks[i].allocationID =0;
            found =1;
            printf("Memory released successfully\n");
            printf("Released Size: %d bytes\n",blocks[i].size);
            break;
        }
    }
    if(found==0){
        printf("Invalid allocation ID or allocation not found\n");
        return;
    }
    mergeFreeBlocks();
}


void resizeAllocation() {

    if (memoryPool == NULL) {
        printf("Memory pool is not initialized.\n");
        return;
    }
    int id;
    int newSize;
    printf("Enter Allocation ID: ");
    scanf("%d", &id);

    printf("Enter new allocation size: ");
    scanf("%d", &newSize);

    if (id <= 0 || newSize <= 0) {
        printf("Invalid ID or size.\n");
        return;
    }
    int index = -1;

    for (int i = 0; i < blockcount; i++) {
        if (blocks[i].isAllocated == 1 && blocks[i].allocationID == id) {
            index = i;
            break;
        }
    }
    if (index == -1) {
        printf("Allocation ID not found.\n");
        return;
    }
    int oldSize = blocks[index].size;
    if (newSize == oldSize) {
        printf("Allocation size is already the same.\n");
        return;
    }

    if (newSize < oldSize) {
        int remainingSize = oldSize - newSize;

        if (blockcount == blockCapacity) {
            if (increaseBlockCapacity() == 0) {
                printf("Unable to resize allocation.\n");
                return;
            }
        }

        for (int i = blockcount; i > index + 1; i--) {
            blocks[i] = blocks[i - 1];
        }

        blocks[index + 1].start = blocks[index].start + newSize;
        blocks[index + 1].size = remainingSize;
        blocks[index + 1].isAllocated = 0;
        blocks[index + 1].allocationID = 0;
        blocks[index].size = newSize;
        blockcount++;

        mergeFreeBlocks();
printf("Allocation decreased successfully.\n");
    }
    else {
        int requiredSize = newSize - oldSize;
        if (index + 1 >= blockcount || blocks[index + 1].isAllocated == 1 ||
            blocks[index + 1].size < requiredSize) {
            printf("Unable to expand allocation.\n");
            return;
        }
        blocks[index].size = newSize;
        blocks[index + 1].start += requiredSize;
        blocks[index + 1].size -= requiredSize;

        if (blocks[index + 1].size == 0) {

            for (int i = index + 1; i < blockcount - 1; i++) {
                blocks[i] = blocks[i + 1];
            }
            blockcount--;
        }

        printf("Allocation increased successfully.\n");
    }

    printf("New allocation size: %d bytes\n", newSize);
}


void displayMemoryStatistics() {
    if (memoryPool == NULL) {
        printf("Memory pool is not initialized.\n");
        return;
    }
    int allocatedMemory = 0;
    int freeMemory = 0;
    int freeBlocks = 0;
    int largestFreeBlock = 0;

    for (int i = 0; i < blockcount; i++) {
        if (blocks[i].isAllocated == 1) {
            allocatedMemory += blocks[i].size;
        }
        else {
            freeMemory += blocks[i].size;
            freeBlocks++;

            if (blocks[i].size > largestFreeBlock) {
                largestFreeBlock = blocks[i].size;
            }
        }}

printf("Total memory:%d bytes\n",totalMemory);
printf("Allocated:%d bytes\n",allocatedMemory);
printf("Free:%d bytes\n",freeMemory);
printf("Free blocks:%d\n",freeBlocks);
printf("Largest free block:%d bytes\n",largestFreeBlock);

if (lastFailedRequest > 0 &&
    freeMemory >= lastFailedRequest &&
    largestFreeBlock < lastFailedRequest) {

    printf("Note: The request for %d bytes failed even ",lastFailedRequest);
printf("though %d bytes are free, because the freememory is fragmented\n",freeMemory);
    }
}

void changeAllocationStrategy() {

    int choice;
    printf("Select Allocation Strategy:\n");
    printf("1.First Fit\n");
    printf("2.Best Fit\n");
    printf("3.Worst Fit\n");

    printf("Enter your choice: ");
    scanf("%d", &choice);

    if (choice == first_fit) {
        allocationStrategy = first_fit;
        printf("Strategy set to First Fit\n");
    }
    else if (choice == best_fit) {
        allocationStrategy = best_fit;
        printf("Strategy set to Best Fit\n");
    }
    else if (choice == worst_fit) {
        allocationStrategy = worst_fit;
        printf("Strategy set to Worst Fit\n");
    }
    else {
        printf("Invalid strategy choice.\n");
    }
}


int main() {

    int choice;
    initializeMemoryPool();

    if (memoryPool == NULL) {
        return 1;
    }

    printf("Select allocation strategy(1. First Fit, 2. Best Fit, 3. Worst Fit): ");

    if (scanf("%d", &choice) != 1) {
        printf("Invalid input.\n");
        free(blocks);
        free(memoryPool);
        return 1;
    }

    if (choice == first_fit) {
        allocationStrategy = first_fit;
        printf("Strategy set to First Fit\n");
    }
    else if (choice == best_fit) {
        allocationStrategy = best_fit;
        printf("Strategy set to Best Fit.\n");
    }
    else if (choice == worst_fit) {
        allocationStrategy = worst_fit;
        printf("Strategy set to Worst Fit.\n");
    }
    else {
        allocationStrategy = first_fit;
        printf("Invalid strategy. First Fit selected by default.\n");
    }

    do {

        printf("\nMenu:\n");
        printf("1. Allocate memory\n");
        printf("2. Release memory\n");
        printf("3. Resize allocation\n");
        printf("4. Display memory map\n");
        printf("5. Display memory statistics\n");
        printf("6. Change allocation strategy\n");
        printf("7. Exit\n");

        printf("\nChoice: ");

        if (scanf("%d", &choice) != 1) {
            printf("Invalid input. Enter a number.\n");
        }

        switch (choice) {
            case 1:
                allocateMemory();
                break;

            case 2:
                releaseMemory();
                break;

            case 3:
                resizeAllocation();
                break;

            case 4:
                displayMemoryMap();
                break;

            case 5:
                displayMemoryStatistics();
                break;

            case 6:
                changeAllocationStrategy();
                break;

            case 7:
                printf("Exiting Smart Memory Manager.\n");
                break;

            default:
                printf("Invalid choice. Please try again.\n");
        }

    } while (choice != 7);
    free(blocks);
    free(memoryPool);

    blocks = NULL;
    memoryPool = NULL;
    return 0;
}
