#ifndef orrubaDet_h
#define orrubaDet_h

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

#include "solidVector.h"
#include "goddessEvent.h"

/**************************************************************************/
/***** ORRUBA Detector base class (from Detector.h, siDet.h, orrubaDet.h) */
/**************************************************************************/

class orrubaDet : public TObject { 
 public: 
  typedef std::map<Short_t, Float_t> valueMap; 
  typedef std::map<Short_t, uint64_t> timeMap; 
  typedef std::map<Short_t, Short_t> chMap;
  Bool_t nTYPE; 
  Bool_t pTYPE; 

 private: 
  UShort_t numPtype, numNtype; 
  valueMap eRawPmap, eRawNmap, eCalPmap, eCalNmap; 
  timeMap timePmap, timeNmap; 

  chMap chMapP, chMapN;
  
  // Calibration parameters and thresholds 
  std::vector< std::vector<Float_t> > parECalP, parECalN; 
  std::vector<Int_t> threshP, threshN; 
  
 protected:
  std::string serialNum; 
  std::string posID;
  UShort_t sector; 
  UShort_t depth; // # of detectors between this one and the target
  Bool_t upStream;
  solidVector detPos;
  
 public: 
  /* Initializing and resetting... */
  orrubaDet();
  orrubaDet(std::string serial_Num, UShort_t sector_, UShort_t depth_, Bool_t upstream_, solidVector position);
  virtual ~orrubaDet() { ; } 
  void Clear(); 

  /* Detector identification stuff */
  std::string GetSerialNum() { return serialNum; }
  std::string GetPosID() { return posID; }
  UShort_t GetSector() { return sector; }
  UShort_t GetDepth() { return depth; }
  Bool_t GetUpStream() { return upStream; }
  void SetDetector(std::string serial_Num, UShort_t sector_, UShort_t depth_, Bool_t upstream_, solidVector position); 

  /* Electronics channel stuff */
  void SetChannelMap(std::map<Short_t, Short_t> channelMap, Bool_t nType);
  std::map<Short_t, Short_t> GetChannelMap(Bool_t nType);

  void SetNumChannels(Int_t pType, Int_t nType = 0); 
  Int_t GetNumChannels(Bool_t nType); 
  Bool_t ValidChannel(UInt_t channel, Bool_t nType); 
  Bool_t ChannelHit(UInt_t detChannel, Bool_t nType);

  /* Loading an event into the detector, setting/getting values. */
  virtual void LoadEvent(goddessEvent *ev, Bool_t ignoreThresholds) = 0;
  
  virtual void SetRawEValue(UInt_t detChannel, UInt_t rawValue, Int_t ignoreThresh); 
  virtual void SetRawEValue(UInt_t detChannel, Bool_t nType, UInt_t rawValue,  
			    Int_t ignoreThresh); 
  virtual Float_t GetRawEValue(UInt_t detChannel, Bool_t nType);
  virtual Float_t GetCalEValue(UInt_t detChannel, Bool_t nType);
  virtual orrubaDet::valueMap GetRawE(Bool_t nType);
  virtual orrubaDet::valueMap GetCalE(Bool_t nType);

  virtual void SetTimestamp(UInt_t detChannel, Bool_t nType, uint64_t timestamp); 
  virtual uint64_t GetTimestamp(UInt_t detChannel, Bool_t nType);
  virtual orrubaDet::timeMap GetTSmap(Bool_t nType);

  /* Calibration parameters and thresholds. */
  virtual Bool_t SetECalParameters(std::vector<Float_t> par, Int_t contact, Bool_t nType);
  virtual std::vector< std::vector<Float_t> > GetECalParameters(Bool_t nType);
  virtual Bool_t SetThresholds(std::vector<Int_t> thresholds, Bool_t nType, Int_t thrSize);
  virtual std::vector<Int_t> GetThresholds(Bool_t nType);

  /* Calibrate, and do analysis-ish stuff */  
  virtual void SortAndCalibrate(Bool_t doCalibrate = kTRUE) = 0;
  virtual TVector3 GetEventPosition(Bool_t calibrated = kTRUE) = 0;
  virtual void GetMaxHitInfo(Int_t* stripMaxP, uint64_t* timestampMaxP,  
  			     Int_t* stripMaxN, uint64_t* timestampMaxN, 
  			     Bool_t calibrated = kTRUE) = 0; 

  /* Multiplicity stuff and total energy */
  virtual Int_t GetChannelMult(Bool_t calibrated = kTRUE) = 0;
  virtual Int_t GetChannelMult(Bool_t nType, Bool_t calibrated) = 0;
  virtual Float_t GetESum(Bool_t nType = kFALSE, Bool_t calibrated = kTRUE) = 0;

 protected:
  virtual void SetPosID();
   
 private: 
  ClassDef (orrubaDet, 1); 
}; 



#endif

