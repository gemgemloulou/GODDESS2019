// ************************************************************
// ** This code takes information about a hit from Sort.C    **
// ** including its energy and angle, and calculates energy  **
// ** losses from the target, and the dead layer in the Si   **
// ************************************************************
 
#include "Eloss.h"
#include <fstream>
#include <iostream>
#include <stdlib.h>
#include <stdio.h>
#include <TF1.h>
#include <TGraph.h>
 
using namespace std;
 
Eloss::Eloss()
{
  int i,j;
  double Ttar[70], Rtar[70];
  double TSi[70], RSi[70];
  ifstream pintar, pinsi;
 
  // Target geometry
  pi = 4.*atan(1.0);
  rt = 5000;     // target radius = 5mm
  depth = 5.4/2; // microns
  thetacrit = atan(rt/depth)*180/pi;
 
 
  // read in table
 
   pintar.open("Pintar3.txt");
  if(!pintar)
    {
      cout << "Could not open file" << endl;
      exit(-1);
    }
  else
    {
      for (i=0;i<70;i++){
    pintar >> Ttar[i] >> Rtar[i];
      }
    }
  pintar.close();
 
  pinsi.open("PinSi3.txt");
  if(!pinsi)
    {
      cout << "Could not open file" << endl;
      exit(-1);
    }
  else
    {
      for (i=0;i<70;i++){
    pinsi >> TSi[i] >> RSi[i];
      }
    }
  pinsi.close();
 
  // Create graphs and splines
 
  g1 = new TGraph(70, Ttar, Rtar);
  s1 = new TSpline3("grs",g1);
 
  g2 = new TGraph(70,TSi, RSi);
  s2 = new TSpline3("grs",g2);
 
  g3 = new TGraph(70, Rtar, Ttar);
  s3 = new TSpline3("grs",g3);
 
  g4 = new TGraph(70, RSi, TSi);
  s4 = new TSpline3("grs",g4);
 
}
 
/*double Eloss::MakeSpline(double SiThick){ 
// Create Spline of Tincident as a function of measured energy for 1st guess of Tincident for punchthrough events
   
  int i;
  double y1,y2,ymax,x1,x2,step;
  double Tincident[100],Tmeasured[100];
  double TmaxMeas;
  
  y1=0;
  ymax=15000;
  step=(ymax-y1)/100;
   
  for(i=0;i<100; i++){
    y2 = y1+SiThick;
    if(i==0) x1 = 0.;
    else x1 = s4->Eval(y1);
    x2 = s4->Eval(y2);
    Tincident[99-i]=x2;
    Tmeasured[99-i]=x2-x1;
    y1=y1+step;
     
    cout << SiThick << "  " << y1 << "  " << y2 << "  " << x1 << "  " << x2 << "  ";
    cout << Tincident[99-i] << "  " << Tmeasured[99-i] << endl;
  }
  TmaxMeas = Tmeasured[99];             // Maximum energy deposited in silicon at point of punchthrough
   
  g5 = new TGraph(100, Tmeasured, Tincident);       // Incident energy vs Measured energy
  s5 = new TSpline3("grs",g5);
 
return TmaxMeas;
}*/
 
double Eloss::ProtonNoPunch(double dT, double thickt, double thickDL){
 
  double R1,R2,R3,R4;
  double T2, Tinit;
 
  // Add energy lost in dead layer
  R1 = s2->Eval(dT);                    // dT is an energy
  //     cout << "dT: " << dT << " R1: " << R1 << endl;
  R2 = R1 +thickDL;
  T2 = s4->Eval(R2);
 
  //    cout << "T2: " << T2 << endl;
 
  // Add energy lost in the target
  R3 = s1->Eval(T2);
  //     cout  << "R3: " << R3 << endl;
   R4 = R3 + thickt;
   //      cout << "thickt: " << thickt << endl;
    //       cout << "R4: " << R4 << endl;
  Tinit = s3->Eval(R4);
  //  cout << "Tinit: " << Tinit << endl << endl;
       
 
   return Tinit;
}
 
