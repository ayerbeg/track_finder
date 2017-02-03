#include "read_root.h"

TString inputfile;
TString outputfile;


TTree   *RTPCTree;
TTree   *chaintree;

ChainStruct ChainArray[MAX_NUM_CHAINS];

Int_t    event;
Double_t X[ndim], Y[ndim], Z[ndim];
Int_t    Hit;

Int_t Entries;
TFile *rootoutfile;

using namespace std;

void read_root()
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
  
  chaintree->Branch("event", &ChainArray[100].ID, "ID/I");  // Create a branch called a, linked to local variable x, of type D (double)
  chaintree->Branch("X", &ChainArray[100].X_rec, "X_rec[500]/D");
  chaintree->Branch("Y", &ChainArray[100].Y_rec, "Y_rec[500]/D");
  chaintree->Branch("Z", &ChainArray[100].Z_rec, "Z_rec[500]/D");
  chaintree->Branch("Hit", &ChainArray[100].Hit, "Hit/I");
  //***********************************
}
