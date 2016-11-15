#ifndef chain_finder_main_h
#define chain_finder_main_h 1

#define ndim 1500//so big? 
#define MAX_NUM_CHAINS 30000 //in the original is 100, should studied
#define MAX_HITS_ON_CHAIN 30000 //in the original is 100, should studied
#define MAX_LINK_SEP 11 //This is the maximum separation to be included in the chain

#include "TVector3.h"


typedef struct {
  //  Int_t EventID;
  Int_t event;
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
HitStruct readevent[ndim];

EventStruct ChainEv;

TTree   *RTPCTree;
TTree   *RTPCTree_2;


Int_t    event;
Double_t X[ndim], Y[ndim], Z[ndim];
Int_t    Hit;

Int_t    EventID;

Int_t    aHit[3];//hits array for two events readout

Int_t    anchor_hit, seed_hit, next_hit, seed_index;
Int_t    num_chains;

Int_t    num_hits_this_chain[MAX_NUM_CHAINS];
Int_t    chain_hits[MAX_NUM_CHAINS][MAX_HITS_ON_CHAIN];

Double_t separation, acceptance;

TVector3 gra_vec[MAX_HITS_ON_CHAIN];
TVector3 pnext_pre;
TVector3 pnext;

Int_t    index_hits;

Double_t diff[3];
double   dif;

Int_t    maxin; //max number of hits in an event. Calculated during the readout of the event

Int_t    max_entries;//temporary variable to control number of entries to read

Int_t    adj_chain_hits[MAX_NUM_CHAINS][MAX_HITS_ON_CHAIN];

Int_t Entries, Entries_2;

//Int_t max_entries;

TString inputfile;
TString outputfile;
double space;
double max_ang;
double min_ang;
double ang_sep;

Int_t temp_eve;
Int_t real_eve_counter;
Int_t double_counter;

Int_t nomore;

TFile *rootoutfile;
TFile *rootoutfile_tmp;

TTree *chaintree ;

void chain_finder(Int_t);

void store_data(Int_t);

void variable(TString);

//void readout(Int_t, Int_t);
void readout(HitStruct[], Int_t, Int_t, Int_t);

void accept_hit(Int_t);



#endif 
