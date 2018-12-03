#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>
#include <math.h>
#include <sys/ioctl.h>
#include <unistd.h>
#include <assert.h>

#include "structs.h"
#include "extras.h"

struct Trie_t* trie;	//arxikos komvos sti domi trie 
int totalines;  		//sunolikes grammes arxeiou 
int results;			//apotelesmata pros emfanisi - global gia xrisi se diafores sunartiseis

int create_map(FILE *fp, char* keimena[])
{ 	
	ssize_t linelen;	//line length - getline
	size_t len = 0 ;	//buffer length - initialisation se 0 wste na kanei moni tis realloc(leitourgia getline me buffersize = 0)
	char delimit[]=" \t\n";   //delimiters here are only space and tab (and \n for the end of line ) 
	char *temp1=NULL, *temp2, *ptemp1;				
	char* id;   //temporary string to the id (first word of each line of the text)
	int idnum;	//string to int id - check an einai idio me lines, wste an den einai, exoume lathos(non - sequential id) kai termatizei to programma

	int lines = 0;   		//ksanarxizw to metrima stis grammes alla twra apo 0 	- counter grammwn- documents
	while(!feof(fp))    //apothikeusi sto map "keimena"
	{	
		
  		if((linelen = getline(&temp1, &len, fp)) == -1) //diavazw to text grammi grammi (kathe grammi kai ena document)
  		{ 	perror("\nGetline"); }

  		ptemp1 = temp1;  //kratw pointer prin peiraksw ton temp1 gia free argotera
  		id = strtok_r(temp1,delimit,&temp2);  //pairnw mia mia leksi tou kathe keimenou
  		
  		//check if id den uparxei kai exw mono spaces /tabs
  		if ((id) == NULL)
  		{	
  			printf("ERROR IN FILE - Line: %d has only spaces/tabs\n", idnum);	

		 	//se periptwsi error sto text, termatizw afou kanw ta aparaitita free kai afou kleisw to arxeio 
		 	for (int j = 0; j<lines; j++) 
				free(keimena[j]);

			fclose(fp);
			free(ptemp1);
  			return -4; 	
  		}

  		//elegxw id mipws den einai mono numbers
  		char* tempcharid = id ;
		char xarakt = *tempcharid;
		int isdig = 1;
		while((xarakt != '\0') && isdig)
		{
			if (isdigit(xarakt)==0)
				isdig = 0;
			tempcharid++;
			xarakt = *tempcharid;
		}

  		if(isdig)
  		{
	 		idnum = atoi(id);
	  		//check if id is sequential
	  		if ( (idnum) != (lines)) 
	  		{
	  			printf("ERROR IN FILE - id was not sequential\n");

				//se periptwsi error sto text, termatizw afou kanw ta aparaitita free kai afou kleisw to arxeio 
				for (int j = 0; j<lines; j++)
					free(keimena[j]);

				fclose(fp);
				free(ptemp1);
		  		return -4;
		  	}

	  		keimena[lines]= (char*) malloc ((strlen(temp2)+1)*sizeof(char)); //dimiourgw ton map - pinaka keimenwn
	  		strcpy(keimena[lines], temp2); //antigrafi tou document ston map "keimena"

	    	lines++;   	//epomeni grammi	
    	}
    	else
    	{
    		printf("\nERROR IN FILE - id's must not contain characters!\n");
    		
    		//se periptwsi error sto text, termatizw afou kanw ta aparaitita free kai afou kleisw to arxeio 
			for (int j = 0; j<lines; j++)
				free(keimena[j]);

			fclose(fp);
			free(temp1);
	  		return -4;
    	}
	}
	free(ptemp1); 
	fclose(fp);  	//kleisimo arxeiou efoson exoume apothikeusei ola ta keimena

	return -7;
}