Eloss::~Eloss(void)
{
#ifdef DEBUG
  cout << "Eloss::Destructor" << endl;
#endif
}
 
       
double Eloss::CalcT0proton(double dT, double theta, double phi, bool punchthrough, int detnum){
 
  int i,j;
  double thickt, thickDL, thickSi;
  double T1,T2,Tinit,dTCalc;
  double R1,R2,R3,R4;
  double diff, diff2,diffprime;
  double Emax;
 

    DLdepth = 0.1;   // microns

  if(theta>thetacrit && theta<(180-thetacrit)){ 
    thickt = fabs(rt/sin(theta*pi/180));
 
  }else{ thickt = fabs(depth/cos(theta*pi/180));    // Target thickness elsewhere
    //    cout << "theta " << theta << " phi " << phi << endl;
    if(detnum == 1 || detnum == 3){
      thickDL = fabs(DLdepth/(sin(theta*pi/180)*sin(phi*pi/180)));      // Dead layer thickness
    }else if(detnum == 2 || detnum == 4 || detnum == 5 || detnum ==7){
        thickDL = fabs(DLdepth/(sin(theta*pi/180)*cos(phi*pi/180)));
    //      cout << "detnum " << detnum << " thickDL " << thickDL << endl;
    }else if(detnum>8 && detnum<13){
       thickDL = fabs(DLdepth/cos(theta*pi/180));
    }
 
   
    //   cout << "CalcT0proton loop. Theta: " << theta << " thickt: " << thickt << endl;
    
   // cout << "Detnum: "<< detnum << " thickDL: " << thickDL << endl;
   //  cout << "Theta: " << theta << " Thickt " << thickt << endl;
  // thickSi = fabs(Sidepth/sin(theta*pi/180));         // Silicon thickness
  }
 
  if(punchthrough==false){                              // NO PUNCHTHROUGH CASE
    Tinit = ProtonNoPunch(dT,thickt,thickDL);
  }else{
    cout << "" << endl;
  }
  
  // ********************************************
  // ** The following can probably be ignored  **
  // ********************************************
 
 
 /*      Emax = MakeSpline(thickSi);
    if(dT>1.0 && dT<Emax){
   Tinit =  s5->Eval(dT);      // First guess for initial energy taken from spline 5 (Initial T vs Measured T).
    diff=tol+1;             // Set diff>tol for each event so that while loop is satisfied
       
    j=0;
    while(fabs(diff)>=tol){
 
      // Subtract energy lost in target from guessed initial energy
      R4 = s1->Eval(Tinit);
      R3 = R4-thickt;
      T2 = s3->Eval(R3);
      
      // Then subtract energy lost in silicon
      R2 = s2->Eval(T2);
      R1 = R2-thickSi;
 
      if (R1>0){ 
    T1 = s4->Eval(R1);               
    dTCalc = T2-T1;       // Calculated energy loss in Si
    diff = dTCalc-dT;     // Difference between calculated and measured energy deposited in silicon (h(x))
 
    if(fabs(diff)>=tol){     // If guess is not good enough, calculate a new guess for initial energy
      Tinit = Tinit+delta;      // Add small amount delta to Tinit
         
      // Subtract energy lost in target
      R4 = s1->Eval(Tinit);
      R3 = R4-thickt;
      T2 = s3->Eval(R3);
   
      // Then subtract energy lost in silicon
      R2 = s2->Eval(T2);
      R1 = R2-thickSi;
      T1 = s4->Eval(R1);
 
      dTCalc = T2-T1;                   // Calculated energy loss with small amount delta added     
      diff2 = dTCalc - dT;              // Difference with delta added
         
      diffprime = (diff2-diff)/delta;           // Gradient of difference function
      Tinit = Tinit - diff/diffprime;           // New guess of Tinit   
    }                           // End if
     
  }else{  // If range is negative (protons around punchthrough region) assume no punchthrough
    Tinit = ProtonNoPunch(dT,thickt,thickDL);      
    diff = 0.0;
      }
      j++;
      if(j>20){
    Tinit = dT;
    break;
      }
}                               // End of while loop 
  }else{     // End spline compatibility check - if energy is outside the bounds, treat as a non-punchthrough event    
 
    if(dT>1.0) Tinit = ProtonNoPunch(dT,thickt,thickDL);     // Protons around punchthrough region    
    else       Tinit = dT;                  // Events below 1MeV at forward angles: Noise      
 
  }
       
 }  // end punchthrough condition
 
  */
  return(Tinit);
}   
  
