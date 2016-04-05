#ifndef chain_finder_h
#define chain_finder_h 1

#define ndim 1500//so big? 
#define MAX_NUM_CHAINS 100 //in the original is 100, should studied
#define MAX_HITS_ON_CHAIN 300 //in the original is 100, should studied
#define MAX_LINK_SEP 7 //this number was estimated by Howard. This is the maximum separation to be included in the chain


typedef struct {
  Int_t EventID;
  Double_t X;
  Double_t Y;
  Double_t Z;
  Int_t Hit;
  Int_t Status;
  Int_t Ev_pos; //if event is the first or the second on the pile
} HitStruct;


typedef struct {
  Int_t ID;
  Double_t X_rec[500];
  Double_t Y_rec[500];
  Double_t Z_rec[500];
  Int_t Hit;
} EventStruct;




HitStruct hitevent[ndim];

EventStruct ChainEv;


TTree *RTPCTree;
Double_t fXRec[ndim], fYRec[ndim], fZRec[ndim];
Int_t    fHit;

Int_t    aHit [2];//temporal array

Int_t anchor_hit, seed_hit, next_hit, seed_index;
Int_t num_chains;

Int_t num_hits_this_chain[MAX_NUM_CHAINS];
Int_t chain_hits[MAX_NUM_CHAINS][MAX_HITS_ON_CHAIN];

Double_t separation;

Double_t pseed[3], pnext[3];

Double_t diff[3];

Int_t maxin; //max number of hits in an event. Calculated during the readout of the event


Int_t HitIn;
Int_t EvId;
