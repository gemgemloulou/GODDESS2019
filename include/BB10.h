#ifndef BB10_h
#define BB10_h

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
/***** BB10 (not BB8...)  detector                                        */
/**************************************************************************/

class BB10 : public orrubaDet {
 public:
  Double_t activeWidth;
  std::vector<Int_t> stripsP;
  std::vector<Float_t> eRawP, eCalP;
  std::vector<uint64_t> timeP;
  
 private:
  /* Geometry information for BB10 detectors */
  /* Positions of edges and centers in mm/radians */
  TVector3 pStripEdgePos[9];
  TVector3 pStripCenterPos[8];
  Float_t binsP[9];
  Float_t binsAzimuthal[9];
  Float_t binsPCenter[8];
  Float_t binsAzimuthalCenter[8];

  TVector3 eventPos;

 public:
//  BB10() { ; } glw 14iii2019
  BB10(); // to match sx3 treatment
  //BB10(std::string serial_Num, UShort_t sector_, UShort_t depth_, Bool_t upstream_, solidVector position){;}
BB10(std::string serial_Num, UShort_t sector_, UShort_t depth_, Bool_t upstream_, solidVector position);
  virtual ~BB10() { ; }

  //virtual void Clear() { ; }
  virtual void Clear();
  /* Geometry functions */
  void SetGeomParameters(map<string, Double_t> geoInfo) { activeWidth = geoInfo["BB10 Active Width"]; }
  TVector3 GetPStripCenterPos(Int_t strip) { return pStripCenterPos[strip]; }
  Int_t GetNumBins() { return 8; }
  Float_t* GetPTypeBins() { return binsP; }
  Float_t* GetAzimuthalBins() { return binsAzimuthal; }
  Float_t* GetPTypeBinCenters() { return binsPCenter; }
  Float_t* GetAzimuthalBinCenters() { return binsAzimuthalCenter; }
  //virtual void ConstructBins() { ; }
  virtual void ConstructBins(); //glw 14iii2019

  /* Multiplicity stuff and total energy */
 /* virtual Int_t GetChannelMult(Bool_t calibrated = kTRUE) { ; }
  virtual Int_t GetChannelMult(Bool_t nType, Bool_t calibrated) { ; }
  virtual Float_t GetESum(Bool_t nType = kFALSE, Bool_t calibrated = kTRUE) { ; }

  virtual void LoadEvent(goddessEvent *ev, Bool_t ignoreThresholds) { ; }
  virtual void SetRawEValue(UInt_t detChannel, Bool_t nType, UInt_t rawValue,  
			    Int_t ignoreThresh) { ; } 
  virtual void SortAndCalibrate(Bool_t doCalibrate = kTRUE) { ; }  

  virtual void GetMaxHitInfo(Int_t* stripMaxP, uint64_t* timestampMaxP,  
  			     Int_t* stripMaxN, uint64_t* timestampMaxN, 
  			     Bool_t calibrated = kTRUE) { ; } */

virtual Int_t GetChannelMult(Bool_t calibrated = kTRUE);
  virtual Int_t GetChannelMult(Bool_t nType, Bool_t calibrated);
  virtual Float_t GetESum(Bool_t nType = kFALSE, Bool_t calibrated = kTRUE);

  virtual void LoadEvent(goddessEvent *ev, Bool_t ignoreThresholds);
  virtual void SetRawEValue(UInt_t detChannel, Bool_t nType, UInt_t rawValue,  
			    Int_t ignoreThresh); 
  virtual void SortAndCalibrate(Bool_t doCalibrate = kTRUE);  

  virtual void GetMaxHitInfo(Int_t* stripMaxP, uint64_t* timestampMaxP,  
  			     Int_t* stripMaxN, uint64_t* timestampMaxN, 
  			     Bool_t calibrated = kTRUE);
  virtual TVector3 GetEventPosition(Bool_t calibrated = kTRUE); /*{ ; } glw 14iii19*/

 private:
  ClassDef(BB10, 1);

};


#endif

