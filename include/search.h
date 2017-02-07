#ifndef search_h
#define search_h 1
#include "Data_handler.h"
#include "TVector3.h"
#include "Rtypes.h"
#include "read_root.h"

/* #define ndim 500//so big?  */
/* #define MAX_NUM_CHAINS 100 //in the original is 100, should studied */
/* #define MAX_HITS_ON_CHAIN 500 //in the original is 100, should studied */
/* #define MAX_LINK_SEP 11 //This is the maximum separation to be included in the chain */


//Int_t    EventID;



//Int_t    anchor_hit, seed_hit, next_hit, seed_index;
/* Int_t    num_chains; */

/* Int_t    num_hits_this_chain[MAX_NUM_CHAINS]; */
/* Int_t    chain_hits[MAX_NUM_CHAINS][MAX_HITS_ON_CHAIN]; */

/* Double_t separation, acceptance; */
/* TVector3 gra_vec[MAX_HITS_ON_CHAIN]; */
/* TVector3 pnext_pre; */
/* TVector3 pnext; */

//Int_t    index_hits;
//Int_t    NoH;//Number of Hits
//Int_t    maxin; //max number of hits in an event. Calculated during the readout of the event


extern Int_t real_eve_counter;
extern Int_t double_counter;
extern Int_t mcts;

extern  Int_t temp_eve;

extern Int_t    num_chains;


// Int_t Entries;

/* double space; */
/* double max_ang; */
/* double min_ang; */
/* double ang_sep; */

//Int_t temp_eve;


void search(HitStruct[], Int_t, Int_t);
void accept_hit(Int_t);
void store_data(Int_t);




#endif 
