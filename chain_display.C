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
#include <TGraph2D.h>

void set_color_env(){   
  //******************************************************************
  //code to improve the color palette
  //from the root web page and color codes from:
  //http://ultrahigh.org/2007/08/20/making-pretty-root-color-palettes/
  //******************************************************************
  
  const Int_t NRGBs = 5;
  const Int_t NCont = 255;
  
  Double_t stops[NRGBs] = { 0.00, 0.34, 0.61, 0.84, 1.00 };
  Double_t red[NRGBs]   = { 0.00, 0.00, 0.87, 1.00, 0.51 };
  Double_t green[NRGBs] = { 0.00, 0.81, 1.00, 0.20, 0.00 };
  Double_t blue[NRGBs]  = { 0.51, 1.00, 0.12, 0.00, 0.00 };
  TColor::CreateGradientColorTable(NRGBs, stops, red, green, blue, NCont);
  gStyle->SetNumberContours(NCont);
  

  //  gStyle->SetNumberContours(255); 
  //******************************************************************
  

  /*/
const Int_t NRGBs = 6;
const Int_t NCont = 999;

  Double_t stops[NRGBs] = { 0.00, 0.1, 0.34, 0.61, 0.84, 1.00 };
  Double_t red[NRGBs]   = { 0.99, 0.0, 0.00, 0.87, 1.00, 0.51 };
  Double_t green[NRGBs] = { 0.00, 0.0, 0.81, 1.00, 0.20, 0.00 };
  Double_t blue[NRGBs]  = { 0.99, 0.0, 1.00, 0.12, 0.00, 0.00 };


  TColor::CreateGradientColorTable(NRGBs, stops, red, green, blue, NCont);
  gStyle->SetNumberContours(NCont);

  */
}

TGraph2D *dt[500];//  = new TGraph2D();  

Color_t color_wh[28]={kRed,kViolet,kGreen-2,kOrange+7,kBlue,kCyan+2,kSpring-5,kMagenta,kRed+3,kViolet-7,kGreen-6,kOrange+3,kBlue-2,kPink-9,kCyan+2,kYellow-6,kMagenta-9,kRed,kViolet,kGreen+4,kOrange,kBlue,kPink,kCyan+2,kSpring-5,kRed-2,kViolet-9,};



void graph2dfile(Int_t k, Int_t  steps, Double_t xrec[300], Double_t yrec[300], Double_t zrec[300])
{
  if (steps == 0) return;
  Int_t col_code = k%28;
  if(k == -1 )
    {
      TPolyLine3D *track3D = new TPolyLine3D(steps,xrec,yrec,zrec);
      
      track3D->SetLineWidth(2);
      track3D->SetLineColor(kBlack);
      track3D->Draw("same");
    }
  else
    {
      
      
      cout<<"steps: "<<steps<<endl;
      TPolyLine3D *track3D = new TPolyLine3D(steps,xrec,yrec,zrec);
      
      track3D->SetLineWidth(2);
      track3D->SetLineColor(color_wh[col_code]);
      track3D->Draw("same");
      
      
      dt[k] = new TGraph2D(steps, xrec, yrec, zrec);
      dt[k]->SetName(Form("Event /%d",k));
      dt[k]->SetMarkerStyle(20);
      dt[k]->SetMarkerSize(.9);
      dt[k]->SetMarkerColor(color_wh[col_code]);
      dt[k]->SetTitle("RTPC - Hits/Event");
      
      dt[k]->GetXaxis()->SetTitle("X (cm)"); dt[k]->GetXaxis()->SetTitleOffset(1.4); dt[k]->GetXaxis()->CenterTitle() ;
      dt[k]->GetYaxis()->SetTitle("Y (cm)"); dt[k]->GetYaxis()->SetTitleOffset(1.4); dt[k]->GetYaxis()->CenterTitle() ;
      dt[k]->GetZaxis()->SetTitle("Z (cm)"); dt[k]->GetZaxis()->SetTitleOffset(1.4); dt[k]->GetZaxis()->CenterTitle() ;
  
      dt[k]->Draw("P SAME");
    }
      
}

