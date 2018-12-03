////////////////////////////////
//	
//	Christina - Theano Kylafi
//
///////////////////////////////

#ifndef FUNS_H_  
#define FUNS_H_

struct Trie_t * search_trie(char* word);
int df_w(char* word);
int df_k(struct Trie_t *komvos);
void df_all(struct Trie_t* trie, int* counter, char* sofar);
void tf(int idnum, char* leksi);
int scorecalc(struct idscore **score, struct Trie_t *komvos, int* wc, float avgdl);

#endif // FUNS_H_

