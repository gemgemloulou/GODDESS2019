#include "solidVector.h"
//
//
solidVector::solidVector(Double_t x_, Double_t y_, Double_t z_, Double_t rot_Z, Double_t rot_Phi):
  TVector3(x_, y_, z_), rotZ(rot_Z), rotPhi(rot_Phi) 
{ ; }

