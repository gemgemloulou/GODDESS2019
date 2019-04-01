#ifndef Sortcode_h
#define Sortcode_h

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

#include "TBranch.h"
#include "TROOT.h"
#include <TVector3.h>
#include <TObject.h>

class Sortcode {

 public :
  TTree *fChain;
  Int_t fCurrent;

  Sortcode(){;} 
  //void SortData(char*, char*);
  void SortData(Int_t );
  Int_t j,i,k,m,n,s,q,g;
  
   Double_t vex, vcmsq, e3cm;
   Double_t SteveEx[48];
   Double_t v1, vcm, ecm;

  Double_t uSX3Pos[48];
  Double_t uSX3Theta[48];
  Double_t uSX3Energy[48];
  Double_t uSX3Ex[48];
  Int_t uSX3fStrip[48];
  Int_t uSX3bStrip[48];
  Int_t uSX3mult;

  Double_t dSX3Pos[48];
  Double_t dSX3Theta[48];
  Double_t dSX3Energy[48];
  Double_t dSX3Ex[48];
  Int_t dSX3fStrip[48];
  Int_t dSX3bStrip[48];
  Int_t dSX3mult;

 
  Double_t uQTheta[48]; // This should probably be higher but it's v unlikely
  Double_t uQEnergy[48];
  Double_t uQEx[48];
  Double_t uQExAR[48];
  Int_t uQfStrip[48];
  Int_t uQbStrip[48];
  Int_t uQmult; 

  Double_t dQTheta[48]; // This should probably be higher but it's v unlikely
  Double_t dQEnergy[48];
  Double_t dQEx[48];
  Double_t Q[48];
  Int_t dQfStrip[48];
  Int_t dQbStrip[48];
  Int_t dQmult; 

  Double_t SiEnergy[48];
  //Double_t pos[48];
  Double_t SiTheta[48];
  Double_t Ex[48];
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
  
  Double_t uSX3bgain[12][4];
  Double_t uSX3boffset[12][4];
  Double_t dSX3bgain[12][4];
  Double_t dSX3boffset[12][4];	

  Double_t uSX3posgain[12][4];
  Double_t uSX3posoffset[12][4];
  Double_t dSX3posgain[12][4];
  Double_t dSX3posoffset[12][4];

  Int_t uQQQ, dQQQ, uSX3, dSX3, dBB10;
  Double_t E3, p3;
       
  //maximum crys number = 44
  Int_t quadCut[11];
  Float_t gamE[44];
  Float_t edop[44];
  Float_t edopSeg[44];
  Float_t edopXtal[44];
  Int_t quadNum[44];
  Int_t crysNum[44];
  Float_t t0[44];
  Int_t crysMult;
  
  //Int_t quadMult;
};
#endif

#ifdef Sortcode_cxx
//TFile *infile = new TFile("/global/data1b/gretina/1707_data1b/Run0091/Run0091.root","READ");
//TFile *infile = new TFile(infilename,"READ");  
//	TTree *AnalysisTree = (TTree*)infile->Get("teb");
#endif
