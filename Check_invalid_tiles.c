#include<stdio.h>
#include<sys/types.h>
#include<sys/wait.h>
#include<unistd.h>
#include<stdlib.h>
#include<errno.h>
#include<pthread.h>
#include<semaphore.h>

pthread_mutex_t mu;

int workerthread[11]={1,1,1,1,1,1,1,1,1,1,1};
int count=0;
int puzzle[9][9] = {6,2,4,5,3,9,1,8,7
		    ,5,1,9,7,2,8,6,3,4
		    ,8,3,7,6,10,4,2,9,5
		    ,1,4,3,8,6,5,7,2,9
		    ,9,5,2,8,4,7,3,6,1
		    ,7,6,2,3,9,1,4,5,8
		    ,3,7,4,9,5,6,6,4,2
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

invalid irow[100];
int rowcount=0;
int threadcount=0;
void row_checker(void *args){
int caught=0;
	parameters *temp =  (parameters *) args;
	//printf("thread called\n");
	for(int i=temp->row;i<9;i++){
		for(int j=temp->column;j<9;j++){
			for(int l=j+1;l<9;l++){
				if(puzzle[i][j]==puzzle[i][l]){
					pthread_mutex_lock(&mu);
					workerthread[count]=0;
					irow[rowcount].invalidrow = i+1;
					//printf("%d",irow[rowcount].invalidrow);
					irow[rowcount].invalidcolumn = j+1;
					irow[rowcount].value = puzzle[i][j];
					char *a = "repeated value";
					irow[rowcount].reason = a;
					rowcount++;
					//printf("invalid digit at (%d , %d) = %d \n",i+1,j+1,puzzle[i][j]);
					pthread_mutex_unlock(&mu);
				}
				else if(caught==0&&(puzzle[i][j]<1 || puzzle[i][j]>9)){
					pthread_mutex_lock(&mu);
					caught=1;
					workerthread[count]=0;
					irow[rowcount].invalidrow = i+1;
					irow[rowcount].invalidcolumn = j+1;
					irow[rowcount].value = puzzle[i][j];
					char *a = "invalid digit";
					irow[rowcount].reason = a;
					rowcount++;
					//printf("invalid digit at (%d , %d) = %d \n",i+1,j+1,puzzle[i][j]);
					pthread_mutex_unlock(&mu);
				}
				
			}
			caught=0;
		}
	}
	count++;
	pthread_exit(NULL);
}

invalid icol[100];
int colcount=0;

void col_checker(void *args){
int caught=0;
	parameters *temp =  (parameters *) args;
	//printf("thread called\n");
	for(int i=temp->row;i<9;i++){
		for(int j=temp->column;j<9;j++){
			for(int l=j+1;l<9;l++){
				if(puzzle[j][i]==puzzle[l][i]){
					pthread_mutex_lock(&mu);
					workerthread[count]=0;
					icol[colcount].invalidrow = j+1;
					//printf("%d",irow[rowcount].invalidrow);
					icol[colcount].invalidcolumn = i+1;
					icol[colcount].value = puzzle[j][i];
					char *a = "repeated value";
					icol[colcount].reason = a;
					colcount++;
					//printf("invalid digit at (%d , %d) = %d \n",i+1,j+1,puzzle[i][j]);
					pthread_mutex_unlock(&mu);
				}
				else if(caught==0&&(puzzle[j][i]<1 || puzzle[j][i]>9)){
					pthread_mutex_lock(&mu);
					caught=1;
					workerthread[count]=0;
					icol[colcount].invalidrow = j+1;
					icol[colcount].invalidcolumn = i+1;
					icol[colcount].value = puzzle[j][i];
					char *a = "invalid digit";
					icol[colcount].reason = a;
					colcount++;
					//printf("invalid digit at (%d , %d) = %d \n",i+1,j+1,puzzle[i][j]);
					pthread_mutex_unlock(&mu);
				}
				
			}
			caught=0;
		}
	}
	count++;
	pthread_exit(NULL);
}

invalid ibox[100];
int boxcount=0;
void box_checker(void *args){	
	parameters *temp =  (parameters *) args;
	int temparray[9];
	for(int i=0;i<9;i++){
		temparray[i]=0;
	}
	for(int i=temp->row;i<temp->row+3;i++){
		for(int j=temp->column;j<temp->column+3;j++){
			if(puzzle[i][j]>0 && puzzle[i][j]<10 && temparray[puzzle[i][j]-1]==0){
				temparray[puzzle[i][j]-1]=puzzle[i][j];
			}	
			else{
				if(puzzle[i][j]>0 && puzzle[i][j]<10){
					workerthread[count]=0;
					ibox[boxcount].invalidrow=temp->row+1;
					ibox[boxcount].invalidcolumn=temp->column+1;
					ibox[boxcount].value=puzzle[i][j];
					char *a="repeated value";
					ibox[boxcount].reason= a;
					boxcount++;
				}
				else{
					workerthread[count]=0;
					ibox[boxcount].invalidrow=temp->row+1;
					ibox[boxcount].invalidcolumn=temp->column+1;
					ibox[boxcount].value=puzzle[i][j];
					char *a="invalid digit";
					ibox[boxcount].reason= a;
					boxcount++;
				}
			}
		}
	}
	
	
	count++;	
	pthread_exit(NULL);
}


void main(){

	printf("----------Phase 1----------\n");
	
	pthread_t rows,columns,box[9];
	
	parameters *rowcheck = (parameters *) malloc(sizeof(parameters));
	rowcheck->row = 0;
	rowcheck->column=0;
	
	parameters *colcheck = (parameters *) malloc(sizeof(parameters));
	colcheck->row = 0;
	colcheck->column=0;
	
	parameters *boxcheck[9];
	
	boxcheck[0] = (parameters *) malloc(sizeof(parameters));
	boxcheck[0]->row = 0;
	boxcheck[0]->column=0;
	
	boxcheck[1] = (parameters *) malloc(sizeof(parameters));
	boxcheck[1]->row = 0;
	boxcheck[1]->column=3;
	
	boxcheck[2] = (parameters *) malloc(sizeof(parameters));
	boxcheck[2]->row = 0;
	boxcheck[2]->column=6;
	
	boxcheck[3] = (parameters *) malloc(sizeof(parameters));
	boxcheck[3]->row = 3;
	boxcheck[3]->column=0;
	
	boxcheck[4] = (parameters *) malloc(sizeof(parameters));
	boxcheck[4]->row = 3;
	boxcheck[4]->column=3;
	
	boxcheck[5] = (parameters *) malloc(sizeof(parameters));
	boxcheck[5]->row = 3;
	boxcheck[5]->column=6;
	
	boxcheck[6] = (parameters *) malloc(sizeof(parameters));
	boxcheck[6]->row = 6;
	boxcheck[6]->column=0;
	
	boxcheck[7] = (parameters *) malloc(sizeof(parameters));
	boxcheck[7]->row = 6;
	boxcheck[7]->column=3;
	
	boxcheck[8] = (parameters *) malloc(sizeof(parameters));
	boxcheck[8]->row = 6;
	boxcheck[8]->column=6;
	
	
	pthread_create(&rows, NULL, row_checker , (void *) rowcheck);
	pthread_join(rows,NULL);
	pthread_create(&columns, NULL, col_checker , (void *) colcheck);
	pthread_join(columns,NULL);
	for(int i=0;i<9;i++){
		pthread_create(&box[i], NULL, box_checker , (void *) boxcheck[i]);
	}
	for(int i=0;i<9;i++){
		pthread_join(box[i],NULL);
	}
	
	int flag=0;
	for(int j=0;j<11;j++){
		if(j==0&&workerthread[0]==0){
			
			printf("Invalid solution\n");
			printf("invalid rows are : \n");
			for(int i=0;i<rowcount;i++){
				printf("Row %d -> (%d,%d) = %d reason = %s\n",irow[i].invalidrow ,irow[i].invalidrow,irow[i].invalidcolumn,irow[i].value,irow[i].reason);
			}
		}
		else if(j==1&&workerthread[1]==0){
			printf("invalid columns are : \n");
			for(int i=0;i<colcount;i++){
				printf("column %d -> (%d,%d) = %d reason = %s\n",icol[i].invalidcolumn ,icol[i].invalidrow,icol[i].invalidcolumn,icol[i].value,icol[i].reason);
			}		
		}
		else if(flag==0&&workerthread[j]==0){
			flag=1;
			printf("invalid boxes are : \n");
			for(int i=0;i<boxcount;i++){
				printf("box starting index : (%d,%d) = %d reason = %s\n", ibox[i].invalidrow,ibox[i].invalidcolumn,ibox[i].value,ibox[i].reason);
			}
			
		}
	}
	
	int totalinvalid=boxcount;
		
	if(flag==0){
		printf("Valid solution\n");
		for(int i=0;i<9;i++){
			for(int j=0;j<9;j++){
				printf("%d ",puzzle[i][j]);	
			}	
			printf("\n");
		}
	}
	else{
		printf("Total invalid entries are : %d\n",totalinvalid);
	}
	pthread_exit(NULL);
}






