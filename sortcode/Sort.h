#ifndef Sortcode_h
#define Sortcode_h

#define MAXSILICON 48 // The name you want to touch
#define MAXGRETINA 44 // ... but you must not touch

#include <math.h>
#include <iostream>
#include "TCutG.h"
#include <iomanip>
#include "TH1.h"
#include "TTree.h"
#include "TChain.h"
#include "TH2F.h"
#include "TFile.h"
#include "TDirectory.h"
#include "TList.h"
#include "orrubaDet.h"
#include "goddessOut.h"
#include "goddessFull.h"
#include "goddessEvent.h"
#include "detectorOut.h"
#include "../../include/GRETINA.h"
#include <fstream>
#include "TMath.h"

#include "TBranch.h"
#include "TROOT.h"
#include <TVector3.h>
#include <TObject.h>
#include <TLorentzVector.h>

class Sortcode {

 public :
  TTree *fChain;
  Int_t fCurrent;

  Sortcode(){;} 
  void SortData(Int_t );
  TLorentzVector GetRecoilVector(int);
  TLorentzVector GetRecoilVector(double,double,double,double);
  Int_t j,i,k,m,n,s,q,g;
 
  TGraph* MakeCatKinematics(double);

  static double m1; 
  static double m2; 
  static double m3; 
  static double m4;
  static double PI;
  static double T1;
  TVector3 xyzlab;

  double  xlab[MAXSILICON];
  double  ylab[MAXSILICON];
  double  zlab[MAXSILICON];

  Double_t uSX3Pos[MAXSILICON];
  Double_t uSX3Theta[MAXSILICON];
  Double_t uSX3Energy[MAXSILICON];
  Double_t uSX3Ex[MAXSILICON];
  Double_t uSX3E3[MAXSILICON];
  Double_t uSX3p3[MAXSILICON];
  Int_t uSX3fStrip[MAXSILICON];
  Int_t uSX3bStrip[MAXSILICON];
  Int_t uSX3mult;

  Double_t dSX3Pos[MAXSILICON];
  Double_t dSX3Theta[MAXSILICON];
  Double_t dSX3Energy[MAXSILICON];
  Double_t dSX3Ex[MAXSILICON];
  Int_t dSX3fStrip[MAXSILICON];
  Int_t dSX3bStrip[MAXSILICON];
  Double_t dSX3E3[MAXSILICON];
  Double_t dSX3p3[MAXSILICON];
  Int_t dSX3mult;

 
  Double_t uQTheta[MAXSILICON]; // This should probably be higher but it's v unlikely
  Double_t uQEnergy[MAXSILICON];
  Double_t uQEx[MAXSILICON];
  Double_t uQExAR[MAXSILICON];
  Int_t uQfStrip[MAXSILICON];
  Int_t uQbStrip[MAXSILICON];
  Int_t BackStrip[MAXSILICON];
  Double_t uQE3[MAXSILICON];
  Double_t uQp3[MAXSILICON];
  Double_t uQQQphi[4][4];
  Double_t uQPhi[MAXSILICON];
  Int_t uQmult; 

  Double_t dQTheta[MAXSILICON]; // This should probably be higher but it's v unlikely
  Double_t dQEnergy[MAXSILICON];
  Double_t dQEx[MAXSILICON];
  Double_t dQE3[MAXSILICON];
  Double_t dQp3[MAXSILICON];
  Int_t dQfStrip[MAXSILICON];
  Int_t dQbStrip[MAXSILICON];
  Int_t dQmult; 

  Double_t SiEnergy[MAXSILICON];
  Double_t SiTheta[MAXSILICON];
  Double_t Ex[MAXSILICON];
  Int_t SiMult;
	
  Double_t dQQQang[32], uQQQang[32];
  Double_t uQbgain[4][4];
  Double_t uQboffset[4][4];
  Double_t uQfgain[4][32];
  Double_t uQfoffset[4][32];
  Double_t dQbgain[3][4];
  Double_t dQboffset[3][4];
  Double_t dQfgain[3][32];
  Double_t dQfoffset[3][32];

  Double_t BB10fgain[8][8];
  Double_t BB10foffset[8][8];

  Double_t uSX3fgain[12][8];
  Double_t uSX3foffset[12][8];
  Double_t dSX3fgain[12][8];
  Double_t dSX3foffset[12][8];
  
  // Double_t uSX3bgain[12][4][4];
  // Double_t uSX3boffset[12][4][4];
  Double_t uSX3bgain[12][4];
  Double_t uSX3boffset[12][4];
  Double_t dSX3bgain[12][4];
  Double_t dSX3boffset[12][4];	

  Double_t uSX3L[12][4];
  Double_t uSX3R[12][4];
  Double_t dSX3L[12][4];
  Double_t dSX3R[12][4];

  Int_t uQQQ, dQQQ, uSX3, dSX3, dBB10;
  
       
  //maximum crys number = 44
  Int_t quadCut[11];
  Float_t gamE[MAXGRETINA];
  Float_t edop[MAXGRETINA];
  Float_t edopSeg[MAXGRETINA];
  Float_t edopXtal[MAXGRETINA];
  Int_t quadNum[MAXGRETINA];
  Int_t crysNum[MAXGRETINA];
  Float_t t0[MAXGRETINA];
  Int_t crysMult;
};
#endif

#ifdef Sortcode_cxx
#endif
