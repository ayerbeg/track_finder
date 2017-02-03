#ifndef DataStruct_H
#define DataStruct_H 1

#include "Rtypes.h"


typedef struct {
  Double_t X;
  Double_t Y;
  Double_t Z;
  Int_t Status;
} HitStruct;


typedef struct {
  Int_t ID;
  Double_t X_rec[500];
  Double_t Y_rec[500];
  Double_t Z_rec[500];
  Int_t Hit;
} ChainStruct;


#endif
