#include <iostream>
#include "string.h"
#include "TFile.h"
#include "TTree.h"
#include "TH1.h"
#include "TH2.h"
#include "TH3.h"
#include "TCanvas.h"
#include "TGraph.h"
#include "math.h"
#include "TStyle.h"
#include "TStopwatch.h"
#include <TBenchmark.h>

#include "chain_finder_main.h"

#include "TVector3.h"

/* The status bits for hits in the hitlists */
// I keep these definitions from the original code from Howard
// they were much more, but it was defined in the rtpc.h code
// 

#define HISUSED 1  /* - (was 1) used */

#define HITUNAV 1 /* reasons to NOT use a hit in the chain-linker */


using namespace std;


/*THIS IS THE EXECUTABLE VERSION OF CHAIN FINDER*/

void chain_finder()
{


  //GLOBAL VARIABLES HERE
  // Bool_t VERBOSE = 0;
  
   Double_t rad2deg = 180/(4*atan(1));


    
  //- ********************************
  // DEFINE VARIABLES FROM ROOT FILE
   
   //   TFile *infile = new TFile("./files/1000SuperEvents.root");
   //TFile *infile = new TFile("./files/MirrorEvents.root");
   //   TFile *infile = new TFile("files/10T-10_200ns_SE-26.root");
   TFile *infile = new TFile("files/nt_P100T90_SE.root");
   
      //TFile *infile=new TFile("./files/nt_Step1mm_SE.root");
  
   RTPCTree =(TTree*)infile->Get("eventtree");
   
   Int_t Entries = RTPCTree->GetEntries();
   cout<<"Entries: "<<Entries<<endl;
   
   RTPCTree ->SetBranchAddress("event", &event);
   RTPCTree ->SetBranchAddress("X", &X);
   RTPCTree ->SetBranchAddress("Y", &Y);
   RTPCTree ->SetBranchAddress("Z", &Z);
   RTPCTree ->SetBranchAddress("Hit", &Hit);
   
   //- ********************************
   

   /*
   // VARIABLES SET FOR 2nd PASS
   TFile *infile=new TFile("ChainEvents_1.root");
  
   RTPCTree =(TTree*)infile->Get("chaintree");
   
   Int_t Entries = RTPCTree->GetEntries();
   cout<<"Entries: "<<Entries<<endl;
   
   RTPCTree ->SetBranchAddress("event", &event);
   RTPCTree ->SetBranchAddress("X", &X);
   RTPCTree ->SetBranchAddress("Y", &Y);
   RTPCTree ->SetBranchAddress("Z", &Z);
   RTPCTree ->SetBranchAddress("Hit", &Hit);
   */

   /*   
   // TEMPORARY SET OF VARIABLES
   TFile *infile=new TFile("./files/nt_Step1mm_SuperEvent.root");
  //   TFile *infile=new TFile("./files/nt_P100T90_SuperEvent.root");
   RTPCTree=(TTree*)infile->Get("ep");
   
   Int_t Entries = RTPCTree->GetEntries();
   cout<<"Entries: "<<Entries<<endl;
   
   RTPCTree ->SetBranchAddress("Index", &event);//number of event
   RTPCTree ->SetBranchAddress("StepX_rec_m", &X);
   RTPCTree ->SetBranchAddress("StepY_rec_m", &Y);
   RTPCTree ->SetBranchAddress("StepZ_rec_m", &Z);
   RTPCTree ->SetBranchAddress("HitNum_m", &Hit);//number of simul hits in every event
   
   */
   
   
   //***********************************
   // DEFINE VARIABLES TO SAVE ROOT FILE
   
   TFile *rootoutfile = new TFile("ChainEvents_2.root", "recreate");
   
   TTree *chaintree = new TTree("chaintree","SE");
   
   chaintree->Branch("event", &ChainEv.ID, "ID/I");  // Create a branch called a, linked to local variable x, of type D (double)
   chaintree->Branch("X", &ChainEv.X_rec, "X_rec[500]/D");
   chaintree->Branch("Y", &ChainEv.Y_rec, "Y_rec[500]/D");
   chaintree->Branch("Z", &ChainEv.Z_rec, "Z_rec[500]/D");
   chaintree->Branch("Hit", &ChainEv.Hit, "Hit/I");
  //***********************************

  
  TH1F *hAngles = new TH1F("hAngles","angles", 181, -0.5,180.5);
  //  TH1F *hseparation = new TH1F("hseparation","separation<11", 120, -0.5, 11.5);
  TH1F *hseparation = new TH1F("hseparation","separation<11", 110, -0.5, 11.5);
  //  TH2F *hAngles_sep = new TH2F("hAngles_sep","angles-sep",  362  ,-0.25,180.25, 50, -0.5, 11.5);
  TH2F *hAngles_sep = new TH2F("hAngles_sep","angles-sep",  362  ,-0.25,180.25, 110, -0.5, 11.5);

  TVector3 nil_vec(0,0,0);//null vector

  
  //LOOP TO READ THE VARIABLES FROM THE ROOT FILE

  //here we are reading the whole file, which has many superevents
  //perhaps we want to read one event at the time, for test purposes.

  //In general, this is a track/chain finder, so this code only FILTER
  // and its output should be individuals chains to be analyzed later
  // with the Kalman Filter or something else.

  //TERMS:
  //anchor: the hit to start the search
  //status: the flag to indicate when a hit is being removed from the hit pool
  //seed_hit: the hit used to search in the sphere with r<11mm near hits. Initially is the same as anchor
  
  
  anchor_hit = 0;
  num_chains = 0;//Chain index (How many chains there are)
  num_hits_this_chain[num_chains] = 0;//Number of hits in the chain (was num_hits_this_chain[0])

  Int_t temp_eve = -1;
  Int_t real_eve_counter = 0;

  Int_t double_counter = 0;
  
  // Int_t max_entries = 1;
     
  Int_t max_entries = Entries;
   
  for(Int_t ii = 0; ii < max_entries; ii++)  //uncomment for the whole file.
    //    for(Int_t ii = 8; ii < max_entries; ii++) 
    {
      //  Int_t ii = 180;
      readout(ii, Entries);//Read the event and store in the struct variables
      
            
      for (anchor_hit = 0; anchor_hit < maxin; anchor_hit++) //index of anchor hit
	{
	  
	  if( (hitevent[anchor_hit].Status & HITUNAV) || hitevent[anchor_hit].Ev_pos == 2)
	    //If Status=1 (used), do not do anything. With the condition Ev_pos, we stop the search of hits on the second event.
	    //We just connect the first with events related on the second.
	    //This is to avoid anchor hits on the second event, which will be processed in the next loop.
	    //I set it first as AND but should be an OR.

	    {
	      //
	    }
	  
	  else
	    {
	      acceptance = 0;
	      num_hits_this_chain[num_chains] = 1;//THERE IS ALWAYS ONE HIT IN A CHAIN
	      
	      chain_hits[num_chains][0] = anchor_hit;// **(A)**
	      
	      hitevent[anchor_hit].Status |= HISUSED;// |= --> or eq;  a = a | b. It just assign the USED status to the hit
	      
	      for (seed_hit = 0; seed_hit < num_hits_this_chain[num_chains]; seed_hit++)
		{
		  
		  seed_index = chain_hits[num_chains][seed_hit];// **(A)** (it is related to the anchor hit)
	
		  TVector3 pseed(hitevent[seed_index].X, hitevent[seed_index].Y, hitevent[seed_index].Z);

		  for(next_hit = 0; next_hit<(aHit[0]+aHit[1]); next_hit++)
		    //the max number of hits is the sum of two events hits. The search is extended to
		    // the events in the second events. Note that the anchors are in a different condition.
		    //NOTE: we have to take care about the variables, where are defined, how are handled
		    
		    {
		      	      
		      
		      if( !(hitevent[next_hit].Status & HITUNAV) )
			{
			  TVector3 pnext(hitevent[next_hit].X, hitevent[next_hit].Y, hitevent[next_hit].Z);
			  
			  if (next_hit>0 && (pnext!=nil_vec))
			    {
			      TVector3 pnext_pre(hitevent[next_hit-1].X, hitevent[next_hit-1].Y, hitevent[next_hit-1].Z);
			      if (!(pnext-pnext_pre).Mag() == 0) // definitely removes the same hits (perhaps redundant)
				{
				  TVector3 dif_vec(pnext - pseed);
				  
				  separation = (dif_vec).Mag();//REMEMBER!! They can be treated as vectors for the calculus we use

				  hseparation->Fill(separation);//Histogram of separation between hits
				  
				  if(separation <= MAX_LINK_SEP && separation > 0.0)

				    //by definition, separation is always>0 but with this condition,
				    //we remove 0's (i.e. repeated hits) IT MUST BE SOLVED WITH THE PREVIOUS CONDITION
				    {
			
				      // SOME FUNCTION SHOULD BE ADDED HERE TO TEMPORARYLY STORE THE
				      // NEAR POINTS AND THEN SELECT THE CLOSEST ONE. (Gail suggestion)
				      // PERHAPS IT COULD HELP THE SELECTION IN CROSSING TRACKS
				      
				      index_hits = num_hits_this_chain[num_chains];//Just rename the variable to handle better

				      //----------------------------------------->
				      
				      //gra_vec[0] is always 0,0,0
				      gra_vec[index_hits].SetX(dif_vec.X());
				      gra_vec[index_hits].SetY(dif_vec.Y());
				      gra_vec[index_hits].SetZ(dif_vec.Z());

				      if(index_hits>1)
					{
					  acceptance = dif_vec.Angle(gra_vec[index_hits-1]) *rad2deg;
					
					}//if(index_hits>1...	  
				      
				      if(acceptance>90.) acceptance = 180. - acceptance;
				      
				      if(acceptance>0.)
					{
					  hAngles->Fill(acceptance);
					  
					  hAngles_sep->Fill(acceptance, separation);
					}
				      
				      //HERE SHOULD BE ADD THE FUNCTION TO CROSS CHECK
				      //IF THE HIT BELONGS TO A TRACK OR NOT.
				      //In this case, the hit is release or not, thus add to the chain. 
				      
				      //The hit is included in first instance, BUT should be checked
				      //if it is suitable for the track
				      
				      /*   
					   if(separation >= 4 && acceptance < 45)
					   //				      if(separation >4 && acceptance< 3)
					   {
					   accept_hit(next_hit);
					   //if (ii == 351) cout<<"num_hits!!"<<  num_hits_this_chain[num_chains]<<endl;
					   }
				      */
				      
					 
				      //Those number are TOTALLY ad-hoc.
				      //A better analysis shopuld be applied to determine the values
				      //But definetly shouyld be a distintion between close and far hits
				      
				      if (separation < 4 && acceptance <39)
					//	    if(acceptance <  40)
					{
					  accept_hit(next_hit);
					}
				      
				      if (separation > 4 && acceptance <33.3)
					{					
					  accept_hit(next_hit);
					}
				      
				    }// if(separation<=
				
				  else
				    {}
				  
				}//if(pnext
			    }//if(next_hit>0			  
			}// if( !(hitevent
		      
		    }// for(next_hit
		  
		}//  for (seed_hit
	      
	      if( num_hits_this_chain[num_chains] >= 6) 
		{
		  if(1)
		    {

		      printf("....KEEPING THIS CHAIN #%d. %d hits \n", num_chains,num_hits_this_chain[num_chains]);


		      //These lines are only useful for test purposes
		      
		      if(ii!=temp_eve)
			{
			  real_eve_counter++;
			  		  cout<<"EVENT: "<<ii<<endl;
			}
		      else
			{
			  cout<<"DOUBLE!!! <<<<<<<<------------------------------------------"<< ii<<endl;
			  double_counter++;
			}
		      temp_eve = ii;
		      
		      
		      for (Int_t jj=0; jj<num_hits_this_chain[num_chains]; jj++)
			{
			  printf(" %d", chain_hits[num_chains][jj]);
			  
			  //Fill the variables value of the found chain
			  ChainEv.X_rec[jj] = hitevent[chain_hits[num_chains][jj]].X;
			  ChainEv.Y_rec[jj] = hitevent[chain_hits[num_chains][jj]].Y;
			  ChainEv.Z_rec[jj] = hitevent[chain_hits[num_chains][jj]].Z;
			 			  
			}
		      
		      for(Int_t kk=num_hits_this_chain[num_chains]; kk<500;kk++)	//Clean up loop
			{
			  ChainEv.X_rec[kk]=ChainEv.Y_rec[kk]=ChainEv.Z_rec[kk]=0;
			}
		      
		      
		      ChainEv.Hit = num_hits_this_chain[num_chains];//number of hits in the chain
		      ChainEv.ID = num_chains;//Event Index. A Chain per index
		      
		      chaintree->Fill();
		      
		      printf("\n\n");
		      
		    }
		  
		  num_chains++; //NEW CHAIN, INCREASE THE INDEX
		  /* save chain if >1 hit on it */
		}
	      else
		{
		  hitevent[next_hit].Status &= ~HISUSED;
		}
	      
	      
	      
	      
	    }//else (from if( hitevent[anchor_hit].Status & HITUNAV)
	  
	}//for(anchor_hit...
      
      
      
    }

     cout<<"Total Events: "<<real_eve_counter<<endl;
     cout<<"double counts (split tracks): " <<double_counter<<endl;
     
     hAngles_sep->Write();
     hAngles->Write();
     hseparation->Write();
     chaintree->Write();
     rootoutfile->Close();
     infile->Close();

}


