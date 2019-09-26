#include "variable.h"
#include "Rtypes.h"

#include <iostream>
#include "string.h"
#include "string"
#include <fstream>
#include <sstream>

using namespace std;


double space;
double max_ang;
double min_ang;
double ang_sep;



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
