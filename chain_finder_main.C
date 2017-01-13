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

void chain_finder()
{
  rad2deg = 180/(4*atan(1));

  //- ********************************
  // DEFINE VARIABLES FROM ROOT FILE
    
  TString inputf = inputfile;
  cout<<"INPUT FILE: " <<inputf<<endl;
  
  
  TFile *infile = new TFile(inputf);
  RTPCTree =(TTree*)infile->Get("eventtree");
  
  Entries = RTPCTree->GetEntries();
  cout<<"Entries: "<<Entries<<endl;

  RTPCTree-> Print();//Temporary, delete!!
  
  RTPCTree ->SetBranchAddress("event", &event);
  RTPCTree ->SetBranchAddress("X", &X);
  RTPCTree ->SetBranchAddress("Y", &Y);
  RTPCTree ->SetBranchAddress("Z", &Z);
  RTPCTree ->SetBranchAddress("Hit", &Hit);
  
  
  
  //***********************************
  // DEFINE VARIABLES TO SAVE ROOT FILE
  
  TString outputfile  = "ChainEvents_1.root";
  rootoutfile  = new TFile(outputfile , "recreate");
  
  //   TString outf  = outputfile;
  
  cout<<"OUTPUT TMP FILE: " <<outputfile <<endl;
  
  //  TFile *rootoutfile = new TFile(outf, "recreate");
  
  chaintree = new TTree("chaintree","SE");
  
  chaintree->Branch("event", &ChainEv.ID, "ID/I");  // Create a branch called a, linked to local variable x, of type D (double)
  chaintree->Branch("X", &ChainEv.X_rec, "X_rec[500]/D");
  chaintree->Branch("Y", &ChainEv.Y_rec, "Y_rec[500]/D");
  chaintree->Branch("Z", &ChainEv.Z_rec, "Z_rec[500]/D");
  chaintree->Branch("Hit", &ChainEv.Hit, "Hit/I");
  //***********************************
  
  
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
  
  temp_eve = -1;
  real_eve_counter = 0;
  
  double_counter = 0;
  
  // Int_t max_entries = 1;
  max_entries = Entries;
  
    
  for(Int_t ii = 0; ii < max_entries; ii++)  //uncomment for the whole file.
    {
      readout(hitevent, ii);//Read the event and store in the struct variables
 
      //The readout function works because we defined global variables. Hitevent as variable
      //is sent to readout, but there is calles readevent and modified. Thus, hitevent is modified too.
      //I'm not sure this is what we want to do in order to separete the search engine. 
      
      
      //FILL PARAMETERS
      search(hitevent, maxin, ii);
	     
	        
    }
  
  cout<<"Total Events: "<<real_eve_counter<<endl;//The number of first time entering into the search
  cout<<"double counts (split tracks): " <<double_counter<<endl;//Although is called double, could be more times analyzing the same event, i.e. split tracks
  cout<<"more than double counts (split tracks): " <<mcts<<endl;//how many tracks are splited in more than two
  
  chaintree->Write();
  
  delete chaintree;
  rootoutfile ->Close();
  
  cout<<"RETURN"<<endl;
    
}


