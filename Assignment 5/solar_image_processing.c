#include<stdio.h>
#include<stdlib.h>
#include<time.h>

void generateMatrix(int *matrix , int n){
 for(int i=0;i<n;i++){
    for(int j=0;j<n;j++){
        *(matrix + i*n+j) = rand()%256;
    }
    }
}
void displayMatrix(int *matrix,int n){
    for(int i=0;i<n;i++){
        for(int j=0;j<n;j++){
            printf("%4d",*(matrix + i*n+j));
        }
        printf("\n");
    }
}
void transposeMatrix(int *matrix,int n){
    for(int i=0;i<n;i++){
        for(int j=i+1;j<n;j++){
            int *p1 = matrix +i*n+j;
            int *p2 = matrix +j*n+i;
            int temp = *p1;
            *p1 = *p2;
            *p2 = temp;
        }
    }
}
void reverse_rows(int *matrix,int n){
    for(int i=0;i<n;i++){
        for(int j=0;j<n/2;j++){
            int *left = matrix + i*n+j;
            int *right = matrix + i*n+(n-1-j);
            int temp = *left;
            *left = *right;
            *right = temp;
        }
    }
}
void rotateMatrix(int *matrix ,int n){
    transposeMatrix(matrix,n);
    reverse_rows(matrix,n);
}
void smoothMatrix(int *matrix,int n){
    for(int i=0;i<n;i++){
        for(int j=0;j<n;j++){
            int sum=0;
            int count=0;

            for(int r=i-1;r<=i+1;r++){
                for(int c=j-1;c<=j+1;c++){
                    if(r>=0 && r<n && c >=0 && c<n){
                        int value = *(matrix+r*n+c)%256;
                        sum +=value;
                        count++;
                    }
                }
            }
            int average = sum/count;
            *(matrix+i*n+j)+= average *256;
        }
    }
    for(int i=0;i<n;i++){
        for(int j=0;j<n;j++){
            *(matrix+i*n+j)/=256;
        }
    }
}

int main(){
    int n;
    if(scanf("%d",&n)!=1){
        printf("Invalid input\n");
        return 1;
    }
    if(n<2 || n>10){
       printf("Invalid size.Value of n should be between 2 to 10\n"); 
       return 1;
    }
    int *matrix =malloc(n*n*sizeof(int));
    if(matrix == NULL){
        printf("Memory allocation failed\n");
        return 1;
    }
    srand(time(NULL));
    generateMatrix(matrix,n);
    printf("\n Original Randomly Generated Matrix:\n\n");
    displayMatrix(matrix,n);

    rotateMatrix(matrix,n);
    printf("\n Matrix after 90 degree clockwise rotation\n\n");
    displayMatrix(matrix,n);
    
    smoothMatrix(matrix,n);
    printf("\nMatrix after smoothing:\n\n");
    displayMatrix(matrix,n);

    free(matrix);
    return 0;

}