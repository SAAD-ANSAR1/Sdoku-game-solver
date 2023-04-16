#include<stdio.h>
#include<sys/types.h>
#include<sys/wait.h>
#include<unistd.h>
#include<stdlib.h>
#include<errno.h>
#include<pthread.h>
#include<semaphore.h>

sem_t sem;
int index=0;
int invalididentifier[3]={0};

pthread_t rows,columns,box,replace;

int repeatedrows[100];
int repeatedcol[100];
int repeatedcount=0;

int repeatedrows1[100];
int repeatedcol1[100];
int repeatedcount1=0;

int invalidrows[100];
int invalidcol[100];
int invalidcount=0;


//int workerthread[11]={1,1,1,1,1,1,1,1,1,1,1};
int count=0;
int puzzle[9][9] = {6,2,4,5,3,9,1,8,7
		    ,5,1,9,7,2,8,6,3,4
		    ,8,3,7,6,1,4,2,9,5
		    ,1,4,3,8,6,5,7,2,7
		    ,9,5,8,2,4,7,3,6,1
		    ,7,6,2,3,9,1,4,5,8
		    ,3,7,1,9,5,6,8,4,2
		    ,4,9,6,1,8,2,5,7,3
		    ,2,8,5,4,7,3,9,1,6} ;


typedef struct
{
int row;
int column;
} parameters;

typedef struct
{
int invalidrow;
int invalidcolumn;
int value;
char *reason;
} invalid;

void *row_checker(void *args){
	
	int temp = *(int *)args;
	int tempmatrix[9]={0};
	for(int i=0;i<9;i++){
		if(tempmatrix[puzzle[temp][i]-1]!=0 || puzzle[temp][i]<1 || puzzle[temp][i]>9){
			tempmatrix[puzzle[temp][i]-1]=puzzle[temp][i];
		}
		else{
			sem_wait(&sem);
	        	invalididentifier[index]=1;
	        	sem_post(&sem);
		}
	}
	index++;
	pthread_exit(NULL);
}

int a=6;
void *col_checker(void *args){	
	int temp = *(int *)args;
	int tempmatrix[9]={0};
	for(int i=0;i<9;i++){
		if(tempmatrix[puzzle[i][temp]-1]!=0 || puzzle[i][temp]<1 || puzzle[i][temp]>9){
			tempmatrix[puzzle[i][temp]-1]=puzzle[i][temp];
		}
		else{
			sem_wait(&sem);
	        	invalididentifier[index]=1;
	        	sem_post(&sem);
		}
	}
	index++;
	pthread_exit(NULL);
}


void *box_checker(void *args){	
	parameters *temp =  (parameters *) args;
	int tempmatrix[9];
	for(int i=0;i<9;i++){
		tempmatrix[i]=0;
	}
	for(int i=temp->row;i<temp->row+3;i++){
		for(int j=temp->column;j<temp->column+3;j++){
			if(tempmatrix[puzzle[i][j]-1]!=0 || puzzle[i][j]<1 || puzzle[i][j]>9){
				tempmatrix[puzzle[i][j]-1]=puzzle[i][j];
			}
			else{
				sem_wait(&sem);
	        		invalididentifier[index]=1;
	        		sem_post(&sem);
			}
		}
	}
	index++;
	
	pthread_exit(NULL);
}

int threadcount=0;
void *replaced(void *args)
{			
    parameters *temp = (parameters *) args;
   // printf("= %d %d \n", temp->row,temp->column);
    int tempmatrix[9] ;
    threadcount++;
    for (int i=0; i<9; i++)
    {
    	tempmatrix[i]=0;
    }
    
    for (int i=0; i<9; i++)
    {
	tempmatrix[puzzle[temp->row][i]-1] = puzzle[temp->row][i];
    }
 /*   for (int i=0; i<9; i++)
    {
	printf("%d\n",tempmatrix[i]);
    }*/
    for (int i=0; i<9; i++)
    {
        if(tempmatrix[i]==0)
        {
            puzzle[temp->row][temp->column] = i+1;
           // printf("%d %d\n",i+1,puzzle[temp->row][temp->column]);
        }
    }

pthread_exit(NULL);
}


