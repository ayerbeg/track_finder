#ifndef variable_h
#define variable_h 1

#include "string.h"
#include "string"
#include <fstream>
#include <sstream>
#include "TString.h"
#include "Rtypes.h"

void variable(TString);

extern TString inputfile;
extern TString outputfile;
extern double space;
extern double max_ang;
extern double min_ang;
extern double ang_sep;

#endif
