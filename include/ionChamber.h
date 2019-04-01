#ifndef ionChamber_h
#define ionChamber_h

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
/***** Ion chamber                                                        */
/**************************************************************************/

class ionChamber: public TObject {
 public:
  typedef std::map<Short_t, Float_t> valueMap; 
  typedef std::map<Short_t, Short_t> chMap;

 private:
  Int_t numAnode, numWireX, numWireY, numDE, numEres;
  valueMap anodeRaw, anodeCal;
  valueMap wireX, wireY;
  Float_t dE, resE, E; // All in MeV
  //Int_t x_wire, y_wire; // AR
  std::vector< std::vector<Float_t> > parAnodeECal;
  std::vector<Int_t> threshAnode, threshX, threshY; 

  chMap mapAnode, mapX, mapY;

 public:
  ionChamber() { ; }
  ionChamber(Int_t numAnode, Int_t numWireX, Int_t numWireY, Int_t numDE, Int_t numEres);
  ~ionChamber() { ; }

  void Clear();
  
  void SetChannelMap(std::map<Short_t, Short_t> channelMap, Int_t chType);
  std::map<Short_t, Short_t> GetChannelMap(Int_t chType);

  Bool_t ValidChannel(UInt_t channel, Int_t chType); 

  void SetRawValue(UInt_t channel, Int_t chType, UInt_t rawValue, Int_t ignoreThresh);
  
  Bool_t SetThresholds(std::vector<Int_t> thresholds, Int_t chType);
  std::vector<Int_t> GetThresholds(Int_t chType);
  // void SetAnodeECalPars(Int_t ch, std::vector<Float_t> pars);

  void LoadEvent(goddessEvent *ev, Bool_t ignoreThresholds);

  Int_t GetNumChannels(Int_t chType);
  Float_t GetAnodeDE() { return dE; }
  Float_t GetAnodeResE() { return resE; }
  Float_t GetAnodeE() { return E; }
  Int_t GetMaxX();
  Int_t GetMaxY();
  //Int_t GetMaxX() { return x_wire; }
  //Int_t GetMaxY() { return y_wire; }
 private:
  ClassDef(ionChamber, 1);
};


#endif

