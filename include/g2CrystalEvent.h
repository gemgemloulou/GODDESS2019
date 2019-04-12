#ifndef g2CrystalEvent_h
#define g2CrystalEvent_h

using namespace std;

//#include "Riostream.h"
//#include "Rtypes.h"
#include "TObject.h"
#include "TString.h"
#include "TVector3.h"
#include "TMath.h"
#include "TRandom1.h"
#include "TClass.h"
#include <vector>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <signal.h>

#include "g2IntPt.h"

class g2CrystalEvent : public TObject {

 public:
  Int_t crystalID;
  Int_t crystalNum;
  Int_t quadNum;
  long long int timestamp;
  Float_t t0;
  Float_t chiSq;
  Float_t normChiSq;
  Float_t bl;
  Int_t error;
  Float_t cc;
  Float_t edop; //glw
  Float_t edop_maxInt; //AR
  Float_t edopSeg; //glw
  Float_t edopXtal; //glw
  Float_t ccCurrent, ccPrior1, ccPrior2;
  UShort_t deltaT1, deltaT2;
  Float_t cc1, cc2, cc3, cc4;
  Float_t segSum;
  Float_t doppler;
  Float_t dopplerSeg;
  Float_t dopplerSegOffset;
  Float_t dopplerCrystal;
  vector<g2IntPt> intpts;
  vector<Short_t> waveAll;
  
 public:
  g2CrystalEvent() { ; }
  ~g2CrystalEvent() { ; }
  void Reset();
  Int_t numIntPts();
  Int_t numSegHit();
  Int_t segsHitAboveThresh(Float_t thresh);
  Float_t averageIntPtsPerSeg();
  Float_t segSumRaw();
  Float_t segSumRawThresh(Float_t thresh);
  TVector3 maxIntPtXYZ();
  Double_t maxIntPtX() { return maxIntPtXYZ().X(); }
  Double_t maxIntPtY() { return maxIntPtXYZ().Y(); }
  Double_t maxIntPtZ() { return maxIntPtXYZ().Z(); }
  Double_t maxIntPtR() { return maxIntPtXYZ().XYvector().Mod(); }
  TVector3 maxIntPtXYZLab();
  TVector3 maxIntPtXYZLabSeg();
  TVector3 maxIntPtXYZLabCrystal();
  Float_t maxIntPtE();
  Int_t maxIntPtSegNum();
  Float_t maxIntPtSegE();
  Float_t gTheta();
  Float_t gPhi();
  Float_t CosDop();
  Float_t cDoppler(Float_t beta);
  Float_t cDopplerSeg(Float_t beta);
  Float_t cDopplerCrystal(Float_t beta);

 public:
  ClassDef(g2CrystalEvent, 2); //glw 11.iii.2019 the compiler made me change this from 1 to 2. 
/* Warning in <TStreamerInfo::BuildCheck>: 
   The StreamerInfo of class g2CrystalEvent read from file /global/data1b/gretina/1707_data1b/Run0086/Run0086.root
   has the same version (=1) as the active class but a different checksum.
   You should update the version to ClassDef(g2CrystalEvent,2).
   Do not try to write objects with the current class definition,
   the files will not be readable.
*/
};

#endif
