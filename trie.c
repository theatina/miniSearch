////////////////////////////////
//	
//	Christina - Theano Kylafi
//
///////////////////////////////

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <math.h>
#include <sys/ioctl.h>
#include <unistd.h>

#include "structs.h"
#include "trie.h"
#include "funs.h"

struct Trie_t* trie;
int results;
int heapSize;  //size heap gia heapsort

//posting list function
void posting_list(struct Trie_t* komvos, int idnum)   
{	
	struct Plist *plist, *temp1, *temp2, *new;
	int docfreq = df_k(komvos);
	int temp;

	plist = (komvos -> postlist);
	if(plist == NULL)   //an den exw posting list, dimiourgw ton prwto komvo me katalliles arxikopoihseis
	{
		new = malloc(sizeof(struct Plist));
		new -> id = idnum;
		new -> freq = 1;
		new -> next = NULL;
		docfreq ++ ;
		komvos -> df = docfreq;

		(komvos -> postlist) = new;

	}
	else //an exw idi posting list, psaxnw an uparxei idi komvos me id keimenou idnum
	{
		temp2 = plist;

		while ( ((temp2 ->id) != idnum)  && (temp1 = temp2->next) != NULL   )  
		{	temp2 = temp1; }

		if ( idnum == (temp2->id) )  	//an uparxei idi posting list kai pair gia to keimeno me arithmo "idnum" tote auksanw emfaniseis
		{
			temp = temp2->freq;
			temp++;
			temp2 -> freq = temp;
		}
		else							//an den uparxei emfanisi gia to sugekrimeno keimeno, prostithetai sto telos o komvos
		{
			new = malloc(sizeof(struct Plist));
			new -> id = idnum;
			new -> freq = 1;
			new -> next = NULL;

			temp2-> next = new;   		//o teleutaios komvos pia einai o neos, to neo zeugari keimenou - emfanisis

			//pleon emfanizetai se ena epipleon keimeno
			docfreq ++ ;
			komvos -> df = docfreq;
		}	
	}
}

//create domi trie - leksiko
void create_trie(char* map[], int lines, int* wordcount, int* maxlength, int* words)
{
	trie = malloc (sizeof(struct Trie_t));  //arxikos komvos trie domis
	trie->child = NULL;

	int i,y, length;
	int maxword = *(maxlength);  
	char *word; 
	struct Trie_t  *temp1, *temp2;
	char delimit[] = " \t\n";
	int eow = 0, firstime = 0;  //eow - flag gia na kratame an einai telos leksis - firstime flag/metritis gia tous komvous tou trie - elegxos
	char ch;
	
	for (i=0; i < lines; i++)    	//gia kathe grammi - document tou arxeiou
	{	
		words[i] = 0;				//metritis leksewn kathe keimenou

		char *temp = (char*) malloc ((strlen(map[i])+1)*sizeof(char));	
		strcpy(temp , map[i]);  //copy to document i stin temp
		char *ptemp = temp; 	//krataw ton pointer gia free argotera

		while ((word = strtok_r(temp, delimit, &temp))) //gia kathe grammi kanei gia kathe leksi insert ta grammata kai posting list
		{	
			(words[i])++; //auksanw to plithos twn leksewn tou keimenou i
			(*wordcount)++; //auksanw to plithos twn leksewn tou arxeiou gia xrisi sto score

			length = strlen (word);

			if(length>maxword)							//find max length leksis olou tou arxeiou
				maxword = length;

			for (y=0; y < length; y++)  				//gia kathe gramma tis kathe leksis
			{
				firstime++;      						//flag gia prwto komvo m
				ch = *(word+y);

				if (y==length-1)
				{ 	eow = 1;  }

				if(i==0 && firstime ==1)   //an einai o prwtos komvos genika pairnw ksexwristi periptwsi epeidi exw ton Info ---???? mipws na to entaxw sto ypoloipo?!
				{
					struct Trie_t* new = malloc (sizeof(struct Trie_t));	
					new->gramma = ch; 
					new->next = NULL;
					new->child = NULL;
					new->wordend = eow;
					new->df = 0;
					new->postlist = NULL;

					if(eow)  //an exw telos leksis, elegxw posting list - leksi me ena gramma px "a" -> arthro
					{
						posting_list(new, i);
						eow = 0;
					}

					trie -> child = new; 
					temp2 = trie->child;
				}
				else
				{	
					if((temp2 -> child) == NULL) //den exw paidi kai exw gramma pou thelw na eisagw sti leksi san child - new child
					{
						struct Trie_t* new = malloc (sizeof(struct Trie_t));	
						new->gramma = ch; 
						new->next = NULL;
						new->child = NULL;
						new->wordend = eow;
						new->df = 0;
						new->postlist = NULL;

						if(eow)  //an exw telos leksis, elegxw posting list - leksi me ena gramma px "a" -> arthro
						{ 	posting_list(new, i); eow = 0; }

						temp2 -> child = new;
						temp2 = new;  //temp2 = temp2 -> child
					}
					else
					{	
						temp2 = temp2->child;
						//psaksimo orizontia stous neighbours idiou epipedou
						while (((temp1 = temp2->next) != NULL) && ( ch != (temp2->gramma) ) )   //oso den eimaste oute ston teleutaio komvo kai den exoume vrei to xaraktira
						{ 	temp2 = temp1; } 

						if(ch == (temp2->gramma) )  	//an uparxei to gramma tote proxwraw sto epomeno child kai kanw to idio
						{	
							if(eow ==1)  //an exw telos leksis, elegxw posting list alliws apla sunexizw na prosthetw grammata sto leksiko
							{
								posting_list(temp2, i);
								eow = 0;
							}
						}
						else	//an den uparxei to gramma "ch" tote to eisagw sto telos se neo komvo
						{
							struct Trie_t* new = malloc (sizeof(struct Trie_t));	
							new->gramma = ch; 
							new->next = NULL;
							new->child = NULL;
							new->wordend = eow;
							new->df = 0;
							new->postlist = NULL;

							if(eow)  //an exw telos leksis, elegxw posting list
							{
								posting_list(new, i);  //epistrefei 
								eow = 0;
							}
							temp2 ->next = new;			//an exw ftasei sto telos tou epipedou kai den exw vrei to gramma, to prosthetw
							temp2 = new;				//temp2 = temp2 ->next
						}	
					}	
				}
			}

			temp2 = trie; 		//kathe fora tha arxizw na elegxw apo to prwto gramma poy eisigaga
			eow = 0;      		//arxizw nea leksi
		}
		*(maxlength) = maxword;  //krataw to max megethos leksis tou arxeiou
		free(ptemp);
	}
}


//destroy - free trie function
void trie_destr(struct Trie_t* trie)
{
	if((trie->child) !=NULL)  
	{	trie_destr(trie->child); } 	//ftanw sto teleutaio child tou trie
		  
	if((trie->next)!=NULL)			//ftanw sto teleutaio geitona tou teleutaiou child
	{	trie_destr(trie->next); }

	if ((trie-> postlist)!=NULL)  //free to posting list tis kathe leksis 
	{
		struct Plist *p1, *p2;
		p1 = trie -> postlist;
		while ((p2 = p1->next)!=NULL)  
		{
			p1->next = NULL;
			free(p1);
			p1 = p2;

		}
		free(p1);
		trie -> postlist = NULL;
	}

	trie-> child = NULL;	
	trie-> next = NULL;	
	free(trie);				//free ton komvo kai epistrefei 
}

