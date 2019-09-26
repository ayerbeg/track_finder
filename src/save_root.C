#include "save_root.h"
#include "read_root.h"

using namespace std;

void save_root()
{

  cout<<"Total Events: "<<real_eve_counter<<endl;//The number of first time entering into the search
  cout<<"double counts (split tracks): " <<double_counter<<endl;//Although is called double, could be more times analyzing the same event, i.e. split tracks
  cout<<"more than double counts (split tracks): " <<mcts<<endl;//how many tracks are splited in more than two
  
  chaintree->Write();
  
  delete chaintree;
  rootoutfile ->Close();
  
  cout<<"RETURN"<<endl;
  

}
