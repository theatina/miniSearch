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
#include "sorting.h"

struct Trie_t* trie;
int totalines;
int results;
int heapSize;  //size heap gia heapsort

//sorting functions
int min(struct idscore** pinakas)  //epistrefei ti thesi tou min stoixeiou tou pinaka
{
	float min; 
	int y, thesi; 

	min = pinakas[0] -> skor;
	thesi = 0;

	for (y = 1; y < results-1; y++)
	{
		if ((pinakas[y] -> skor) < min)
		{
			thesi = y;
			min = pinakas[y] -> skor;
		}
	}
	return thesi;
}

void set_max(struct idscore** pinakas)  //vriskei to max stoixeio kai to metakinei sti thesi "results-1" tou pinaka
{
	float max; 
	int y, thesi; 

	max = pinakas[0] -> skor;
	thesi = 0;

	for (y = 1; y < results; y++)
	{
		if ((pinakas[y] -> skor) > max)
		{
			thesi = y;
			max = pinakas[y] -> skor;
		}
	}

	int tempid = pinakas[thesi]->id;
	double tempskor = pinakas[thesi]->skor;
	pinakas[thesi] -> id = pinakas[results-1] -> id;
	pinakas[thesi] -> skor = pinakas[results-1] -> skor;
	pinakas[results-1] -> id = tempid;
	pinakas[results-1] -> skor = tempskor;
	
}

//heapsort - 3 functions - source: "https://gist.github.com/nefarel/616667"
void Heapify(struct idscore **topk, int i)
{
  int l = 2 * i + 1;
  int r = 2 * i + 2;
  int largest;
 
  if(l <= heapSize && (topk[l]->skor) > (topk[i]->skor))
    largest = l;
  else
    largest = i;
  if(r <= heapSize && (topk[r]->skor) > (topk[largest]->skor))
    largest = r;
  if(largest != i)
    {
      int tempid = topk[i]->id;
      double tempskor = topk[i]->skor;
      topk[i] -> id = topk[largest] -> id;
      topk[i] -> skor = topk[largest] -> skor;
      topk[largest] -> id = tempid;
      topk[largest] -> skor = tempskor;
      Heapify(topk, largest);
    }
}

void BuildHeap(struct idscore **topk)
{ 
  heapSize = results - 1;
  int i;
  for(i = (results - 1) / 2; i >= 0; i--)
    Heapify(topk, i);
}

void HeapSort(struct idscore **topk)
{
  BuildHeap(topk);
  int i;
  for(i = results - 1; i > 0; i--)
    {
      int tempid = topk[heapSize]->id;
      double tempskor = topk[heapSize]->skor;
      topk[heapSize] -> id = topk[0] -> id;
      topk[heapSize] -> skor = topk[0] -> skor;
      topk[0] -> id = tempid;
      topk[0] -> skor = tempskor;
      heapSize--;
      Heapify(topk, 0);
    }
}

//dimiourgia pinaka me top k apotelesmata
int topkres(struct idscore **score, struct idscore **topk, int tempsize)
{
	float max, temp;
	int i = 0, y, thesi, k = tempsize, countertop = 0;

	while ((countertop < k) && i<totalines)  //gemizw ton pinaka me ta topk apotelesmata tyxaia arxika
	{
		if((score[i]-> id )!= -9)
		{
			topk[countertop]-> id = score[i] -> id;
			topk[countertop]-> skor = score[i] -> skor;
			countertop++;
		}
		i++;
	}
	results = tempsize;
	heapSize = results;
	set_max(topk); //twra tha vrw to max sta k prwta scores wste na sunexisw me max sto topk[results-1]

	max = topk[results-1] -> skor;
	double tempskor;
	int tempid;
	for (y = i; y < totalines; y++) 
	{
		if((score[y]-> id )!= -9)    //an exei skor diladi, to sugrinw
		{
			temp = score[y] -> skor;
			if(temp > max)      //an einai megalutero to skor apo to max  - pou vrisketai sto topk[countertop], to allazoume me to topk[0] kai thetoume max to kainourio score
			{	
				tempid = topk[results-1] -> id;
				tempskor = topk[results-1] -> skor;
				topk[results-1] -> id = score[y] -> id;
				topk[results-1] -> skor = score[y] -> skor;

				thesi = min(topk);    //to min ektopizetai kai mpainei to proigoumeno max
				topk[thesi] -> id = tempid;
				topk[thesi] -> skor = tempskor;
				
				max = topk[results-1] -> skor;
			}
			else   //alliws vriskw to min kai apla antikathistw to min me to neo stoixeio
			{	
				thesi = min(topk);
				if(temp > (topk[thesi]) -> skor )
				{
					topk[thesi] -> id = score[y] -> id;
					topk[thesi] -> skor = score[y] -> skor;
				}
			}
		}
	}
	HeapSort(topk);  //kanw tin teliki taksinomisi tou pinaka me heapsort 

	return k;  //epistrefw tis theseis pou kalifthikan ston topk(!=k se periptwsi pou exw ligotera keimena apo k me ti leksi pou zhtaw)
}