void readout(Int_t ii, Int_t Entries)
{
  
  //This function is designed to readout two events and store
  //its contents in a expanded variable array with hit_1+hit_2 elements.
  //The second event is marked with a flag 'Ev_pos'
  //when its switch to the first position, first is checked
  //if the hit was used before. The first hit in the new iteration,
  //corresponds to the hit_1 position (from 0) in the previous iteration
  //and this is the hit we want to consider if was used or not.
  //Then it is filled as normal.

  maxin = 0;

   //Each event contents a different number of hits. If the next event has
  // less hits than the previous one, the higher indexes of the array
  // will keep the previous values. Only applies to the space coordinates
  hitevent[ndim].X = {};
  hitevent[ndim].Y = {};
  hitevent[ndim].Z = {}; 
  
  RTPCTree->GetEntry(ii);
  //  nb += RTPCTree->GetEntry(ii);     
  //      RTPCTree->Show(ii);
 

  
  aHit[0]=Hit;
  aHit[1]=0;

  //FILL THE FIRST EVENT
  
  for(Int_t p = 0; p<aHit[0]; p++)
    {
      if (ii>=0)//TO BE REMOVED?
	
	{
	  //	  cout<<"p+aHit[2]: "<<p+aHit[2]<<endl;
	  if (hitevent[p+aHit[2]].Status == 0)//This condition, check if the event read joint previously has marked hits
	    {
	      hitevent[p].X = X[p];
	      hitevent[p].Y = Y[p];
	      hitevent[p].Z = Z[p];
	      
	      hitevent[p].Status = 0;
	      hitevent[p].Ev_pos = 1;
	      maxin++;
	    }
	  else
	    {
	      hitevent[p].Status = 1;
	      hitevent[p].Ev_pos = 1;
	      maxin++;
	      }
	}
      else
	
	{
	  hitevent[p].X = X[p];
	  hitevent[p].Y = Y[p];
	  hitevent[p].Z = Z[p];
	  hitevent[p].Status = 0;
	  hitevent[p].Ev_pos = 1;
	  maxin++;
	}
    }

  aHit[2]=aHit[0];//temporary variable, to store the number of hits of the first event in order to use in the next cycle
  
  //CHECK THIS CONDITION!! I THINK SHOULD TAKE CARE ABOUT OUT OF BOUNDS 
  // right now is just to avoid the second event. Remove once the mirror test has finished
  if(ii<Entries-1)
    {
      RTPCTree->GetEntry(ii+1);
      aHit[1]=Hit;
      
      //FILL THE SECOND EVENT
      //    if(ii<max_entries)
      {
	for(Int_t p = aHit[0]; p<(aHit[0]+aHit[1]); p++)
	  {
	    hitevent[p].X = X[p-aHit[0]];
	    hitevent[p].Y = Y[p-aHit[0]];
	    hitevent[p].Z = Z[p-aHit[0]];
	    hitevent[p].Status = 0;
	    hitevent[p].Ev_pos = 2;
	    maxin++;
	  }
      }
      
    }
  
  //This outoput line is an easy cross check
  //The first number (maxin) is the total number of hits in with the two events
  //aHit[0] and aHit[1] give the number of hits in each event
  //When printed on screen, the number of hits in the chain should be equal or smaller than aHit[0]
  //BUT only when testing files and every event represents ONLY one chain.
  //In case the number of hits in the candidate chain is larger than aHit[0], hits from the second event are being taken
  //into the first chain.
  cout<<"*******IN********:"<<maxin<<"     EVENT 1: "<< ii<<" | EVENT 2: "<<ii+1<<"    aHit[0]: "<<aHit[0]<<" |  aHit[1]: "<<aHit[1]<<endl;
  //       RTPCTree->Show(i);
  
}


