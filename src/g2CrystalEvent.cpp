#include "g2CrystalEvent.h"
void g2CrystalEvent::Reset() { 
  intpts.clear();
  crystalNum = -1;  quadNum = -1;
  timestamp = 0;
  t0 = -1.0; chiSq = -1.0;
  bl = -9999; /* 2015-09-20 CMC: added for wf baseline and PZ checks */
  error = -1;
  cc = 0.0; segSum = 0.0;
  ccCurrent = 0.0; ccPrior1 = 0.0; ccPrior2 = 0.0;
  deltaT1 = 0.0; deltaT2 = 0.0;
  doppler = 0.0; dopplerSeg = 0.0; dopplerCrystal = 0.0; dopplerSegOffset = 0.0; 
  edop = 0.0;
  edop_maxInt = 0.0;
}

/**************************************************************/

Int_t g2CrystalEvent::numIntPts() {  return intpts.size();  }

/**************************************************************/

Int_t g2CrystalEvent::numSegHit() {
  Int_t hit[40] = {0};
  Int_t segsHit = 0;
  for (Int_t i=0; i<intpts.size(); i++) {
    if (hit[intpts[i].segNum] == 0) {
      hit[intpts[i].segNum] = 1;
      segsHit++;
    }
  }
  return segsHit;
}

/**************************************************************/

Int_t g2CrystalEvent::segsHitAboveThresh(Float_t thresh) {
  Int_t hit[40] = {0};
  Int_t segsHit = 0;
  for (Int_t i=0; i<intpts.size(); i++) {
    if (hit[intpts[i].segNum] == 0 && intpts[i].segE > thresh) {
      hit[intpts[i].segNum] = 1;
      segsHit++;
    }
  }
  return segsHit;
}

/**************************************************************/

Float_t g2CrystalEvent::averageIntPtsPerSeg() {
  if (numSegHit() > 0) {
    return ((Float_t)intpts.size() / (Float_t)(numSegHit()));
  } else { return 0.; }
} 

/**************************************************************/

Float_t g2CrystalEvent::segSumRaw() {
  Float_t sum = 0;
  Int_t used[40] = {0};
  for (Int_t i=0; i<intpts.size(); i++) {
    if (used[intpts[i].segNum] == 0) {
      sum += intpts[i].segE;
      used[intpts[i].segNum] = 1;
    }
  }
  return sum;
}

/**************************************************************/

Float_t g2CrystalEvent::segSumRawThresh(Float_t thresh) {
  Float_t sum = 0;
  Int_t used[40] = {0};
  for (Int_t i=0; i<intpts.size(); i++) {
    if (used[intpts[i].segNum] == 0 && intpts[i].segE > thresh) {
      sum += intpts[i].segE;
      used[intpts[i].segNum] = 1;
    }
  }
  return sum;
}

/**************************************************************/

TVector3 g2CrystalEvent::maxIntPtXYZ() {
  Float_t maxE = 0; Int_t max = -1;
  for (Int_t i=0; i<intpts.size(); i++) {
    if (intpts[i].e > maxE) {
      maxE = intpts[i].e;
      max = i;
    }
  }
  if (max > -1) { return intpts[max].xyz; }
  else { return TVector3(0., 0., 0.); }
}

/**************************************************************/

TVector3 g2CrystalEvent::maxIntPtXYZLab() {
  Float_t maxE = 0; Int_t max = -1;
  for (Int_t i=0; i<intpts.size(); i++) {
    if (intpts[i].e > maxE) {
      maxE = intpts[i].e;
      max = i;
    }
  }
  if (max > -1) { return intpts[max].xyzLab; }
  else { return TVector3(0., 0., 0.); }
}

/**************************************************************/

TVector3 g2CrystalEvent::maxIntPtXYZLabSeg() {
  Float_t maxE = 0; Int_t max = -1;
  for (Int_t i=0; i<intpts.size(); i++) {
    if (intpts[i].e > maxE) {
      maxE = intpts[i].e;
      max = i;
    }
  }
  if (max > -1) { return intpts[max].xyzLabSeg; }
  else { return TVector3(0., 0., 0.); }
}

/**************************************************************/

TVector3 g2CrystalEvent::maxIntPtXYZLabCrystal() {
  Float_t maxE = 0; Int_t max = -1;
  for (Int_t i=0; i<intpts.size(); i++) {
    if (intpts[i].e > maxE) {
      maxE = intpts[i].e;
      max = i;
    }
  }
  if (max > -1) { return intpts[max].xyzLabCrys; }
  else { return TVector3(0., 0., 0.); }
}

/**************************************************************/

Float_t g2CrystalEvent::maxIntPtE() {
  Float_t maxE = 0; Int_t max = -1;
  for (Int_t i=0; i<intpts.size(); i++) {
    if (intpts[i].e > maxE) {
      maxE = intpts[i].e;
      max = i;
    }
  }
  if (max > -1) { return intpts[max].e; }
  else { return 0.; }
}

/**************************************************************/

Int_t g2CrystalEvent::maxIntPtSegNum() {
  Float_t maxE = 0; Int_t max = -1;
  for (Int_t i=0; i<intpts.size(); i++) {
    if (intpts[i].e > maxE) {
      maxE = intpts[i].e;
      max = i;
    }
  }
  if (max > -1) { return intpts[max].segNum; }
  else { return -1; }
}

/**************************************************************/

Float_t g2CrystalEvent::maxIntPtSegE() {
  Float_t maxE = 0; Int_t max = -1;
  for (Int_t i=0; i<intpts.size(); i++) {
    if (intpts[i].e > maxE) {
      maxE = intpts[i].e;
      max = i;
    }
  }
  if (max > -1) { return intpts[max].segE; }
  else { return 0.0; }
}

/**************************************************************/

Float_t g2CrystalEvent::gTheta() {
  if (numIntPts() > 0) {
    return maxIntPtXYZLab().Theta();
  } else { return 0.0; }
}

/**************************************************************/

Float_t g2CrystalEvent::gPhi() {
  if (numIntPts() > 0) {
    if (maxIntPtXYZLab().Phi() < 0) {
      return (maxIntPtXYZLab().Phi() + TMath::TwoPi());
    } else {
      return maxIntPtXYZLab().Phi();
    }
  } else { return 0.0; }
}

/**************************************************************/

Float_t g2CrystalEvent::CosDop() {
  if (numIntPts() > 0) {
    return maxIntPtXYZLab().CosTheta();
  } else {
    return 0.0;
  }
}

/**************************************************************/

Float_t g2CrystalEvent::cDoppler(Float_t beta) {
  if (numIntPts() > 0) {
    Float_t cosDop = maxIntPtXYZLab().CosTheta();
    Float_t gamma = 1/TMath::Sqrt(1. - beta*beta);
    return (gamma*(1. - beta*cosDop));
  } else {
    return 0.0;
  }
}

/**************************************************************/

Float_t g2CrystalEvent::cDopplerSeg(Float_t beta) {
  if (numIntPts() > 0) {
    Float_t cosDop = maxIntPtXYZLabSeg().CosTheta();
    Float_t gamma = 1/TMath::Sqrt(1. - beta*beta);
    return (gamma*(1. - beta*cosDop));
  } else {
    return 0.0;
  }
}

/**************************************************************/

Float_t g2CrystalEvent::cDopplerCrystal(Float_t beta) {
  if (numIntPts() > 0) {
    Float_t cosDop = maxIntPtXYZLabCrystal().CosTheta();
    Float_t gamma = 1/TMath::Sqrt(1. - beta*beta);
    return (gamma*(1. - beta*cosDop));
  } else {
    return 0.0;
  }
}

