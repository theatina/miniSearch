////////////////////////////////
//	
//	Christina - Theano Kylafi
//
///////////////////////////////

#ifndef STRUCTS_H_  
#define STRUCTS_H_

struct Plist 
{
	int id;					//id keimenou sto opoio periexetai h leksi
	int freq; 				//emfaniseis leksis sto keimeno me id
	struct Plist* next;  	//next keimeno sto opoio emfanizetai h leksi
};

struct Trie_t 
{
	char gramma;					//the letter
	struct Trie_t* next;			// next letter horizontally - same level (under the same letter);	
	struct Trie_t* child;			// child letter
	int wordend;      				//0 - if not end of word, 1 - if end of word - flag gia end of word
	int df ;							// ean wordend = 1, tote krataw to document frequency, diladi se posa docs emfanizetai
	struct Plist* postlist; 		//the posting list - if the wordend variable is 1 !! - deixnei ston prwto komvo tou posting list			
};

struct idscore
{	
	int id;
	float skor;	
};


#endif /* STRUCTS_H_ */