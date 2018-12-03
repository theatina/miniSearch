////////////////////////////////
//	
//	Christina - Theano Kylafi
//
///////////////////////////////

#ifndef SORTING_H_  
#define SORTING_H_

int topkres(struct idscore **score, struct idscore **topk, int tempsize);
int min(struct idscore** pinakas);
void set_max(struct idscore** pinakas);
void HeapSort(struct idscore** topk);
void Heapify(struct idscore **topk, int i);
void BuildHeap(struct idscore **topk);

#endif /* SORTING_H_ */