void main(){

printf("Orignal matrix \n");
for(int i=0; i<9; i++)
    {
       for(int j=0; j<9; j++)
      {
         printf("%d ",puzzle[i][j]);
    }
    printf("\n");
    }
    


	int flaginvalid =0;
	int threadcount=0;
	int totalinvalid=0;
	printf("----------Phase 2----------\n");
	
	for(int i = 0; i < 9; i++){
		for(int j = 0; j < 9; j++){
			for (int k = j + 1; k < 9; k++){
				if (puzzle[i][j] == puzzle[i][k]) {
                    			repeatedrows[repeatedcount]=i;
                   			repeatedcol[repeatedcount]=j;
                   			repeatedcount++;
                   			repeatedrows[repeatedcount]=i;
                   			repeatedcol[repeatedcount]=k;
                   			repeatedcount++;
                		}
            		}
            		if(puzzle[i][j]<1 || puzzle[i][j]>9){
		        	invalidrows[invalidcount]=i;
		        	invalidcol[invalidcount]=j; 
		        	invalidcount++; 
       		}
        	}
    	}

	for(int i = 0; i < 9; i++){
		for(int j = 0; j < 9; j++){
			for (int k = j + 1; k < 9; k++){
				if (puzzle[j][i] == puzzle[k][i]) {
                    			repeatedrows1[repeatedcount1]=j;
                   			repeatedcol1[repeatedcount1]=i;
                   			repeatedcount1++;
                   			repeatedrows1[repeatedcount1]=k;
                   			repeatedcol1[repeatedcount1]=i;
                   			repeatedcount1++;
                		}
            		}
		}
	}

    	sem_init(&sem,0,1);
	for(int i=0;i<repeatedcount;i++){
		threadcount=0;
		for(int j=0;j<3;j++){
		invalididentifier[j]=0;
		}
		index=0;
		pthread_create(&rows, NULL, row_checker, (void *) &repeatedrows[i]);
         	pthread_create(&columns, NULL, col_checker, (void *) &repeatedcol[i]);
		threadcount++;
		threadcount++;
  	    	parameters *boxcheck = (parameters *) malloc(sizeof(parameters));	
         	if(repeatedrows[i]<3 && repeatedcol[i]<3){
			boxcheck->row = 0;
			boxcheck->column=0;
		}
		else if(repeatedrows[i]<3 && repeatedcol[i]<6){
			boxcheck->row = 0;
			boxcheck->column=3;
		}
		else if(repeatedrows[i]<3 && repeatedcol[i]<9){
			boxcheck->row = 0;
			boxcheck->column=6;
		}
		else if(repeatedrows[i]<6 && repeatedcol[i]<3){
			boxcheck->row = 3;
			boxcheck->column=0;
		}
		else if(repeatedrows[i]<6 && repeatedcol[i]<6){
			boxcheck->row = 3;
			boxcheck->column=3;
		}
		else if(repeatedrows[i]<3 && repeatedcol[i]<9){
			boxcheck->row = 3;
			boxcheck->column=6;
		}
		else if(repeatedrows[i]<9 && repeatedcol[i]<3){
			boxcheck->row = 6;
			boxcheck->column=0;
		}
		else if(repeatedrows[i]<9 && repeatedcol[i]<6){
			boxcheck->row = 6;
			boxcheck->column=3;
		}
		if(repeatedrows[i]<9 && repeatedcol[i]<9){
			boxcheck->row = 6;
			boxcheck->column=6;
		}
		pthread_create(&box, NULL, box_checker, (void *) boxcheck);
		threadcount++;
		pthread_join(rows, NULL);
      	 	pthread_join(columns, NULL);
      		pthread_join(box, NULL);
      		flaginvalid=0;
      		if(invalididentifier[0]==1 && invalididentifier[1]==1 && invalididentifier[2]==1){
      			int a=repeatedrows[i],b=repeatedcol[i];
      			for(int m=0;m<repeatedcount1;m++){
      			//printf("(%d, %d)  (%d ,%d) \n",repeatedrows[i],repeatedcol[i], repeatedrows1[m],repeatedcol1[m]);
      				if(repeatedrows1[m]==repeatedrows[i] && repeatedcol1[m]==repeatedcol[i]){
      					a = repeatedrows[i];
      					b = repeatedcol[i];	
      					flaginvalid=1;
      					totalinvalid++;
      				}
      			}
      			//printf("values are : %d %d \n",a,b);
			if(flaginvalid==1){
			printf("invalid entry at : (%d,%d) = %d \n",a,b,puzzle[a][b]);
			//printf("invalid entry is : %d at (%d,%d) \n",puzzle[repeatedrows[i]][repeatedcol[i]],repeatedrows[i],repeatedcol[i]);
			boxcheck->row = a;
			boxcheck->column = b;
			//printf("valid\n");
			pthread_create(&replace, NULL, replaced, (void *) boxcheck);
			threadcount++;
			pthread_join(replace, NULL);
			printf("replaced entry at : (%d,%d) = %d \n",a,b,puzzle[a][b]);
			//printf("valid\n");
			printf("Threads created for this entry = %d \n" , threadcount);
			}
            }
	}
	
	parameters *boxcheck1 = (parameters *) malloc(sizeof(parameters));
	 for(int i=0; i<invalidcount; i++){
	      	 threadcount=0;        
	      	 totalinvalid++;
	      	 boxcheck1->row = invalidrows[i];
		 boxcheck1->column = invalidcol[i];
		printf("invalid entry at : (%d,%d) = %d \n",boxcheck1->row,boxcheck1->column,puzzle[boxcheck1->row][boxcheck1->column]);
		pthread_create(&replace, NULL, replaced, (void *) boxcheck1);
		threadcount++;
		pthread_join(replace, NULL);
		printf("replaced entry : (%d,%d) = %d \n",boxcheck1->row,boxcheck1->column,puzzle[boxcheck1->row][boxcheck1->column]);
		printf("Threads created for this entry = %d \n" , threadcount);
		
    	}
    	printf("Total invalid entries are : %d \n",totalinvalid);
    	printf("Total moves are : %d \n",a);
    	printf("Total threads are : %d \n",threadcount);
   printf("Resultant Matrix:\n");
    for(int i=0; i<9; i++)
    {
       for(int j=0; j<9; j++)
      {
         printf("%d ",puzzle[i][j]);
    }
    printf("\n");
    }
    
    pthread_exit(0);	
}
	