void print(char *keimena[], struct idscore **topk, char *argarray[], int argnum, int tempsize)
{
	int z, intro;
	int counter = 0;
	int ztemp = 0;	
	int width = 0;    
	struct winsize w;
	ioctl(0, TIOCGWINSZ, &w);  //size terminal window se columns kai rows

	for (z = tempsize -1; z >= 0; z--)
	{
		if((topk[z]->id)!= -9) //elegxos mipws den exw gemato pinaka me topk apotelesmata(an kai opws dimiourgeitai den einai dynaton)
		{
			counter++;  //den exw keni thesi, uparxei keimeno sti thesi auti
			intro = printf("\n%d. (%d) [%.4f] ", counter, topk[z]->id, topk[z]->skor ); //ektupwsi score

			char* token ;
			char d[] = " \t\n";

			ztemp = topk[z] -> id;				//id keimenou ston pinaka topk thesi z
			width = (w.ws_col) - intro -1 ; 	//width grammis ektupwsis keimenou sto terminal      
			int lenn = strlen(keimena[ztemp]);  //mikos keimenou ztemp
			int linespr = lenn / (width-1);		//grammes
			int ypol = lenn % (width-1); 		//grammata pou perisseyoun
			
			if (ypol)   //an exw lekseis/grammata pou exoun perissepsei, exw mia extra grammi
				linespr++;



			int size = width-1; //printf("size %d= witdth%d -1\n",size, width );								
			char *ptr = keimena[ztemp]; //deiktis sto keimeno ztemp
			
			int flagkenou = 1;		//arxika tupwnw kena 
			int flagtoken = 0;		//arxika den exw leksi
			
			char *ttoks = (char*) malloc((strlen(keimena[ztemp]) + 1)*sizeof(char)); //gia na mi xalasw ta strings me ta keimena
			char *pttoks = ttoks;  			//gia free meta
			strcpy(ttoks, keimena[ztemp]);  //tha pairnw leksi leksi tin ttoks kathe fora pou tha vriskw gramma 
			int size1 = size;
			for (int f = 0; f < linespr; f++)  //gia kathe grammi tou kathe keimenou
			{
				if ((f==(linespr-1)) && (ypol!=0)) //an eimai stin teleutaia grammi to size tha einai iso me to ypoloipo - ta grammata pou emeinan
				{ 	size = ypol; }
				else
				{ 	size = size1; }

				if(*ptr == ' ') //an exw keno stin arxi tis epomenis grammis kanw tis aparaitites allages
				{
					size = size - 1; 	//meiwnw to size gia na min parei parapanw xaraktires (exw idi kanei xwrismo se grammes)
					ptr++;
					flagtoken = 0;  	//an exw keno den exw pia leksi 
					flagkenou = 1;		//tupwnw kena mexri na vrw ksana idia leksi me leksi tou argarray
				}

				//simvoloseira ypogrammisis
				char *telikiypogram = (char*) malloc((size+1)*sizeof(char));
				char *ypogram;
				ypogram = telikiypogram;

				char *printtemp = (char*) malloc((size+1)*sizeof(char));
				char *pprintemp = printtemp; //gia free meta
				strncpy(printtemp, ptr , size);

				if (f!=0)  //se oles ektos apo tin prwti grammi, dimiourgw stoixisi
				{
					for (int t=0; t<intro-1; t++)
						printf(" ");						
				}

				int eoline = 0;			//metraw xaraktires
				char tempch;									
				tempch = *printtemp;	//xaraktiras					
				while ((eoline < size)  && ((tempch != '\n' ) && (tempch != '\0')))
				{	
					//if(tempch != '\n' )
					printf("%c", tempch );      //ektupwsi xaraktira
					if ((tempch!=' ')  && (tempch!='\t') )  //oso exw akoma xaraktires
					{	
						if(!flagtoken)
						{	
							flagtoken = 1;  //vrika leksi ara exw flagtoken 1
							token = strtok_r(ttoks,d,&ttoks);
							int s = 0;
							while ((s < argnum) && (flagkenou) && (token!= NULL))  //mexri na vrei idio h na teleiwsoun ta queries tou pinaka
							{ 	//an vrei gramma elegxei to token, ki an einai sto array me ta queries kanei flagkenou 0 wste na ypogrammisei
								
								if ((strcmp(token, argarray[s]))==0) 
								{	flagkenou = 0; } //vrikame idia leksi ara arxizoyme upogrammisi
									
								s++;
							}	
						}	
					}
					else
					{
						flagtoken = 0;  //an exw keno simainei oti h leksi teleiwse
						flagkenou = 1;  //kathe fora pou tha exoume keno / tab kanw flagkenou 1 wste na grafw kena 	
					}

					//gemisma string upogrammisis analoga me to flagkenou
					if(flagkenou)
					{ 	strcpy(ypogram , " "); }
					else
					{	strcpy(ypogram , "^"); }

					eoline++ ; 		//auksisi metriti grammatwn
					ptr++ ;			//auksisi deikti keimenou
					ypogram++ ;		//auksisi deikti string upogrammisis
					tempch = *ptr ;	//epomenos xaraktiras 
				}
				printf("\n");
			
				for (int t=0; t<intro-1; t++)
					printf(" ");

				printf("%s", telikiypogram);  //string upogrammisis 
				printf("\n");

				//free
				free(telikiypogram);
				free(pprintemp);	
			}

			free(pttoks);  
		}
	}				
				
}



