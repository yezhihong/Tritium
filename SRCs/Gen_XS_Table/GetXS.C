////////////////////////////////////////////////
// Generate Cross Section Look-up Table
//  --Zhihong Ye, 07/03/2012
////////////////////////////////////////////////
/*Include{{{*/
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>
#include <math.h>
#include <cmath>
#include <iostream>
#include <fstream>
#include <vector>
#include <algorithm>
#include <map>
#include <errno.h>
#include <sstream>
//#include <iterator>
//from root
#include <TMath.h>
#include <TRandom3.h>
#include <TLorentzVector.h>
#include <TMatrix.h>
#include <TFile.h>
#include <TTree.h>
#include <TStopwatch.h>
/*}}}*/
using namespace::std;
using namespace::TMath;
#include "XEMC.h"
#include "XGT2_Main.h"
#include "/work/halla/e08014/disk1/yez/Xbj_Cross_Section/SRC/XGT2_XEMC.h"
char* gTarget;
Double_t E0=-1.0;   //GeV
Double_t Ep=-1.0;   //GeV
Double_t Theta=-1.0; //Degree
Bool_t bAsk=kFALSE;
int getargs(int argc, char**argv);

int main(int argc,char** argv){

	Double_t xbj, cs_qe, cs_dis, cs_rad,cs_born;
	TString Target;
	int gerr = getargs(argc,argv);

	if(gerr<0)
		bAsk=kTRUE;
    else{
	    bAsk=kFALSE;
		Target = gTarget;
	}
	if(bAsk){
		cerr<<" Target (H2,He3,He4,C12,Al,Ca40,Ca48) = "; cin >> Target;
		//cerr<<" E0 (GeV/c) = "; cin >> E0;
        E0=3.356;
		cerr<<" Ep (GeV/c) = "; cin >> Ep;
		cerr<<" Theta (Degree) = "; cin >> Theta;	
	}

	Double_t Q2 = 4.0 * E0 * Ep * pow(sin(Theta*3.1415926/180./2.0),2);
	xbj = Q2/2.0/0.938272/(E0-Ep);

	Int_t A=1; 
	Int_t Z=0;

	TString Target_Input = Form("/work/halla/e08014/disk1/yez/Gen_XS_Table/input/%s_Input.dat",Target.Data());	

	/*Set Target{{{*/
	if(Target == "H2") {
		A = 2; Z = 1;}
	else if(Target == "He3") {
		A = 3; Z = 2;}
	else if(Target == "He4") {
		A = 4; Z = 2;}
	else if(Target == "C12") { 
		A = 12; Z = 6;}
	else if(Target == "Al") { 
		A = 27; Z = 13;}
	else if(Target == "Ca40") {
		A = 40; Z = 20;}
	else if(Target == "Ca48") {
		A = 48; Z = 20;}
	else if(Target == "Dummy") { 
		A = 27; Z = 13;}
	else{
		cerr<<"I don't understand the Target!"<<endl;}

    TARGET_MASS_A = gGet_Target_Mass_A(Target.Data());
    /*}}}*/

	//Define a event to calculate radiated cross section
	XEMC* Event = new XEMC(); 
    Event->SetTargetTable("/work/halla/e08014/disk1/yez/XEMC/SRC/target.table");
    //Event->SetTargetTable("/work/halla/e08014/disk1/yez/XEMC/SRC/target.table.20140512");
    Event->Init(Target_Input.Data());
	Int_t err = -1;

	err = Event->Process(E0,Ep,Theta);	
    if(err>=0){
		cs_qe = Event->XS_QE();
		cs_dis = Event->XS_DIS();
		cs_rad = Event->XS_Rad();
		cs_born = Event->XS_Born();
	}
	delete Event;
	cerr <<"------------------------------------------------------------------------"<<endl;
	cerr <<"------------------------------------------------------------------------"<<endl;
	cerr << Form("For Ep=%f, Theta=%f, xbj=%f, Q2=%f", Ep, Theta, xbj, Q2)<<endl;
	cerr <<"------------------------------------------------------------------------"<<endl;
	cerr << Form("@@@ XS_QE = %e, XS_DIS = %e, XS_Born = %e, XS_Rad = %e", cs_qe, cs_dis, cs_born,cs_rad)<<endl;
	cerr <<"------------------------------------------------------------------------"<<endl;
	cerr <<"------------------------------------------------------------------------"<<endl;

    TString Kin;
    cerr<<" Kin (3.1,3.2 ...)= "; cin >> Kin;
    Get_XS *xs1 = new Get_XS();
    xs1->Load_Table(Target.Data(),Kin.Data());
    Double_t cs_rad_table = xs1->gGet_XS(E0,Ep,Theta,0);	
    Double_t cs_born_table = xs1->gGet_XS(E0,Ep,Theta, 1);	
    Double_t cs_dis_table = xs1->gGet_XS(E0,Ep,Theta, 2);	
    Double_t cs_qe_table = xs1->gGet_XS(E0,Ep,Theta, 3);	

    cerr << Form("From Table, , XS_DIS = %e, XS_QE = %e, XS_Born = %e, XS_Rad = %e", cs_dis_table, cs_qe_table, cs_born_table,cs_rad_table)<<endl;

	return 0;
}


/*int getargs(int argc,char** argv){{{*/
int getargs(int argc,char** argv){
	char* argptr;
	bool noStop;  
	int err=-1;

	for(int i=0;i<argc;i++){
		argptr = argv[i];
		if(*argptr=='-'){
			argptr++;
			switch (*argptr){
				case 'E':
					if(*(++argptr))
						E0 = atof(argptr);
					err=0;
					break;
				case 'P':
					if(*(++argptr))
						Ep = atof(argptr);
					err=0;
					break;
				case 'A':
					if(*(++argptr))
						Theta = atof(argptr);
					err=0;
					break;
				case 'T':
					if(*(++argptr))
						gTarget = argptr;
					err=0;
					break;
				case 'h':
					cerr <<"================================================"<<endl;
					cerr <<"Option: " <<endl;
					cerr <<"-E[GeV]  Beam Energy" <<endl;
					cerr <<"-P[GeV]  Scattering Momentum" <<endl;
					cerr <<"-A[Deg]  Scattering Angle"<<endl;
					cerr <<"-T[H2,He3,He4,C12,Ca40,Ca48,...]  Target Name" <<endl;
					cerr <<endl<<"================================================"<<endl;
					noStop=false;
					err=-1;
					goto OUT;
					break;
				default:
					cerr <<"Unrecognized argument: " << argptr << endl;
					cerr <<endl<<"================================================"<<endl;
					cerr <<"Option: " <<endl;
					cerr <<"-E[GeV]  Beam Energy" <<endl;
					cerr <<"-P[GeV]  Scattering Momentum" <<endl;
					cerr <<"-A[Deg]  Scattering Angle"<<endl;
					cerr <<"-T[H2,He3,He4,C12,Ca40,Ca48,...]  Target Name" <<endl;
					cerr <<"             Zhihong Ye 10/04/2009" <<endl;
					cerr <<"================================================"<<endl;
					err=-1;
					break;
			}

		}
		noStop=true;
OUT:if(!noStop){break;}
	continue;
	}
	return err; 
}
/**/
