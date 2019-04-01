#ifndef tdc_h
#define tdc_h

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
/***** tdc                                                                */
/**************************************************************************/

class tdc_t: public TObject {
 public:
  typedef std::map<Short_t, Float_t> valueMap; 
  typedef std::map<Short_t, Short_t> chMap;

 private:
	Int_t numChannels;
	Float_t tdc0, tdc1, tdc2, tdc3, tdc4, tdc5;
	std::vector<Int_t> threshTDC;

	valueMap tdcRaw, tdcCal;
	chMap mapTDC;
 public:
  tdc_t() { ; }
  tdc_t(Int_t numChannels);
  ~tdc_t() { ; }

  void Clear();
  
  void SetChannelMap(std::map<Short_t, Short_t> channelMap, Int_t chType);
  std::map<Short_t, Short_t> GetChannelMap(Int_t chType);

  Bool_t ValidChannel(UInt_t channel, Int_t chType); 

  void SetRawValue(UInt_t channel, Int_t chType, UInt_t rawValue, Int_t ignoreThresh);
  
  Bool_t SetThresholds(std::vector<Int_t> thresholds, Int_t chType);
  std::vector<Int_t> GetThresholds();

  void LoadEvent(goddessEvent *ev, Bool_t ignoreThresholds);

  //Int_t GetNumChannels(Int_t chType);

  Int_t GetTDCValue(UInt_t channel);
  void SetTDCValue(UInt_t channel, Int_t value);

 private:
  ClassDef(tdc_t, 1);
};
#endif

