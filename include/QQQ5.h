#ifndef QQQ5_h
#define QQQ5_h

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
#include "goddessEvent.h"

/**************************************************************************/
/***** QQQ5 detector                                                      */
/**************************************************************************/

class QQQ5 : public orrubaDet {
 public:
  Double_t firstStripWidth;
  Double_t deltaPitch;

  std::vector<Int_t> stripsP;
  std::vector<Float_t> eRawP, eCalP;
  std::vector<uint64_t> timeP;
  
  std::vector<Int_t> stripsN;
  std::vector<Float_t> eRawN, eCalN;
  std::vector<uint64_t> timeN;
  
 private:
  TVector3 pStripEdgePos[33], nStripEdgePos[5];
  TVector3 pStripCenterPos[32], nStripCenterPos[4];
  Float_t binsP[33], binsPCenter[32];
  Float_t binsN[5], binsNCenter[4];
  Float_t binsRho[33];
  Float_t binsAzimuthal[5];
  Float_t binsPolar[33], binsPolarCenter[32];

  TVector3 eventPos; 
  
 public:
  QQQ5();
  QQQ5(std::string serial_Num, UShort_t sector_, UShort_t depth_, Bool_t upstream_, solidVector position);
  virtual ~QQQ5() { ; }

  void Clear();

  /* Geometry functions */
  void SetGeomParameters(map<string, Double_t> geoInfo) { 
    firstStripWidth = geoInfo["QQQ5 First Strip Width"]; 
    deltaPitch = geoInfo["QQQ5 Delta Pitch"];
  }
  TVector3 GetPStripCenterPos(Int_t strip) { return pStripCenterPos[strip]; }
  TVector3* GetNStripCenterPos() { return nStripCenterPos; }
  Int_t GetNumNTypeBins() { return 4; }
  Int_t GetNumPTypeBins() { return 32; }
  Float_t* GetPTypeBins() { return binsP; }
  Float_t* GetPTypeBinsCenter() { return binsPCenter; }
  Float_t* GetNTypeBins() { return binsN; }
  Float_t* GetNTypeBinsCenter() { return binsNCenter; }
  Float_t* GetRhoBins() { return binsRho; }
  Float_t* GetAzimuthalBins() { return binsAzimuthal; }
  Float_t* GetPolarBins() { return binsPolar; }
  Float_t* GetPolarBinsCenter() { return binsPolarCenter; }
  void ConstructBins();

  /* Multiplicity stuff and total energy */
  virtual Int_t GetChannelMult(Bool_t calibrated = kTRUE);
  virtual Int_t GetChannelMult(Bool_t nType, Bool_t calibrated);
  virtual Float_t GetESum(Bool_t nType = kFALSE, Bool_t calibrated = kTRUE);

  virtual void LoadEvent(goddessEvent *ev, Bool_t ignoreThresholds);
  virtual void SetRawEValue(UInt_t detChannel, Bool_t nType, UInt_t rawValue,  
   			   Int_t ignoreThresh); 
  virtual void SortAndCalibrate(Bool_t doCalibrate);

  virtual void GetMaxHitInfo(Int_t* stripMaxP, uint64_t* timestampMaxP,  
  			     Int_t* stripMaxN, uint64_t* timestampMaxN, 
  			     Bool_t calibrated = kTRUE); 

  virtual TVector3 GetEventPosition(Bool_t calibrated = kTRUE);

 protected:
  void SetPosID();

 private:
  ClassDef(QQQ5, 1);

};


#endif

