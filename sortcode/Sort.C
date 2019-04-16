//g++ Sort.C -std=c++0x -I../include -L../bin -L../../lib -lGoddess -lGRETINA `root-config --cflags --libs` -lTreePlayer -lSpectrum -lGODDESS -o SortData

// J. Henderswan changes:
// Use MAXSILICON and MAXGRETINA to define size of arrays (MAXSILICON = 48, MAXGRETINA = 44, change in Sort.h)

#define Sortcode_cxx

#include "Sort.h"

using namespace std;

// Some static variables for the code
double Sortcode::m1 = 133.911369*931.494013; // mass of beam, 134Te
double Sortcode::m2 = 2.0141077785*931.494013; // target mass, d
double Sortcode::m3 = 1.00782503207*931.494013; // ejectile mass, p
double Sortcode::m4 = 134.91645 * 931.494013; // recoil mass, 135Te
double Sortcode::PI = 4.0*atan(1.0); // Pi is a very special number, used by clever people who like circles. Basically just 3. B.S.J.
double Sortcode::T1 = 1192.; // 8.9 MeV/u for 134Te. Not corrected yet for losses in the target


//double Sortcode::m1_i = 133.909776*931.494013; // mass of beam, 134I
//double Sortcode::m4_i = 134.9100594*931.494013; // recoil mass, 135I

