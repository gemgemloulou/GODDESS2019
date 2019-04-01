#ifndef g2Out_h
#define g2Out_h

using namespace std;
#include "Riostream.h"
#include "Rtypes.h"
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
#include "TRandom1.h"
#include "TClass.h"

#include <vector>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include "g2CrystalEvent.h"

class g2OUT : public TObject {

 public:
  Int_t runNumber;
  vector<g2CrystalEvent> xtals;
  
 public:
  g2OUT() { ; }
  ~g2OUT() { ; }
  void Reset();
  Float_t calorimeterDoppler(Float_t beta);
  Float_t calorimeterE(Float_t thresh);
  Float_t calorimeterE();
  UInt_t crystalMult();

 public:
  ClassDef(g2OUT, 1);
};

#endif
