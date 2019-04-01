#ifndef goddessFull_h
#define goddessFull_h

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
#include "goddessOut.h"

#include "QQQ5.h"
#include "superX3.h"
#include "BB10.h"
#include "ionChamber.h"
#include "tdc.h"

/**************************************************************************/
/***** Overall GODDESS structure                                          */
/**************************************************************************/

class goddessFull: public TObject {
 public:
  UInt_t buf[2048];
  goddessEvent *rawGoddess;

  std::vector<QQQ5> qqs;
  std::vector<superX3> s3s;
  std::vector<BB10> bbs;
  std::vector<ionChamber> ics;
  std::vector<tdc_t> tdcs;
	
  goddessOut *eventOut;

  uint64_t ts;

  std::map<string, Double_t> geomInfo;
  
 public:
  goddessFull() { ; }
  ~goddessFull() { ; }
  void Initialize();
  void Clear();

  Bool_t ParseID(std::string id, Short_t &sector, Short_t &depth, Bool_t &upStream);
  void ReadPositions(TString filename);
  solidVector GetPosVector(std::string type, Short_t sector, Short_t depth, Bool_t upStream);
  void ReadConfiguration(TString filename);

  void InitializeDetectors();

  void getAnalogGoddess(FILE *inf, Int_t evtLength);
  void printAnalogRawEvent();
  void ProcessEvent();
  void ResetOutput(detectorOUT* det);

 public:
    
 private:
  ClassDef(goddessFull, 1);
};

#endif