void search(HitStruct hitevent[], Int_t max_hits, Int_t event_ii )
{
  Max_Link_Sep = space;
  Max_Ang      = max_ang;
  Min_Ang      = min_ang;
  Ang_Sep      = ang_sep;
  
  TVector3 nil_vec(0,0,0);//null vector

  for (anchor_hit = 0; anchor_hit < max_hits; anchor_hit++) //index of anchor hit
    {
      if (event_ii==0)	  cout<<hitevent[anchor_hit].Status<<" "<<anchor_hit<<endl;
	  
      if ( hitevent[anchor_hit].Status & HITUNAV )
	//If Status=1 (used), do not do anything. 
	{
	  //
	}
	 
      else
	{
	  acceptance = 0;
	  num_hits_this_chain[num_chains] = 1;//THERE IS ALWAYS ONE HIT IN A CHAIN
	      
	  chain_hits[num_chains][0] = anchor_hit;// **(A)**
	      
	  hitevent[anchor_hit].Status |= HISUSED;// |= --> or eq;  a = a | b. It just assign the USED status to the hit

	  if (event_ii==0) cout<< hitevent[anchor_hit].Status<<endl;

	  //SEARCH ALGORITHM----->
	      
	  for (seed_hit = 0; seed_hit < num_hits_this_chain[num_chains]; seed_hit++)
	    {
	      if (event_ii==0) cout<<" HERE 1: "<<hitevent[seed_hit].X<<" "<< hitevent[seed_hit].Y<<" "<< hitevent[seed_hit].Z<<" seed_hit: "<<seed_hit<<endl;
		  
	      seed_index = chain_hits[num_chains][seed_hit];// **(A)** (it is related to the anchor hit)
	
	      TVector3 pseed(hitevent[seed_index].X, hitevent[seed_index].Y, hitevent[seed_index].Z);

	      for(next_hit = 0; next_hit<max_hits; next_hit++)
		//max_hits: number of hits inside the event
		//NOTE: we have to take care about the variables, where are defined, how are handled

		{
		  //	  if (event_ii==0) cout<<" HERE 1: "<<hitevent[next_hit].X<<" "<< hitevent[next_hit].Y<<" "<< hitevent[next_hit].Z<<" next_hit: "<<next_hit<<endl;		      	      
		      
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
				  
			      //  we differenciate the separation in the first to the second
			      if(
				 ( separation <= Max_Link_Sep && separation > 0.0 )
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
				       //if (event_ii == 351) cout<<"num_hits!!"<<  num_hits_this_chain[num_chains]<<endl;
				       }
				  */
				      
				      
				  //Those number are TOTALLY ad-hoc.
				  //A better analysis shopuld be applied to determine the values
				  //But definetly should be a distintion between close and far hits
				      
				  //closer hits, larger angles
				  //far hits, small angles
				      
				  //HERE ARE THE CONDITIONS TO ACCEPT A HIT
				      
				  // if (separation <=2)
				  // 	{
				  // 	  accept_hit(next_hit);
				  // 	}
				  // else
				  // 	{
				  // 	  if(k == 0)
				  // 	    {
				  // 	      if (separation <= Ang_Sep && acceptance < Min_Ang)
				  // 		{
				  // 		  accept_hit(next_hit);
				  // 		}
				      
				  // 	      if (separation > Ang_Sep && acceptance < Max_Ang)
				  // 		//	if (separation > 4 && acceptance <33.3)
				  // 		{					
				  // 		  accept_hit(next_hit);
				  // 		}
				  // 	    }
				      
				  if (separation <= Ang_Sep && acceptance < Max_Ang-15)
				    {
				      accept_hit(next_hit);
				    }
				      
				  if (separation > Ang_Sep && acceptance < Min_Ang-10)
				    //	if (separation > 4 && acceptance <33.3)
				    {					
				      accept_hit(next_hit);
				    }
				      
				  //					}
				      
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

	      store_data(event_ii);// sending the index of event (ii) is just to control how many times the event was studied, i.e. the split tracks. it can be rid out. 
	
	    }
	  else
	    {
	      hitevent[next_hit].Status &= ~HISUSED;
	    }
	      
	}//else (from if( hitevent[anchor_hit].Status & HITUNAV)
	  
    }//for(anchor_hit...
      
  
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
      mcts = 0;
    }
  else
    {
      cout<<"DOUBLE!!! <<<<<<<<------------------------------------------"<< ii<<endl;
      double_counter++;
      
      if(mcts==1)mcts++;
      if(mcts==0)mcts = 1;
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
  
  
  for(Int_t kk=num_hits_this_chain[num_chains]; kk<500;kk++)	//Clean up loop, over the last value registered.
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
  TO BE USED, KEEP IN QUARENTINE

  //left and right, first index and last index

  void quickSort(int arr[], int left, int right) {

  // ****************************************************** //
  // http://www.algolist.net/Algorithms/Sorting/Quicksort //
  // ****************************************************** //

  
  int i = left, j = right;
  int tmp;
  int pivot = arr[(left + right) / 2];
   
  // partition 
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
  }; //TAKE CARE OF THIS SEMICOLON, MAYBE A MISTAKE??
   
  // * recursion * /
  if (left < j)
  quickSort(arr, left, j);
  if (i < right)
  quickSort(arr, i, right);
  }
*/


void readout(HitStruct readevent[], Int_t ii)
{
  maxin = 0;//This should be the number of hits inside the event. 
            //Since the readout is simplified, it could be useless.
  
    
  //Each event contents a different number of hits. If the next event has
  // less hits than the previous one, the higher indexes of the array
  // will keep the previous values, thus, they are available for the search. 
  // Only applies to the space coordinates
  for (Int_t tt=0;tt<ndim;tt++)
    {
      readevent[tt].X = 0;
      readevent[tt].Y = 0;
      readevent[tt].Z = 0;
    }

  RTPCTree->GetEntry(ii);
  
  NoH=Hit;//Number of Hits (NoH).From the event
    
  for(Int_t p = 0; p<NoH; p++)
    {
      //	  if (ii==0&&pass==1) cout<<"HELLO!! "<<readevent[p].Status<<endl;
      //	  cout<<"p+aHit[2]: "<<p+aHit[2]<<endl;
      
      readevent[p].X = X[p];
      readevent[p].Y = Y[p];
      readevent[p].Z = Z[p];
      readevent[p].Status = 0;
      maxin++;// Maybe is useless now, since we read one event at the time.
    	  
    }
  
    
  //This outoput line is an easy cross check
  //The first number (maxin) is the total number of hits in with the two events
  //NoH and aHit[1] give the number of hits in each event
  //When printed on screen, the number of hits in the chain should be equal or smaller than NoH
  //BUT only when testing files and every event represents ONLY one chain.
  //In case the number of hits in the candidate chain is larger than NoH, hits from the second event are being taken
  //into the first chain.
  cout<<"*******IN******** Number of hits: "<<maxin<<"     EVENT: "<< ii<<"    NoH: "<<NoH<<endl;
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
    chain_finder();
    //    cout<<"cycle: "<<k<<endl;
  }
    
  timer.Stop();
  
  Double_t rtime = timer.RealTime();
  Double_t ctime = timer.CpuTime();
  printf("RealTime=%f seconds, CpuTime=%f seconds\n",rtime,ctime);
  
  return 0;
}
