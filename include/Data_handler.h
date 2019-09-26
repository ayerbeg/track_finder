#ifndef Data_handler_h
#define Data_handler_h 1

#include "DataStruct.h"
#include "TTree.h"

#define ndim 500//so big? 
#define MAX_NUM_CHAINS 2000 //in the original is 100, should studied
#define MAX_HITS_ON_CHAIN 500 //in the original is 100, should studied
#define MAX_LINK_SEP 11 //This is the maximum separation to be included in the chain




extern HitStruct hitevent[ndim];//The structure has dimensions of how many hits in the event

extern ChainStruct ChainArray[MAX_NUM_CHAINS];


extern TTree   *RTPCTree;

extern Int_t    event;
extern Double_t X[ndim], Y[ndim], Z[ndim];
extern Int_t    Hit;

extern TTree   *chaintree;

#endif
