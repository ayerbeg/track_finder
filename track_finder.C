#include <iostream>
#include "string.h"
#include "string"


#include "math.h"
#include "TStyle.h"
#include "TStopwatch.h"
#include <TBenchmark.h>

#include <fstream>
#include <sstream>

#include "Rtypes.h"

#include "DataStruct.h"
#include "Data_handler.h"
#include "read_root.h"
#include "save_root.h"
#include "search.h"
#include "variable.h"
#include "readout.h"


using namespace std;

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
    read_root();

    cout<<"Entries: "<<Entries<<endl;
    
    for(Int_t ii = 0; ii < Entries; ii++)  //uncomment for the whole file.
      {

	readout(hitevent, ii);

	search(hitevent, NoH, ii);
    //    cout<<"cycle: "<<k<<endl;
      }
    save_root();
  }
  timer.Stop();
  
  Double_t rtime = timer.RealTime();
  Double_t ctime = timer.CpuTime();
  printf("RealTime=%f seconds, CpuTime=%f seconds\n",rtime,ctime);
  
  return 0;
}
