#include<stdio.h>

const float STANDARD_PRICE = 150.0f;
const float PREMIUM_PRICE = 250.0f;
const float RECLINER_PRICE = 400.0f;
struct Customer{
    char name[50];
    int total_tickets;
    int cat_choice;
};

float subtotal(int cat_choice,int total_tickets){
 float amount ;
 switch(cat_choice)
 {
    case 1:
        amount = STANDARD_PRICE;
        break;
    case 2:
        amount = PREMIUM_PRICE;
        break;
    case 3:
        amount = RECLINER_PRICE;
        break;
    default:
        amount =0;
        break;
 }
  return amount*total_tickets;
}

float discount_amount(int total_tickets , float subtotal){
    float discount =0.0f;
    if(total_tickets>=5){
         discount = subtotal*0.10f;
    }else{
        discount = 0.0f;
    }
    return discount;
}

float final_bill(float discount , float subtotal){
    return subtotal-discount;
}

void printdetail(char name[50],int cat_choice , int total_tickets,float sb,float ds,float fb){
    printf("Customer: %s\n",name);
    if(cat_choice==1){
        printf("Ticket Category: Standard\n");
    }
    else if(cat_choice==2){
        printf("Ticket Category: Premium\n");
    }
    else{
        printf("Ticket Category: Recliner\n");
    }
    printf("Tickets: %d\n",total_tickets);
    printf("Subtotal: %.2f\n",sb);
    printf("Discount: %.2f\n",ds);
    printf("Final Bill: %.2f\n",fb);
}
int main(){
    struct Customer c ;
    if(scanf("%49s",c.name)!=1){
        printf("Invalid inpugct");
        return 1;
    }
    if(scanf("%d",&c.total_tickets)!=1){
        printf("Invalid input");
        return 1;
    }
    if(c.total_tickets<1 || c.total_tickets>20){
        printf("Invalid Ticket count.Ticekts must be between 1 to 20.\n");
        return 1;
    }
    if(scanf("%d",&c.cat_choice)!=1){
         printf("Invalid input");
        return 1;
    }
    if(c.cat_choice<1 || c.cat_choice>3){
        printf("Invalid category.Category choice should be 1,2 or 3 only.\n");
        return 1;
    }

float sb = subtotal(c.cat_choice,c.total_tickets);
float ds = discount_amount(c.total_tickets ,sb);
float fb = final_bill(ds,sb);
printdetail(c.name,c.cat_choice,c.total_tickets,sb,ds,fb);
return 0;
}