void Sortcode::SortData(Int_t RunNum){

  std::string dNames[5] = {"uQQQ","dQQQ","uSX3","dSX3","dBB10"};

  // ***************************************************
  // ***** Call the exterminator! You've got bugs! *****
  /* *********** */ Bool_t iverb = 0; /* ************ */
  /* ********* */ Bool_t iverbGam = 0; /* *********** */
  // ***************************************************
  // ***************************************************

  char runbuf[128];
 if(RunNum<10){ 
     sprintf(runbuf, "/global/data1b/gretina/1484_data1b/Run000%d/Run000%d.root",RunNum,RunNum); // 134Te
  } else if(RunNum<100){ 
     sprintf(runbuf, "/global/data1b/gretina/1484_data1b/Run00%d/Run00%d.root",RunNum,RunNum); // 134Te
  }else{
     sprintf(runbuf, "/global/data1b/gretina/1484_data1b/Run0%d/Run0%d.root",RunNum,RunNum); 
  }

  char outbuf[128];
  
  if((RunNum>49 && RunNum<56)||(RunNum>70 && RunNum<75)||(RunNum==257)||(RunNum==305)){
    sprintf(outbuf,"/global/data1b/gretina/1484_data1b/Histograms/bkgd_hist%d.root",RunNum);
  }else if(RunNum==390){
    sprintf(outbuf,"/global/data1b/gretina/1484_data1b/Histograms/notarget_hist%d.root",RunNum);
  }else if(RunNum==389||RunNum>391){
    sprintf(outbuf,"/global/data1b/gretina/1484_data1b/Histograms/CH2_hist%d.root",RunNum);
  }else{
   sprintf(outbuf,"/global/data1b/gretina/1484_data1b/Histograms/hist%d.root",RunNum);
      }
     
     
  TFile *infile = new TFile(runbuf,"READ");
  TTree *AnalysisTree = (TTree*)infile->Get("teb");
  TFile *outfile = new TFile(outbuf,"RECREATE");

   TFile *fcuts = new TFile("/global/data1b/gretina/1484_data1b/cuts.root","READ"); 
   TFile *dc = new TFile("/global/data1b/gretina/1484_data1b/dirkcut.root","READ"); 
   TFile *gc = new TFile("/global/data1b/gretina/1484_data1b/globalCuts.root","READ");

   /* ************************************************************************************* */
   /* A note about the cuts! Cuts on the 134te peak have been done run by run - these are   */
   /* read in from cuts.root, they're called cut[runNum] and is called CutIC throughout the */ 
   /* code. These cuts 'contain' uncorrected ICdE and ICE values, as they're run by run and */
   /* subject to shifting around. We're still kinda using these for now, and that's fine.   */
   /* ************************************************************************************* */
   /* Andrew Ratkiewicz wrote a script that essentially gain matches the E and the dE for   */
   /* the IC. If the gains are read in run by run, the gain shifted E and dE in the IC will */
   /* all line up. Then we can use One Cut To Fit Them All, which are stored in the file    */
   /* globalCuts.root. When checking if an event is inside these cuts, you MUST use the     */
   /* gainmatched E and dE. Otherwise it could be anywhere. We're working towards using one */
   /* system, but I want it to be foolproof. (I realise using two systems simultaneously is */
   /* probably a bad idea, but hey ho!)                                                     */ 
   /*        - Gemma, 4/4/19 (Hey look! A date that's correct in the UK and the US!)        */
   /* ************************************************************************************* */

   // read in Dirk cuts
   TCutG *cutdirk = (TCutG *)dc->Get("dirk");
   TCutG *cutundirk = (TCutG *)dc->Get("undirk");

   // read in global IC cuts
   TCutG *cut_134Te = (TCutG *)gc->Get("te134"); // IC cut one locus out of the 4 grouped together (gainmatched dE, E values)
   TCutG *cut_134I = (TCutG *)gc->Get("i134"); // IC cut, the one immediately to the left of ^^ (gainmatched dE, E values)
   TCutG *cut_tedp = (TCutG *)gc->Get("tedp"); // proton kinematic lines from energy v theta from upstream Si
   TCutG *cut_whole = (TCutG *)gc->Get("te134_whole"); // IC cut on the whole sausage (gainmatched dE, E values)

   if(!cutdirk) cout << "no dirk" << endl;
   if(!cutundirk) cout << "no undirk" << endl;


  //Use the run number to reference which cut you need! Yeah boi
  char cutname[32];

    if(RunNum==4){
    sprintf(cutname,"cut%d",16);
  }else{
    sprintf(cutname,"cut%d",RunNum);
    }

// *********************************************************************************************
  TCutG *cutIC = (TCutG *)fcuts->Get(cutname);
  
  if(!cutIC){
    cout << "Cannae find yer IC cut for run "<< RunNum <<"! Attempting to use a cut from the previous run: ";
    sprintf(cutname,"cut%d",(RunNum-1));
    cutIC = (TCutG *)fcuts->Get(cutname);
    cout << cutname << endl;
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

  /* Andrew's IC calibration */

  double IC_gain[450][2];
  int temp1;
  double temp2, temp3;

  ifstream ic_cal("CalFiles/ic_cor.dat",ios::in);
  // intialize the IC gains, so if there's no gain in the file for that run nothing happens to the centroid
  for (i = 0; i < 450; i++) {
    IC_gain[i][0] = 1.0;
    IC_gain[i][1] = 1.0;      
  }

  if (!ic_cal) {
    printf("couldn't open file at %s","CalFiles/ic_cor.dat");
    exit(1);
  } else {
    // this will read forever (or until it runs out of spaces in IC_gain).
    while (ic_cal >> temp1 >> temp2 >> temp3 ) {
      IC_gain[temp1][0] = temp2; // dE
      IC_gain[temp1][1] = temp3; // E

      //  cout << "IC_gain["<<temp1<<"][0] = " << temp2 << ", IC_gain["<<temp1<<"][1] = " << temp2 << endl;
    }
  }

  ic_cal.close();
 
  /* For gains and offsets, the notation is [up/down][det][f/b]gain[det num][strip num] */
  /* so, uSX3fgain[3][4] = gain of upstream SuperX3, detector 4/12, strip 5/8 */

  /* QQQ */

  ifstream QQQangle;
  QQQangle.open("CalFiles/QQQangle.dat");
  for(i=1;i<33;i++){
    QQQangle >> dQQQang[32-i] >> uQQQang[32-i]; // not dependent on detector number, but just pStrip. 
    // Reading these in back to front because GODDAMN IT STEVE
  }
  QQQangle.close();
  
  ifstream QQQphi;
  QQQphi.open("CalFiles/QQQphi.dat");
  for(i=0;i<16;i++){
    QQQphi >> uQQQphi[i/4][i%4]; // Guess at the phi at the centre of each upstream QQQ back strip  
  }
  QQQphi.close();

  // A = 0, B = 1, C = 2, D = 3, 4 detectors, 4 back strips
  ifstream ubQQQcal;
  ubQQQcal.open("CalFiles/uQQQbCal.dat"); //
  for(i=0;i<16;i++){ 
    ubQQQcal >> uQbgain[i/4][i%4] >> uQboffset[i/4][i%4];
    if(iverb)  cout << "uQ gain["<<i/4<<"]["<<i%4<<"] = " << uQbgain[i/4][i%4];
    if(iverb)cout << "   uQ off["<<i/4<<"]["<<i%4<<"] = " << uQboffset[i/4][i%4] << endl;

  }
  ubQQQcal.close();
  
  ifstream ufQQQcal; // four detectors, 32 front strips 
  ufQQQcal.open("CalFiles/uQQQfCal.dat");
  for(i=0;i<126;i++){  
    ufQQQcal >> uQfgain[i/32][i%32] >> uQfoffset[i/32][i%32]; 
  }
  ufQQQcal.close();


  // --------------------------------------------------------------------------------------------------

  /* BB10 */
  ifstream BB10cal; // 8 detectors, 8 front strips
  BB10cal.open("CalFiles/BB10Cal.dat");
  for(i=0;i<64;i++){
    BB10cal >> BB10fgain[i/8][i%8] >> BB10foffset[i/8][i%8];
  }
  BB10cal.close();
  // --------------------------------------------------------------------------------------------------

  /* Super X3 */
  /* energies */ 
  ifstream uSX3b; uSX3b.open("CalFiles/uSX3bCal.dat");
  ifstream uSX3f; uSX3f.open("CalFiles/uSX3fEnergyCal.dat");
  ifstream dSX3b; dSX3b.open("CalFiles/dSX3bCal.dat");
  ifstream dSX3f; dSX3f.open("CalFiles/dSX3fEnergyCal.dat");
  
  // 12 detectors: 8 front contacts, 4 back strips
  // 8 coefficients for the contacts (near and far) 
  // Is a separate adjustment needed for combined strip E?

  for(i=0;i<96;i++){
    uSX3f >> uSX3fgain[i/8][i%8] >> uSX3foffset[i/8][i%8];
    dSX3f >> dSX3fgain[i/8][i%8] >> dSX3foffset[i/8][i%8];
  } 
    
  // for the back strips, there's a different gain/offset for each combination
  // of front strip/back strip firing, so 16 per detector. Ordering is:
  // [det][F][B]:  [det][0][0], [det][0][1], [det][0][2], [det][0][3], [det][1][0] etc etc
  // This is the case for just the upstream SX3s for now... 
  // for(i=0;i<48;i++){
  // for(j=0;j<4;j++){
  //  uSX3b >> uSX3bgain[i/4][i%4][j] >> uSX3boffset[i/4][i%4][j];
  //  }
  // }
  // *** This has been commented out until we have the calibration for all detectors

  // rough back strip calibration - i.e. no cross talk effects! 
  for(i=0;i<48;i++){
    uSX3b >> uSX3bgain[i/4][i%4] >> uSX3boffset[i/4][i%4];
  }
  for(i=0;i<48;i++){
    dSX3b >> dSX3bgain[i/4][i%4] >> dSX3boffset[i/4][i%4];
  }
  uSX3b.close();    
  uSX3f.close();
  dSX3b.close();
  dSX3f.close();

  /* positions */
    
  ifstream uSX3pos; uSX3pos.open("CalFiles/uSX3PosCal.dat");
  ifstream dSX3pos; dSX3pos.open("CalFiles/dSX3PosCal.dat");
    
  for(i=0;i<MAXSILICON;i++){
    uSX3pos >> uSX3L[i/4][i%4] >> uSX3R[i/4][i%4];
    dSX3pos >> dSX3L[i/4][i%4] >> dSX3R[i/4][i%4];
  }
  uSX3pos.close();
  dSX3pos.close();

  // --------------------------------------------------------------------------------------------------
  
  /* gretina */
 
  ifstream FquadCut; // this the high energy cut off where the quads go screwy
  FquadCut.open("CalFiles/QuadCutoff.dat");
  for(i=0;i<11;i++){
    FquadCut >> quadCut[i];
  }
  FquadCut.close();

  // -----------------------------------------------------------------------------------------------

  TList *rawGamList = new TList;
  TH1F *hgam = new TH1F("gam","gamma energy",5000,0,10000); rawGamList->Add(hgam);

  // AR
  TH1F *hCryI[11]; // crystal-by-crystal rest frame gammas
  for (int ii = 0; ii < 11; ii++ ) {
    hCryI[ii] = new TH1F(Form("hCR_%01i",ii),Form("rest frame gammas for crystal %01i",ii),
		     10000,0,10000); rawGamList->Add(hCryI[ii]);
  }
  
  TH1F *hedop = new TH1F("edop","doppler corrected (basic) egam",5000,0,10000); rawGamList->Add(hedop);
  TH1F *hedopSeg = new TH1F("edopSeg","doppler corrected (seg) egam",5000,0,10000); rawGamList->Add(hedopSeg);
  TH1F *hedopXtal = new TH1F("edopXtal","doppler corrected (xtal) egam",5000,0,10000); rawGamList->Add(hedopXtal);
  TH1F *hedop_maxInt = new TH1F("edop_maxInt","doppler corrected egam using max e int point",5000,0,10000); rawGamList->Add(hedop_maxInt);
  TH1F *hedop_IC = new TH1F("edop_IC","edop, gate in IC",5000,0,10000);
  TH1F *hedop_ICTDC = new TH1F("edop_ICTDC","edop, IC and TDC gate",5000,0,10000);
  TH1F *hedopMI_IC = new TH1F("edopMI_IC","edop (using max int), IC gate",5000,0,10000);
  TH1F *hedop_ICTDCSi = new TH1F("edop_ICTDCSi","edop, IC, E > 1MeV in all Si and TDC gate",5000,0,10000);
  TH1F *hedop_ICTDCSiBk = new TH1F("edop_ICTDCSiBk","edop, IC, E > 1MeV in all Si and bkgd TDC gate",5000,0,10000);
  TH1F *hedop_ICbkTDC = new TH1F("edop_ICbkTDC","edop, good IC gate but bkgd TDC 1300<tdc0<2150 ",5000,0,10000);
  TH1F *hedop_ICSi = new TH1F("edop_ICSi","edop, IC, E>2MeV in all Si",5000,0,10000);
  TH1F *hedop_ICuQ = new TH1F("edop_ICuQ","edop, IC, E>2MeV in uQQQ",5000,0,10000);
  TH1F *hedopuQ_ICTDC = new TH1F("edopuQ_ICTDC","edop, IC, TDC, E>2MeV in uQQQ",5000,0,10000);
  TH1F *hedop_dSX3 = new TH1F("edop_dSX3","doppler corrected egam, dSX3>2MeV",5000,0,10000);
 
  TH1F *htheta = new TH1F("theta","silicon theta",180,0,180);
  TH1F *hSteve = new TH1F("Steve","edop, EQQQ>3MeV, ICE>200",2000,0,10000);
  TH1F *hEBE_egam = new TH1F("EBE_egam","event by event Edop, uQQQ, TDC & IC",2000,0,10000);
  TH2F *hDirk = new TH2F("Dirk","Dirk gate",60,60,120,8000,0,16000);

  TH2F *hpos = new TH2F("pos","pos",1000,-3,3,100,0,100);

  TH1F *hedop_ICdirk = new TH1F("edop_ICdirk","doppler corrected Egam, IC and dirk gate",5000,0,10000);
  TH1F *hedop_ICbkdirk = new TH1F("edop_ICbkdirk","doppler corrected Egam, IC and background dirk gate",5000,0,10000);
 
  TH2F *hGamTdc2 = new TH2F("GamTdc2","Gamma energy vs TDC2",1000,0,2000,1100,-400,4000);

  TH2F *hSiEvTh = new TH2F("SiEvTh","E v Th for all Si",180,0,180,5000,0,20000);
  TH2F *hSiEvTh_IC = new TH2F("SiEvTh_IC","E v Th for all Si, IC gate",180,0,180,5000,0,20000);
  TH2F *hSiEvTh_ICTDC = new TH2F("SiEvTh_ICTDC","E v Th for all Si, IC & TDC",180,0,180,5000,0,20000);
  TH2F *hSiEvTh_wlIC = new TH2F("SiEvTh_wlIC","E v Th for all Si, whole locus IC gate",180,0,180,5000,0,20000);
  TH2F *hSiEvTh_wlICTDC = new TH2F("SiEvTh_wlICTDC","E v Th for all Si, whole locus IC & TDC",180,0,180,5000,0,20000);
  TH2F *hSiEvTh_WIC = new TH2F("SiEvTh_WIC","E v Th for all Si, whole IC gate",180,0,180,5000,0,20000);
  TH2F *hSiEvTh_WICTDC = new TH2F("SiEvTh_WICTDC","E v Th for all Si, whole IC & TDC",180,0,180,5000,0,20000);

  TH1F *hEx = new TH1F("Ex","Ex, all si",500,-10,40);
  TH1F *hEx_upstream = new TH1F("Ex_upstream","Ex_upstream",500,-10,40);
  TH1F *hEx_upstream_ICTDC = new TH1F("Ex_upstream_ICTDC","Ex_upstream_ICTDC",500,-10,40);
  TH1F *huQEx = new TH1F("uQEx","Ex in uQQQ, 500<tdc0<900, IC cut",100,-4,14);
  TH1F *huQEx_bg = new TH1F("uQEx_bg","Time-random background Ex in uQQQ, 1300<tdc0<2150, IC cut",100,-4,14);
  TH1F *huSXEx = new TH1F("uSXEx","Ex in uSX3, 500<tdc0<900, IC cut",100,-4,14);
  TH1F *huSXEx_bg = new TH1F("uSXEx_bg","Time-random background Ex in uSX3, 1300<tdc0<2150, IC cut",100,-4,14);

  TH2F *hICxy = new TH2F("ICxy","IC x vs y, ungated",32,0,32,32,0,32);
  TH2F *hIC_EdE = new TH2F("IC_EdE","IC E vs dE, ungated",600,0,4800,600,0,4800);
  TH2F *hIC_test = new TH2F("IC_test","IC E vs dE in gate",600,0,4800,600,0,4800);
  TH2F *hICcorrected = new TH2F("ICcorrected","IC corrected with Andrew's method",600,0,4800,600,0,4800);
  TH2F *hICCor_EdE_tdc0 = new TH2F("ICCor_EdE_tdc0","IC E vs dE, gated on tdc0 between 560 and 630",600,0,4800,600,0,4800);

  TH2F *huQEvTh = new TH2F("uQEvTh","E v Ringnum for uQQQ",32,0,32,5000,0,10000);
  TH2F *huQEvBStrip = new TH2F("uQEvBStrip","E v Back Strip for uQQQ",16,0,16,5000,0,10000);

  TH2F *hExvicE = new TH2F("ExvicE","Ex vs E in IC",600,0,4800,500,-10,40);

  TH1F *htdc0 = new TH1F("tdc0","IC stop delayed",400,0,4000);
  TH1F *htdc1 = new TH1F("tdc1","gamma stop tdc1",400,0,4000);
  TH1F *htdc2 = new TH1F("tdc2","gamma stop tdc 2",400,0,4000);
  TH1F *htdc3 = new TH1F("tdc3","RF -doesn't work for all runs",400,0,4000);
  TH1F *htdc4 = new TH1F("tdc4","Si stop delayed",400,0,4000);

  TH2F *hExEgam = new TH2F("ExEgam","Ex vs Edop all Si, ungated",5000,0,10000,140,0,14);
  TH2F *hExEgam1 = new TH2F("ExEgam1","Ex vs Edop all Si, ungated, SiMult==1",2000,0,10000,140,0,14);
  TH2F *hExEgam_IC = new TH2F("ExEgam_IC","Ex vs Edop all Si, IC gate",5000,0,10000,140,0,14);
  TH2F *hExEgam_ICTDC = new TH2F("ExEgam_ICTDC","Ex vs Edop all Si, IC,TDC gate",5000,0,10000,140,0,14);
  TH2F *hExTh = new TH2F("hExTh","Ex vs Theta, all Silicon",180,0,180,140,0,14);

  TH2F *hExEgamuQ = new TH2F("ExEgamuQ","Ex vs Edop uQQQ, ungated",5000,0,10000,140,0,14);
  TH2F *hExEgamuQ_IC = new TH2F("ExEgamuQ_IC","Ex vs Edop uQQQ, IC gate",5000,0,10000,140,0,14);
  TH2F *hExEgamuQ_ICTDC = new TH2F("ExEgamuQ_ICTDC","Ex vs Edop uQQQ, IC,TDC gate",5000,0,10000,140,0,14);
  TH2F *hExThuQ = new TH2F("ExThuQ","Ex v Th, uQQQ, IC&TDC",180,0,180,140,0,14);
  
  TH2F *hEBE_ExEgamQ = new TH2F("EBE_ExEgamQ","Ex vs Edop(EBE) uQQQ, ungated",5000,0,10000,140,0,14);
  TH2F *hEBE_ExEgamQ_IC = new TH2F("EBE_ExEgamQ_IC","Ex vs Edop(EBE) uQQQ, IC gate",5000,0,10000,140,0,14);
  TH2F *hEBE_ExEgamQ_ICTDC = new TH2F("EBE_ExEgamQ_ICTDC","Ex vs Edop (EBE) uQQQ, IC,TDC gate",5000,0,10000,140,0,14);

  TH2F *hExEgamdQ = new TH2F("ExEgamdQ","Ex vs Edop dQQQ, ungated",5000,0,10000,140,0,14);
  TH2F *hExEgamdQ_IC = new TH2F("ExEgamdQ_IC","Ex vs Edop dQQQ, IC gate",5000,0,10000,140,0,14);
  TH2F *hExEgamdQ_ICTDC = new TH2F("ExEgamdQ_ICTDC","Ex vs Edop dQQQ, IC,TDC gate",5000,0,10000,140,0,14);
  TH2F *hExThdQ = new TH2F("ExThdQ","Ex v Th, dQQQ, IC&TDC",180,0,180,140,0,14);

  TH2F *hExEgamuSX = new TH2F("ExEgamuSX","Ex vs Edop uSX3, ungated",5000,0,10000,140,0,14);
  TH2F *hExEgamuSX_IC = new TH2F("ExEgamuSX_IC","Ex vs Edop uSX3, IC gate",5000,0,10000,140,0,14);
  TH2F *hExEgamuSX_ICTDC = new TH2F("ExEgamuSX_ICTDC","Ex vs Edop uSX3, IC,TDC gate",5000,0,10000,140,0,14);
  TH2F *hExThuSX = new TH2F("ExThuSX","Ex v Th, uSX3, IC&TDC",180,0,180,140,0,14);

  TH2F *hExEgamdSX = new TH2F("ExEgamdSX","Ex vs Edop dSX3, ungated",5000,0,10000,140,0,14);
  TH2F *hExEgamdSX_IC = new TH2F("ExEgamdSX_IC","Ex vs Edop dSX3, IC gate",5000,0,10000,140,0,14);
  TH2F *hExEgamdSX_ICTDC = new TH2F("ExEgamdSX_ICTDC","Ex vs Edop dSX3, IC,TDC gate",5000,0,10000,140,0,14);
  TH2F *hExThdSX = new TH2F("ExThdSX","Ex v Th, dSX3, IC&TDC",180,0,180,140,0,14);

  TH2F *hTdc0Tdc4 = new TH2F("TDC0TDC4","tdc0 v tdc4, Si gated",1100,-400,4000,1100,-400,4000);

  TH2F *hFB_uQ = new TH2F("FB_uQ","Front vs back, upstream QQQ",512,0,8192,512,0,8192);
  TH2F *hFB_uSX = new TH2F("FB_uSX","Front vs back, upstream SX3",512,0,8192,512,0,8192);
  TH2F *hFB_dSX = new TH2F("FB_dSX","Front vs back, downstream SX3",512,0,8192,512,0,8192);

  TH2F *hT0_Ex = new TH2F("T0_Ex","TDC0 vs Ex in uQQQs and uSX3s",500,-2,18,500,0,4000);
  TH1F *htdc0_dp = new TH1F("tdc0_dp","IC stop delayed, with gate on 134Te(d,p)",400,0,4000);

// ------------------------------------------------------------------------------------------- 
  Double_t E1 = T1+m1; //134Te total energy
  Double_t Etot = E1+m2;  //total energy in the lab frame???
  Double_t Etotcm = sqrt((m1*m1)+(m2*m2)+2.*E1*m2);  // total energy in the CM frame

  // cout << "E te = " << E1 << ", E1_i " << E1_i << ", Etot " << Etot << ", Etot_i " << Etot_i << endl;
  // cout << "Etotcm " << Etotcm << ", Etotcm_i " <<Etotcm_i << endl; 

  TGraph *kinematics = MakeCatKinematics(0);
  kinematics->SetName("Kinematics_GS");
  TGraph *kinematics_1 = MakeCatKinematics(1);
  kinematics_1->SetName("Kinematics_1MeV");
  TGraph *kinematics_2 = MakeCatKinematics(2);
  kinematics_2->SetName("Kinematics_2MeV");

  Double_t p1 = sqrt(T1*T1+(2.0*m1*T1));
  //  cout << "p1 = " << p1 << ", p1_i = " << p1_i << endl;
  Double_t r = 100.; // mm, distance from target to inner barrel 
   
  SiMult = uSX3mult = dSX3mult = 0;
  uQmult = dQmult = 0;
 
  // ------------------------------------------------------------------------------------------- 
  // LOOP OVER ALL EVENTS
  for(Long64_t jentry=0;jentry<nentries;jentry++){
   
    AnalysisTree->GetEntry(jentry);
   
    // initialise to zero for every event
    for(j=0;j<MAXSILICON;j++){
      uSX3Pos[j] = uSX3Theta[j] = uSX3Energy[j] = uSX3Ex[j] = 0.;
      uSX3fStrip[j] = uSX3bStrip[j] = 0;
      dSX3Pos[j] = dSX3Theta[j] = dSX3Energy[j] = dSX3Ex[j] = 0.;
      dSX3fStrip[j] = dSX3bStrip[j] = 0;
      uQTheta[j] = uQEnergy[j] = uQEx[j] = 0.;
      uQfStrip[j] = uQbStrip[j] = BackStrip[j] = 0;
      dQTheta[j] = dQEnergy[j] = dQEx[j] = 0.;
      dQfStrip[j] = dQbStrip[j] = 0;
      SiTheta[j] = SiEnergy[j] = Ex[j] = 0.;
      uQE3[j] = uQp3[j] = dQE3[j] = dQp3[j] = 0.;
      uSX3E3[j] = uSX3p3[j] = dSX3E3[j] = dSX3p3[j] = 0.; 
      UpEnergy[j] = UpTheta[j] = UpEx[j] = 0.;
    }

    for(j=0;j<MAXGRETINA;j++){
      gamE[j] = edop[j] = edopSeg[j] = edopXtal[j] = 0;
      quadNum[j] = crysNum[j] = t0[j] = 0;
    }
  
    SiMult = uSX3mult = dSX3mult = 0;
    uQmult = dQmult = 0;
    crysMult = 0;
    UpMult = 0;

  // ------------------------------------------------------------------------------------------------
    // if(iverb) cout << endl << "vector size check, entry " << jentry << endl;
    // if(iverb) cout << "goddess size = " << goddess->si.size() << endl;
    //the size of the vector changes with jentry... it's a multiplicity!!
 

    // Ionisation chamber
    // branches are not a member of detectorOUT, so it doesn't need to be looped!

    
   
    // derp derp derp
    if (( goddess->tdc0 > 560.0 ) && ( goddess->tdc0 < 630.0 )) {
      hICCor_EdE_tdc0->Fill(((goddess->icDE)*IC_gain[RunNum][0]),((goddess->icE)*IC_gain[RunNum][1]));
    }


    Double_t icEcorr = (goddess->icE)*IC_gain[RunNum][1];
    Double_t icdEcorr = (goddess->icDE)*IC_gain[RunNum][0];
    //cout << RunNum << endl << endl;
    //   cout << "icEcorr = " << goddess->icE << " x " << IC_gain[RunNum][1] << " = " << icEcorr << endl;
    //  cout << "icdEcorr = " << goddess->icDE << " x " << IC_gain[RunNum][0] << " = " << icdEcorr << endl;
    //hICcorrected->Fill(((goddess->icE)*IC_gain[RunNum][1]),((goddess->icDE)*IC_gain[RunNum][0]));

    hICcorrected->Fill(icEcorr,icdEcorr);
    hICxy->Fill(goddess->icX,goddess->icY);
    hIC_EdE->Fill(goddess->icE,goddess->icDE);
    if(cutIC->IsInside(goddess->icE,goddess->icDE)){
      if(iverb) cout << "entry "<<jentry<<" has a hit in the IC" << endl;
      hIC_test->Fill(goddess->icDE,goddess->icE);
    }
    
    htdc0->Fill(goddess->tdc0);
    htdc1->Fill(goddess->tdc1);
    htdc2->Fill(goddess->tdc2);
    htdc3->Fill(goddess->tdc3);
    htdc4->Fill(goddess->tdc4);
 // ------------------------------------------------------------------------------------------------

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

      
      //*************************************************************
      //************* Loop over detector types! *********************
      //*************************************************************

      if(goddess->siType.at(i) == "SuperX3" && det.upstream){ // UPSTREAM SX3

	uSX3Energy[uSX3mult] = (det.nECal*uSX3bgain[uSX3][det.nStrip])+uSX3boffset[uSX3][det.nStrip];


	
	// for cross talk effects, gain[det][front/p][back/n] << still not included, as we don't have a calibration for all detectors yet!
	// uSX3Energy[uSX3mult] = (det.nECal*uSX3bgain[uSX3][det.pStrip][det.nStrip])+uSX3boffset[uSX3][det.pStrip][det.nStrip];
	//cout << "uSX3E[" << uSX3 << "]["<<det.pStrip<<"]["<<det.nStrip<<"] = " << det.nECal <<" x " << uSX3bgain[uSX3][det.pStrip][det.nStrip];
	//cout << " + " << uSX3boffset[uSX3][det.pStrip][det.nStrip] << " = " <<uSX3Energy[uSX3mult] << endl; 
       
        uSX3fStrip[uSX3mult] = det.pStrip;
        uSX3bStrip[uSX3mult] = det.nStrip;
	if(iverb) cout << "F strip = " <<  uSX3fStrip[uSX3mult] << ", back " <<  uSX3bStrip[uSX3mult] << endl;

	// uSX3Pos[uSX3mult] = ((((det.eFarCal-det.eNearCal)*1.8)/(det.eNearCal+det.eFarCal))*37.5)+37.5; // Original, 'fudged'
	double factor = (det.eFarCal-det.eNearCal)/(det.eFarCal+det.eNearCal)*1.8; // 04022019 ar
	uSX3Pos[uSX3mult] = ((((factor-uSX3L[uSX3][det.pStrip])/(uSX3R[uSX3][det.pStrip]-uSX3L[uSX3][det.pStrip]))-0.5)*37.5)+37.5;
      	
        uSX3Theta[uSX3mult] = (TMath::ATan(uSX3Pos[uSX3mult]/r) + PI/2.)*(180./PI);	
        uSX3E3[uSX3mult] = (uSX3Energy[uSX3mult]/1000) + m3;
        uSX3p3[uSX3mult] = TMath::Sqrt(TMath::Power(uSX3Energy[uSX3mult]/1000.0,2.0) + 2.0*m3*uSX3Energy[uSX3mult]/1000.0);

        uSX3Ex[uSX3mult] = (TMath::Sqrt((Etotcm*Etotcm + m3*m3)-2*(uSX3E3[uSX3mult]*Etot - p1*uSX3p3[uSX3mult]*TMath::Cos(uSX3Theta[uSX3mult]*PI/180.))) - m4);
	 
        hT0_Ex->Fill(uSX3Ex[uSX3mult],goddess->tdc0);

        hFB_uSX->Fill(uSX3Energy[uSX3mult],(det.eNearCal+det.eFarCal));
        if(uSX3Energy[uSX3mult] > 50){
         hEx_upstream->Fill(uSX3Ex[uSX3mult]);
      
          if(cutIC->IsInside(goddess->icE,goddess->icDE) && ((goddess->tdc0)>500 && (goddess->tdc0)<900)){
            huSXEx->Fill(uSX3Ex[uSX3mult]);
              hEx_upstream_ICTDC->Fill(uSX3Ex[uSX3mult]);
              }
          if(cutIC->IsInside(goddess->icE,goddess->icDE) && ((goddess->tdc0)>2000 && (goddess->tdc0)<2400))
            huSXEx_bg->Fill(uSX3Ex[uSX3mult]);
          // add these things to an overall Si array...
          SiEnergy[SiMult] = uSX3Energy[uSX3mult];
	  UpEnergy[UpMult] = uSX3Energy[uSX3mult];
          // pos[SiMult] = uSX3Pos[uSX3mult];
          SiTheta[SiMult] = uSX3Theta[uSX3mult];
	  UpTheta[UpMult] = uSX3Theta[uSX3mult];
          Ex[SiMult] = uSX3Ex[uSX3mult];
	  UpEx[UpMult] = uSX3Ex[uSX3mult];

          hSiEvTh->Fill(SiTheta[SiMult],SiEnergy[SiMult]);
          if(cutIC->IsInside(goddess->icE,goddess->icDE)){
            hSiEvTh_IC->Fill(SiTheta[SiMult],SiEnergy[SiMult]);
            if((goddess->tdc0)>500 && (goddess->tdc0)<900)
              hSiEvTh_ICTDC->Fill(SiTheta[SiMult],SiEnergy[SiMult]);
          }
	   if(cut_whole->IsInside(icEcorr,icdEcorr)){
            hSiEvTh_wlIC->Fill(uSX3Theta[uSX3mult],uSX3Energy[uSX3mult]);
            if((goddess->tdc0)>500 && (goddess->tdc0)<900)
              hSiEvTh_wlICTDC->Fill(uSX3Theta[uSX3mult],uSX3Energy[uSX3mult]);
          }
	   if(icEcorr>100 && icdEcorr>100){
            hSiEvTh_WIC->Fill(uSX3Theta[uSX3mult],uSX3Energy[uSX3mult]);
            if((goddess->tdc0)>500 && (goddess->tdc0)<900)
              hSiEvTh_WICTDC->Fill(uSX3Theta[uSX3mult],uSX3Energy[uSX3mult]);
          }


	   if(cut_tedp->IsInside(uSX3Theta[uSX3mult],uSX3Energy[uSX3mult])) htdc0_dp->Fill(goddess->tdc0);

          hEx->Fill(Ex[SiMult]);
	 
          htheta->Fill(SiTheta[SiMult]);
          if(iverb) cout << "uSX3E["<<uSX3mult<<"] = " << uSX3Energy[uSX3mult] << ", but raw = " <<det.nECal << endl;
          if(iverb) cout << ", uSX3Th["<<uSX3mult<<"] = " << uSX3Theta[uSX3mult];
          uSX3mult++;
          SiMult++;
	  UpMult++;
	  if(iverbGam) cout << "jentry " << jentry << ": uSX3mult = " << uSX3mult << ", SiMult = " << SiMult << " up = " << UpMult << endl;
        }	
      }

      //*************************************************************

      else if(goddess->siType.at(i) == "SuperX3" && !(det.upstream)){ // DOWNSTREAM SX3
        
        dSX3Energy[dSX3mult] = (det.nECal*dSX3bgain[dSX3][det.nStrip])+dSX3boffset[dSX3][det.nStrip];
        dSX3fStrip[dSX3mult] = det.pStrip;
        dSX3bStrip[dSX3mult] = det.nStrip;
	// dSX3Pos[dSX3mult] = ((((det.eNearCal-det.eFarCal)*1.8)/(det.eNearCal+det.eFarCal))*37.5)-37.5; // original, 'fudged'
       	double dFactor = (det.eNearCal-det.eFarCal)/(det.eFarCal+det.eNearCal)*1.8;
	dSX3Pos[dSX3mult] = ((((dFactor-dSX3L[dSX3][det.pStrip])/(dSX3R[dSX3][det.pStrip]-dSX3L[dSX3][det.pStrip]))-0.5)*37.5)-37.5;
	hpos->Fill(((dFactor-dSX3L[dSX3][det.pStrip])/(dSX3R[dSX3][det.pStrip]-dSX3L[dSX3][det.pStrip])-1.0)+0.5,det.pStrip+(4*dSX3));

        dSX3Theta[dSX3mult] = (TMath::ATan(dSX3Pos[dSX3mult]/r) + (PI)/2.)*(180./(PI));
        dSX3E3[dSX3mult] = (dSX3Energy[dSX3mult]/1000) + m3; 
        dSX3p3[dSX3mult] = TMath::Sqrt(TMath::Power(dSX3Energy[dSX3mult]/1000.0,2.0) + 2.0*m3*dSX3Energy[dSX3mult]/1000.0);
        dSX3Ex[dSX3mult] = (TMath::Sqrt((Etotcm*Etotcm + m3*m3)-2*(dSX3E3[dSX3mult]*Etot - p1*dSX3p3[dSX3mult]*TMath::Cos(dSX3Theta[dSX3mult]*PI/180.))) - m4);

        hFB_dSX->Fill(dSX3Energy[dSX3mult],(det.eNearCal+det.eFarCal));
        if(dSX3Energy[dSX3mult] > 50){
          SiEnergy[SiMult] = dSX3Energy[dSX3mult];
          SiTheta[SiMult] = dSX3Theta[dSX3mult];
          Ex[SiMult] = dSX3Ex[dSX3mult];

          hSiEvTh->Fill(SiTheta[SiMult],SiEnergy[SiMult]);
          if(cutIC->IsInside(goddess->icE,goddess->icDE)){
            hSiEvTh_IC->Fill(SiTheta[SiMult],SiEnergy[SiMult]);
            if((goddess->tdc0)>500 && (goddess->tdc0)<900)
              hSiEvTh_ICTDC->Fill(SiTheta[SiMult],SiEnergy[SiMult]);
          }

	  if(cut_whole->IsInside(icEcorr,icdEcorr)){
            hSiEvTh_wlIC->Fill(dSX3Theta[dSX3mult],dSX3Energy[dSX3mult]);
            if((goddess->tdc0)>500 && (goddess->tdc0)<900)
              hSiEvTh_wlICTDC->Fill(dSX3Theta[dSX3mult],dSX3Energy[dSX3mult]);
          }
	   if(icEcorr>100 && icdEcorr>100){
            hSiEvTh_WIC->Fill(dSX3Theta[dSX3mult],dSX3Energy[dSX3mult]);
            if((goddess->tdc0)>500 && (goddess->tdc0)<900)
              hSiEvTh_WICTDC->Fill(dSX3Theta[dSX3mult],dSX3Energy[dSX3mult]);
          }

          hEx->Fill(Ex[SiMult]);
          htheta->Fill(SiTheta[SiMult]);
          if(iverb) cout << "dsx3, E["<<i<<"] = " << SiEnergy[SiMult] << ", SiTheta["<<i<<"] = " << SiTheta[SiMult] << endl;
          if(iverb) cout << "dSX3E["<<dSX3mult<<"] = " << dSX3Energy[dSX3mult];
          if(iverb) cout << ", dSX3Th["<<dSX3mult<<"] = " << dSX3Theta[dSX3mult];
          dSX3mult++;
          SiMult++;
	  // if(iverbGam) cout << "jentry " << jentry << ": dSX3mult = " << dSX3mult << ", SiMult = " << SiMult << " [up = " << up << "] " << endl;
        }
      }
      //*************************************************************

      else if(goddess->siType.at(i) == "QQQ5" && det.upstream){ // UPSTREAM QQQ
        uQEnergy[uQmult] = (det.nECal*uQbgain[uQQQ][det.nStrip])+uQboffset[uQQQ][det.nStrip];
        uQbStrip[uQmult] = det.nStrip;
        BackStrip[uQmult] = det.nStrip + (uQQQ*4); // back strips numbered from 1 to 16, from uQQQ 0 to 3.
        //RoughPhi[uQmult] =;  
        uQfStrip[uQmult] = det.pStrip;
        uQTheta[uQmult] = uQQQang[det.pStrip];
        if (iverb) cout << "uQTheta in uQ loop = " << uQTheta[uQmult] << endl;
        uQPhi[uQmult] = uQQQphi[uQQQ][det.nStrip];
        if(iverb) cout << "uQEnergy["<<uQmult<<"] = " << uQEnergy[uQmult] << " (raw = " << det.nECal << ")";
        uQE3[uQmult] = (uQEnergy[uQmult]/1000) + m3; // can get overwritten
        if(iverb) cout << ", E3[uQmult] = " << uQE3[uQmult];	
        uQp3[uQmult] = TMath::Sqrt(TMath::Power(uQEnergy[uQmult]/1000.0,2.0) + 2.0*m3*uQEnergy[uQmult]/1000.0);
        if(iverb) cout << ", p3[uQmult] = " << uQp3[uQmult];

        uQEx[uQmult] = (TMath::Sqrt((Etotcm*Etotcm + m3*m3)-2*(uQE3[uQmult]*Etot - p1*uQp3[uQmult]*TMath::Cos(uQTheta[uQmult]*PI/180.))) - m4);

         hEx_upstream->Fill(uQEx[uQmult]);

        if(cutIC->IsInside(goddess->icE,goddess->icDE) && ((goddess->tdc0)>500 && (goddess->tdc0)<900)){
          huQEx->Fill(uQEx[uQmult]);
           hEx_upstream_ICTDC->Fill(uQEx[uQmult]);
           }
        if(cutIC->IsInside(goddess->icE,goddess->icDE) && ((goddess->tdc0)>2000 && (goddess->tdc0)<2400))
          huQEx_bg->Fill(uQEx[uQmult]);

        hT0_Ex->Fill(uQEx[uQmult],goddess->tdc0);
        hFB_uQ->Fill(uQEnergy[uQmult],det.pECal);
        if(uQEnergy[uQmult] > 50){ 
          // add these things to an overal Si array
                
          SiEnergy[SiMult] = uQEnergy[uQmult];
          SiTheta[SiMult] = (uQQQang[det.pStrip]);
          Ex[SiMult] = uQEx[uQmult];
	  UpEnergy[UpMult] = uQEnergy[uQmult];
          UpTheta[UpMult] = (uQQQang[det.pStrip]);
          UpEx[UpMult] = uQEx[uQmult];

          hSiEvTh->Fill(SiTheta[SiMult],SiEnergy[SiMult]);
          if(cutIC->IsInside(goddess->icE,goddess->icDE)){
            hSiEvTh_IC->Fill(SiTheta[SiMult],SiEnergy[SiMult]);
            if((goddess->tdc0)>500 && (goddess->tdc0)<900)
              hSiEvTh_ICTDC->Fill(SiTheta[SiMult],SiEnergy[SiMult]);
          }
	  if(cut_whole->IsInside(icEcorr,icdEcorr)){
            hSiEvTh_wlIC->Fill(uQTheta[uQmult],uQEnergy[uQmult]);
            if((goddess->tdc0)>500 && (goddess->tdc0)<900)
              hSiEvTh_wlICTDC->Fill(uQTheta[uQmult],uQEnergy[uQmult]);
          }
	   if(icEcorr>100 && icdEcorr>100){
            hSiEvTh_WIC->Fill(uQTheta[uQmult],uQEnergy[uQmult]);
            if((goddess->tdc0)>500 && (goddess->tdc0)<900)
              hSiEvTh_WICTDC->Fill(uQTheta[uQmult],uQEnergy[uQmult]);
          }
          hEx->Fill(Ex[SiMult]);
          htheta->Fill(SiTheta[SiMult]);

	  if(cut_tedp->IsInside(uQTheta[uQmult],uQEnergy[uQmult])) htdc0_dp->Fill(goddess->tdc0);
	  
          if(iverb) cout << " uQE["<<uQmult<<"] = " << uQEnergy[uQmult];
          if(iverb) cout << ", uQTh["<<uQmult<<"] = " << uQTheta[uQmult];

          uQmult++;
          SiMult++;
	  UpMult++;
	  if(iverbGam) cout << "jentry " << jentry << ": uQmult = " << uQmult << ", SiMult = " << SiMult << " up = " << UpMult << endl;
        }
      }

      //*************************************************************

      else if(goddess->siType.at(i) == "QQQ5" && !(det.upstream)){ // DOWNSTREAM QQQ
        //dQEnergy[dQmult] = det.nECal;
        dQEnergy[dQmult] = (det.nECal*dQbgain[dQQQ][det.nStrip])+dQboffset[dQQQ][det.nStrip];
        dQbStrip[dQmult] = det.nStrip;
        dQfStrip[dQmult] = det.pStrip;
        dQTheta[dQmult] = dQQQang[det.pStrip];
        dQE3[dQmult] = (dQEnergy[dQmult]/1000) + m3; // can get overwritten
        dQp3[dQmult] = TMath::Sqrt(TMath::Power(dQEnergy[dQmult]/1000.0,2.0) + 2.0*m3*dQEnergy[dQmult]/1000.0);
        dQEx[dQmult] = (TMath::Sqrt((Etotcm*Etotcm + m3*m3)-2*(dQE3[dQmult]*Etot - p1*dQp3[dQmult]*TMath::Cos(dQTheta[dQmult]*PI/180.))) - m4);

        if(dQEnergy[dQmult] > 50){
          // add these things to an overall Si array... 
          SiEnergy[SiMult] = dQEnergy[dQmult];
          SiTheta[SiMult] = (dQQQang[det.pStrip]);
          Ex[SiMult] = dQEx[dQmult];

          hSiEvTh->Fill(SiTheta[SiMult],SiEnergy[SiMult]);
          if(cutIC->IsInside(goddess->icE,goddess->icDE)){
            hSiEvTh_IC->Fill(SiTheta[SiMult],SiEnergy[SiMult]);
            if((goddess->tdc0)>500 && (goddess->tdc0)<900)
              hSiEvTh_ICTDC->Fill(SiTheta[SiMult],SiEnergy[SiMult]);
          }
	  if(cut_whole->IsInside(icEcorr,icdEcorr)){
            hSiEvTh_wlIC->Fill(dQTheta[dQmult],dQEnergy[dQmult]);
            if((goddess->tdc0)>500 && (goddess->tdc0)<900)
              hSiEvTh_wlICTDC->Fill(dQTheta[dQmult],dQEnergy[dQmult]);
          }
	   if(icEcorr>100 && icdEcorr>100){
            hSiEvTh_WIC->Fill(dQTheta[dQmult],dQEnergy[dQmult]);
            if((goddess->tdc0)>500 && (goddess->tdc0)<900)
              hSiEvTh_WICTDC->Fill(dQTheta[dQmult],dQEnergy[dQmult]);
          }

          hEx->Fill(Ex[SiMult]);
          htheta->Fill(SiTheta[SiMult]);
          if(iverb) cout << "dQE["<<dQmult<<"] = " << dQEnergy[dQmult];
          if(iverb) cout << ", dQTh["<<dQmult<<"] = " << dQTheta[dQmult];
          dQmult++;
          SiMult++;
	  // if(iverbGam) cout << "jentry " << jentry << ": dQmult = " << dQmult << ", SiMult = " << SiMult << " [up = " << up << "]" << endl;
        }
      }
 
      
    } // end goddess
   
    // So now, SiMult corresponds to a hit in any silicon greater than a threshold. 
   
    //When filling histograms here, loop over mult!
    for(m=0;m<uQmult;m++){
      huQEvTh->Fill(uQfStrip[m],uQEnergy[m]);
      huQEvBStrip->Fill(BackStrip[m],uQEnergy[m]);
    }

    hTdc0Tdc4->Fill(goddess->tdc0,goddess->tdc4);


      /* *********************************************************** */
      /* ********************** GRETINA **************************** */ 
      /*    initial loop sets variables and iterates a multiplicity  */
      /* *********************************************************** */

    Float_t ar_beta = 0.1351202;
    Float_t ar_gamma = 1.0/TMath::Sqrt(1.0-TMath::Power(ar_beta,2.0));
    

    for(size_t j=0;j<gretina->xtals.size();j++){
      g2CrystalEvent g2 = gretina->xtals.at(j); 
      if(g2.cc < quadCut[g2.quadNum] && g2.cc > 0.){ // cut out high lying detector crap
        xlab[crysMult] = g2.maxIntPtXYZLab().X();
        ylab[crysMult] = g2.maxIntPtXYZLab().Y();
        zlab[crysMult] = g2.maxIntPtXYZLab().Z();
                
        edop_AR[crysMult] = g2.maxIntPtE()*ar_gamma*(1.0-ar_beta*g2.maxIntPtXYZLab().CosTheta());        
                       
        gamE[crysMult] = g2.cc;
        edop[crysMult] = g2.edop;
	    edop_maxInt[crysMult] = g2.edop_maxInt;
        edopSeg[crysMult] = g2.edopSeg;
        edopXtal[crysMult] = g2.edopXtal;
        quadNum[crysMult] = g2.quadNum;
        hGamTdc2->Fill(g2.edop,goddess->tdc2);
        t0[crysMult] = g2.t0;
        if(iverbGam) cout << jentry << ": E["<<crysMult<<"] = " << gamE[crysMult] << ", edp[" << crysMult << "] = " << edop[crysMult] << endl;
        crysMult++;
      }
    }


    /* ****************************************************** */
    /* ************* Filling histograms ********************* */
    /* ****************************************************** */
    // gamma singles only - no silicon

    for(k=0;k<crysMult;k++){
      //These are NOT event by event corrected
      hgam->Fill(gamE[k]);
      hedop->Fill(edop[k]);
      hedopSeg->Fill(edopSeg[k]);
      hedopXtal->Fill(edopXtal[k]);
      hedop_maxInt->Fill(edop_AR[k]);
    }

    if(dSX3mult>0){
       for(k=0;k<crysMult;k++){
	  hedop_dSX3->Fill(edop[k]);
       }}


    /*  if((goddess->icE)>200){
        for(n=0;n<uQmult;n++){
          if(uQEnergy[n]>3000){
           hSteve->Fill(edop[k]); 
	   }}} */
    for(n=0;n<uQmult;n++){
      if(goddess->icE>200){
	if(uQEnergy[n]>2000){
	  for(k=0;k<crysMult;k++){
	    hSteve->Fill(edop[k]);
	  }
	  break;
	}}}
   
    
    if(cutIC->IsInside(goddess->icE,goddess->icDE)){
      for(k=0;k<crysMult;k++){
	hedopMI_IC->Fill(edop_AR[k]);
        hedop_IC->Fill(edop[k]);
	 if((goddess->tdc0)>500 && (goddess->tdc0)<900){ 
	   hedop_ICTDC->Fill(edop[k]);
	 } else if ((goddess->tdc0)>2000 && (goddess->tdc0)<2400){ 
	   hedop_ICbkTDC->Fill(edop[k]);}
      }
        for(m=0;m<uQmult;m++){
	  if(uQEnergy[m]>3000){
	    for(k=0;k<crysMult;k++){
            hedop_ICuQ->Fill(edop[k]);
        }
	    break;
	  }}}

 
        for(s=0;s<UpMult;s++){
          if(SiEnergy[s]>1000 && (cutIC->IsInside(goddess->icE,goddess->icDE))){
	    for(k=0;k<crysMult;k++){
	      hedop_ICSi->Fill(edop[k]);
	    }}
	    break;
	  if(SiEnergy[s]>1000 &&(goddess->tdc0)>500 && (goddess->tdc0)<900){
	    for(k=0;k<crysMult;k++){
	    hedop_ICTDCSi->Fill(edop[k]);
	    }}
	  break;
	 if (SiEnergy[s]>1000 && (goddess->tdc0)>2000 && (goddess->tdc0)<2400){
	   for(k=0;k<crysMult;k++){
	    hedop_ICTDCSiBk->Fill(edop[k]);
	   }}
	 break;
	}
  
    


    /* ***************************************************************************************************** */
    /* ***************************************************************************************************** */
    
	for(s=0;s<SiMult;s++){
	   hExvicE->Fill(icEcorr,Ex[s]);
	}
     // Filling Ex vs gamma ray energy spectra, for all silicon
    for(s=0;s<SiMult;s++){
      for(k=0;k<crysMult;k++){
        hExEgam->Fill(edop[k],Ex[s]);
        hExEgam1->Fill(edop[k],Ex[0]); // only looking at 1st silicon

        if(cutIC->IsInside(goddess->icE,goddess->icDE)){
          hExEgam_IC->Fill(edop[k],Ex[s]); 

          if((goddess->tdc0)>500 && (goddess->tdc0)<900){
            hExEgam_ICTDC->Fill(edop[k],Ex[s]);
            hExTh->Fill(SiTheta[s],Ex[s]);

          }
        }
      }
    }

    // Filling Ex vs gamma ray energy spectra, for just uQQQ
    for(q=0;q<uQmult;q++){
      for(k=0;k<crysMult;k++){
        hExEgamuQ->Fill(edop[k],uQEx[q]);

        if(cutIC->IsInside(goddess->icE,goddess->icDE)){
          hExEgamuQ_IC->Fill(edop[k],uQEx[q]); 

          if((goddess->tdc0)>500 && (goddess->tdc0)<900){
            hExEgamuQ_ICTDC->Fill(edop[k],uQEx[q]);
            hExThuQ->Fill(uQTheta[q],uQEx[q]);
          }
        }
      }
    }
        
    // Event by Event doppler correction, but only for uQQQ events
    for(q=0;q<uQmult;q++){
      TLorentzVector recVec = GetRecoilVector(q);
      for(k=0;k<crysMult;k++){
        TVector3 gamVec(xlab[k],ylab[k],zlab[k]);
        double angle = recVec.Angle(gamVec);
        double dopp_ebe = gamE[k] * recVec.Gamma() * (1- recVec.Beta() *cos(angle));
        if (iverb) cout << "dop = " << gamE[k] << " x " << recVec.Gamma() << " x (1 - " << recVec.Beta() << " x cos(" << angle << ") = " << dopp_ebe << endl;
        if (iverb) cout << "recVec.Theta = " << recVec.Theta() << endl;
        hEBE_ExEgamQ->Fill(dopp_ebe,uQEx[q]);

        if(cutIC->IsInside(goddess->icE,goddess->icDE)){
          hEBE_ExEgamQ_IC->Fill(dopp_ebe,uQEx[q]); 

          if((goddess->tdc0)>500 && (goddess->tdc0)<900){
            hEBE_ExEgamQ_ICTDC->Fill(dopp_ebe,uQEx[q]);
            hEBE_egam->Fill(dopp_ebe);
            hedopuQ_ICTDC->Fill(edop[k]);
          }
        }
      }
    }
            

    // Filling Ex vs gamma ray energy spectra, for just dQQQ
    for(q=0;q<dQmult;q++){
      for(k=0;k<crysMult;k++){
        hExEgamdQ->Fill(edop[k],dQEx[q]);

        if(cutIC->IsInside(goddess->icE,goddess->icDE)){
          hExEgamdQ_IC->Fill(edop[k],dQEx[q]); 

          if((goddess->tdc0)>500 && (goddess->tdc0)<900){
            hExEgamdQ_ICTDC->Fill(edop[k],dQEx[q]);
            hExThdQ->Fill(dQTheta[q],dQEx[q]);
          }
        }
      }
    }


    // Filling Ex vs gamma ray energy spectra, for just uSX3
    for(q=0;q<uSX3mult;q++){
      for(k=0;k<crysMult;k++){
        hExEgamuSX->Fill(edop[k],uSX3Ex[q]);

        if(cutIC->IsInside(goddess->icE,goddess->icDE)){
          hExEgamuSX_IC->Fill(edop[k],uSX3Ex[q]); 

          if((goddess->tdc0)>500 && (goddess->tdc0)<900){
            hExEgamuSX_ICTDC->Fill(edop[k],uSX3Ex[q]);
            hExThuSX->Fill(uSX3Theta[q],uSX3Ex[q]);
          }
        }
      }
    }

    // Filling Ex vs gamma ray energy spectra, for just dSX3
    for(q=0;q<dSX3mult;q++){
      for(k=0;k<crysMult;k++){
        hExEgamdSX->Fill(edop[k],dSX3Ex[q]);

        if(cutIC->IsInside(goddess->icE,goddess->icDE)){
          hExEgamdSX_IC->Fill(edop[k],dSX3Ex[q]); 

          if((goddess->tdc0)>500 && (goddess->tdc0)<900){
            hExEgamdSX_ICTDC->Fill(edop[k],dSX3Ex[q]);
            hExThdSX->Fill(dSX3Theta[q],dSX3Ex[q]);
          }
        }
      }
    }
    
 
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
  hedop_maxInt->Write();
  hedop_IC->Write();
  hedop_ICSi->Write();
  hedop_ICuQ->Write();
  hedopuQ_ICTDC->Write();
  htheta->Write();
  hSteve->Write();
  hpos->Write();
 
  hedop_dSX3->Write();

  hedop_ICTDC->Write();
  hedop_ICbkTDC->Write();  

  hedop_ICTDCSi->Write();
  hedop_ICTDCSiBk->Write();
  


  TH1F *hedop_bksub = (TH1F*)hedop_ICTDC->Clone("hedop_bksub");
  hedop_bksub->Add(hedop_ICbkTDC,-1);
  hedop_bksub->SetTitle("Time-random background subtracted doppler-corrected gamma");
  for(int i=1;i<hedop_bksub->GetNbinsX();i++){
    hedop_bksub->SetBinError(i,hedop_ICTDC->GetBinError(i)+hedop_ICbkTDC->GetBinError(i));
  }

  hedop_bksub->Write();

  TH1F *hedop_ar_bksub = (TH1F*)hedop_ICTDCSi->Clone("hedop_ar_bksub");
  hedop_ar_bksub->Add(hedop_ICTDCSiBk,-1.0);
  hedop_ar_bksub->SetTitle("Andrew's Time-random background subtracted doppler-corrected gamma");
  
  // don't want to include error bars for now
  hedop_ar_bksub->Write();
 
  hICcorrected->Write();
  hSiEvTh->Write();
  hSiEvTh_IC->Write();
  hSiEvTh_ICTDC->Write();
  hSiEvTh_wlIC->Write();
  hSiEvTh_wlICTDC->Write();
  hSiEvTh_WIC->Write();
  hSiEvTh_WICTDC->Write();

  kinematics->Write();
  kinematics_1->Write();
  kinematics_2->Write();
  hEx->Write();
  huQEx->Write();
  huQEx_bg->Write();
   hEx_upstream_ICTDC->Write();
   hEx_upstream->Write();
  

  TH1F *huQEx_bgsub = huQEx;
  huQEx_bgsub->Add(huQEx_bg,-1);
  huQEx_bgsub->SetName("uQEx_bgsub");
  huQEx_bgsub->SetTitle("Time-random background subtraction uQQQ");
  for(int i=1;i<=huQEx_bgsub->GetNbinsX();i++)
    huQEx_bgsub->SetBinError(i,huQEx->GetBinError(i)+huQEx_bg->GetBinError(i));
  huQEx_bgsub->Write();

  huSXEx->Write();
  huSXEx_bg->Write();

  TH1F *huSXEx_bgsub = huSXEx;
  huSXEx_bgsub->Add(huSXEx_bg,-1);
  huSXEx_bgsub->SetName("uSXEx_bgsub");
  huSXEx_bgsub->SetTitle("Time-random background subtraction uSXQQ");
  for(int i=1;i<=huSXEx_bgsub->GetNbinsX();i++)
    huSXEx_bgsub->SetBinError(i,huSXEx->GetBinError(i)+huSXEx_bg->GetBinError(i));
  huSXEx_bgsub->Write();

  hICxy->Write();
  hIC_EdE->Write();
  hIC_test->Write();
  hICCor_EdE_tdc0->Write();
  huQEvTh->Write();
  huQEvBStrip->Write();
  hExvicE->Write();

  htdc0->Write();
  htdc0_dp->Write();
  htdc1->Write();
  htdc2->Write();
  htdc3->Write();
  htdc4->Write();

  hExEgam->Write();
  hExEgam1->Write();
  hExEgam_IC->Write();
  hExEgam_ICTDC->Write();

  hExEgamuQ->Write();
  hExEgamuQ_IC->Write();
  hExEgamuQ_ICTDC->Write();
  hExThuQ->Write();

  hExEgamdQ->Write();
  hExEgamdQ_IC->Write(); 
  hExEgamdQ_ICTDC->Write();
  hExThdQ->Write();

  hExEgamuSX->Write();
  hExEgamuSX_IC->Write();
  hExEgamuSX_ICTDC->Write();
  hExThuSX->Write();

  hExEgamdSX->Write();
  hExEgamdSX_IC->Write();
  hExEgamdSX_ICTDC->Write();
  hExThdSX->Write(); 
  hEBE_ExEgamQ->Write();
  hEBE_ExEgamQ_IC->Write();
  hEBE_ExEgamQ_ICTDC->Write();
  hEBE_egam->Write();

  hT0_Ex->Write();
  hTdc0Tdc4->Write();
  hGamTdc2->Write();
  hedopMI_IC->Write();
  hFB_uQ->Write(); 
  hFB_uSX->Write();
  hFB_dSX->Write();
 
  for (int ii = 0; ii < 11; ii++) {
    hCryI[ii]->Write();
  }
  fcuts->Close();
  dc->Close();
  gc->Close();
  outfile->Close();

  }

// Calculate recoil vector from proton vector and excitation energy
TLorentzVector Sortcode::GetRecoilVector(int uq){

    
  Double_t p1 = sqrt(T1*T1+(2.0*m1*T1));  
  
  // for now, only do this for uQQQ events
 
  double proton_px = double(uQp3[uq])*(sin(double(uQTheta[uq])*PI/180.))*(cos(double(uQPhi[uq])*PI/180.));
  double proton_py = double(uQp3[uq])*(sin(double(uQTheta[uq])*PI/180.))*(sin(double(uQPhi[uq])*PI/180.));
  double proton_pz = double(uQp3[uq])*(cos(double(uQTheta[uq])*PI/180.));
    
  TLorentzVector total_4p(0.,0.,double(p1),double(T1+m1+m2));
  TLorentzVector proton_4p(proton_px,proton_py,proton_pz,uQE3[uq]);
  TLorentzVector recoil_4p;
    
  recoil_4p = total_4p - proton_4p;
  double p4 = sqrt((recoil_4p.E()*recoil_4p.E()) - m4*m4);
  return recoil_4p;
}
/*
// Calculate recoil vector based on P3, E3, theta and phi
TLorentzVector Sortcode::GetRecoilVector(double P3, double E3, double Theta, double Phi){

    
  Double_t p1 = sqrt(T1*T1+(2.0*m1*T1));  
  
  // for now, only do this for uQQQ events
 
  double proton_px = double(P3)*(sin(double(Theta)*PI/180.))*(cos(double(Phi)*PI/180.));
  double proton_py = double(P3)*(sin(double(Theta)*PI/180.))*(sin(double(Phi)*PI/180.));
  double proton_pz = double(P3)*(cos(double(Theta)*PI/180.));
    
  TLorentzVector total_4p(0.,0.,double(p1),double(T1+m1+m2));
  TLorentzVector proton_4p(proton_px,proton_py,proton_pz,E3);
  TLorentzVector recoil_4p;
    
  recoil_4p = total_4p - proton_4p;
  double p4 = sqrt((recoil_4p.E()*recoil_4p.E()) - m4*m4);
  return recoil_4p;
}
*/
TGraph* Sortcode::MakeCatKinematics(double Ex){

  TGraph *g = new TGraph();
  Double_t E1 = T1+m1; //30P total energy
  Double_t Etotcm = sqrt((m1*m1)+(m2*m2)+2.*E1*m2);  // total energy in the CM frame
  double tmp_e3cm = (pow(Etotcm,2) + (m3 + (m4 + Ex)) * (m3 - (m4 + Ex)))/(2*Etotcm);
  double tmp_beta3c = sqrt(1 - 1./pow(tmp_e3cm/m3,2));
  double tmp_gamma = m1 + m2;
  double tmp_betac = sqrt(T1 * (T1 + 2*m1))/(tmp_gamma + T1);
  double tmp_newgamma = pow(tmp_e3cm/m3,2) * (1 - pow(tmp_betac,2));
  for(int i=0;i<180;i++){
    double tmp_cosangle = TMath::Cos(TMath::DegToRad()*(double)i);
    double tmp_b = -tmp_cosangle * tmp_betac;
    double tmp_a = tmp_newgamma + pow(tmp_b,2);
    double tmp_c = 1 - tmp_newgamma;
    double tmp_d2 = pow(tmp_b,2) - tmp_a * tmp_c;
    double tmp_b3l1 = (-tmp_b + sqrt(tmp_d2))/tmp_a;
    g->SetPoint(i,i,m3 * ((1/sqrt(1 - pow(tmp_b3l1,2)))-1) * 1000);
  }

  return g;

}

int main(int argc, char** argv){ 
  Sortcode *mysort = new Sortcode();

  int num; 
  sscanf (argv[1],"%d",&num);
  mysort->SortData(num);
  return 0;
  
}



/*** Alternate sort method ***/
/*
for(size_t p=0;p<goddess->si.size();i++){
  int detType = -1; int detID = -1;
  detectorOUT det = goddess->si.at(i);

  if(goddess->siID.at(i)=="UA_dE"){ detType = 0; detID = 0; }  if(goddess->siID.at(i)=="UB_dE"){ detType = 0; detID = 1;  }
  if(goddess->siID.at(i)=="UC_dE"){ detType = 0; detID = 2; }  if(goddess->siID.at(i)=="UD_dE"){ detType = 0; detID = 3;  }

  if(goddess->siID.at(i)=="D1_dE"){ detType = 1; detID = 0; } // maybe change these three
  if(goddess->siID.at(i)=="D1_E1"){ detType = 1; detID = 1; }
  if(goddess->siID.at(i)=="D1_E2"){ detType = 1; detID = 2; }

  if(goddess->siID.at(i)=="U0_E1"){ detType = 2; detID = 0;   }  if(goddess->siID.at(i)=="U1_E1"){ detType = 2; detID = 1;    }
  if(goddess->siID.at(i)=="U2_E1"){ detType = 2; detID = 2;   }  if(goddess->siID.at(i)=="U3_E1"){ detType = 2; detID = 3;    }
  if(goddess->siID.at(i)=="U4_E1"){ detType = 2; detID = 4;   }  if(goddess->siID.at(i)=="U5_E1"){ detType = 2; detID = 5;    }
  if(goddess->siID.at(i)=="U6_E1"){ detType = 2; detID = 6;   }  if(goddess->siID.at(i)=="U7_E1"){ detType = 2; detID = 7;    }
  if(goddess->siID.at(i)=="U8_E1"){ detType = 2; detID = 8;   }  if(goddess->siID.at(i)=="U9_E1"){ detType = 2; detID = 9;    }
  if(goddess->siID.at(i)=="U10_E1"){ detType = 2; detID = 10; }  if(goddess->siID.at(i)=="U11_E1"){ detType = 2; detID = 11;  }

  if(goddess->siID.at(i)=="D0_E1"){ detType = 3; detID = 0;   }   if(goddess->siID.at(i)=="D1_E1"){ detType = 3; detID = 1;   }
  if(goddess->siID.at(i)=="D2_E1"){ detType = 3; detID = 2;   }   if(goddess->siID.at(i)=="D3_E1"){ detType = 3; detID = 3;   }
  if(goddess->siID.at(i)=="D4_E1"){ detType = 3; detID = 4;   }   if(goddess->siID.at(i)=="D5_E1"){ detType = 3; detID = 5;   }
  if(goddess->siID.at(i)=="D6_E1"){ detType = 3; detID = 6;   }   if(goddess->siID.at(i)=="D7_E1"){ detType = 3; detID = 7;   }
  if(goddess->siID.at(i)=="D8_E1"){ detType = 3; detID = 8;   }   if(goddess->siID.at(i)=="D9_E1"){ detType = 3; detID = 9;   }
  if(goddess->siID.at(i)=="D10_E1"){ detType = 3; detID = 10; }   if(goddess->siID.at(i)=="D11_E1"){ detType = 3; detID = 11; }

  if(goddess->siID.at(i)=="D0_dE"){ detType = 4; detID = 0;   }   if(goddess->siID.at(i)=="D1_dE"){ detType = 4; detID = 1;   }
  if(goddess->siID.at(i)=="D2_dE"){ detType = 4; detID = 2;   }   if(goddess->siID.at(i)=="D3_dE"){ detType = 4; detID = 3;   }
  if(goddess->siID.at(i)=="D4_dE"){ detType = 4; detID = 4;   }   if(goddess->siID.at(i)=="D5_dE"){ detType = 4; detID = 5;   }
  if(goddess->siID.at(i)=="D6_dE"){ detType = 4; detID = 6;   }   if(goddess->siID.at(i)=="D7_dE"){ detType = 4; detID = 7;   }
  if(goddess->siID.at(i)=="D8_dE"){ detType = 4; detID = 8;   }   if(goddess->siID.at(i)=="D9_dE"){ detType = 4; detID = 9;   }
  if(goddess->siID.at(i)=="D10_dE"){ detType = 4; detID = 10; }   if(goddess->siID.at(i)=="D11_dE"){ detType = 4; detID = 11; }

  double eGain    = 1.;
  double eOffset  = 0.;
  double Theta    = 0;
  double Phi      = 0;
  if(detType == 0){
    eGain   = uQbgain[detID][det.nStrip];
    eOffset = uQboffset[detID][det.nStrip];
    Theta   = uQQQang[det.pstrip];
    Phi     = uQQQphi[detID][det.nStrip];
  }
  else if(detType == 1){
    eGain   = dQbgain[detID][det.nStrip];
    eOffset = dQboffset[detID][det.nStrip];
    Theta   = dQQQang[det.pstrip];
    //Phi   = dQQQphi[detID][det.nStrip];
  }
  else if(detType == 2){
    eGain   = uSX3bgain[detID][det.nStrip];
    eOffset = uSX3boffset[detID][det.nStrip];
    double tmpPos = (((det.eFarCal-det.eNearCal)/(det.eNearCal+det.eFarCal))*37.5)+37.5;
    Theta   = (TMath::ATan(tmpPos/r) + PI/2.)*(180./PI);
    //Phi   = uSX3phi[detID][det.nStrip];
  }
  else if(detType == 3){
    eGain   = dSX3bgain[detID][det.nStrip];
    eOffset = dSX3boffset[detID][det.nStrip];
    double tmpPos = (((det.eFarCal-det.eNearCal)/(det.eNearCal+det.eFarCal))*37.5)+37.5;
    Theta   = (TMath::ATan(tmpPos/r) + PI/2.)*(180./PI);
    //Phi   = dSX3phi[detID][det.nStrip];
  }
  //else if(detType == 4){
  //  eGain   = dBB10bgain[detID][det.nStrip];
  //  eOffset = dBB10boffset[detID][det.nStrip];
  //}

  double eCalib   = det.nEcal * eGain + eOffset;

  if(eCalib < 50) // We don't want noisy events, so if we see something with a low energy, skip it 
    continue;

  double  tmpE3   = eCalib/1000. + m3;
  double  tmpP3   = TMath::Sqrt(TMath::Power(eCalib/1000.0,2.0) + 2.0*m3*eCalib/1000.0);
  double  tmpEx   = (TMath::Sqrt((Etotcm*Etotcm + m3*m3)-2*(tmpE3*Etot - p1*tmpP3*TMath::Cos(Theta*PI/180.))) - m4);

  hExAll->Fill(tmpEx);
  hdetEx[detType]->Fill(tmpEx);
  if(cutIC->IsInside(goddess->icE,goddess->icDE)){
    hdetExIC[detType]->Fill(tmpEx);
    if((goddess->tdc0)>400 && (goddess->tdc0)<1250)
      hdetExICTDC[detType]->Fill(tmpEx);
  }

  hThetaAll->Fill(Theta);
  hSiETheta->Fill(Theta,eCalib);
  if(cutIC->IsInside(goddess->icE,goddess->icDE)){
    hSiEThetaIC->Fill(Theta,eCalib);
    if((goddess->tdc0)>400 && (goddess->tdc0)<1250)
      hSiEThetaICTDC->Fill(Theta,eCalib);
  }
  // Now we loop over the GRETINA Stuff
  for(size_t g=0;g<gretina->xtals.size();g++){
    g2CrystalEvent g2 = gretina->xtals.at(g);
    if(g2.cc > quadCut[g2.quadNum]) //  Skip crap high energy events
      continue;
    TLorentzVector recVec = GetRecoilVector(tmpP3,tmpE3,Theta,Phi);
    TVector3 gamVec(g2.maxIntPtXYZLab().X(),g2.maxIntPtXYZLab().Y(),g2.maxIntPtXYZLab().Z());
    double angle = recVec.Angle(gamVec);
    double dopp_e = g2.cc * recVec.Gamma() * (1 - recVec.Beta() * cos(angle));

  }
}
// Gamma singles (no coincidence requirement)
for(size_t g=0;g<gretina->xtals.size();g++){
  g2CrystalEvent g2 = gretina->xtals.at(g);
  hgam->Fill(g2.cc);
  hedop->Fill(g2.edop);
  hedopSeg->Fill(g2.edopSeg);
  hedopXtal->Fill(g2.edopXtal);
}*/

