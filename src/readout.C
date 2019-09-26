#include "readout.h"
#include <iostream>

Int_t  NoH;
HitStruct hitevent[ndim];
using namespace std;
//HitStruct hitevent[ndim];

void readout(HitStruct hitevent[], Int_t ii)
{
  
  //This function is designed to readout two events and store
  //its contents in a expanded variable array with hit_1+hit_2 elements.
  //The second event is marked with a flag 'Ev_pos'
  //when its switch to the first position, first is checked
  //if the hit was used before. The first hit in the new iteration,
  //corresponds to the hit_1 position (from 0) in the previous iteration
  //and this is the hit we want to consider if was used or not.
  //Then it is filled as normal.
   
 Int_t  maxin = 0;
  
    
  //Each event contents a different number of hits. If the next event has
  // less hits than the previous one, the higher indexes of the array
  // will keep the previous values, thus, they are available for the search. 
  // Only applies to the space coordinates
  for (Int_t tt=0;tt<ndim;tt++)
    {
      hitevent[tt].X = 0;
      hitevent[tt].Y = 0;
      hitevent[tt].Z = 0;
    }

  RTPCTree->GetEntry(ii);
  
  NoH=Hit;//Number of Hits (NoH).From the event

 cout<<"NoH: "<<NoH<<endl;
  for(Int_t p = 0; p<NoH; p++)
    {
      //	  if (ii==0&&pass==1) cout<<"HELLO!! "<<hitevent[p].Status<<endl;
      //	  cout<<"p+aHit[2]: "<<p+aHit[2]<<endl;
      
      hitevent[p].X = X[p];
      hitevent[p].Y = Y[p];
      hitevent[p].Z = Z[p];
      hitevent[p].Status = 0;
      maxin++;// Maybe is useless now, since we read one event at the time.
    	  
    }
  
    
  //This outoput line is an easy cross check
  //The first number (maxin) is the total number of hits in with the two events
  //NoH and aHit[1] give the number of hits in each event
  //When printed on screen, the number of hits in the chain should be equal or smaller than NoH
  //BUT only when testing files and every event represents ONLY one chain.
  //In case the number of hits in the candidate chain is larger than NoH, hits from the second event are being taken
  //into the first chain.
  //  cout<<"*******IN******** Number of hits: "<<maxin<<"     EVENT: "<< ii<<"    NoH: "<<NoH<<endl;
  //       RTPCTree->Show(i);
  
  
  
}

