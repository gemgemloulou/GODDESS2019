#ifndef detectorOut_h
#define detectorOut_h

using namespace std;

#include <map>
#include <cstring>
#include <string>
#include <vector>
#include <iostream>
#include <sstream>
#include <fstream>
#include <stdint.h>

#include "TObject.h"
#include "TMath.h"
#include "TString.h"
#include "TVector3.h"

#include "orrubaDet.h"
#include "solidVector.h"
//#include "goddessFull.h"

struct detectorOUT {
  UShort_t depth;
  UShort_t upstream;
  Int_t pStrip, nStrip;
  Float_t pECal, nECal;
  TVector3 evPos;
  // sx3
  Float_t eNearCal, eFarCal;
  // Int_t nearStrip, farStrip;


};


#endif

