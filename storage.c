#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "storage.h"

/* 
  definition of storage cell structure ----
  members :
  int building : building number of the destination
  int room : room number of the destination
  int cnt : number of packages in the cell
  char passwd[] : password setting (4 characters)
  char *contents : package context (message string)
*/
typedef struct storage_t{
	int building;
	int room;
	int cnt; //�ù� ����
	char passwd[PASSWD_LEN+1];
	
	char *context;
} storage_t;


static storage_t** deliverySystem; 			//deliverySystem
static int storedCnt = 0;					//number of cells occupied
static int systemSize[2] = {0, 0};  		//row/column of the delivery system
static char masterPassword[PASSWD_LEN+1];	//master password




// ------- inner functions ---------------

//print the inside context of a specific cell
//int x, int y : cell to print the context
static void printStorageInside(int x, int y) {
	printf("\n------------------------------------------------------------------------\n");
	printf("------------------------------------------------------------------------\n");
	if (deliverySystem[x][y].cnt > 0)
		printf("<<<<<<<<<<<<<<<<<<<<<<<< : %s >>>>>>>>>>>>>>>>>>>>>>>>>>>>\n", deliverySystem[x][y].context);
	else
		printf("<<<<<<<<<<<<<<<<<<<<<<<< empty >>>>>>>>>>>>>>>>>>>>>>>>>>>>\n");
		
	printf("------------------------------------------------------------------------\n");
	printf("------------------------------------------------------------------------\n\n");
}

//initialize the storage
//set all the member variable as an initial value
//and allocate memory to the context pointer
//int x, int y : cell coordinate to be initialized
static void initStorage(int x, int y) {
	int i, j;

	deliverySystem[x][y].cnt = 0;	
}

//get password input and check if it is correct for the cell (x,y)
//int x, int y : cell for password check
//return : 0 - password is matching, -1 - password is not matching
static int inputPasswd(int x, int y) {
	char input_Passwd[PASSWD_LEN+1];
	
	printf("input password for (%d, %d) : ", x, y);
	scanf("%s", input_Passwd);
	
	//if inputPasswd != passwd -> return -1
	if(strcmp(input_Passwd,deliverySystem[x][y].passwd) != 0)
	{
		return -1;
	}
	//if inputPasswd == passwd -> return 0
	else if(strcmp(input_Passwd,deliverySystem[x][y].passwd) == 0)
	{
		return 0;
	}
	//if inputPasswd == masterPassword	-> return 0
	
	else if(strcmp(input_Passwd,masterPassword) == 0)
	{
		return 0;
	}
}





// ------- API function for main.c file ---------------

//backup the delivery system context to the file system
//char* filepath : filepath and name to write
//return : 0 - backup was successfully done, -1 - failed to backup
int str_backupSystem(char* filepath) {
	FILE *fp = NULL;
	char c;
	int x, y;

	//filepath open
	fp = fopen(filepath, "w");
	
	if(fp == NULL)
	{
		return -1;
	}
	else
	{
		fprintf(fp, "%d %d", systemSize[0], systemSize[1]);
		fprintf(fp, "%s", masterPassword);
		
			for(x=0; x<systemSize[0]; x++)
			{	
				for(y=0; y<systemSize[1]; y++)
				{
					if(deliverySystem[x][y].cnt == 1);
					//input_row, input_column
					fprintf(fp, "%d %d", x, y); 
					// building, room
					fprintf(fp, "%d %d", deliverySystem[x][y].building, deliverySystem[x][y].room);
					// passwd
					fprintf(fp, "%s", deliverySystem[x][y].passwd);
					// context
					fprintf(fp, "%s\n", deliverySystem[x][y].context);
			}
	}

	fclose(fp);
	
	return 0;
	}
}



