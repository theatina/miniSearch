////////////////////////////////
//	
//	Christina - Theano Kylafi
//
///////////////////////////////

#ifndef TRIE_H_  
#define TRIE_H_

void create_trie(char* map[], int lines, int* wordcount, int* maxword, int* words);
void posting_list(struct Trie_t* komvos, int idnum);
void trie_destr(struct Trie_t *trie);

#endif // TRIE_H_