#ifndef g2IntPt_h
#define g2IntPt_h

#include "Riostream.h"
#include "Rtypes.h"

#include "TObject.h"
#include "TString.h"
#include "TVector3.h"
#include "TMath.h"
#include "TRandom1.h"
#include "TRandom1.h"
#include "TClass.h"

#include <vector>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <signal.h>

class g2IntPt : public TObject {

 public:
  Int_t segNum;
  TVector3 xyz;
  TVector3 xyzLab;
  TVector3 xyzLabSeg;
  TVector3 xyzLabCrys;
  Float_t e, segE;

 public:
  g2IntPt() { ; }
  ~g2IntPt() { ; }

 public:
  ClassDef(g2IntPt, 1);
};

#endif
