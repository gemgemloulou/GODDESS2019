#ifndef superX3_h
#define superX3_h

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
/***** SuperX3 detector                                                   */
/**************************************************************************/

class superX3 : public orrubaDet { 
 public: 
  Double_t activeLength, activeWidth; 
  std::vector<Int_t> stripsP; 
  std::vector<Float_t> eNearRaw, eFarRaw; 
  std::vector<Float_t> eNearCal, eFarCal; 
  std::vector<uint64_t> timeNear, timeFar; 
  
  std::vector<Int_t> stripsN; 
  std::vector<Float_t> eRawN, eCalN; 
  std::vector<uint64_t> timeN; 

 private: 
  TVector3 pStripEdgePos[5], nStripEdgePos[5]; // vector to mid point of edge, in mm 
  TVector3 pStripCenterPos[4], nStripCenterPos[4]; // absolute center, in mm 
  TVector3 eventPos; 
  
  Float_t binsP[5], binsPCenter[4];   
  Float_t binsN[5], binsNCenter[4]; 
  Float_t binsZ[5], binsZCenter[4]; 
  Float_t binsAzimuthal[5], binsAzimuthalCenter[4];
  Float_t binsPolar[5]; 
  
  orrubaDet::valueMap stripPosRaw, stripPosCal; 
  Int_t stripChannelMult[4]; 
  std::vector<Float_t> parStripPosCal[4], parStripECal[4]; 

 public: 
  superX3(); 
  superX3( std::string serial_Num, UShort_t sector_, UShort_t depth_, Bool_t upstream_, solidVector position); 
  virtual ~superX3() { ; } 
  
  void Clear(); 

  /* Geometry functions */
  void SetGeomParameters(map<string, Double_t> geoInfo) { 
    activeWidth = geoInfo["SuperX3 Active Width"]; 
    activeLength = geoInfo["SuperX3 Active Length"];
  }
  TVector3 GetPStripCenterPos(Int_t strip) { return pStripCenterPos[strip]; } 
  TVector3 GetNStripCenterPos(Int_t strip) { return nStripCenterPos[strip]; } 
  Int_t GetNumBins() { return 4; } 
  Float_t* GetPtypeBins() { return binsP; } 
  Float_t* GetNtypeBins() { return binsN; } 
  Float_t* GetPtypeCenterBins() { return binsPCenter; } 
  Float_t* GetNtypeCenterBins() { return binsNCenter; } 
  Float_t* GetZbins() { return binsZ; } 
  Float_t* GetZCenterBins() { return binsZCenter; } 
  Float_t* GetAzimuthalBins() { return binsAzimuthal; } 
  Float_t* GetAzimuthalCenterBins() { return binsAzimuthalCenter; } 
  Float_t* GetPolarBins() { return binsPolar; } 
  void ConstructBins();

  /* Resistive strip stuff */
  Bool_t ValidStrip(UShort_t strip) { if (strip>=0 && strip<=3) { return kTRUE; } return kFALSE; }
  Int_t GetStrip(Int_t channel);
  UShort_t GetNearChannel(UShort_t strip);
  UShort_t GetFarChannel(UShort_t strip);
  Float_t GetNearE(Bool_t calibrated = kTRUE);
  Float_t GetFarE(Bool_t calibrated = kTRUE);
  void UpdatePosition(UShort_t strip);
  orrubaDet::valueMap GetStripPosRaw() { return stripPosRaw; } 
  orrubaDet::valueMap GetStripPosCal() { return stripPosCal; } 

  /* Resistive strip calibration.... */
  void SetStripPosCalParameters(Int_t strip, std::vector<Float_t> pars);
  void SetStripECalParameters(Int_t strip, std::vector<Float_t> pars);
  std::vector<Float_t>* GetResStripParCal() { return parStripECal; } 
  
  /* Multiplicity stuff and total energy */
  virtual Int_t GetChannelMult(Bool_t calibrated = kTRUE);
  virtual Int_t GetChannelMult(Bool_t nType, Bool_t calibrated);
  std::vector<Float_t> GetResE(Bool_t calibrated = kTRUE);

  /* Actual main analysis-ish functions */
  virtual void LoadEvent(goddessEvent *ev, Bool_t ignoreThresholds);
  virtual void SetRawEValue(UInt_t detChannel, Bool_t nType, UInt_t rawValue, Int_t ignoreThresh); 
  
  virtual void SortAndCalibrate(Bool_t doCalibrate);
  virtual Float_t GetESum(Bool_t nType = kFALSE, Bool_t calibrated = kTRUE);
  virtual void GetMaxHitInfo(Int_t* stripMaxP, uint64_t* timestampMaxP,  
  			     Int_t* stripMaxN, uint64_t* timestampMaxN, 
  			     Bool_t calibrated = kTRUE); 

  virtual TVector3 GetEventPosition(Bool_t calibrated = kTRUE);
  
 private: 
  ClassDef(superX3, 1); 

}; 

#endif