//create delivery system on the double pointer deliverySystem
//char* filepath : filepath and name to read config parameters (row, column, master password, past contexts of the delivery system
//return : 0 - successfully created, -1 - failed to create the system
int str_createSystem(char* filepath) {

	FILE *fp = NULL;
	char c;
	int input_row = 0;
	int input_column = 0;
	int i, j;
	
	//filepath open
	fp = fopen(filepath, "r");
	
	//if filepath not open
	if(fp == NULL)	
		return -1;
	
	//read row, column, master password
	fscanf(fp, "%d %d", &systemSize[0], &systemSize[1]);
	fscanf(fp, "%s", masterPassword);
	
	deliverySystem = (struct storage_t **)malloc(systemSize[0] * sizeof(struct storage_t*));
	for(i=0;i<systemSize[0];i++)
		{
			deliverySystem[i] = (struct storage_t *)malloc(systemSize[1] * sizeof(struct storage_t));
		}
		
	for(i=0;i<systemSize[0];i++)
	{
		for(j=0;j<systemSize[1];j++)
			deliverySystem[i][j].context = (char *)malloc(100 * sizeof(char));
	}
	
	//deliverySystem, cnt initialized
	for(i=0;i<systemSize[0];i++)
	{
		for(j=0;j<systemSize[1];j++)
		{
			deliverySystem[i][j].cnt = 0;
		}
	}

	//read past contexts of the delivery system
	//if stored > storedCnt++
	while((c = fgetc(fp)) != EOF)
	{	
		//input_row, input_column
		fscanf(fp, "%d %d", &input_row, &input_column); 
		// building, room
		fscanf(fp, "%d %d", &deliverySystem[input_row][input_column].building, &deliverySystem[input_row][input_column].room);
		// passwd
		fscanf(fp, "%s", deliverySystem[input_row][input_column].passwd);
		// context
		fscanf(fp, "%s", deliverySystem[input_row][input_column].context);
		
		deliverySystem[input_row][input_column].cnt = 1;
		
		storedCnt++;
	}
	
	fclose(fp);
	
	return 0;
}

//free the memory of the deliverySystem 
void str_freeSystem(void) {
	int i, j;
	
	free(deliverySystem);
	for(i=0;i<systemSize[0];i++)
		free(deliverySystem[i]);
	for(i=0;i<systemSize[0];i++)
	{
		for(j=0;j<systemSize[1];j++)
			free(deliverySystem[i][j].context);
	}
}

//print the current state of the whole delivery system (which cells are occupied and the destination of the each occupied cells)
void str_printStorageStatus(void) {
	int i, j;
	printf("----------------------------- Delivery Storage System Status (%i occupied out of %i )-----------------------------\n\n", storedCnt, systemSize[0]*systemSize[1]);
	
	printf("\t");
	for (j=0;j<systemSize[1];j++)
	{
		printf(" %i\t\t",j);
	}
	printf("\n-----------------------------------------------------------------------------------------------------------------\n");
	
	for (i=0;i<systemSize[0];i++)
	{
		printf("%i|\t",i);
		for (j=0;j<systemSize[1];j++)
		{
			if (deliverySystem[i][j].cnt > 0)
			{
				printf("%i,%i\t|\t", deliverySystem[i][j].building, deliverySystem[i][j].room);
			}
			else
			{
				printf(" -  \t|\t");
			}
		}
		printf("\n");
	}
	printf("--------------------------------------- Delivery Storage System Status --------------------------------------------\n\n");
}

//check if the input cell (x,y) is valid and whether it is occupied or not
int str_checkStorage(int x, int y) {
	if (x < 0 || x >= systemSize[0])
	{
		return -1;
	}
	
	if (y < 0 || y >= systemSize[1])
	{
		return -1;
	}
	
	return deliverySystem[x][y].cnt;
}


//put a package (msg) to the cell
//input parameters
//int x, int y : coordinate of the cell to put the package
//int nBuilding, int nRoom : building and room numbers of the destination
//char msg[] : package context (message string)
//char passwd[] : password string (4 characters)
//return : 0 - successfully put the package, -1 - failed to put
int str_pushToStorage(int x, int y, int nBuilding, int nRoom, char msg[MAX_MSG_SIZE+1], char passwd[PASSWD_LEN+1]) {
	deliverySystem[x][y].building = nBuilding;
	deliverySystem[x][y].room = nRoom;
	deliverySystem[x][y].passwd[PASSWD_LEN+1] = passwd;
	deliverySystem[x][y].context = msg;
	deliverySystem[x][y].cnt = 1;
	storedCnt++;
	
	return 0;
}



//extract the package context with password checking
//after password checking, then put the msg string on the screen and re-initialize the storage
//int x, int y : coordinate of the cell to extract
//return : 0 - successfully extracted, -1 = failed to extract
int str_extractStorage(int x, int y) {
	
	if (inputPasswd(x,y) != 0)
	{	
		return -1;
	}
	
	
	else
	{	
		initStorage(x,y);
		storedCnt--;
		printf("-----------> extracting the storage (%d, %d)... \n", x, y);
		
		return 0;
	}
}

//find my package from the storage
//print all the cells (x,y) which has my package
//int nBuilding, int nRoom : my building/room numbers
//return : number of packages that the storage system has
int str_findStorage(int nBuilding, int nRoom) {
	int cnt = 0;
	int x, y;
	for (x=0; x<systemSize[0]; x++)
	{
		for(y=0; y<systemSize[1]; y++)
			if(((deliverySystem[x][y].building) == nBuilding) && ((deliverySystem[x][y].room) == nRoom))
			{
				printf("-----------> Found a package in (%d, %d) \n", x, y);
				cnt++;
			}
	}	
	return cnt;
}