void chain_display()
{
  
  set_color_env();
  //    gStyle->SetOptFit(1111);
  
  Bool_t VERBOSE = 0;
  
  gStyle->SetOptStat(0);
  
  Double_t rad2deg = 180/(4*atan(1));

  const Int_t ndim = 500;

  
  Int_t    steps;
  Int_t    steps_temp, rsteps;//I need this variable 
  Int_t    fIndex;
  Int_t    fHit;
  Int_t    fPid;

  Int_t    fSenPad[ndim] =0;
  Int_t    fTDC[ndim] =0;
  Int_t    fADC[ndim] =0;
  Double_t fXRec[ndim], fYRec[ndim], fZRec[ndim];

  Double_t fX[ndim], fY[ndim], fZ[ndim];



  Double_t xrec[ndim],yrec[ndim],zrec[ndim];
  Double_t x[ndim],y[ndim],z[ndim];

  

  
  Double_t fEdep , time;

  Double_t fkineEne;
  Int_t    fPid;
  Double_t fTheta;
  Double_t fPhi;

 
  TFile *infile = new TFile("ChainEvents.root");
 
  TTree *RTPCTree=(TTree*)infile->Get("chaintree");
  
  Int_t Entries = RTPCTree->GetEntries();
  cout<<"Entries: "<<Entries<<endl;
  
  RTPCTree ->SetBranchAddress("event", &fIndex);
  RTPCTree ->SetBranchAddress("Hit", &fHit);
  RTPCTree ->SetBranchAddress("X", &fXRec[steps]);
  RTPCTree ->SetBranchAddress("Y", &fYRec[steps]);
  RTPCTree ->SetBranchAddress("Z", &fZRec[steps]);


 
  TCanvas *cGraph = new TCanvas("cGraph","Graph2D example",0,0,600,1200); 
  cGraph->SetGridx();
  
  // This histogram is neccesary to plot the axis and keep the multi graph2D 
  TH3F *frame3d = new TH3F("frame3d","RTPC - Hits/Chains",10,-70,70, 10,-70,70, 10,-220,220);
  
  frame3d->GetXaxis()->SetTitle("X (cm)"); frame3d->GetXaxis()->SetTitleOffset(1.4); frame3d->GetXaxis()->CenterTitle() ;
  frame3d->GetYaxis()->SetTitle("Y (cm)"); frame3d->GetYaxis()->SetTitleOffset(1.4); frame3d->GetYaxis()->CenterTitle() ;
  frame3d->GetZaxis()->SetTitle("Z (cm)"); frame3d->GetZaxis()->SetTitleOffset(1.4); frame3d->GetZaxis()->CenterTitle() ;
  
  frame3d->Draw();

  

  Double_t xgr, ygr, zgr;
  
    
  Int_t nentries = RTPCTree->GetEntries();

  
  Int_t p = nentries;//INDEX TO CONTROL FLUX

  cout<< " ENTRIES: "<<nentries<<endl;
  
  for(Int_t i = 0; i < p; i++)
    {
      
      RTPCTree->GetEntry(i);
      
      steps_temp = fHit;
      
      RTPCTree   ->Show(i,20)/10;  
      //	 
      
      cout<<"Hits Num: "<<fHit<<endl;
      
      steps = 0;
      rsteps = 0;
      
      for (Int_t k=0;k<steps_temp;k++)
	{
	  //	      cout<<"Step: "<<fSenPad[k]<<endl;
	  
	  if((fXRec[k]!=0) && (fYRec[k]!=0) && (fZRec[k]!=0))
	    {
	      // cout<<"fXRec[k]: "<<fZRec[k]<<endl;
	      xrec[steps] = fXRec[k];
	      yrec[steps] = fYRec[k];
	      zrec[steps] = fZRec[k];
	      
	      xgr = xrec[steps];
	      ygr = yrec[steps];
	      zgr = zrec[steps];
	      
	      steps++;
	    }


	}
      
     graph2dfile(i, steps, xrec, yrec, zrec);//The use of cm was due to the OpenGL, maybe we should return to mm dividing by 10
 
  
         
      //      cout<<"Entry: "<<fIndex<<endl;

       cout<<steps<<endl;

       TPolyLine3D *track3D = new TPolyLine3D(steps,xrec,yrec,zrec);
       
       //TGraph2D *track3D = new TGraph2D(steps,xrec,yrec,zrec);

         
       // track3D->SetLineWidth(0);
       // track3D->SetLineColor(kBlue);
       // track3D->Draw("same");
       
    }

  //Code to draw a circle
  Int_t steps_cir = 40;
  Double_t pi = 4*atan(1);
  Double_t x_cir[40], y_cir[40], z_cir[40]; 

  Double_t ar;
  
  for (Int_t pp=0; pp < steps_cir; pp++)
    {
      ar =  pp*0.05*pi;
      x_cir[pp] = 70.*cos(ar);
      y_cir[pp] = 70.*sin(ar);
      z_cir[pp] = -220;
    }

  graph2dfile(-1, steps_cir, x_cir, y_cir, z_cir);//The use of cm was due to the OpenGL, maybe we should return to mm dividing by 10
     

    
}

void DrawGrid()
{

Int_t ncol = 100;
  Int_t nrow = 200;
  
  
   TPad *grid = new TPad("grid","",0,0,1,1);
   grid->Draw();
   grid->cd();
   grid->SetGrid();
   grid->SetFillStyle(4000);
   grid->SetFrameFillStyle(0);


   TH2 *hgrid = new TH2C("hgrid","", ncol+1, -5, ncol+5, nrow, -5, nrow-1+5);
   hgrid->Draw();
   hgrid->GetXaxis()->SetNdivisions(6,100);
   hgrid->GetYaxis()->SetNdivisions(200);
   hgrid->GetYaxis()->SetLabelOffset(999.);
   hgrid->GetXaxis()->SetLabelOffset(999.);
}



