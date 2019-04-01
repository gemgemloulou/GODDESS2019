#ifndef solidVector_h
#define solidVector_h

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

/**************************************************************************/
/***** SolidVector to define detector positions                           */
/**************************************************************************/

class solidVector : public TVector3 {
 private:
  Double_t rotZ;
  Double_t rotPhi;

 public:
  solidVector(Double_t x_=0, Double_t y_=0, Double_t z_=0, Double_t rot_Z=0, Double_t rot_Phi = 0);
  
  Double_t GetRotZ() { return rotZ; }
  Double_t GetRotPhi() { return rotPhi; }
  void SetRotationZ(Double_t angle) { rotZ = angle; }
  void SetRotationPhi(Double_t angle) { rotPhi = angle; }
  TVector3 GetTVector3() { return TVector3(X(), Y(), Z()); }

 private:
  ClassDef(solidVector, 1);
};


#endif

