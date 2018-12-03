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
#include "funs.h"

struct Trie_t* trie;
int totalines;
int results;

#define k1 1.2
#define b 0.75

//search trie 
struct Trie_t* search_trie(char* word)
{
	int y;
	int length = strlen (word);
	char ch;
	struct Trie_t *temp2, *temp1;

	if((trie->child)!=NULL)  //elegxos an to trie einai keno
	{  	temp2 = trie->child; }
	else
	{ 	printf("\nERROR: empty trie!\n"); return NULL; }

	int eow = 0;  	//end of word leksis pou psaxnw
	int found = 0; 	//flag gia to an vrethike i leksi pou psaxnw

	for (y=0; y < length; y++)  //gia kathe gramma tis leksis
	{
		ch = *(word+y); //proxwraw to deikti tis leksis sto epomeno char mexri ton "\0"

		if (y==length-1) //an eimai sto teleutaio gramma , tote exw end of word pou psaxnw
		{	eow = 1; }

		while (( ch != (temp2->gramma) )  && ((temp1 = temp2->next) != NULL)  )  
		{	temp2 = temp1; }

		if ( ch == (temp2->gramma)  )  //an vrw gramma idio me tis leksis pou psaxnw
		{	
			if(eow  && ((temp2->postlist)!=NULL))  //an exw telos leksis pou psaxnw kai telos leksis sto trie, vrika ti leksi  
			{	found = 1; return temp2; }  //epistrefw ton pointer ston komvo tou teleutaiou grammatos tis leksis
			else
			{
				if((temp2 -> child) == NULL) // an den exw paidi i posting list ston komvo tote den uparxei i leksi
				{	return NULL; }  //epistrefw null afou i leksi de vrethike
				else
				{	temp2 = temp2->child;  } //sunexizw tin anazitisi sto paidi - (exw !eow)
			}
		}
		else //an de vrw gramma meta apo to while loop kai teleiwsan oi komvoi tou trie, de tha uparxei i leksi
		{	return NULL; }
	}
	
	if (found)  	//an vrethike i leksi epestrepse pointer ston komvo
	{	return temp2; }
			
	return NULL;  	//alliws null

}

//df functions
int df_k( struct Trie_t *komvos)  //document frequency me dosmeno komvo grammatos 
{	
	int docfreq = komvos -> df;
	return docfreq;
}

int df_w( char* word)  //document frequency gia to word - me dosmeni leksi
{	
	struct Trie_t* komvos;
	komvos = search_trie(word);

	if (komvos == NULL)
		return -11;
	else
	{ 	int docfreq = df_k(komvos); return docfreq; }
}



void df_all(struct Trie_t* trie, int* level, char* sofar)
{
	int eow;
	eow = trie->wordend;

	if((trie->child) !=NULL)  
	{	
		(*level)++;
		sofar[*level-1] = trie->gramma;  //gemizei o pinakas

		df_all(trie->child, level, sofar);
		(*level)--;
	}
		  
	if((trie->next)!=NULL)
	{ 	df_all(trie->next , level, sofar); }

	sofar[*level] = trie->gramma;
	
	if(eow)
	{
		sofar[(*level)+1] = '\0';		

		int docfr = df_w(sofar);
		printf("\n%s   %d\n", sofar, docfr);	
	}
}

//tf function
void tf( int idnum, char *leksi)
{	
	struct Plist *temp1, *temp2;
	int docfreq = 0 ;

	if(idnum < totalines)
	{	
		struct Trie_t *tfkomvos = search_trie(leksi);
		
		if (tfkomvos == NULL)  //an den exw vrei ti leksi genika sto trie- leksiko
		{ 	printf("\nWord \"%s\" not found in document %d\n", leksi, idnum); }
		else
		{	
			temp2 = tfkomvos->postlist;
			//tfnum = tf(tfkomvos,  idkeimenou);  //an uparxei i leksi, psaxnw to tf sto sugekrimeno keimeno
			
			while ( ((temp2 ->id) != idnum)  && (temp1 = temp2->next) != NULL   )  
				temp2 = temp1; 

			if ( idnum == (temp2->id) ) //an vrika id idio me tou keimenou pou psaxnw, tote docfreq =  frequency
			{ 	docfreq = temp2->freq;  } 


			if (!docfreq)  	//an den uparxei sto keimeno
			{ 	printf("\nWord \"%s\" not found in document %d\n", leksi, idnum); }
			else			//an uparxei apla tupwnw ta stoixeia
			{ 	printf("\n%d %s %d\n", idnum, leksi, docfreq ); }
		}
	}
	else
	{	 printf("\nERROR\nId must be between 0 - %d\nUsage: /tf <id keimenou> <query>\n", totalines-1 ); }

}


//score function
int scorecalc(struct idscore **score, struct Trie_t *komvos, int* wc, float avgdl)
{	
	struct Plist* plist;
	double sc, x, x1, x2, y, y1, y2, z;							
	plist = komvos -> postlist;
	int dfreq = komvos -> df, tfreq, tempid;

	
	while ( plist != NULL  )
	{	
		//calculating score
		tempid = plist -> id;
		tfreq = plist -> freq;
		
		x1 = tfreq * (k1+1);
		x2 = tfreq + (k1 * (1 - b + (b * ((wc[tempid]) / avgdl))));
		x = (double)(x1 / x2);  
		
		y1 = totalines - dfreq + 0.5;
		y2 = dfreq + 0.5;
		y = (double)(y1 / y2);
		
		z = log10(y);  			//IDF leksis
		sc = x * z;				//score keimenou "id" gia ti sygkekrimeni leksi
		
		
		if ((score[tempid] -> id)==-9)
			score[tempid] -> id = tempid;
		

		double tempsc = score[tempid] -> skor;
		tempsc += sc; //to score prostithetai sto uparxon score tou keimenou gia to query mexri auti ti leksi
		score[tempid] -> skor = tempsc; 

		plist = plist-> next; //proxwraw sto epomeno keimeno pou periexei ti leksi
		
	}

	int tempsize = 0;
	for (int r = 0; r < totalines; ++r)
	{	
		if((score[r]->id)!=-9)
		{
			tempsize++;
		}
	}

	return tempsize;
}


