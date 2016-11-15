#include <iostream>
#include "string.h"
#include "string"
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

#include <fstream>
#include <sstream>

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

void chain_finder(Int_t k)
{


  //TEMPORARY
  if(k == 1)
    {
      cout<<" SECOND PASS!!"<<endl;
      //     return;
    }

  //GLOBAL VARIABLES HERE
  // Bool_t VERBOSE = 0;
  
   Double_t rad2deg = 180/(4*atan(1));


  if(k==0)
     {   
       //- ********************************
       // DEFINE VARIABLES FROM ROOT FILE
       
       //   TFile *infile = new TFile("./files/1000SuperEvents.root");
       //   TFile *infile = new TFile("./files/MirrorEvents.root");
       //   TFile *infile = new TFile("files/10T-10_200ns_SE-26.root");
       //   TFile *infile = new TFile("files/nt_P100T90_SE.root");
       
       TString inputf = inputfile;
       cout<<"INPUT FILE: " <<inputf<<endl;
       
       
       TFile *infile = new TFile(inputf);
       RTPCTree =(TTree*)infile->Get("eventtree");
       
       Entries = RTPCTree->GetEntries();
       cout<<"Entries: "<<Entries<<endl;
       
       RTPCTree ->SetBranchAddress("event", &event);
       RTPCTree ->SetBranchAddress("X", &X);
       RTPCTree ->SetBranchAddress("Y", &Y);
       RTPCTree ->SetBranchAddress("Z", &Z);
       RTPCTree ->SetBranchAddress("Hit", &Hit);
       
       
       
       //***********************************
       // DEFINE VARIABLES TO SAVE ROOT FILE
       
       TString outputfile_tmp = "ChainEvents_1.root";
       rootoutfile_tmp = new TFile(outputfile_tmp, "recreate");
       
       //   TString outf_tmp = outputfile;
       
       cout<<"OUTPUT TMP FILE: " <<outputfile_tmp<<endl;
       
       //  TFile *rootoutfile = new TFile(outf, "recreate");
       
       chaintree = new TTree("chaintree","SE");
       
       chaintree->Branch("event", &ChainEv.ID, "ID/I");  // Create a branch called a, linked to local variable x, of type D (double)
       chaintree->Branch("X", &ChainEv.X_rec, "X_rec[500]/D");
       chaintree->Branch("Y", &ChainEv.Y_rec, "Y_rec[500]/D");
       chaintree->Branch("Z", &ChainEv.Z_rec, "Z_rec[500]/D");
       chaintree->Branch("Hit", &ChainEv.Hit, "Hit/I");
       //***********************************


     }
  
  //- ********************************
  // VARIABLES SET FOR 2nd PASS
   if(k==1)
     {
       TFile *infile_2=new TFile("ChainEvents_1.root");
       
       RTPCTree_2 =(TTree*)infile_2->Get("chaintree");
       
       Entries_2 = RTPCTree_2->GetEntries();
       cout<<"Entries_2: "<<Entries_2<<endl;
       
       RTPCTree_2 -> SetBranchAddress("event", &event);
       RTPCTree_2 -> SetBranchAddress("X", &X);
       RTPCTree_2 -> SetBranchAddress("Y", &Y);
       RTPCTree_2 -> SetBranchAddress("Z", &Z);
       RTPCTree_2 -> SetBranchAddress("Hit", &Hit);

       //***********************************
       // DEFINE VARIABLES TO SAVE ROOT FILE
       
       //   TFile *rootoutfile = new TFile("ChainEvents_2.root", "recreate");
       
       TString outf = outputfile;
       
       cout<<"OUTPUT FILE: " <<outf<<endl;
       
       rootoutfile = new TFile(outf, "recreate");
       
       chaintree = new TTree("chaintree","SE");
       
       chaintree->Branch("event", &ChainEv.ID, "ID/I");  // Create a branch called a, linked to local variable x, of type D (double)
       chaintree->Branch("X", &ChainEv.X_rec, "X_rec[500]/D");
       chaintree->Branch("Y", &ChainEv.Y_rec, "Y_rec[500]/D");
       chaintree->Branch("Z", &ChainEv.Z_rec, "Z_rec[500]/D");
       chaintree->Branch("Hit", &ChainEv.Hit, "Hit/I");
       //***********************************




     }

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
   
  

   /*
  TH1F *hAngles = new TH1F("hAngles","angles", 181, -0.5,180.5);
  //  TH1F *hseparation = new TH1F("hseparation","separation<11", 120, -0.5, 11.5);
  TH1F *hseparation = new TH1F("hseparation","separation<11", 110, -0.5, 11.5);
  //  TH2F *hAngles_sep = new TH2F("hAngles_sep","angles-sep",  362  ,-0.25,180.25, 50, -0.5, 11.5);
  TH2F *hAngles_sep = new TH2F("hAngles_sep","angles-sep",  362  ,-0.25,180.25, 110, -0.5, 11.5);
   */
   
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


  Double_t Max_Link_Sep = space;
  Double_t Max_Ang = max_ang;
  Double_t Min_Ang = min_ang;
  Double_t Ang_Sep = ang_sep;

  
  
  
  anchor_hit = 0;
  num_chains = 0;//Chain index (How many chains there are)
  num_hits_this_chain[num_chains] = 0;//Number of hits in the chain (was num_hits_this_chain[0])

  temp_eve = -1;
  real_eve_counter = 0;

  double_counter = 0;
  
  // Int_t max_entries = 1;
  if(k==0)
    max_entries = Entries;
  
  if(k==1)
    max_entries = Entries_2;
  
  for(Int_t ii = 0; ii < max_entries; ii++)  //uncomment for the whole file.
    {
      
      readout(hitevent, ii, max_entries, k);//Read the event and store in the struct variables
      
      for (anchor_hit = 0; anchor_hit < maxin; anchor_hit++) //index of anchor hit
	{
	  if (ii==0)	  cout<<hitevent[anchor_hit].Status<<" "<<hitevent[anchor_hit].Ev_pos<<" "<<anchor_hit<<endl;
	  
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

	      if (ii==0&&k==1) cout<< hitevent[anchor_hit].Status<<endl;
	      //SEARCH ALGORITHM----->
	      
	      for (seed_hit = 0; seed_hit < num_hits_this_chain[num_chains]; seed_hit++)
		{
		  if (ii==0&&k==1) cout<<" HERE 1: "<<hitevent[seed_hit].X<<" "<< hitevent[seed_hit].Y<<" "<< hitevent[seed_hit].Z<<" seed_hit: "<<seed_hit<<endl;
		  
		  seed_index = chain_hits[num_chains][seed_hit];// **(A)** (it is related to the anchor hit)
	
		  TVector3 pseed(hitevent[seed_index].X, hitevent[seed_index].Y, hitevent[seed_index].Z);

		  for(next_hit = 0; next_hit<(aHit[0]+aHit[1]); next_hit++)
		    //the max number of hits is the sum of two events hits. The search is extended to
		    // the events in the second events. Note that the anchors are in a different condition.
		    //NOTE: we have to take care about the variables, where are defined, how are handled
		    
		    {
		      if (ii==0&&k==1) cout<<" HERE 1: "<<hitevent[next_hit].X<<" "<< hitevent[next_hit].Y<<" "<< hitevent[next_hit].Z<<" next_hit: "<<next_hit<<endl;		      	      
		      
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

				  //	  hseparation->Fill(separation);//Histogram of separation between hits
				  
				  // if(separation <= MAX_LINK_SEP && separation > 0.0)

				  if(
				     (k==0 && (separation <= Max_Link_Sep && separation > 0.0))
				     ||
				     (k==1 && (separation <= Max_Link_Sep+2 && separation > 0.0))
				     )

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


				      // FILLING HISTOGRAMS
				      // if(acceptance>0.)
				      // 	{
				      // 	  hAngles->Fill(acceptance);
					  
				      // 	  hAngles_sep->Fill(acceptance, separation);
				      // 	}
				      
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
				      
				      //    if (separation < 4 && acceptance <39)

				      if(k == 0)
					{
					  if (separation < Ang_Sep && acceptance < Max_Ang)
					    {
					      accept_hit(next_hit);
					    }
					  
					  if (separation > Ang_Sep && acceptance < Min_Ang)
					    //	if (separation > 4 && acceptance <33.3)
					    {					
					      accept_hit(next_hit);
					    }
					}

				      if(k == 1)
					{
					  if (separation < Ang_Sep && acceptance < Max_Ang-15)
					    {
					      accept_hit(next_hit);
					    }
					  
					  if (separation > Ang_Sep && acceptance < Min_Ang-10)
					    //	if (separation > 4 && acceptance <33.3)
					    {					
					      accept_hit(next_hit);
					    }
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

		  store_data(ii);
	
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


     // hAngles_sep->Write();
     // hAngles->Write();
     // hseparation->Write();

 
     chaintree->Write();

     if(k==0)
       {
	 delete chaintree;
	 rootoutfile_tmp->Close();
       }
     
     cout<<"RETURN"<<endl;

     if(k==1)
	rootoutfile->Close();
     //    infile->Close();

}

  



void store_data(Int_t ii)
{
  //I just move the algorithm to a function. SHOULD BE CLEANED. I think I don't need to send any argument as
  // is written now. 
  
	      
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
  
  //HERE SHOULD BE CALLED THE SORTING FUNCTION
  //Thus those variables ChainEv should be temporary. Perhapas just a temporary array
  //and then the store struct
  
  
  ChainEv.Hit = num_hits_this_chain[num_chains];//number of hits in the chain
  ChainEv.ID = num_chains;//Event Index. A Chain per index
  
  chaintree->Fill();
  
  printf("\n\n");


  num_chains++; //NEW CHAIN, INCREASE THE INDEX

  
  
}


  

/*
 TO BE USED KEEP IN QUARENTINE

//left and right, first index and last index

void quickSort(int arr[], int left, int right) {

  //****************************************************** //
  // http://www.algolist.net/Algorithms/Sorting/Quicksort //
  //****************************************************** //

  
   int i = left, j = right;
   int tmp;
   int pivot = arr[(left + right) / 2];
   
   //* partition * /
   while (i <= j)
     {
       while (arr[i] < pivot)
	 i++;
       while (arr[j] > pivot)
	 j--;
       if (i <= j)
	 {
	   tmp = arr[i];
	   arr[i] = arr[j];
	   arr[j] = tmp;
	   i++;
	   j--;
	 }
     }; //tAKE CARE OF THIS SEMICOLON, MAYBE A MISTAKE??
   
   //* recursion * /
   if (left < j)
     quickSort(arr, left, j);
   if (i < right)
     quickSort(arr, i, right);
 }
*/


void readout(HitStruct readevent[], Int_t ii, Int_t Entrie, Int_t pass)
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
  if (ii==0)
  {
    aHit[0] = aHit[1] = aHit[2]=0;
    if(pass==1 && nomore!=1)
      {
	for(Int_t init = 0; init<ndim; init++)   readevent[init].Status = 0;
	nomore =1;
      }
  }

   cout<<"aHit[2]: "<<aHit[2]<<endl;

  
   //Each event contents a different number of hits. If the next event has
  // less hits than the previous one, the higher indexes of the array
  // will keep the previous values. Only applies to the space coordinates
  readevent[ndim].X = {};
  readevent[ndim].Y = {};
  readevent[ndim].Z = {}; 

  if(pass==0)
    {
      RTPCTree->GetEntry(ii);
    }

  if(pass==1)
    {
      cout<<"second pass"<<endl;
      RTPCTree_2->GetEntry(ii);
    }
  //  nb += RTPCTree->GetEntry(ii);     
  //      RTPCTree->Show(ii);

  
  aHit[0]=Hit;
  aHit[1]=0;


  //FILL THE FIRST EVENT
  
  for(Int_t p = 0; p<aHit[0]; p++)
    {
      if (ii>=0)//TO BE REMOVED?
	{

	  if (ii==0&&pass==1) cout<<"HELLO!! "<<readevent[p].Status<<endl;

	  //	  cout<<"p+aHit[2]: "<<p+aHit[2]<<endl;
	  if (readevent[p+aHit[2]].Status == 0)//This condition, check if the event read joint previously has marked hits
	    {
	      readevent[p].X = X[p];
	      readevent[p].Y = Y[p];
	      readevent[p].Z = Z[p];
	      
	      readevent[p].Status = 0;
	      readevent[p].Ev_pos = 1;
	      maxin++;
	      if (ii==0&&pass==1) cout<<"HELLO 1!!"<<endl;

	    }
	  else
	    {
	      readevent[p].Status = 1;
	      readevent[p].Ev_pos = 1;
	      maxin++;
	      if (ii==0&&pass==1) cout<<"HELLO 2!!"<<endl;
	      }
	}
      else
	
	{
	  readevent[p].X = X[p];
	  readevent[p].Y = Y[p];
	  readevent[p].Z = Z[p];
	  readevent[p].Status = 0;
	  readevent[p].Ev_pos = 1;
	  maxin++;
	  if (ii==0&&pass==1) cout<<"HELLO 3!!"<<endl;
	}
    }

  aHit[2]=aHit[0];//temporary variable, to store the number of hits of the first event in order to use in the next cycle
  
  //CHECK THIS CONDITION!! I THINK SHOULD TAKE CARE ABOUT OUT OF BOUNDS 
  // right now is just to avoid the second event. Remove once the mirror test has finished
  if(ii<Entrie-1)
    {

      if(pass==0)
	{
	  RTPCTree->GetEntry(ii+1);
	}
      
      if(pass==1)
	{
	  cout<<"second pass 2"<<endl;
	  RTPCTree_2->GetEntry(ii+1);
	}
      
      //     RTPCTree->GetEntry(ii+1);
      aHit[1]=Hit;
      
      //FILL THE SECOND EVENT
      //    if(ii<max_entries)
      {
	for(Int_t p = aHit[0]; p<(aHit[0]+aHit[1]); p++)
	  {
	    readevent[p].X = X[p-aHit[0]];
	    readevent[p].Y = Y[p-aHit[0]];
	    readevent[p].Z = Z[p-aHit[0]];
	    readevent[p].Status = 0;
	    readevent[p].Ev_pos = 2;
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
  cout<<"*******IN******** (sum of two consecutive events):"<<maxin<<"     EVENT 1: "<< ii<<" | EVENT 2: "<<ii+1<<"    aHit[0]: "<<aHit[0]<<" |  aHit[1]: "<<aHit[1]<<" |  aHit[2]: "<<aHit[2]<<endl;
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







void variable(TString FileName)
{

    
  ifstream infile;
  string line;
  infile.open(FileName);
  if (infile)
    {
      while (!infile.eof())
	{
	  if (getline(infile,line))
	    {
	      	string buf;
		string wert;
		double doublewert;
		stringstream ss(line);
		//		cout << ">" << line << endl;
		ss >> buf;
		
		if (buf == "inputfile")	 
		  {			
		    ss >> inputfile;
		  }

		if (buf == "outputfile")	 
		  {			
		    ss >> outputfile;
		    if (outputfile == "") outputfile = "ChainCandidates.root"; //DEFAULT VALUE
		  }

		if (buf == "space")
		  {
		    ss >> doublewert;
		    space = doublewert;	
		  }
		
		if (buf == "max_ang")
		  {
		    ss >> doublewert;
		    max_ang = doublewert;	
		  }

		if (buf == "min_ang")
		  {
		    ss >> doublewert;
		    min_ang = doublewert;	
		  }

		if (buf == "ang_sep")
		  {
		    ss >> doublewert;
		    ang_sep = doublewert;	
		  }
	    }
	}
    }
}



int main(int argc, char** argv)
{
  
  TString dataname;
  dataname = "chain.ini";
  cout << "Compiled on: " __DATE__ " " __TIME__ "." << endl;

  cout<<"\n\nINI file: "<<dataname<<endl;
  variable(dataname);

  
  TStopwatch timer;
  timer.Start();
  
  if(argc < 1) return 1;

  //    tree(argv[1]);
  //  for(Int_t k=0;k<1000;k++)
  {
    for (Int_t k = 0; k<2; k++)
      chain_finder(k);
      //    cout<<"cycle: "<<k<<endl;
    }
    
    timer.Stop();
  
  Double_t rtime = timer.RealTime();
  Double_t ctime = timer.CpuTime();
  printf("RealTime=%f seconds, CpuTime=%f seconds\n",rtime,ctime);
  
  return 0;
}