void accept_hit(Int_t next_hit)
{
  	  
  if (num_hits_this_chain[num_chains] >= MAX_HITS_ON_CHAIN)
    {        
      printf("Too many hits for the chain list. Aborting.\n"); 
      return;//It was -1 but is a void function
    }
    
  //THE HIT INDEX WHICH ACOMPLISH THE CONDITION, IS STORED-->
  chain_hits[num_chains][num_hits_this_chain[num_chains]] = next_hit;
  
   
  /* mark it as used */
  hitevent[next_hit].Status |= HISUSED; //this kind of assignment is for 'historical' reasons
  num_hits_this_chain[num_chains]++;//ADD HIT TO THE CHAIN, INCREASE INDEX

  //Its final number is the total number of hits in the chain
  
}





int main(int argc, char** argv)
{
  
  TStopwatch timer;
  timer.Start();
  
  if(argc < 1) return 1;
  //    tree(argv[1]);
  //  for(Int_t k=0;k<1000;k++)
  {
      chain_finder();
      //    cout<<"cycle: "<<k<<endl;
    }
    
    timer.Stop();
  
  Double_t rtime = timer.RealTime();
  Double_t ctime = timer.CpuTime();
  printf("RealTime=%f seconds, CpuTime=%f seconds\n",rtime,ctime);
  
  return 0;
}
