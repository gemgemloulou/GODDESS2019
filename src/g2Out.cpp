#include "g2Out.h"
/**************************************************************/
/* g2OUT Class Functions **************************************/
/**************************************************************/

void g2OUT::Reset() {
  runNumber = -1;
  for (UInt_t ui=0; ui<crystalMult(); ui++) {
    xtals[ui].intpts.clear();
    xtals[ui].waveAll.clear();
  }
  xtals.clear();
}

/**************************************************************/

Float_t g2OUT::calorimeterDoppler(Float_t beta) {
  Float_t maxE = 0;
  Int_t max = -1;
  for (UInt_t ui=0; ui<crystalMult(); ui++) {
    if (xtals[ui].cc > maxE) { maxE = xtals[ui].cc;  max = ui; }
  }
  if (max >= 0) {
    if (beta > 0) {
      return (xtals[max].cDoppler(beta));
    } else { return (xtals[max].doppler); }
  } else { return 0.; }
}

/**************************************************************/

/*! Determines the calorimeter mode energy for a mode2 event, 
    subject to an energy threshold for inclusion of a given crystal
    energy.  Calculated based on the CC energy values.
    
    \param thresh The float value of the energy threshold that is 
           checked for each crystal to be included in the calorimeter
           sum
    \return Returns the float value of the calorimeter mode energy sum
            for the mode2 event
*/

Float_t g2OUT::calorimeterE(Float_t thresh) {
  Float_t sum = 0.;
  for (UInt_t ui=0; ui<crystalMult(); ui++) {
    if (xtals[ui].cc3 > thresh) { sum += xtals[ui].cc3; }
  }
  return sum;
}

/**************************************************************/

Float_t g2OUT::calorimeterE() {
  return calorimeterE(0.0);
}

/**************************************************************/

UInt_t g2OUT::crystalMult() {
  return xtals.size();
}
