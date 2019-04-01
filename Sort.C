//g++ Sort.C -std=c++0x -I../include -L../bin -L../../lib -lGoddess -lGRETINA `root-config --cflags --libs` -lTreePlayer -lSpectrum -lGODDESS -o SortData

#define Sortcode_cxx

#include "Sort.h"

using namespace std;

// put in a better method to parse the run number as an int, and tack it onto the path

//void Sortcode::SortData(char* infilename,char* outfilename){
void Sortcode::SortData(Int_t RunNum){


  // ***************************************************
  // ***** Call the exterminator! You've got bugs! *****
  /* *********** */ Bool_t iverb = 0; /* ************ */
  // ***************************************************
  // ***************************************************

  char runbuf[128];
  if(RunNum<100){ 
    sprintf(runbuf, "/global/data1b/gretina/1707_data1b/Run00%d/Run00%d.root",RunNum,RunNum);
  }else{
    sprintf(runbuf, "/global/data1b/gretina/1707_data1b/Run0%d/Run0%d.root",RunNum,RunNum);
  }

 char outbuf[128];
  if(RunNum>202 && RunNum<232){
  sprintf(outbuf,"/global/data1b/gretina/1707_data1b/Histograms/CH2_hist%d.root",RunNum);
  }else{
  sprintf(outbuf,"/global/data1b/gretina/1707_data1b/Histograms/hist%d.root",RunNum);
  }

  TFile *infile = new TFile(runbuf,"READ");
  TTree *AnalysisTree = (TTree*)infile->Get("teb");
  TFile *outfile = new TFile(outbuf,"RECREATE");
 
  //  TTree *t = new TTree("t","analysis");

  TFile *fcuts = new TFile("/global/data1b/gretina/1707_data1b/cuts.root","READ");
  //Use the run number to reference which cut you need! Yeah boi
  char cutname[32];

  if(RunNum<74||RunNum==107||RunNum==110||RunNum==289||RunNum==331||RunNum==339){
    // not analysing runs prior to 74, rest are dud and I don't want complaints. 
    sprintf(cutname,"cut%d",101); // a random choice. Shrug
  }else{
    sprintf(cutname,"cut%d",RunNum);
  }

  TCutG *p30IC = (TCutG *)fcuts->Get(cutname);
  
  if(!p30IC){
    cout << "Cannae find yer IC cut for run "<<RunNum<<"! Using something potentially substandard." << endl;
    sprintf(cutname,"cut%d",101);
        cout << "cutname in broke loop " << cutname << endl;
    // TCutG *p30IC;
      p30IC = (TCutG *)fcuts->Get(cutname);
  }
  
  //----------------------------------------------------------------------------------------------------
  cout << "Reading in " << runbuf << endl;

  goddessOut *goddess = 0;
  g2OUT *gretina = 0;
  AnalysisTree->SetBranchAddress("godEv",&goddess);
  AnalysisTree->SetBranchAddress("g2",&gretina);
  
  Long64_t nentries = AnalysisTree->GetEntries();
 
  /* ************************************************************************************* */
  /* * * * * * * * * C A L I B R A T E  G O O D  T I M E S  C O M E  O N ! * * * * * * * * */
  /* ************************************************************************************* */
  /* * * * * * * * * * * * * I T ' S  A  C A L I B R A T I O N ! * * * * * * * * * * * * * */
  /* ************************************************************************************* */

  /* For gains and offsets, the notation is [up/down][det][f/b]gain[det num][strip num] */
  /* so, uSX3fgain[3][4] = gain of upstream SuperX3, detector 4/12, strip 5/8 */

  /* QQQ */

  ifstream QQQangle;
  QQQangle.open("QQQangle.dat");
  for(i=1;i<33;i++){
    QQQangle >> dQQQang[32-i] >> uQQQang[32-i]; // not dependent on detector number, but just pStrip. 
    // Reading these in back to front because GODDAMN IT STEVE
  }
  QQQangle.close();

  // A = 0, B = 1, C = 2, D = 3, 4 detectors, 4 back strips
  ifstream ubQQQcal;
  ubQQQcal.open("uQQQbCal.dat"); //
  for(i=0;i<16;i++){ 
    ubQQQcal >> uQbgain[i/4][i%4] >> uQboffset[i/4][i%4];
    if(iverb)  cout << "uQ gain["<<i/4<<"]["<<i%4<<"] = " << uQbgain[i/4][i%4];
    if(iverb)cout << "   uQ off["<<i/4<<"]["<<i%4<<"] = " << uQboffset[i/4][i%4] << endl;

  }
  ubQQQcal.close();
  
  ifstream ufQQQcal; // four detectors, 32 front strips 
  ufQQQcal.open("uQQQfCal.dat");
  for(i=0;i<126;i++){  
    ufQQQcal >> uQfgain[i/32][i%32] >> uQfoffset[i/32][i%32]; 
  }
  ufQQQcal.close();
  // --------------------------------------------------------------------------------------------------

  /* BB10 */
  ifstream BB10cal; // 8 detectors, 8 front strips
  BB10cal.open("BB10Cal.dat");
  for(i=0;i<64;i++){
    BB10cal >> BB10fgain[i/8][i%8] >> BB10foffset[i/8][i%8];
  }
  BB10cal.close();
  // --------------------------------------------------------------------------------------------------

  /* Super X3 */
  /* energies */ 
  ifstream uSX3b; uSX3b.open("uSX3bCal.dat");
  ifstream uSX3f; uSX3f.open("uSX3fEnergyCal.dat");
  ifstream dSX3b; dSX3b.open("dSX3bCal.dat");
  ifstream dSX3f; dSX3f.open("dSX3fEnergyCal.dat");
  
  // 12 detectors: 8 front contacts, 4 back strips
  // 8 coefficients for the contacts (near and far) 
  // Is a separate adjustment needed for combined strip E?

  for(i=0;i<96;i++){
    uSX3f >> uSX3fgain[i/8][i%8] >> uSX3foffset[i/8][i%8];
    dSX3f >> dSX3fgain[i/8][i%8] >> dSX3foffset[i/8][i%8];
  } 
    
  for(i=0;i<48;i++){
    uSX3b >> uSX3bgain[i/4][i%4] >> uSX3boffset[i/4][i%4];
    dSX3b >> dSX3bgain[i/4][i%4] >> dSX3boffset[i/4][i%4];
  }
  uSX3b.close();    
  uSX3f.close();
  dSX3b.close();
  dSX3f.close();

  /* positions */
    
  ifstream uSX3pos; uSX3pos.open("uSX3PosCal.dat");
  ifstream dSX3pos; dSX3pos.open("dSX3PosCal.dat");
    
  for(i=0;i<48;i++){
    uSX3pos >> uSX3posgain[i/12][i%12] >> uSX3posoffset[i/12][i%12];
    dSX3pos >> dSX3posgain[i/12][i%12] >> dSX3posoffset[i/12][i%12];
  }
  uSX3pos.close();
  dSX3pos.close();

  // --------------------------------------------------------------------------------------------------
  
  /* gretina */
 
  ifstream FquadCut; // this the high energy cut off where the quads go screwy
  FquadCut.open("QuadCutoff.dat");
  for(i=0;i<11;i++){
    FquadCut >> quadCut[i];
  }
  FquadCut.close();

  // -----------------------------------------------------------------------------------------------

  TH1F *hgam = new TH1F("gam","gamma energy",1000,0,10000);

  // AR
  TH1F *hCryI[11]; // crystal-by-crystal rest frame gammas
  for (int ii = 0; ii < 11; ii++ ) {
    hCryI[ii] = new TH1F(Form("hCR_%01i",ii),Form("rest frame gammas for crystal %01i",ii),
		     10000,0,10000);
  }
  //
  TH1F *hedop = new TH1F("edop","doppler corrected (basic) egam",1000,0,10000);
  TH1F *hedopSeg = new TH1F("edopSeg","doppler corrected (seg) egam",1000,0,10000);
  TH1F *hedopXtal = new TH1F("edopXtal","doppler corrected (xtal) egam",1000,0,10000);
  TH1F *hedop_IC = new TH1F("edop_IC","edop, 30P gate in IC",1000,0,10000);
  TH1F *hedop_ICSi = new TH1F("edop_ICSi","edop, 30P in IC, E>2MeV in all Si",1000,0,10000);
  TH1F *hedop_ICuQ = new TH1F("edop_ICuQ","edop, 30P in IC, E>2MeV in uQQQ",1000,0,10000);
  TH1F *htheta = new TH1F("theta","theta of silicon",180,0,180);
  TH1F *hSteve = new TH1F("Steve","edop, EQQQ>3MeV, ICE>200",2000,0,10000);

  TH2F *hSiEvTh = new TH2F("SiEvTh","E v Th for all Si",180,0,180,5000,0,10000);
  TH1F *hEx = new TH1F("Ex","Ex, all si",500,0,50);
  TH1F *huQEx = new TH1F("uQEx","Ex in uQQQ, 400<tdc0<1250, 30P IC cut",1000,0,20);
  TH2F *hQExTh = new TH2F("QExTh","Ex v Th, uQQQ, ic&tdc",180,0,180,500,0,50);
  TH2F *hICxy = new TH2F("ICxy","IC x vs y, ungated",32,0,32,32,0,32);
  TH2F *hIC_EdE = new TH2F("IC_EdE","IC E vs dE, ungated",400,0,2400,400,0,2400);
  TH2F *hIC_test = new TH2F("IC_test","IC E vs dE in gate",400,0,2400,400,0,2400);
 
  TH2F *huQEvTh = new TH2F("uQEvTh","E v Rnum for uQQQ",32,0,32,5000,0,10000);
  TH2F *huQEvBStrip = new TH2F("uQEvBStrip","E v Back Strip for uQQQ",16,0,16,5000,0,10000);
  TH1F *htdc0 = new TH1F("tdc0","IC stop delayed",400,0,4000);
  TH1F *htdc1 = new TH1F("tdc1","gamma stop tdc1",400,0,4000);
  TH1F *htdc2 = new TH1F("tdc2","gamma stop tdc 2",400,0,4000);
  TH1F *htdc3 = new TH1F("tdc3","RF -doesn't work for all runs",400,0,4000);
  TH1F *htdc4 = new TH1F("tdc4","Si stop delayed",400,0,4000);
  TH2F *ExEgam = new TH2F("ExEgam","particle Ex vs E gam dop. IC gate",1000,0,10000,500,0,50);
  TH2F *ExEgam1 = new TH2F("ExEgam1","particle Ex vs E gam dop. IC gate. SiMult==1",2000,0,10000,200,0,20);
  TH2F *ExEgamQ = new TH2F("ExEgamQ","particle Ex vs E gam dop for uQQQ only. IC gate",2000,0,10000,200,0,20);
  TH2F *hQvRing = new TH2F("QvRing","Q value vs Ring",32,0,32,500,0,50);
  TH2F *hQvRing_ICTDC = new TH2F("QvRing_ICTDC","Q value vs Ring, IC TDC",32,0,32,500,0,50);

 
  // --------------------------------------------------------------------------------------------------
  Double_t m1 = 29.978313489*931.494013; // mass of beam, 30P
  Double_t m2 = 2.0141077785*931.494013; // target mass, d
  Double_t m3 = 1.00782503207*931.494013; // ejectile mass, p
  Double_t m4 = 30.97376199863*931.494013; // recoil mass, 31P

  /*  // parameters in the SDP universe
  Double_t sdp_m1 = 29.978313489; // mass of beam, 30P
  Double_t sdp_m2 = 2.0141077785; // target mass, d
  Double_t sdp_m3 = 1.00782503207; // ejectile mass, p
  Double_t sdp_m4 = 30.97376199863; // recoil mass, 31P
  */

  // AR -- I calculate (with LISE) that a 8 MeV 30P^{15+} beam has an energy of
  // 236.237 MeV after going through a 300ug/cm2 thick CD2 target (midtarget energy)
  // Double_t T1 = 234.; // 8MeV/u, Patrick calculated loss in the target from SRIM - put in routine and do it properly
  Double_t T1 = 236.237; // Andrew's number 
  //  Double_t sdp_T1 = 236.237*1000.0; // Andrew's number in the SDP universe
  
  /* Double_t v1 = TMath::Sqrt((2.0*T1)/m1);                                                                       // beamV
  Double_t sdp_v1 = TMath::Sqrt((2.0*sdp_T1)/sdp_m1);
  Double_t vcm = (v1*m1)/(m1+m2);       // velocity of the CoM in the lab                                       // comV
  Double_t sdp_vcm = (sdp_v1*sdp_m1)/(sdp_m1+sdp_m2);       // velocity of the CoM in the lab
    
  //Double_t ecm = (0.5*m1*((v1-vcm)*(v1-vcm)))+(0.5*m2*vcm*vcm);
  Double_t ecm = (0.5*m1*TMath::Power(v1-vcm,2.0))+(0.5*m2*TMath::Power(vcm,2.0));                              // Ein
  //Double_t sdp_ecm = (0.5*m1*TMath::Power(sdp_v1-sdp_vcm,2.0))+(0.5*m2*TMath::Power(sdp_vcm,2.0));
  // found some missing SDP_
  Double_t sdp_ecm = (0.5*sdp_m1*TMath::Power(sdp_v1-sdp_vcm,2.0))+(0.5*sdp_m2*TMath::Power(sdp_vcm,2.0));*/


  Double_t E1 = T1+m1; //30P total energy
  Double_t Etot = E1+m2;  //total energy in the lab frame???
  Double_t Etotcm = sqrt((m1*m1)+(m2*m2)+2.*E1*m2);  // total energy in the CM frame

  Double_t p1 = sqrt(T1*T1+(2.0*m1*T1));

  // this is the velocity of the CM in the lab frame, but we want the velocity in the lab frame
  //Double_t beta = p1/Etot; cout << "beta = " << beta << endl;

  Double_t beta = p1/E1; cout << "beta = " << beta << endl;
  Double_t gamma = 1.0/sqrt(1.0-(beta*beta));

  Double_t r = 100.; // distance from target to inner barrel 
   
  SiMult = uSX3mult = dSX3mult = 0;
  uQmult = dQmult = 0;
 
  // ------------------------------------------------------------------------------------------- 
  for(Long64_t jentry=0;jentry<nentries;jentry++){
    AnalysisTree->GetEntry(jentry);
   
    //  cout << endl << "entry " << jentry << endl;
    // initialise for every event
    for(j=0;j<48;j++){
      uSX3Pos[j] = uSX3Theta[j] = uSX3Energy[j] = uSX3Ex[j] = uSX3fStrip[j] = uSX3bStrip[j] = 0;
      dSX3Pos[j] = dSX3Theta[j] = dSX3Energy[j] = dSX3Ex[j] = dSX3fStrip[j] = dSX3bStrip[j] = 0;
      uQTheta[j] = uQEnergy[j] = uQEx[j] = uQfStrip[j] = uQbStrip[j] = BackStrip[j] = 0;
      dQTheta[j] = dQEnergy[j] = dQEx[j] = dQfStrip[j] = dQbStrip[j] = 0;
      SiTheta[j] = SiEnergy[j] = Ex[j] = Q[j] = SteveEx[j] = 0;
    }

    for(j=0;j<44;j++){
      gamE[j] = edop[j] = edopSeg[j] = edopXtal[j] = 0;
      quadNum[j] = crysNum[j] = t0[j] = 0;
    }
  
    SiMult = uSX3mult = dSX3mult = 0;
    uQmult = dQmult = 0;
    crysMult = 0;
   

    // AR --- Betting real hard this should not be this way.
    //  - I thought you weren't a betting man, Andrew. And yet...
    // vcm = v1 = vcmsq = 0;
    //  quadMult = 0;

    // ------------------------------------------------------------------------------------------------
    if(iverb) cout << endl << "vector size check, entry " << jentry << endl;
    if(iverb) cout << "goddess size = " << goddess->si.size() << endl;
    //the size of the vector changes with jentry... it's a multiplicity!!
 

    // Ionisation chamber
    // branches are not a member of detectorOUT, so it doesn't need to be looped!
     
    hICxy->Fill(goddess->icX,goddess->icY);
    hIC_EdE->Fill(goddess->icDE,goddess->icE);
    if(p30IC->IsInside(goddess->icE,goddess->icDE)){
      if(iverb) cout << "entry "<<jentry<<" has a hit in the IC" << endl;
      hIC_test->Fill(goddess->icDE,goddess->icE);
    }
    
    htdc0->Fill(goddess->tdc0);
    htdc1->Fill(goddess->tdc1);
    htdc2->Fill(goddess->tdc2);
    htdc3->Fill(goddess->tdc3);
    htdc4->Fill(goddess->tdc4);

    // GODDESS Stuff
    Double_t xxxAR = 0.0;
    
    for(size_t i=0;i<goddess->si.size();i++){
      detectorOUT det = goddess->si.at(i);

      //*************************************************************
      //** Make sensible arrays rather than using h*cking strings! **
      //*************************************************************
      if(goddess->siID.at(i)=="UA_dE") uQQQ = 0;  if(goddess->siID.at(i)=="UB_dE") uQQQ = 1;
      if(goddess->siID.at(i)=="UC_dE") uQQQ = 2;  if(goddess->siID.at(i)=="UD_dE") uQQQ = 3;

      if(goddess->siID.at(i)=="D1_dE") dQQQ = 0; // maybe change these three
      if(goddess->siID.at(i)=="D1_E1") dQQQ = 1;
      if(goddess->siID.at(i)=="D1_E2") dQQQ = 2;

      if(goddess->siID.at(i)=="U0_E1") uSX3 = 0;   if(goddess->siID.at(i)=="U1_E1") uSX3 = 1;
      if(goddess->siID.at(i)=="U2_E1") uSX3 = 2;   if(goddess->siID.at(i)=="U3_E1") uSX3 = 3;
      if(goddess->siID.at(i)=="U4_E1") uSX3 = 4;   if(goddess->siID.at(i)=="U5_E1") uSX3 = 5;
      if(goddess->siID.at(i)=="U6_E1") uSX3 = 6;   if(goddess->siID.at(i)=="U7_E1") uSX3 = 7;
      if(goddess->siID.at(i)=="U8_E1") uSX3 = 8;   if(goddess->siID.at(i)=="U9_E1") uSX3 = 9;
      if(goddess->siID.at(i)=="U10_E1") uSX3 = 10; if(goddess->siID.at(i)=="U11_E1") uSX3 = 11;

      if(goddess->siID.at(i)=="D0_E1") dSX3 = 0;   if(goddess->siID.at(i)=="D1_E1") dSX3 = 1;
      if(goddess->siID.at(i)=="D2_E1") dSX3 = 2;   if(goddess->siID.at(i)=="D3_E1") dSX3 = 3; 
      if(goddess->siID.at(i)=="D4_E1") dSX3 = 4;   if(goddess->siID.at(i)=="D5_E1") dSX3 = 5;
      if(goddess->siID.at(i)=="D6_E1") dSX3 = 6;   if(goddess->siID.at(i)=="D7_E1") dSX3 = 7;
      if(goddess->siID.at(i)=="D8_E1") dSX3 = 8;   if(goddess->siID.at(i)=="D9_E1") dSX3 = 9;
      if(goddess->siID.at(i)=="D10_E1") dSX3 = 10; if(goddess->siID.at(i)=="D11_E1") dSX3 = 11;

      if(goddess->siID.at(i)=="D0_dE") dBB10 = 0;   if(goddess->siID.at(i)=="D1_dE") dBB10 = 1;
      if(goddess->siID.at(i)=="D2_dE") dBB10 = 2;   if(goddess->siID.at(i)=="D3_dE") dBB10 = 3;
      if(goddess->siID.at(i)=="D4_dE") dBB10 = 4;   if(goddess->siID.at(i)=="D5_dE") dBB10 = 5;
      if(goddess->siID.at(i)=="D6_dE") dBB10 = 6;   if(goddess->siID.at(i)=="D7_dE") dBB10 = 7;
      if(goddess->siID.at(i)=="D8_dE") dBB10 = 8;   if(goddess->siID.at(i)=="D9_dE") dBB10 = 9;
      if(goddess->siID.at(i)=="D10_dE") dBB10 = 10; if(goddess->siID.at(i)=="D11_dE") dBB10 = 11;

    
      if(iverb) cout << "Jentry = " << jentry << ", i = " << i << endl;
      
      //*************************************************************
      //************* Loop over detector types! *********************
      //*************************************************************

      if(goddess->siType.at(i) == "SuperX3" && det.upstream){ // UPSTREAM SX3
	//uSX3Energy[uSX3mult] = det.nECal; //  energy from backs
        uSX3Energy[uSX3mult] = (det.nECal*uSX3bgain[uSX3][det.nStrip])+uSX3boffset[uSX3][det.nStrip];
	// eNear = (det.eNearCal * gain) + offset;
	// eFar = (det.eFarCal * gain) + offset;
	uSX3fStrip[uSX3mult] = det.pStrip;
	uSX3bStrip[uSX3mult] = det.nStrip;
	uSX3Pos[uSX3mult] = (((det.eNearCal-det.eFarCal)/(det.eNearCal+det.eFarCal))*37.5)+37.5;
	uSX3Theta[uSX3mult] = (TMath::ATan(uSX3Pos[uSX3mult]/r) + (TMath::Pi())/2.)*(180./(TMath::Pi()));
	E3 = (uSX3Energy[uSX3mult]/1000) + m3; // can get overwritten
	p3 = TMath::Sqrt(TMath::Power(uQEnergy[uQmult]/1000.0,2.0) + 2.0*m3*uQEnergy[uQmult]/1000.0);
	uSX3Ex[uSX3mult] = (TMath::Sqrt((Etotcm*Etotcm + m3*m3)-2*(E3*Etot - p1*p3*TMath::Cos(uSX3Theta[uSX3mult]*TMath::Pi()/180.))) - m4);

	// add these things to an over Si array... (so index i for the goddess vector)
	SiEnergy[i] = uSX3Energy[uSX3mult];
	// pos[i] = uSX3Pos[uSX3mult];
	SiTheta[i] = uSX3Theta[uSX3mult];
	Ex[i] = uSX3Ex[uSX3mult];
	if(iverb) cout << "uSX3E["<<uSX3mult<<"] = " << uSX3Energy[uSX3mult] << ", but raw = " <<det.nECal << endl;
	if(iverb) cout << ", uSX3Th["<<uSX3mult<<"] = " << uSX3Theta[uSX3mult];
    
	uSX3mult++;
	SiMult++;
      }

      //*************************************************************

      else if(goddess->siType.at(i) == "SuperX3" && !(det.upstream)){ // DOWNSTREAM SX3
	//dSX3Energy[dSX3mult] = det.nECal; //  energy from backs
	dSX3Energy[dSX3mult] = (det.nECal*dSX3bgain[dSX3][det.nStrip])+dSX3boffset[dSX3][det.nStrip];
	//  eNear = (det.eNearCal * gain) + offset;
	// eFar = (det.eFarCal * gain) + offset;
	dSX3fStrip[dSX3mult] = det.pStrip;
	dSX3bStrip[dSX3mult] = det.nStrip;
	dSX3Pos[dSX3mult] = (((det.eNearCal-det.eFarCal)/(det.eNearCal+det.eFarCal))*37.5)-37.5;
	dSX3Theta[dSX3mult] = (TMath::ATan(dSX3Pos[dSX3mult]/r) + (TMath::Pi())/2.)*(180./(TMath::Pi()));
	E3 = (dSX3Energy[dSX3mult]/1000) + m3; // can get overwritten
	p3 = TMath::Sqrt(TMath::Power(uQEnergy[uQmult]/1000.0,2.0) + 2.0*m3*uQEnergy[uQmult]/1000.0);
	dSX3Ex[dSX3mult] = (TMath::Sqrt((Etotcm*Etotcm + m3*m3)-2*(E3*Etot - p1*p3*TMath::Cos(dSX3Theta[dSX3mult]*TMath::Pi()/180.))) - m4);

	// add these things to an over Si array... (so index i for the goddess vector)
	SiEnergy[i] = dSX3Energy[dSX3mult];
	// pos[i] = dSX3Pos[dSX3mult];
	SiTheta[i] = dSX3Theta[dSX3mult];
	Ex[i] = dSX3Ex[dSX3mult];
	if (iverb) cout << "dsx3, E["<<i<<"] = " << SiEnergy[i] << ", SiTheta["<<i<<"] = " << SiTheta[i] << endl;
	if(iverb) cout << "dSX3E["<<dSX3mult<<"] = " << dSX3Energy[dSX3mult];
	if(iverb) cout << ", dSX3Th["<<dSX3mult<<"] = " << dSX3Theta[dSX3mult];
	dSX3mult++;
	SiMult++;
      }

      //*************************************************************

      else if(goddess->siType.at(i) == "QQQ5" && det.upstream){ // UPSTREAM QQQ
	//uQEnergy[uQmult] = det.nECal;
	uQEnergy[uQmult] = (det.nECal*uQbgain[uQQQ][det.nStrip])+uQboffset[uQQQ][det.nStrip];
	uQbStrip[uQmult] = det.nStrip;
	BackStrip[uQmult] = det.nStrip + (uQQQ*4); // back strips numbered from 1 to 16, from uQQQ 0 to 3.  
	uQfStrip[uQmult] = det.pStrip;
	uQTheta[uQmult] = uQQQang[det.pStrip];
	if(iverb) cout << "uQEnergy["<<uQmult<<"] = " << uQEnergy[uQmult] << " (raw = " << det.nECal << ")";
	E3 = (uQEnergy[uQmult]/1000) + m3; // can get overwritten
	if(iverb) cout << ", E3 = " << E3;	
	p3 = TMath::Sqrt(TMath::Power(uQEnergy[uQmult]/1000.0,2.0) + 2.0*m3*uQEnergy[uQmult]/1000.0);
	if(iverb) cout << ", p3 = " << p3;

	uQEx[uQmult] = (TMath::Sqrt((Etotcm*Etotcm + m3*m3)-2*(E3*Etot - p1*p3*TMath::Cos(uQTheta[uQmult]*TMath::Pi()/180.))) - m4);
		
	if(iverb) cout << ", uQEx["<<uQmult<<"] = " << uQEx[uQmult] << endl;
	
	// add these things to an over Si array... (so index i for the goddess vector)	  
	SiEnergy[i] = uQEnergy[uQmult];
	//	  if(iverb) cout << "Inside! SiEnergy["<<i<<"] = " << SiEnergy[i] << endl;
	SiTheta[i] = (uQQQang[det.pStrip]);
	Ex[i] = uQEx[uQmult];
	
	/*	Q[uQmult] = m1+m2-m3-TMath::Sqrt((m1*m1)+(m2*m2)+(m3*m3)+(2*m2*E1)-2*E3*(E1+m2)+(2*p1*p3)*TMath::Cos(uQTheta[uQmult]*TMath::Pi()/180.));	
	vex = TMath::Sqrt(2*uQEnergy[uQmult]/1000/m3); //cout << "vex = " << vex;
	sdp_vex = TMath::Sqrt(2*uQEnergy[uQmult]/sdp_m3); //cout << "vex = " << vex;
	vcmsq = vcm*vcm + vex*vex - 2*vcm*vex*TMath::Cos(uQTheta[uQmult]*TMath::Pi()/180.); //cout << ", vcmsq = " << vcmsq;
	sdp_vcmsq = sdp_vcm*sdp_vcm + sdp_vex*sdp_vex - 2*sdp_vcm*sdp_vex*TMath::Cos(uQTheta[uQmult]*TMath::Pi()/180.); //cout << ", vcmsq = " << vcmsq;
	//e3cm = (0.5*m3 + m3*m3/m4)*vcmsq; //cout << ", e3cm = " << e3cm << endl;
	e3cm = 0.5*(m3 + m3*m3/m4)*vcmsq; //cout << ", e3cm = " << e3cm << endl;
	//sdp_e3cm = (0.5*sdp_m3 + sdp_m3*sdp_m3/sdp_m4)*sdp_vcmsq; //cout << ", e3cm = " << e3cm << endl;
	// I think this is a bug... this should be:
	sdp_e3cm = 0.5*(sdp_m3 + sdp_m3*sdp_m3/sdp_m4)*sdp_vcmsq; //cout << ", e3cm = " << e3cm << endl;
	//	SteveEx[uQmult] = ecm-e3cm + 10.0866; // previous version
	//cout << "sdp_ecm " << sdp_ecm << " sdp_e3cm " << sdp_e3cm << endl;
	//cout << Form("ecm %.04f\tsdp_ecm %.02f\n",ecm,sdp_ecm);
	//cout << Form("e3cm %.04f\tsdp_e3cm %.02f\n",e3cm,sdp_e3cm);
	//cout << vcm << "\t" << sdp_vcm << endl;
	//cout << vex << "\t" << sdp_vex << endl;
	//exit(1);
	SteveEx[uQmult] = (sdp_ecm-sdp_e3cm + 10086.6)/1000.0; // new version using sdp_ variables
	//cout << uQEx[uQmult] << "\t" << SteveEx[uQmult] << "\t" <<  SteveEx[uQmult]-uQEx[uQmult] << "\t" << Q[uQmult] << endl;
        //cout << "SteveEx["<<uQmult<<"] = " << SteveEx[uQmult] << endl;
	*/
	//	  if(iverb) cout << "uQQQ, E["<<i<<"] = " << SiEnergy[i] << ", SiTheta["<<i<<"] = " << SiTheta[i] << endl;
	if(iverb) cout << " uQE["<<uQmult<<"] = " << uQEnergy[uQmult];
	if(iverb) cout << ", uQTh["<<uQmult<<"] = " << uQTheta[uQmult];
	SiMult++;
	uQmult++;
      }

      //*************************************************************

      else if(goddess->siType.at(i) == "QQQ5" && !(det.upstream)){ // DOWNSTREAM QQQ
	//dQEnergy[dQmult] = det.nECal;
	 dQEnergy[dQmult] = (det.nECal*dQbgain[dQQQ][det.nStrip])+dQboffset[dQQQ][det.nStrip];
	dQbStrip[dQmult] = det.nStrip;
	dQfStrip[dQmult] = det.pStrip;
	dQTheta[dQmult] = dQQQang[det.pStrip];
	E3 = (dQEnergy[dQmult]/1000) + m3; // can get overwritten
	p3 = TMath::Sqrt(TMath::Power(uQEnergy[uQmult]/1000.0,2.0) + 2.0*m3*uQEnergy[uQmult]/1000.0);
	dQEx[dQmult] = (TMath::Sqrt((Etotcm*Etotcm + m3*m3)-2*(E3*Etot - p1*p3*TMath::Cos(dQTheta[dQmult]*TMath::Pi()/180.))) - m4);

	// add these things to an over Si array... (so index i for the goddess vector)	  
	SiEnergy[i] = dQEnergy[dQmult];
	// if(iverb) cout << "Inside! SiEnergy["<<i<<"] = " << SiEnergy[i] << endl;
	SiTheta[i] = (dQQQang[det.pStrip]);
	Ex[i] = dQEx[dQmult];
	// if(iverb) cout << "dQQQ, E["<<i<<"] = " << SiEnergy[i] << ", SiTheta["<<i<<"] = " << SiTheta[i] << endl;
	if(iverb) cout << "dQE["<<dQmult<<"] = " << dQEnergy[uQmult];
	if(iverb) cout << ", dQTh["<<dQmult<<"] = " << dQTheta[uQmult];
	SiMult++;
	dQmult++;
      }

      hSiEvTh->Fill(SiTheta[i],SiEnergy[i]);
      hEx->Fill(Ex[i]);
      htheta->Fill(SiTheta[i]);
      
    } // end goddess
   
   
    //When filling histograms here, loop over mult!
    for(m=0;m<uQmult;m++){
      huQEvTh->Fill(uQfStrip[m],uQEnergy[m]);
      huQEvBStrip->Fill(BackStrip[m],uQEnergy[m]);
    }


    // ---------------------------------------------------------------------------------------------
    // GRETINA Stuff
    for(size_t j=0;j<gretina->xtals.size();j++){
      g2CrystalEvent g2 = gretina->xtals.at(j);
       
      if(g2.cc < quadCut[g2.quadNum]){ // cut out high lying detector crap
	gamE[crysMult] = g2.cc;
	edop[crysMult] = g2.edop;
	edopSeg[crysMult] = g2.edopSeg;
	edopXtal[crysMult] = g2.edopXtal;
	quadNum[crysMult] = g2.quadNum;
	t0[crysMult] = g2.t0;
	if(iverb) cout << "E " << gamE[crysMult] << ", edp " << edop[crysMult] << endl;
	crysMult++;
      }
    }

    for(k=0;k<crysMult;k++){
      hgam->Fill(gamE[k]);
      hedop->Fill(edop[k]);
      hedopSeg->Fill(edopSeg[k]);
      hedopXtal->Fill(edopXtal[k]);
      if((goddess->icE)>200){
	for(n=0;n<uQmult;n++){
	  if(uQEnergy[n]>3000){
	    hSteve->Fill(edop[k]);
	    // AR
	    // subtract from quadNum[1] to fix the indexing
	    hCryI[quadNum[k]-1]->Fill(gamE[k]);
	  }
	}}   
       
      if(p30IC->IsInside(goddess->icE,goddess->icDE)){
       	if (iverb) cout << "entry " << jentry << " inside the fill loop" << endl;
	hedop_IC->Fill(edop[k]);
       	if (iverb) cout << "in QQQ mult = " << uQmult << endl << endl;
	for(m=0;m<uQmult;m++){
	  if(uQEnergy[m]>3000){
	    hedop_ICuQ->Fill(edop[k]);
	  }
	}
	if(iverb) cout << "inside fill Si mult = " << SiMult << endl;
	for(s=0;s<SiMult;s++){
	  if(SiEnergy[s]>2000){
	    hedop_ICSi->Fill(edop[k]);
	  }
	}
      }
    }
       
    // Everything in here gated on IC and TDC
    if(p30IC->IsInside(goddess->icE,goddess->icDE)){
      if((goddess->tdc0)>400 && (goddess->tdc0)<1250){
	for(q=0;q<uQmult;q++){
	  huQEx->Fill(uQEx[q]);
	  hQExTh->Fill(uQTheta[q],uQEx[q]);
	  hQvRing_ICTDC->Fill(uQfStrip[q],Q[q]);
	}
      for(s=0;s<SiMult;s++){
	for(k=0;k<crysMult;k++){
	  ExEgam->Fill(edop[k],Ex[s]);
	  ExEgam1->Fill(edop[k],Ex[0]); // only looking at 1st silicon
	}
      }
      for(q=0;q<uQmult;q++){
	for(k=0;k<crysMult;k++){
	  ExEgamQ->Fill(edop[k],uQEx[q]);
	}
      }
    }
    }


for(q=0;q<uQmult;q++){
    hQvRing->Fill(uQfStrip[q],Q[q]);
    }
  
    // t->Fill();	       
    // -----------------------------------------------------------------------------------------------
    if(jentry%100000 == 0) cout << setiosflags(ios::fixed) << "Entry " << jentry << " of " << nentries << ", " << 100 * jentry/nentries << "% complete" << "\r" << flush;   
  } // end of nentries

  cout << "Entry " << nentries << " of " << nentries << ", 100% Complete!\n";	

  


  // ------------------------------------------------------------------------------------------------
  outfile->cd();


  hgam->Write();
  hedop->Write();
  hedopSeg->Write();
  hedopXtal->Write();
	
  hedop_IC->Write();
  hedop_ICSi->Write();
  hedop_ICuQ->Write();

  htheta->Write();
  hSiEvTh->Write();
  huQEvTh->Write();
  huQEvBStrip->Write();
  hEx->Write();
  huQEx->Write();

  hQExTh->Write();

  hICxy->Write();
  hIC_EdE->Write();
  hIC_test->Write();
  hSteve->Write();
  htdc0->Write();
  htdc1->Write();
  htdc2->Write();
  htdc3->Write();
  htdc4->Write();
  ExEgam->Write();
  ExEgam1->Write();
  ExEgamQ->Write();
 
  hQvRing->Write();
  hQvRing_ICTDC->Write();
  
  //	t->Write();
  // AR
  for (int ii = 0; ii < 11; ii++) {
    hCryI[ii]->Write();
  }
  fcuts->Close();
  outfile->Close();

  }

int main(int argc, char** argv){ 
  Sortcode *mysort = new Sortcode();

  // mysort->SortData(argv[1],argv[2]);
 int num; 
 sscanf (argv[1],"%d",&num);
  mysort->SortData(num);
  return 0;
  
}

