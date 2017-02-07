#ifndef read_root_h
#define read_root_h 1

#include "Data_handler.h"
#include "variable.h"
#include <iostream>
#include "TFile.h"
#include "TTree.h"

/* #define ndim 500//so big?  */
/* #define MAX_NUM_CHAINS 2000 //in the original is 100, should studied */
/* #define MAX_HITS_ON_CHAIN 500 //in the original is 100, should studied */
/* #define MAX_LINK_SEP 11 //This is the maximum separation to be included in the chain */
void read_root();

extern Int_t Entries;
extern TFile *rootoutfile;




extern ChainStruct ChainArray[MAX_NUM_CHAINS];


extern TTree   *RTPCTree;

extern Int_t    event;
extern Double_t X[ndim], Y[ndim], Z[ndim];
extern Int_t    Hit;

extern TTree   *chaintree;


#endif
