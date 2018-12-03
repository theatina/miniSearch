#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>
#include <math.h>
#include <sys/ioctl.h>
#include <unistd.h>
#include <assert.h>

#include "structs.h"
#include "funs.h"
#include "sorting.h"
#include "trie.h"
#include "extras.h"

//#define BUFFSIZE 512

struct Trie_t* trie;	//arxikos komvos sti domi trie 
int totalines;  		//sunolikes grammes arxeiou 
int results;			//apotelesmata pros emfanisi - global gia xrisi se diafores sunartiseis
	
int main(int argc, char **argv)
{	
	FILE* fp;
  	int kresults, i, lines = 1;      //lines - counter grammwn arxeiou	

  	//elegxoi gia arguments 
  	if((argc == 5) || (argc == 4))
  	{
  		if(!((!(strcmp(argv[1], "-k"))) || (!(strcmp(argv[3], "-k")))))
  		{	fprintf(stderr,"\n\"-k\" flag missing!\nUsage: %s -i <soure-file> -k <max results>\n", argv[0]); return -51;	}
  	}
  	else if(argc < 4)
  	{	
  		printf("\nToo Few Arguments!\n");
  		fprintf(stderr,"Usage: %s -i <soure-file> -k <max results>\n", argv[0]);
    	return -51;
  	}
  	else if(argc > 5)
  	{
  		printf("\nToo Many Arguments!\n");
  		fprintf(stderr,"Usage: %s -i <soure-file> -k <max results>\n", argv[0]);
    	return -51;
  	}

  	if(!(strcmp(argv[1], "-i")))
  	{
  		if(!(strcmp(argv[3], "-k")))
  		{
  			if ((fp = fopen(argv[2], "r")) == NULL)
  			{  	perror("\nfopen source-file"); printf("\n"); return 1; }

  			if(argv[4] == NULL)
  			{	kresults = 10; }
  			else
  			{
	  			char* temp = argv[4] ;
	  			char xarakt = *temp;
	  			int isdig = 1;
	  			while((xarakt != '\0') && isdig)
	  			{
	  				if (isdigit(xarakt)==0)
	  					isdig = 0;
	  				temp++;
	  				xarakt = *temp;
	  			}

	  			if (!isdig)
	  			{	fprintf(stderr,"\nUsage: %s -i <soure-file> -k <max results>\n", argv[0]); return -7; }
	  			else
	  			{
	  				kresults = atoi(argv[4]);
	  				if(kresults<=0)
	  				{
	  					printf("\nMax Results must be greater than zero/\n");
	  					fprintf(stderr,"\nUsage: %s -i <soure-file> -k <max results>\n", argv[0]);
	  					return -7;
	  				}  				
	  			}
  			}
  		}
  		else
  		{	fprintf(stderr,"Usage: %s -i <soure-file> -k <max results>\n", argv[0]); return -51; }
  	}
  	else if(!(strcmp(argv[2], "-i")))
	{
  		if(!(strcmp(argv[1], "-k")))
  		{
  			if ((fp = fopen(argv[3], "r")) == NULL)
  			{  	perror("\nfopen source-file"); printf("\n"); return 1; }
  			
  			kresults = 10;  //ta apotelesmata pairnoun default timi afou de dwthike san orisma
  		}
  		else 
  		{ 	fprintf(stderr,"Usage: %s -i <soure-file> -k <max results>\n", argv[0]); return -51; }
  	}
  	else if (!(strcmp(argv[3], "-i")))
  	{	
  		if(!(strcmp(argv[1], "-k")))
  		{
  			if ((fp = fopen(argv[4], "r")) == NULL)
  			{ 	perror("\nfopen source-file"); printf("\n"); return 1; }
	
  			char* temp = argv[2] ;
  			char xarakt = *temp;
  			int isdig = 1;
  			while((xarakt != '\0') && isdig)
  			{
  				if (isdigit(xarakt)==0)
  					isdig = 0;
  				temp++;
  				xarakt = *temp;
  			}

  			if (!isdig)
  			{
  				printf("\n<max results> should not contain characters\n");
  				fprintf(stderr,"\nUsage: %s -i <soure-file> -k <max results>\n", argv[0]);
  				return -7;
  			}
  			else
  			{ 	kresults = atoi(argv[2]); }	
  		}
  		else 
  		{ 	fprintf(stderr,"Usage: %s -i <soure-file> -k <max results>\n", argv[0]); return -51; }
  	}
  	else
  	{	fprintf(stderr,"\n\"-i\" flag missing!\nUsage: %s -i <soure-file> -k <max results>\n", argv[0]); return -51; }
	
	results = kresults;

	printf("\nLoading...");

  	char ch;
  	while(!feof(fp))   //count file lines
	{	

  		ch = fgetc(fp);
  		if(ch == '\n')
  		{	
    		lines++;   		
  		}
	}
	printf("\n");
	totalines = lines;  //sunolikes grammes text (plithos keimenwn)

	//dilwsi tou array gia apothikeusi twn keimenwn - keimena array 
	char *keimena[lines]; 	// to id tou keimenou tha einai i thesi tou keimenou ston pinaka, ektos ki an exei lathos to arxeio opote termatizei to programma
	int words[lines]; 		//pinakas me tis lekseis kathe keimenou - kathe thesi array dilwnei to antistoixo id keimenou

	//domes gia score
	struct idscore *score[lines];  	 	//score keimenwn gia kathe query  
	struct idscore *topk[results];	  	//ta top k apotelesmata me vasi to score	

	
	rewind(fp);  //ksanathetw ton pointer stin arxi tou arxeiou
 	
 	//dimiourgia map- "keimena"
  	char *temp1=NULL, *temp2; 			
	int createmap = 0;
	if((createmap = create_map(fp, keimena))==-4)
	{
		return -4;
	}

	//dimiourgia trie domis
	int wc = 0;    		//word count text gia to score
	int maxlength = 0;  //megisto mikos leksis gia tin df_all argotera

	create_trie(keimena, totalines, &wc, &maxlength, words); 

	double avgdl = (double)(wc / totalines ) ;  //mesos oros leksewn arxeiou

	ssize_t linelen;
 	size_t len = 0 ; 	//me len (buffersize) 0, i getline kanei realloc oso xreiazetai gia to input
 	char *input = NULL, *quer; 
 	char delimiters[] = " \t\n";

 	printf("\n");
 	printf("\nSelect mode:\n1. \"/search <query1> <query2> ... <query10>\"\n");
 	printf("2. \"/df or /df <query>\"\n3. \"/tf <id keimenou> <query1> <query2> ... <queryi>\"\n4. \"/exit\"\n");
 	printf("Enter command: ");

 	if ((linelen = getline(&input, &len, stdin)) == -1)
 	{	perror("\nGetline\n");  }
	char* command = strtok_r(input,delimiters,&temp2);

	while ((command == NULL)) //pairnw input apo stdin mexri na dwthei swsta mia entoli
	{
		printf("\nCommand must be one of the following!\n");
		printf("\nSelect mode:\n1. \"/search <query1> <query2> ... <query10>\"\n");
		printf("2. \"/df or /df <query1> <query2> ... <queryi>\"\n3. \"/tf <id keimenou> <query>\"\n4. \"/exit\"\n");
		printf("Enter command: ");
		if ((linelen = getline(&input, &len, stdin)) == -1)
 		{	perror("\nGetline\n");  }
		command = strtok_r(input,delimiters,&temp2);
	}

	int test;
	int df = -18;  		//gia mellontiko elegxo
	//int tempsize = 0;	//
	while((test=(strcmp(command, "/exit"))!=0) )
	{	
		if ((strcmp(command, "/search"))==0) //search
		{  
		    
			char *argstring ;
			int argnum = 0;
			

			printf("\n-Search mode selected-\n");
			printf("\n");
		
			if (temp2!=NULL)  
			{
				argstring =  (char*) malloc((strlen(temp2)+1)*sizeof(char));					
				strcpy(argstring, temp2); //antigrafw ta queries gia epeksergasia
			}
			else
			{ 	argstring = (char*)malloc (sizeof(char)); }

			char *pargstring = argstring; 	//krataw pointer gia free meta
			
			quer = strtok_r(temp2,delimiters,&temp1);	
			if(quer == NULL)  //an den exw orisma meta ti /search -> error
			{	
				printf("ERROR\nUsage: /search <query1> <query2> ... <query10>\n");
			}
			else
			{
				for(i = 0; i<totalines; i++)  //dimiourgw pinaka me scores gia kathe keimeno 
					{	
						score[i] = (struct idscore*) malloc (sizeof(struct idscore));
						score[i] -> id = -9;  	//to orizw ws -9 gia mellontiko elegxo
						score[i] -> skor = 0;
					}

				
				struct Trie_t *sres = NULL;
				int atleast1found= 0;
				int q = 0;  //# queries 
				while ((quer !=NULL) && q<10)  //psaxnw gia to polu 10 queries an uparxoun sto arxeio 
				{		
					sres = search_trie(quer);  //psaxnw ti leksi kai epistrefw ton komvo tou teleutaiou grammatos h null an den uparxei
					
					if(sres == NULL)
					{												
						printf("Word \"%s\" not found\n", quer);
					}
					else
					{	
						if (!atleast1found) //vrika estw ena query
							atleast1found= 1;

						printf("Word \"%s\" found\n", quer);

						scorecalc(score, sres, words, avgdl); //ypologismos score
					}
					printf("\n");
					
					q++;
					temp2= temp1;
					quer = strtok_r(temp2,delimiters,&temp1); //epomeno query 
				}

				int tempsize = 0;  //an uparxoun ligotera keimena apo kresults, vriskw akrivws posa einai 
				for (int r = 0; r < totalines; ++r) 
				{	
					if((score[r]->id)!=-9)
					{
						tempsize++;
					}
				}

				if (tempsize > kresults)
					tempsize = kresults;

				for(i = 0; i < tempsize; i++)	//dimiourgw pinaka gia ta k top scores - an exw keimena < kresults, dimiourgw mikrotero pinaka
				{	
					topk[i] = (struct idscore*) malloc (sizeof(struct idscore));	
					topk[i] -> id = -9;		//to orizw ws -9 gia mellontiko elegxo
					topk[i] -> skor = 0;
				}

				if (atleast1found )  		//an exw vrei estw ena query oti uparxei
				{
					topkres(score, topk, tempsize); //pairnw to size tou pinaka topk - mporei na nai mikrotero apo "k"
					argnum = q; //pairnw ta queries mexri 10 to poly

					//make the argument array			
					char* token ;
					char d[] = " \t\n";
					char *argarray[argnum];
					char *tempor;
					int l = 0;
					while ((l < argnum) && ((token = strtok_r(argstring,d,&tempor))!=NULL)  )//krataw ta queries se pinaka
					{	
						argarray[l] = (char*)malloc((strlen(token)+1)*sizeof(char));
						strcpy(argarray[l], token);						
						argstring = tempor;
						l++;
					}

					print(keimena, topk, argarray, argnum, tempsize);  //ektypwsi twn apotelesmatwn me upogrammisi
					
					printf("\n");

					for (i = 0; i<argnum; i++)
						free(argarray[i]);
				}

				for(i = 0; i<totalines; i++)
					free(score[i]);
				
				for (i = 0; i<tempsize; i++)
					free(topk[i]);
				
			}
			free(pargstring);

			results = kresults; //epananathetw sta results ta kresults giati ta allazw eswterika stis sunartiseis
			
		}
		else if ((strcmp(command, "/df"))==0) //df
		{
			printf("\n-Df mode selected-\n");
			int flag = 0;	//flag gia na kserw poia apo tis 2 df tha kalesw
			int q = 0; 		//poses lekseis exw - max 10
			while (((quer = strtok_r(temp2,delimiters,&temp1))!=NULL ) && q<10)
			{		
				flag = 1;			//exw estw mia leksi meta tin entoli 
				df = df_w(quer); 	//psaxnw df leksis
				if(df <= 0)  		//de vrethike i leksi
				{ 	printf("\nWord \"%s\" not found\n", quer); }
				else
				{ 	printf("\n%s  %d\n", quer, df ); }

				temp2 = temp1;
				q++;
			}

			if (!flag) //an den exw leksi meta tin entoli "/df" tote thelw tin df_all gia oles tis lekseis
			{
				printf("\n");
				int counter = 0;  				//level counter for the recursion
				char sofar[maxlength];			//string me max length gia apothikeusi sto recursion to mexri tote string tis leksis pou diatrexoume
				df_all(trie->child, &counter, sofar);
			}
		}
		else if ((strcmp(command, "/tf"))==0)  //tf
		{
			int flag1 = 0, flag2 = 0; //flags gia elegxo orismatwn - flag1 1o orisma kai malista arithmo, flag2 - 2o orisma
			int idkeimenou; 
			char* leksi;
			printf("\n-Tf mode selected-\n");

			if((quer = strtok_r(temp2,delimiters,&temp1))!=NULL ) //an exw 1o orisma meta tin entoli "/tf"
			{
				flag1 = 1;
				//elegxos an id na exei mono noumera
				char* tempcharid = quer ;
				char xarakt = *tempcharid;
				int isdig = 1;
				while((xarakt != '\0') && isdig)  
				{
					if (isdigit(xarakt)==0)
						isdig = 0;
					tempcharid++;
					xarakt = *tempcharid;
				}

				if(isdig)  	//an exw arithmo san id proxwraw
				{ 	idkeimenou = atoi(quer); }
				else		//to id keimenou periexei xaraktires
				{ 	flag1 = 0; }

				temp2 = temp1;
			}

			if((quer = strtok_r(temp2,delimiters,&temp1))!=NULL )  //an exw 2o orisma meta tin "/tf"
			{
				flag2 = 1;
				leksi = quer;
			}

			if(flag1 && flag2)  //an exw alla 2 orismata swsta , meta tin entoli "/tf" tote sunexizw
			{

				tf(idkeimenou , leksi);
				/*if(idkeimenou < totalines)
				{	
					struct Trie_t *tfkomvos = search_trie(leksi);
					if (tfkomvos == NULL)  //an den exw vrei ti leksi genika sto trie- leksiko
					{ 	printf("\nWord \"%s\" not found in document %d\n", quer, idkeimenou); }
					else
					{	
						tfnum = tf(tfkomvos,  idkeimenou);  //an uparxei i leksi, psaxnw to tf sto sugekrimeno keimeno
						if (!tfnum)  	//an den uparxei sto keimeno
						{ 	printf("\nWord \"%s\" not found in document %d\n", quer, idkeimenou); }
						else			//an uparxei apla tupwnw ta stoixeia
						{ 	printf("\n%d %s %d\n", idkeimenou, leksi, tfnum ); }
					}
				}
				else
				{	 printf("\nERROR\nId must be between 0 - %d\nUsage: /tf <id keimenou> <query>\n", lines-1 ); }
			*/
			}
			else
			{ 	printf("\nERROR\nUsage: /tf <id keimenou> <query>\n"); }
		}
		else  //lathos command
		{ 	printf("\nCommand must be one of the following !\n"); }

		printf("\nSelect mode:\n1. \"/search <query1> <query2> ... <query10>\"\n");
 		printf("2. \"/df or /df <query1> <query2> ... <queryi>\"\n3. \"/tf <id keimenou> <query>\"\n4. \"/exit\"\n");
 		printf("Enter command: ");

		if ((linelen = getline(&input, &len, stdin)) == -1)
 		{	perror("\nGetline\n");  }
		command = strtok_r(input,delimiters,&temp2);
		while ((command == NULL))
		{
			printf("\nCommand must be one of the following !\n");
			printf("\nSelect mode:\n1. \"/search <query1> <query2> ... <query10>\"\n");
 			printf("2. \"/df or /df <query1> <query2> ... <queryi>\"\n3. \"/tf <id keimenou> <query>\"\n4. \"/exit\"\n");
			printf("Enter command: ");
			if ((linelen = getline(&input, &len, stdin)) == -1)
 			{	perror("\nGetline\n");  }
			command = strtok_r(input,delimiters,&temp2);
		}
	}

	//exit - free domes
	for (i = 0; i<totalines; i++) 
	{	 free(keimena[i]); }
	
	free(input);

	trie_destr(trie->child);
	trie->child = NULL;
	free(trie);
	trie = NULL;  

	printf("\nMemory Freed\n\n");
	
	return 1;
}

