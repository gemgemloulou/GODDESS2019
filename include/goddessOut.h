#ifndef goddessOut_h
#define goddessOut_h

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
#include "detectorOut.h"

class goddessOut: public TObject {
 public:
  std::vector<string> siID;
  std::vector<string> siType;
  std::vector<detectorOUT> si;
  Float_t icDE;
  Float_t icE;
  Float_t icX;
  Float_t icY;

  Float_t tdc0;
  Float_t tdc1;
  Float_t tdc2;
  Float_t tdc3;
  Float_t tdc4;
  Float_t tdc5;

 public:
  goddessOut() { ; }
  ~goddessOut() { ; }
  void Clear();

 private:
  ClassDef(goddessOut, 1);
};

#endif

