//HEAD_DSPH
/*
<DUALSPHYSICS>  Copyright (c) 2018 by Dr Jose M. Dominguez et al. (see http://dual.sphysics.org/index.php/developers/). 

EPHYSLAB Environmental Physics Laboratory, Universidade de Vigo, Ourense, Spain.
School of Mechanical, Aerospace and Civil Engineering, University of Manchester, Manchester, U.K.

This file is part of DualSPHysics. 

DualSPHysics is free software: you can redistribute it and/or modify it under the terms of the GNU Lesser General Public License 
as published by the Free Software Foundation; either version 2.1 of the License, or (at your option) any later version.

DualSPHysics is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU Lesser General Public License for more details. 

You should have received a copy of the GNU Lesser General Public License along with DualSPHysics. If not, see <http://www.gnu.org/licenses/>. 
*/

/// \file JSph.cpp \brief Implements the class \ref JSph.

#include "JSph.h"
#include "Functions.h"
#include "JSphMotion.h"
#include "JXml.h"
#include "JSpaceCtes.h"
#include "JSpaceEParms.h"
#include "JSpaceParts.h"
#include "JFormatFiles2.h"
#include "JCellDivCpu.h"
#include "JFormatFiles2.h"
#include "JSphDtFixed.h"
#include "JSaveDt.h"
#include "JTimeOut.h"
#include "JSphVisco.h"
#include "JWaveGen.h"
#include "JSphAccInput.h"
#include "JPartDataBi4.h"
#include "JPartDataHead.h"
#include "JPartOutBi4Save.h"
#include "JPartFloatBi4.h"
#include "JPartsOut.h"
#include <climits>

//using namespace std;
using std::string;
using std::ofstream;
using std::endl;

//==============================================================================
/// Constructor.
//==============================================================================
JSph::JSph(bool cpu):Cpu(cpu){
  ClassName="JSph";
  DataBi4=NULL;
  DataOutBi4=NULL;
  DataFloatBi4=NULL;
  PartsOut=NULL;
  Log=NULL;
  ViscoTime=NULL;
  DtFixed=NULL;
  SaveDt=NULL;
  TimeOut=NULL;
  MkList=NULL;
  Motion=NULL;
  FtObjs=NULL;
//£££££££££££££££££££££££££££££££££££££££££££££££££££££££££££££££££££££££//MP
  PhaseData=NULL;
//£££££££££££££££££££££££££££££££££££££££££££££££££££££££££££££££££££££££//MP
  WaveGen=NULL;
  AccInput=NULL;
  InitVars();
}

//==============================================================================
/// Destructor.
//==============================================================================
JSph::~JSph(){
  delete DataBi4;
  delete DataOutBi4;
  delete DataFloatBi4;
  delete PartsOut;
  delete ViscoTime;
  delete DtFixed;
  delete SaveDt;
  delete TimeOut;
  ResetMkInfo();
  delete Motion;
  AllocMemoryFloating(0);
  delete WaveGen;
  delete AccInput;
}

//==============================================================================
/// Initialisation of variables.
//==============================================================================
void JSph::InitVars(){
  ClearCfgDomain();
  OutPosCount=OutRhopCount=OutMoveCount=0;
  Simulate2D=false;
  Simulate2DPosY=0;
  Stable=false;
  Psimple=true;
  SvDouble=false;
  RunCode=CalcRunCode();
  RunTimeDate="";
  CaseName=""; DirCase=""; DirOut=""; RunName="";
//£££££££££££££££££££££££££££££££££££££££££££££££££££££££££££££££££££££££//MP
  TPhase=FLOW_Noflow;
  MPCoef=0;
  ShiftGrad=TFloat3(0);
//£££££££££££££££££££££££££££££££££££££££££££££££££££££££££££££££££££££££//MP
  TStep=STEP_None;
  VerletSteps=40;
  TKernel=KERNEL_Wendland;
  Awen=Bwen=0;
  memset(&CubicCte,0,sizeof(StCubicCte));
  TVisco=VISCO_None;
  TDeltaSph=DELTA_None; DeltaSph=0;
  TShifting=SHIFT_None; ShiftCoef=ShiftTFS=0;
  Surft=false;
  Visco=0; ViscoBoundFactor=1;
  UseDEM=false;  //(DEM)
  DemDtForce=0;  //(DEM)
  memset(DemObjs,0,sizeof(StDemData)*DemObjsSize);  //(DEM)
  RhopOut=true; RhopOutMin=700; RhopOutMax=1300;
  TimeMax=TimePart=0;
  DtIni=DtMin=0; CoefDtMin=0; DtAllParticles=false;
  PartsOutMax=0;
  NpMinimum=0;

  SvData=byte(SDAT_Binx)|byte(SDAT_Info);
  SvRes=false;
  SvTimers=false;
  SvDomainVtk=false;

  H=CteB=Gamma=RhopZero=CFLnumber=0;
  Dp=0;
  Cs0=0;
  Delta2H=0;
  MassFluid=MassBound=0;
  LatticeFluid=LatticeBound=0;
  Gravity=TFloat3(0);
  Dosh=H2=Fourh2=Eta2=0;
  SpsSmag=SpsBlin=0;

  CasePosMin=CasePosMax=TDouble3(0);
  CaseNp=CaseNbound=CaseNfixed=CaseNmoving=CaseNfloat=CaseNfluid=CaseNpb=0;

  ResetMkInfo();

  memset(&PeriodicConfig,0,sizeof(StPeriodic));
  PeriActive=0;
  PeriX=PeriY=PeriZ=PeriXY=PeriXZ=PeriYZ=false;
  PeriXinc=PeriYinc=PeriZinc=TDouble3(0);

  PartBeginDir=""; 
  PartBegin=PartBeginFirst=0;
  PartBeginTimeStep=0; 
  PartBeginTotalNp=0;

  MotionTimeMod=0;
  MotionObjCount=0;
  memset(MotionObjBegin,0,sizeof(unsigned)*256);

  FtCount=0;
  FtPause=0;

//£££££££££££££££££££££££££££££££££££££££££££££££££££££££££££££££££££££££//MP
  AllocMemoryMP(0);
//£££££££££££££££££££££££££££££££££££££££££££££££££££££££££££££££££££££££//MP
  AllocMemoryFloating(0);

  CellOrder=ORDER_None;
  CellMode=CELLMODE_None;
  Hdiv=0;
  Scell=0;
  MovLimit=0;

  Map_PosMin=Map_PosMax=Map_Size=TDouble3(0);
  Map_Cells=TUint3(0);
  MapRealPosMin=MapRealPosMax=MapRealSize=TDouble3(0);

  DomCelIni=DomCelFin=TUint3(0);
  DomCells=TUint3(0);
  DomPosMin=DomPosMax=DomSize=TDouble3(0);
  DomRealPosMin=DomRealPosMax=TDouble3(0);
  DomCellCode=0;

  NpDynamic=ReuseIds=false;
  TotalNp=0; IdMax=0;

  DtModif=0;
  PartDtMin=DBL_MAX; PartDtMax=-DBL_MAX;

  MaxMemoryCpu=MaxMemoryGpu=MaxParticles=MaxCells=0;

  PartIni=Part=0; 
  Nstep=0; PartNstep=-1;
  PartOut=0;

  TimeStepIni=0;
  TimeStep=TimeStepM1=0;
  TimePartNext=0;
}

//£££££££££££££££££££££££££££££££££££££££££££££££££££££££££££££££££££££££//MP
//==============================================================================
/// Shell sorting algorithm to arrange the maximum height array in order to
/// find the hydrostatic pressure -££//MP
//==============================================================================
void JSph::ShellSortMP(double v[], unsigned n){
    unsigned gap, i;
	int j;
	double temp;
	for (gap = n/2; gap > 0; gap /= 2){
		for (i=gap; i<n; i++){
			for (j=i-gap; (j>=0 && v[j+gap]>=v[j]); j-=gap) {
				temp=v[j];
				v[j]=v[j+gap];
				v[j+gap]=temp;
			}
		}
	}
}
//£££££££££££££££££££££££££££££££££££££££££££££££££££££££££££££££££££££££//MP

//==============================================================================
/// Generates a random code to identify the file of the results of the execution.
//==============================================================================
std::string JSph::CalcRunCode()const{
  srand((unsigned)time(NULL));
  const unsigned len=8;
  char code[len+1];
  for(unsigned c=0;c<len;c++){
    char let=char(float(rand())/float(RAND_MAX)*36);
    code[c]=(let<10? let+48: let+87);
  } 
  code[len]=0;
  return(code);
}

//==============================================================================
/// Returns the code version in text format.
//==============================================================================
std::string JSph::GetVersionStr(){
  return(fun::PrintStr("%1.2f",float(VersionMajor)/100));
}

//==============================================================================
/// Sets the configuration of the domain limits by default.
//==============================================================================
void JSph::ClearCfgDomain(){
  CfgDomainParticles=true;
  CfgDomainParticlesMin=CfgDomainParticlesMax=TDouble3(0);
  CfgDomainParticlesPrcMin=CfgDomainParticlesPrcMax=TDouble3(0);
  CfgDomainFixedMin=CfgDomainFixedMax=TDouble3(0);
}

//==============================================================================
/// Sets the configuration of the domain limits using given values.
//==============================================================================
void JSph::ConfigDomainFixed(tdouble3 vmin,tdouble3 vmax){
  ClearCfgDomain();
  CfgDomainParticles=false;
  CfgDomainFixedMin=vmin; CfgDomainFixedMax=vmax;
}

//==============================================================================
/// Sets the configuration of the domain limits using positions of particles.
//==============================================================================
void JSph::ConfigDomainParticles(tdouble3 vmin,tdouble3 vmax){
  CfgDomainParticles=true;
  CfgDomainParticlesMin=vmin; CfgDomainParticlesMax=vmax;
}

//==============================================================================
/// Sets the configuration of the domain limits using positions plus a percentage.
//==============================================================================
void JSph::ConfigDomainParticlesPrc(tdouble3 vmin,tdouble3 vmax){
  CfgDomainParticles=true;
  CfgDomainParticlesPrcMin=vmin; CfgDomainParticlesPrcMax=vmax;
}

//==============================================================================
/// Allocates memory of floating objectcs.
//==============================================================================
void JSph::AllocMemoryFloating(unsigned ftcount){
  delete[] FtObjs; FtObjs=NULL;
  if(ftcount)FtObjs=new StFloatingData[ftcount];
}

//£££££££££££££££££££££££££££££££££££££££££££££££££££££££££££££££££££££££//MP
//==============================================================================
/// Allocates memory for multiphase flows. -££//MP
//==============================================================================
void JSph::AllocMemoryMP(unsigned mpcount){
  delete[] PhaseData; PhaseData=NULL;
  if(mpcount)PhaseData=new StPhaseData[mpcount];
}
//£££££££££££££££££££££££££££££££££££££££££££££££££££££££££££££££££££££££//MP

//==============================================================================
/// Returns the allocated memory in CPU.
//==============================================================================
llong JSph::GetAllocMemoryCpu()const{  
  //-Allocated in AllocMemoryCase().
  llong s=0;
  //-Allocated in AllocMemoryFloating().
  if(FtObjs)s+=sizeof(StFloatingData)*FtCount;
//£££££££££££££££££££££££££££££££££££££££££££££££££££££££££££££££££££££££//MP
  //-Allocated in AllocMemoryMP().
  if(PhaseData)s+=sizeof(StPhaseData)*MkListSize;
//£££££££££££££££££££££££££££££££££££££££££££££££££££££££££££££££££££££££//MP
  //-Allocated in other objects.
  if(PartsOut)s+=PartsOut->GetAllocMemory();
  if(ViscoTime)s+=ViscoTime->GetAllocMemory();
  if(DtFixed)s+=DtFixed->GetAllocMemory();
  if(AccInput)s+=AccInput->GetAllocMemory();
  return(s);
}

//==============================================================================
/// Loads the configuration of the execution.
//==============================================================================
void JSph::LoadConfig(const JCfgRun *cfg){
  const char* met="LoadConfig";
  TimerTot.Start();
  Stable=cfg->Stable;
  Psimple=true; SvDouble=false; //-Options by default.
  DirOut=fun::GetDirWithSlash(cfg->DirOut);
  CaseName=cfg->CaseName; 
  DirCase=fun::GetDirWithSlash(fun::GetDirParent(CaseName));
  CaseName=CaseName.substr(DirCase.length());
  if(!CaseName.length())RunException(met,"Name of the case for execution was not indicated.");
  RunName=(cfg->RunName.length()? cfg->RunName: CaseName);
  PartBeginDir=cfg->PartBeginDir; PartBegin=cfg->PartBegin; PartBeginFirst=cfg->PartBeginFirst;

  //-Opciones de salida:
  SvData=byte(SDAT_None); 
  if(cfg->Sv_Csv)SvData|=byte(SDAT_Csv);
  if(cfg->Sv_Binx)SvData|=byte(SDAT_Binx);
  if(cfg->Sv_Info)SvData|=byte(SDAT_Info);
  if(cfg->Sv_Vtk)SvData|=byte(SDAT_Vtk);

  SvRes=cfg->SvRes;
  SvTimers=cfg->SvTimers;
  SvDomainVtk=cfg->SvDomainVtk;

  printf("\n");
  RunTimeDate=fun::GetDateTime();
  Log->Printf("[Initialising %s v%s  %s]",ClassName.c_str(),GetVersionStr().c_str(),RunTimeDate.c_str());

  string tx=fun::VarStr("CaseName",CaseName);
  tx=tx+";\n"+fun::VarStr("DirCase",DirCase)+";\n"+fun::VarStr("RunName",RunName)+";\n"+fun::VarStr("DirOut",DirOut)+";";
  if(PartBegin){
    Log->Print(fun::VarStr("PartBegin",PartBegin));
    Log->Print(fun::VarStr("PartBeginDir",PartBeginDir));
    Log->Print(fun::VarStr("PartBeginFirst",PartBeginFirst));
  }

  LoadCaseConfig();

  //-Aplies configuration using command line.
  if(cfg->PosDouble==0){      Psimple=true;  SvDouble=false; }
  else if(cfg->PosDouble==1){ Psimple=false; SvDouble=false; }
  else if(cfg->PosDouble==2){ Psimple=false; SvDouble=true;  }
//£££££££££££££££££££££££££££££££££££££££££££££££££££££££££££££££££££££££//MP
  if(cfg->TPhase)TPhase=cfg->TPhase;
//£££££££££££££££££££££££££££££££££££££££££££££££££££££££££££££££££££££££//MP
  if(cfg->TStep)TStep=cfg->TStep;
  if(cfg->VerletSteps>=0)VerletSteps=cfg->VerletSteps;
  if(cfg->TKernel)TKernel=cfg->TKernel;
  if(cfg->TVisco){ TVisco=cfg->TVisco; Visco=cfg->Visco; }
  if(cfg->ViscoBoundFactor>=0)ViscoBoundFactor=cfg->ViscoBoundFactor;
  if(cfg->DeltaSph>=0){
    DeltaSph=cfg->DeltaSph;
    TDeltaSph=(DeltaSph? DELTA_Dynamic: DELTA_None);
  }
  if(TDeltaSph==DELTA_Dynamic && Cpu)TDeltaSph=DELTA_DynamicExt; //-It is necessary because the interaction is divided in two steps: fluid-fluid/float and fluid-bound.

  if(cfg->Shifting>=0){
    switch(cfg->Shifting){
      case 0:  TShifting=SHIFT_None;     break;
      case 1:  TShifting=SHIFT_NoBound;  break;
      case 2:  TShifting=SHIFT_NoFixed;  break;
      case 3:  TShifting=SHIFT_Full;     break;
      default: RunException(met,"Shifting mode is not valid.");
    }
    if(TShifting!=SHIFT_None){
      ShiftCoef=-2; ShiftTFS=0;
    }
    else ShiftCoef=ShiftTFS=0;
  }
//£££££££££££££££££££££££££££££££££££££££££££££££££££££££££££££££££££££££//MP
  if(cfg->ShiftGrad.x!=0)ShiftGrad.x=cfg->ShiftGrad.x;
  if(cfg->ShiftGrad.y!=0)ShiftGrad.y=cfg->ShiftGrad.y;
  if(cfg->ShiftGrad.z!=0)ShiftGrad.z=cfg->ShiftGrad.z;
//£££££££££££££££££££££££££££££££££££££££££££££££££££££££££££££££££££££££//MP
  if(cfg->FtPause>=0)FtPause=cfg->FtPause;
  if(cfg->TimeMax>0)TimeMax=cfg->TimeMax;
  //-Configuration of JTimeOut with TimePart.
  TimeOut=new JTimeOut();
  if(cfg->TimePart>=0){
    TimePart=cfg->TimePart;
    TimeOut->Config(TimePart);
  }
  else TimeOut->Config(FileXml,"case.execution.special.timeout",TimePart);

  CellOrder=cfg->CellOrder;
  CellMode=cfg->CellMode;
  if(cfg->DomainMode==1){
    ConfigDomainParticles(cfg->DomainParticlesMin,cfg->DomainParticlesMax);
    ConfigDomainParticlesPrc(cfg->DomainParticlesPrcMin,cfg->DomainParticlesPrcMax);
  }
  else if(cfg->DomainMode==2)ConfigDomainFixed(cfg->DomainFixedMin,cfg->DomainFixedMax);
  if(cfg->RhopOutModif){
    RhopOutMin=cfg->RhopOutMin; RhopOutMax=cfg->RhopOutMax;
  }
  RhopOut=(RhopOutMin<RhopOutMax);
  if(!RhopOut){ RhopOutMin=-FLT_MAX; RhopOutMax=FLT_MAX; }
}

//==============================================================================
/// Loads the case configuration to be executed.
//==============================================================================
void JSph::LoadCaseConfig(){
  const char* met="LoadCaseConfig";
  FileXml=DirCase+CaseName+".xml";
  if(!fun::FileExists(FileXml))RunException(met,"Case configuration was not found.",FileXml);
  JXml xml; xml.LoadFile(FileXml);
  JSpaceCtes ctes;
  ctes.LoadXmlRun(&xml,"case.execution.constants");
//£££££££££££££££££££££££££££££££££££££££££££££££££££££££££££££££££££££££//MP
  ctes.LoadXmlRunLattMP(&xml,"case.casedef.constantsdef");
//£££££££££££££££££££££££££££££££££££££££££££££££££££££££££££££££££££££££//MP
  JSpaceEParms eparms; eparms.LoadXml(&xml,"case.execution.parameters");
  JSpaceParts parts;   parts.LoadXml(&xml,"case.execution.particles");

  //-Execution parameters.
  switch(eparms.GetValueInt("PosDouble",true,0)){
    case 0:  Psimple=true;  SvDouble=false;  break;
    case 1:  Psimple=false; SvDouble=false;  break;
    case 2:  Psimple=false; SvDouble=true;   break;
    default: RunException(met,"PosDouble value is not valid.");
  }
//£££££££££££££££££££££££££££££££££££££££££££££££££££££££££££££££££££££££//MP
  switch(eparms.GetValueInt("FlowType",true,1)){ 
    case 1:  TPhase=FLOW_Single;     break;
    case 2:  TPhase=FLOW_Multi;		 break;
    default: RunException(met,"Flow type is not valid.");
  }
//£££££££££££££££££££££££££££££££££££££££££££££££££££££££££££££££££££££££//MP
  switch(eparms.GetValueInt("RigidAlgorithm",true,1)){ //(DEM)
    case 1:  UseDEM=false;  break;
    case 2:  UseDEM=true;   break;
    default: RunException(met,"Rigid algorithm is not valid.");
  }
  switch(eparms.GetValueInt("StepAlgorithm",true,1)){
    case 1:  TStep=STEP_Verlet;      break;
    case 2:  TStep=STEP_Symplectic;  break;
    default: RunException(met,"Step algorithm is not valid.");
  }
  VerletSteps=eparms.GetValueInt("VerletSteps",true,40);
  switch(eparms.GetValueInt("Kernel",true,2)){
    case 1:  TKernel=KERNEL_Cubic;     break;
    case 2:  TKernel=KERNEL_Wendland;  break;
    default: RunException(met,"Kernel choice is not valid.");
  }
  switch(eparms.GetValueInt("ViscoTreatment",true,1)){
    case 1:  TVisco=VISCO_Artificial;  break;
    case 2:  TVisco=VISCO_LaminarSPS;  break;
    default: RunException(met,"Viscosity treatment is not valid.");
  }
  Visco=eparms.GetValueFloat("Visco");
  ViscoBoundFactor=eparms.GetValueFloat("ViscoBoundFactor",true,1.f);
  string filevisco=eparms.GetValueStr("ViscoTime",true);
  if(!filevisco.empty()){
    ViscoTime=new JSphVisco();
    ViscoTime->LoadFile(DirCase+filevisco);
  }
  DeltaSph=eparms.GetValueFloat("DeltaSPH",true,0);
  TDeltaSph=(DeltaSph? DELTA_Dynamic: DELTA_None);

  switch(eparms.GetValueInt("Shifting",true,0)){
    case 0:  TShifting=SHIFT_None;     break;
    case 1:  TShifting=SHIFT_NoBound;  break;
    case 2:  TShifting=SHIFT_NoFixed;  break;
    case 3:  TShifting=SHIFT_Full;     break;
    default: RunException(met,"Shifting mode is not valid.");
  }
  if(TShifting!=SHIFT_None){
    ShiftCoef=eparms.GetValueFloat("ShiftCoef",true,-2);
    if(ShiftCoef==0)TShifting=SHIFT_None;
    else ShiftTFS=eparms.GetValueFloat("ShiftTFS",true,0);
  }

//£££££££££££££££££££££££££££££££££££££££££££££££££££££££££££££££££££££££//MP
  MPCoef=eparms.GetValueFloat("MPCoefficient",true,0);
  ShiftGrad.x=eparms.GetValueFloat("ShiftNormalGradX",true,0);
  ShiftGrad.y=eparms.GetValueFloat("ShiftNormalGradY",true,0);
  ShiftGrad.z=eparms.GetValueFloat("ShiftNormalGradZ",true,0);
//£££££££££££££££££££££££££££££££££££££££££££££££££££££££££££££££££££££££//MP
  FtPause=eparms.GetValueFloat("FtPause",true,0);
  TimeMax=eparms.GetValueDouble("TimeMax");
  TimePart=eparms.GetValueDouble("TimeOut");

  DtIni=eparms.GetValueDouble("DtIni",true,0);
  DtMin=eparms.GetValueDouble("DtMin",true,0);
  CoefDtMin=eparms.GetValueFloat("CoefDtMin",true,0.05f);
  DtAllParticles=(eparms.GetValueInt("DtAllParticles",true,0)==1);

  string filedtfixed=eparms.GetValueStr("DtFixed",true);
  if(!filedtfixed.empty()){
    DtFixed=new JSphDtFixed();
    DtFixed->LoadFile(DirCase+filedtfixed);
  }
  if(eparms.Exists("RhopOutMin"))RhopOutMin=eparms.GetValueFloat("RhopOutMin");
  if(eparms.Exists("RhopOutMax"))RhopOutMax=eparms.GetValueFloat("RhopOutMax");
  PartsOutMax=eparms.GetValueFloat("PartsOutMax",true,1);

  //-Configuration of periodic boundaries.
  if(eparms.Exists("XPeriodicIncY")){ PeriXinc.y=eparms.GetValueDouble("XPeriodicIncY"); PeriX=true; }
  if(eparms.Exists("XPeriodicIncZ")){ PeriXinc.z=eparms.GetValueDouble("XPeriodicIncZ"); PeriX=true; }
  if(eparms.Exists("YPeriodicIncX")){ PeriYinc.x=eparms.GetValueDouble("YPeriodicIncX"); PeriY=true; }
  if(eparms.Exists("YPeriodicIncZ")){ PeriYinc.z=eparms.GetValueDouble("YPeriodicIncZ"); PeriY=true; }
  if(eparms.Exists("ZPeriodicIncX")){ PeriZinc.x=eparms.GetValueDouble("ZPeriodicIncX"); PeriZ=true; }
  if(eparms.Exists("ZPeriodicIncY")){ PeriZinc.y=eparms.GetValueDouble("ZPeriodicIncY"); PeriZ=true; }
  if(eparms.Exists("XYPeriodic")){ PeriXY=PeriX=PeriY=true; PeriXZ=PeriYZ=false; PeriXinc=PeriYinc=TDouble3(0); }
  if(eparms.Exists("XZPeriodic")){ PeriXZ=PeriX=PeriZ=true; PeriXY=PeriYZ=false; PeriXinc=PeriZinc=TDouble3(0); }
  if(eparms.Exists("YZPeriodic")){ PeriYZ=PeriY=PeriZ=true; PeriXY=PeriXZ=false; PeriYinc=PeriZinc=TDouble3(0); }
  PeriActive=DefPeriActive(PeriX,PeriY,PeriZ);

  //-Configuration of domain size.
//£££££££££££££££££££££££££££££££££££££££££££££££££££££££££££££££££££££££//MP
  float incx=eparms.GetValueFloat("IncX",true,0.f);
  if(incx){
    ClearCfgDomain();
    CfgDomainParticlesPrcMax.x=incx;
  }
  float incy=eparms.GetValueFloat("IncY",true,0.f);
  if(incy){
    ClearCfgDomain();
    CfgDomainParticlesPrcMax.y=incy;
  }
//£££££££££££££££££££££££££££££££££££££££££££££££££££££££££££££££££££££££//MP
  float incz=eparms.GetValueFloat("IncZ",true,0.f);
  if(incz){
    ClearCfgDomain();
    CfgDomainParticlesPrcMax.z=incz;
  }
  if(eparms.Exists("DomainParticles")){
    string key="DomainParticles";
    ConfigDomainParticles(TDouble3(eparms.GetValueNumDouble(key,0),eparms.GetValueNumDouble(key,1),eparms.GetValueNumDouble(key,2)),TDouble3(eparms.GetValueNumDouble(key,3),eparms.GetValueNumDouble(key,4),eparms.GetValueNumDouble(key,5)));
  }
  if(eparms.Exists("DomainParticlesPrc")){
    string key="DomainParticlesPrc";
    ConfigDomainParticlesPrc(TDouble3(eparms.GetValueNumDouble(key,0),eparms.GetValueNumDouble(key,1),eparms.GetValueNumDouble(key,2)),TDouble3(eparms.GetValueNumDouble(key,3),eparms.GetValueNumDouble(key,4),eparms.GetValueNumDouble(key,5)));
  }
  if(eparms.Exists("DomainFixed")){
    string key="DomainFixed";
    ConfigDomainFixed(TDouble3(eparms.GetValueNumDouble(key,0),eparms.GetValueNumDouble(key,1),eparms.GetValueNumDouble(key,2)),TDouble3(eparms.GetValueNumDouble(key,3),eparms.GetValueNumDouble(key,4),eparms.GetValueNumDouble(key,5)));
  }

  //-Predefined constantes.
  if(ctes.GetEps()!=0)Log->Print("\n*** Attention: Eps value is not used (this correction is deprecated).\n");
  H=(float)ctes.GetH();
  CteB=(float)ctes.GetB();
  Gamma=(float)ctes.GetGamma();
  RhopZero=(float)ctes.GetRhop0();
  CFLnumber=(float)ctes.GetCFLnumber();
  Dp=ctes.GetDp();
  Gravity=ToTFloat3(ctes.GetGravity());
  MassFluid=(float)ctes.GetMassFluid();
  MassBound=(float)ctes.GetMassBound();
  LatticeFluid=ctes.GetLatticeFluid();
  LatticeBound=ctes.GetLatticeBound();

  //-Particle data.
  CaseNp=parts.Count();
  CaseNfixed=parts.Count(TpPartFixed);
  CaseNmoving=parts.Count(TpPartMoving);
  CaseNfloat=parts.Count(TpPartFloating);
  CaseNfluid=parts.Count(TpPartFluid);
  CaseNbound=CaseNp-CaseNfluid;
  CaseNpb=CaseNbound-CaseNfloat;

  NpDynamic=ReuseIds=false;
  TotalNp=CaseNp; IdMax=CaseNp-1;

  //-Loads and configures MK of particles.
  LoadMkInfo(&parts);

  //-Configuration of WaveGen.
  if(xml.GetNode("case.execution.special.wavepaddles",false)){
    if(!JWaveGen::Available())RunException(met,"Code for Wave-Paddles is not included in the current compilation.");
    WaveGen=new JWaveGen(Log,DirCase,&xml,"case.execution.special.wavepaddles");
  }

  //-Configuration of AccInput.
  if(xml.GetNode("case.execution.special.accinputs",false)){
    AccInput=new JSphAccInput(Log,DirCase,&xml,"case.execution.special.accinputs");
  }

  //-Loads and configures MOTION.
  MotionObjCount=0;
  for(unsigned c=0;c<parts.CountBlocks();c++){
    const JSpacePartBlock &block=parts.GetBlock(c);
    if(block.Type==TpPartMoving){
      if(MotionObjCount>=255)RunException(met,"The number of mobile objects exceeds the maximum.");
      MotionObjBegin[MotionObjCount]=block.GetBegin();
      MotionObjBegin[MotionObjCount+1]=MotionObjBegin[MotionObjCount]+block.GetCount();
      if(WaveGen)WaveGen->ConfigPaddle(block.GetMkType(),MotionObjCount,block.GetBegin(),block.GetCount());
      MotionObjCount++;
    }
  }

  if(MotionObjCount){
    Motion=new JSphMotion();
    if(int(MotionObjCount)<Motion->Init(&xml,"case.execution.motion",DirCase))RunException(met,"The number of mobile objects is lower than expected.");
  }

//£££££££££££££££££££££££££££££££££££££££££££££££££££££££££££££££££££££££//MP
  //-Loads fluid properties for multiphase flow
  if(TPhase==FLOW_Multi){
	  AllocMemoryMP(MkListSize);
	  unsigned mpobj=0;
	  for(unsigned c=0;c<parts.CountBlocks();c++){
        unsigned mpmk=0;
		const JSpacePartBlock &block=parts.GetBlock(c);
		for(;mpmk<MkListSize && MkList[mpmk].mk!=unsigned(block.GetMk());mpmk++);
		if(mpmk>=MkListSize)RunException(met,"Error loading multiphase data.");
		const JSpacePartBlock &mpblock=(const JSpacePartBlock &)block;
		PhaseData[c].MkValue=CODE_GetTypeAndValue(MkList[c].code);
		PhaseData[c].PhaseRhop0=(float)mpblock.GetSubValueFloat("rhop0","value",false,0);
		if(!block.ExistsSubValue("rhop0","value"))RunException(met,fun::PrintStr("Object mk=%u - Value of rhop0 is invalid.",block.GetMk()));
		PhaseData[c].PhaseGamma=(float)mpblock.GetSubValueFloat("gamma","value",false,0);
		if(!block.ExistsSubValue("gamma","value"))RunException(met,fun::PrintStr("Object mk=%u - Value of gamma is invalid.",block.GetMk()));
		PhaseData[c].PhaseCs0=(float)mpblock.GetSubValueFloat("cs0","value",false,0);
		if(!block.ExistsSubValue("cs0","value"))RunException(met,fun::PrintStr("Object mk=%u - Value of cs0 is invalid.",block.GetMk()));
		PhaseData[c].PhaseVisco=(float)mpblock.GetSubValueFloat("visco","value",false,0);
		if(!block.ExistsSubValue("visco","value"))RunException(met,fun::PrintStr("Object mk=%u - Value of visco is invalid.",block.GetMk()));
		PhaseData[c].PhaseType=mpblock.GetSubValueInt("phasetype","value",false,0);
		if(!block.ExistsSubValue("phasetype","value"))RunException(met,fun::PrintStr("Object mk=%u - Phase is invalid.",block.GetMk()));
		PhaseData[c].PhaseSurf=(float)mpblock.GetSubValueFloat("surfcoef","value",true,0);
		if(PhaseData[c].PhaseSurf)Surft=true;
	}
  }
//£££££££££££££££££££££££££££££££££££££££££££££££££££££££££££££££££££££££//MP

  //-Loads floating objects.
  FtCount=parts.CountBlocks(TpPartFloating);
  if(FtCount){
    AllocMemoryFloating(FtCount);
    unsigned cobj=0;
    for(unsigned c=0;c<parts.CountBlocks()&&cobj<FtCount;c++){
      const JSpacePartBlock &block=parts.GetBlock(c);
      if(block.Type==TpPartFloating){
        const JSpacePartBlock_Floating &fblock=(const JSpacePartBlock_Floating &)block;
        StFloatingData* fobj=FtObjs+cobj;
        fobj->mkbound=fblock.GetMkType();
        fobj->begin=fblock.GetBegin();
        fobj->count=fblock.GetCount();
        fobj->mass=(float)fblock.GetMassbody();
        fobj->massp=fobj->mass/fobj->count;
        fobj->radius=0;
        fobj->center=fblock.GetCenter();
        fobj->fvel=ToTFloat3(fblock.GetVelini());
        fobj->fomega=ToTFloat3(fblock.GetOmegaini());
        cobj++;
      }
    }
  }
  else UseDEM=false;

  //-Carga datos DEM de objetos. (DEM)
  //-Loads DEM data for the objects. (DEM)
  if(UseDEM){
    memset(DemObjs,0,sizeof(StDemData)*DemObjsSize);
    for(unsigned c=0;c<parts.CountBlocks();c++){
      const JSpacePartBlock &block=parts.GetBlock(c);
      if(block.Type!=TpPartFluid){
        unsigned cmk=0;
        for(;cmk<MkListBound && MkList[cmk].mk!=unsigned(block.GetMk());cmk++);
        if(cmk>=MkListBound)RunException(met,"Error loading DEM objects.");
        const unsigned tav=CODE_GetTypeAndValue(MkList[c].code);
        if(block.Type==TpPartFloating){
          const JSpacePartBlock_Floating &fblock=(const JSpacePartBlock_Floating &)block;
          DemObjs[tav].mass=(float)fblock.GetMassbody();
          DemObjs[tav].massp=(float)(fblock.GetMassbody()/fblock.GetCount());
        }
        else DemObjs[tav].massp=MassBound;
        if(!block.ExistsSubValue("Young_Modulus","value"))RunException(met,fun::PrintStr("Object mk=%u - Value of Young_Modulus is invalid.",block.GetMk()));
        if(!block.ExistsSubValue("PoissonRatio","value"))RunException(met,fun::PrintStr("Object mk=%u - Value of PoissonRatio is invalid.",block.GetMk()));
        if(!block.ExistsSubValue("Kfric","value"))RunException(met,fun::PrintStr("Object mk=%u - Value of Kfric is invalid.",block.GetMk()));
        if(!block.ExistsSubValue("Restitution_Coefficient","value"))RunException(met,fun::PrintStr("Object mk=%u - Value of Restitution_Coefficient is invalid.",block.GetMk()));
        DemObjs[tav].young=block.GetSubValueFloat("Young_Modulus","value",true,0);
        DemObjs[tav].poisson=block.GetSubValueFloat("PoissonRatio","value",true,0);
        DemObjs[tav].tau=(DemObjs[tav].young? (1-DemObjs[tav].poisson*DemObjs[tav].poisson)/DemObjs[tav].young: 0);
        DemObjs[tav].kfric=block.GetSubValueFloat("Kfric","value",true,0);
        DemObjs[tav].restitu=block.GetSubValueFloat("Restitution_Coefficient","value",true,0);
        if(block.ExistsValue("Restitution_Coefficient_User"))DemObjs[tav].restitu=block.GetValueFloat("Restitution_Coefficient_User");
      }
    }
  }

  NpMinimum=CaseNp-unsigned(PartsOutMax*CaseNfluid);
  Log->Print("**Basic case configuration is loaded");
}

//==============================================================================
/// Shows coefficients used for DEM objects.
//==============================================================================
void JSph::VisuDemCoefficients()const{
  //-Gets info for each block of particles.
  Log->Printf("Coefficients for DEM:");
  for(unsigned c=0;c<MkListSize;c++){
    const word code=MkList[c].code;
    const word type=CODE_GetType(code);
    const unsigned tav=CODE_GetTypeAndValue(MkList[c].code);
    if(type==CODE_TYPE_FIXED || type==CODE_TYPE_MOVING || type==CODE_TYPE_FLOATING){
      Log->Printf("  Object %s  mkbound:%u  mk:%u",(type==CODE_TYPE_FIXED? "Fixed": (type==CODE_TYPE_MOVING? "Moving": "Floating")),MkList[c].mktype,MkList[c].mk);
      //Log->Printf("    type: %u",type);
      Log->Printf("    Young_Modulus: %g",DemObjs[tav].young);
      Log->Printf("    PoissonRatio.: %g",DemObjs[tav].poisson);
      Log->Printf("    Kfric........: %g",DemObjs[tav].kfric);
      Log->Printf("    Restitution..: %g",DemObjs[tav].restitu);
    }
  }
}

//£££££££££££££££££££££££££££££££££££££££££££££££££££££££££££££££££££££££//MP
//==============================================================================
/// Shows coefficients used for multiphase flows. -££//MP
//==============================================================================
void JSph::VisuMP()const{
  //-Gets info for each block of particles.
  Log->Printf("================Multiphase flow data================");
  Log->Print("----------------------------------");
  for(unsigned c=0;c<MkListSize;c++){
	  Log->Print(fun::VarStr("Mk Value",PhaseData[c].MkValue));
      const word code=MkList[c].code;
	  const word type=CODE_GetType(code);
	  Log->Printf("  Object Type: %s ",(type==CODE_TYPE_FIXED? "Fixed": (type==CODE_TYPE_MOVING? "Moving": (type==CODE_TYPE_FLOATING?"Floating":"Fluid"))));
	  Log->Printf("  Phase Type: %s ",(PhaseData[c].PhaseType>1? "Solid": (PhaseData[c].PhaseType==0? "Liquid":"Gas")));
	  Log->Print(fun::VarStr("Initial density",PhaseData[c].PhaseRhop0));
	  Log->Print(fun::VarStr("Speed of sound",PhaseData[c].PhaseCs0));
	  Log->Print(fun::VarStr("Polytropic index",PhaseData[c].PhaseGamma));
	  Log->Print(fun::VarStr("Particle Mass",PhaseData[c].PhaseMass));
	  Log->Print(fun::VarStr("Dynamic viscosity value",PhaseData[c].PhaseVisco));
	  Log->Print(fun::VarStr("Surface tension coefficient",PhaseData[c].PhaseSurf));
	  Log->Print("----------------------------------");
  }
}
//£££££££££££££££££££££££££££££££££££££££££££££££££££££££££££££££££££££££//MP

//==============================================================================
/// Initialisation of MK information.
//==============================================================================
void JSph::ResetMkInfo(){
  delete[] MkList; MkList=NULL;
  MkListSize=MkListFixed=MkListMoving=MkListFloat=MkListBound=MkListFluid=0;
}

//==============================================================================
/// Load MK information of particles.
//==============================================================================
void JSph::LoadMkInfo(const JSpaceParts *parts){
  ResetMkInfo();
  MkListSize=parts->CountBlocks();
  MkListFixed=parts->CountBlocks(TpPartFixed);
  MkListMoving=parts->CountBlocks(TpPartMoving);
  MkListFloat=parts->CountBlocks(TpPartFloating);
  MkListFluid=parts->CountBlocks(TpPartFluid);
  MkListBound=MkListFixed+MkListMoving+MkListFloat;
  //-Allocates memory.
  MkList=new StMkInfo[MkListSize];
  //-Gets info for each block of particles.
  for(unsigned c=0;c<MkListSize;c++){
    const JSpacePartBlock &block=parts->GetBlock(c);
    MkList[c].begin=block.GetBegin();
    MkList[c].count=block.GetCount();
    MkList[c].mk=block.GetMk();
    MkList[c].mktype=block.GetMkType();
    switch(block.Type){
      case TpPartFixed:     MkList[c].code=CodeSetType(0,PART_BoundFx,c);                           break;
      case TpPartMoving:    MkList[c].code=CodeSetType(0,PART_BoundMv,c-MkListFixed);               break;
      case TpPartFloating:  MkList[c].code=CodeSetType(0,PART_BoundFt,c-MkListFixed-MkListMoving);  break;
      case TpPartFluid:     MkList[c].code=CodeSetType(0,PART_Fluid,c-MkListBound);                 break;
    }
  }
}

//==============================================================================
/// Returns the block in MkList according to a given Id.
//==============================================================================
unsigned JSph::GetMkBlockById(unsigned id)const{
  unsigned c=0;
  for(;c<MkListSize && id>=(MkList[c].begin+MkList[c].count);c++);
  return(c);
}

//==============================================================================
/// Returns the block in MkList according to a given MK.
//==============================================================================
unsigned JSph::GetMkBlockByMk(word mk)const{
  unsigned c=0;
  for(;c<MkListSize && unsigned(mk)!=MkList[c].mk;c++);
  return(c);
}

//==============================================================================
/// Returns the code of a particle according to the given parameters.
//==============================================================================
word JSph::CodeSetType(word code,TpParticle type,unsigned value)const{
  const char met[]="CodeSetType"; 
  //-Chooses type.
  word tp;
  if(type==PART_BoundFx)tp=CODE_TYPE_FIXED;
  else if(type==PART_BoundMv)tp=CODE_TYPE_MOVING;
  else if(type==PART_BoundFt)tp=CODE_TYPE_FLOATING;
  else if(type==PART_Fluid)tp=CODE_TYPE_FLUID;
  else RunException(met,"Type of particle is invalid.");
  //-Checks the value.
  word v=word(value&CODE_MASKVALUE);
  if(unsigned(v)!=value)RunException(met,"The value is invalid.");
  //-Returns the new code.
  return(code&(~CODE_MASKTYPEVALUE)|tp|v);
}

//==============================================================================
/// Carga el codigo de grupo de las particulas y marca las nout ultimas
/// particulas como excluidas.
///
/// Loads the code of a particle group and flags the last "nout" 
/// particles as excluded. 
//==============================================================================
void JSph::LoadCodeParticles(unsigned np,const unsigned *idp,word *code)const{
  const char met[]="LoadCodeParticles"; 
  //-Assigns code to each group of particles (moving & floating).
  const unsigned finfixed=CaseNfixed;
  const unsigned finmoving=finfixed+CaseNmoving;
  const unsigned finfloating=finmoving+CaseNfloat;
  for(unsigned p=0;p<np;p++){
    const unsigned id=idp[p];
    word cod=0;
    unsigned cmk=GetMkBlockById(id);
    if(id<finfixed)cod=CodeSetType(cod,PART_BoundFx,cmk);
    else if(id<finmoving){
      cod=CodeSetType(cod,PART_BoundMv,cmk-MkListFixed);
      if(cmk-MkListFixed>=MotionObjCount)RunException(met,"Motion code of particles was not found.");
    }
    else if(id<finfloating){
      cod=CodeSetType(cod,PART_BoundFt,cmk-MkListFixed-MkListMoving);
      if(cmk-MkListFixed-MkListMoving>=FtCount)RunException(met,"Floating code of particles was not found.");
    }
    else{
      cod=CodeSetType(cod,PART_Fluid,cmk-MkListBound);
      if(cmk-MkListBound>=MkListSize)RunException(met,"Fluid code of particles was not found.");
    }
    code[p]=cod;
  }
}

//£££££££££££££££££££££££££££££££££££££££££££££££££££££££££££££££££££££££//MP
//==============================================================================
/// Updates initial particle density according to case configuration for multiphase flows. -££//MP
//==============================================================================
void JSph::UpdateRhopMP(unsigned np, unsigned npb,tdouble3* pos,tfloat4 *velrhop,word *code){
	const char met[]="UpdateRhopMP";
	//-Flows without gravity
	bool nograv=abs(Gravity.z)>0? 0 :(abs(Gravity.x)>0? 0:(Simulate2D? 1:(Gravity.y? 0:1))); //Checks if gravity forces apply on the flow
	if (nograv){ //no gravity applied
		for (unsigned p=0;p<CaseNp;p++){
	  		for (unsigned iphase=0; iphase<MkListSize; iphase++){
				if (CODE_GetTypeAndValue(code[p])==PhaseData[iphase].MkValue) {
					velrhop[p].w=PhaseData[iphase].PhaseRhop0;
					break;
				}
			}
		}					
	}
	else{
	//Define variables to identify hydrostatic pressure
		tdouble3 *maxheight=new tdouble3[MkListSize]; memset(maxheight,-10^10,sizeof(tdouble3)*MkListSize);
		tdouble3 *minheight=new tdouble3[MkListSize]; memset(minheight,10^10,sizeof(tdouble3)*MkListSize);
    //-Identify maximum and minimum heights for each phase and coordinate depending on the gravity vector. Only applies to fluid particles

		for (unsigned p=npb;p<np;p++){
			for (unsigned iphase=0; iphase<MkListSize; iphase++){
				//-Find maximum height for each phase
				if (CODE_GetTypeAndValue(code[p])==PhaseData[iphase].MkValue){
					if(Gravity.x!=0) {
						maxheight[iphase].x=((pos[p].x>maxheight[iphase].x)? pos[p].x : maxheight[iphase].x);  
						minheight[iphase].x=((pos[p].x<minheight[iphase].x)? pos[p].x : minheight[iphase].x);
					}
					if(Gravity.z!=0) {
						maxheight[iphase].z=((pos[p].z>maxheight[iphase].z)? pos[p].z : maxheight[iphase].z);
						minheight[iphase].z=((pos[p].z<minheight[iphase].z)? pos[p].z : minheight[iphase].z);
					}
					if(Gravity.y!=0&&!Simulate2D) {
						maxheight[iphase].y=((pos[p].y>maxheight[iphase].y)? pos[p].y : maxheight[iphase].y);
						minheight[iphase].y=((pos[p].y<minheight[iphase].y)? pos[p].y : minheight[iphase].y);
					}
					break;
				}
			}
		}
		unsigned SortSize=0;
		for (unsigned iphase=0; iphase<MkListSize; iphase++)if(PhaseData[iphase].MkValue>=6144)SortSize++;
		double *mheightsortx=new double[SortSize]; memset(mheightsortx,10^10,sizeof(float)*SortSize);
		double *mheightsorty=new double[SortSize]; memset(mheightsorty,10^10,sizeof(float)*SortSize);
		double *mheightsortz=new double[SortSize]; memset(mheightsortz,10^10,sizeof(float)*SortSize);
		double *Mheightsortx=new double[SortSize]; memset(Mheightsortx,-10^10,sizeof(float)*SortSize);
		double *Mheightsorty=new double[SortSize]; memset(Mheightsorty,-10^10,sizeof(float)*SortSize);
		double *Mheightsortz=new double[SortSize]; memset(Mheightsortz,-10^10,sizeof(float)*SortSize);
		unsigned hsort=0;
		for (unsigned iphase=0; iphase<MkListSize; iphase++){
			if(PhaseData[iphase].MkValue>=6144){
				if(Gravity.x!=0) {
					Mheightsortx[hsort]=maxheight[iphase].x;
					mheightsortx[hsort]=minheight[iphase].x;
				}
				if(Gravity.z!=0){
					Mheightsortz[hsort]=maxheight[iphase].z;
					mheightsortz[hsort]=minheight[iphase].z;
				}
				if(Gravity.y!=0&&!Simulate2D){
					Mheightsorty[hsort]=maxheight[iphase].y;
					mheightsorty[hsort]=minheight[iphase].y;
				}
				hsort++;
			}

		}
		//Rearrange phase order and find pressure based on maximum height
		if(Gravity.x!=0){ShellSortMP(mheightsortx,SortSize); ShellSortMP(Mheightsortx,SortSize);}
		if(Gravity.y!=0&&!Simulate2D){ShellSortMP(mheightsorty,SortSize); ShellSortMP(Mheightsorty,SortSize);}
		if(Gravity.z!=0){ShellSortMP(mheightsortz,SortSize); ShellSortMP(Mheightsortz,SortSize);}
		float *sortpres=new float[MkListSize]; memset(sortpres,0,sizeof(float)*MkListSize);
		float *maxpres=new float[MkListSize]; memset(maxpres,0,sizeof(float)*MkListSize);
		float tempres=0.f;
		for (unsigned isort=0; isort<SortSize; isort++){
			for (unsigned iphase=0; iphase<MkListSize; iphase++){
				if(Gravity.x!=0 && maxheight[iphase].x==Mheightsortx[isort]){
					maxpres[iphase]=sortpres[isort];
					tempres+=PhaseData[iphase].PhaseRhop0*abs(Gravity.x)*float(Mheightsortx[isort]-mheightsortx[isort])+sortpres[isort];
				}
				if((Gravity.y!=0&&!Simulate2D) && maxheight[iphase].y==Mheightsorty[isort]){
					maxpres[iphase]=sortpres[isort];
					tempres+=PhaseData[iphase].PhaseRhop0*abs(Gravity.y)*float(Mheightsorty[isort]-mheightsorty[isort])+sortpres[isort];
				}
				if(Gravity.z!=0 && maxheight[iphase].z==Mheightsortz[isort]){
					maxpres[iphase]=sortpres[isort];
					tempres+=PhaseData[iphase].PhaseRhop0*abs(Gravity.z)*float(Mheightsortz[isort]-mheightsortz[isort])+sortpres[isort];
				}
				if(isort<SortSize){sortpres[isort+1]=tempres; tempres=0.f;}
			}
		}
		//-Flows using gravity require creating a hydrostatic pressure profile, as the GenCase values cannot be imported
		float tempa=0.f;
		for (unsigned p=npb;p<np;p++){
			velrhop[p].w=0.f; //Re-initialises the particle density value
			for (unsigned iphase=0; iphase<MkListSize; iphase++){
				float overcteb=PhaseData[iphase].PhaseGamma/(PhaseData[iphase].PhaseCs0*PhaseData[iphase].PhaseCs0*PhaseData[iphase].PhaseRhop0);
				tempa=0.f;
				if(Gravity.x)if(CODE_GetTypeAndValue(code[p])==PhaseData[iphase].MkValue){
						tempa=PhaseData[iphase].PhaseRhop0*abs(Gravity.x)*float(maxheight[iphase].x-pos[p].x)+maxpres[iphase];
						velrhop[p].w+=PhaseData[iphase].PhaseRhop0* powf((1.f+tempa*overcteb),1.f/PhaseData[iphase].PhaseGamma);
				}
				if(Gravity.y!=0) {
					if(CODE_GetTypeAndValue(code[p])==PhaseData[iphase].MkValue){
						tempa=PhaseData[iphase].PhaseRhop0*abs(Gravity.y)*float(maxheight[iphase].y-pos[p].y)+maxpres[iphase];
						velrhop[p].w+=PhaseData[iphase].PhaseRhop0* powf((1.f+tempa*overcteb),1.f/PhaseData[iphase].PhaseGamma);
					}
				}
				if(Gravity.z!=0){
					if(CODE_GetTypeAndValue(code[p])==PhaseData[iphase].MkValue){
						tempa=PhaseData[iphase].PhaseRhop0*abs(Gravity.z)*float(maxheight[iphase].z-pos[p].z)+maxpres[iphase];
						velrhop[p].w+=PhaseData[iphase].PhaseRhop0* powf((1.f+tempa*overcteb),1.f/PhaseData[iphase].PhaseGamma);
					}
				}
			}
		}
		//-Alter particle density for the boundaries. Hydrostatic pressure not applied.
		for (unsigned p=0;p<CaseNpb;p++){
			for (unsigned iphaseb=0; iphaseb<MkListSize; iphaseb++){
				if (CODE_GetTypeAndValue(code[p])==PhaseData[iphaseb].MkValue){
					velrhop[p].w=PhaseData[iphaseb].PhaseRhop0;
					break;
				}
			}
		}
	}
}
//£££££££££££££££££££££££££££££££££££££££££££££££££££££££££££££££££££££££//MP

//£££££££££££££££££££££££££££££££££££££££££££££££££££££££££££££££££££££££//MP
//==============================================================================
/// Updates particle mass according to case configuration for multiphase flows. -££//MP
//==============================================================================
void JSph::UpdateMassMP(unsigned nphase,bool simulate2d){
	const char met[]="UpdateMassMP";
	float vol;
	if (simulate2d){
	  	for (unsigned iphase=0; iphase<nphase; iphase++) {
			vol=float(Dp*Dp);
			if (CODE_GetType(PhaseData[iphase].MkValue)==CODE_TYPE_FLUID) vol=vol/float(LatticeFluid);
			else vol=vol/float(LatticeBound);
			PhaseData[iphase].PhaseMass=PhaseData[iphase].PhaseRhop0*vol;
		}
	}
	else{
	  	for (unsigned iphase=0; iphase<nphase; iphase++) {
			vol=float(Dp*Dp*Dp);
			if (CODE_GetType(PhaseData[iphase].MkValue)==CODE_TYPE_FLUID) vol=vol/float(LatticeFluid);
			else vol=vol/float(LatticeBound);
			PhaseData[iphase].PhaseMass=PhaseData[iphase].PhaseRhop0*vol;
		}
	}
}
//£££££££££££££££££££££££££££££££££££££££££££££££££££££££££££££££££££££££//MP

//==============================================================================
/// Resizes limits of the map according to case configuration.
//==============================================================================
void JSph::ResizeMapLimits(){
  Log->Print(string("MapRealPos(border)=")+fun::Double3gRangeStr(MapRealPosMin,MapRealPosMax));
  tdouble3 dmin=MapRealPosMin,dmax=MapRealPosMax;
  if(CfgDomainParticles){
    tdouble3 dif=dmax-dmin;
    dmin=dmin-dif*CfgDomainParticlesPrcMin;
    dmax=dmax+dif*CfgDomainParticlesPrcMax;
    dmin=dmin-CfgDomainParticlesMin;
    dmax=dmax+CfgDomainParticlesMax;
  }
  else{ dmin=CfgDomainFixedMin; dmax=CfgDomainFixedMax; }
  if(dmin.x>MapRealPosMin.x||dmin.y>MapRealPosMin.y||dmin.z>MapRealPosMin.z||dmax.x<MapRealPosMax.x||dmax.y<MapRealPosMax.y||dmax.z<MapRealPosMax.z)RunException("ResizeMapLimits","Domain limits is not valid.");
  if(!PeriX){ MapRealPosMin.x=dmin.x; MapRealPosMax.x=dmax.x; }
  if(!PeriY){ MapRealPosMin.y=dmin.y; MapRealPosMax.y=dmax.y; }
  if(!PeriZ){ MapRealPosMin.z=dmin.z; MapRealPosMax.z=dmax.z; }
}

//==============================================================================
/// Configures value of constants.
//==============================================================================
void JSph::ConfigConstants(bool simulate2d){
  const char* met="ConfigConstants";
  //-Computation of constants.
  const double h=H;
  Delta2H=float(h*2*DeltaSph);
//£££££££££££££££££££££££££££££££££££££££££££££££££££££££££££££££££££££££//MP
  if (TPhase==FLOW_Multi){
	Cs0=sqrt(double(Gamma)*double(CteB)/double(RhopZero));
	float cs0max=0.f;
	for(unsigned ipres=0; ipres<MkListSize; ipres++) cs0max=((PhaseData[ipres].PhaseCs0>cs0max)? PhaseData[ipres].PhaseCs0 : cs0max);
	if(!DtIni)DtIni=h/cs0max;
	if(!DtMin)DtMin=(h/cs0max)*CoefDtMin;
  }
  else{
	Cs0=sqrt(double(Gamma)*double(CteB)/double(RhopZero));
	if(!DtIni)DtIni=h/Cs0;
	if(!DtMin)DtMin=(h/Cs0)*CoefDtMin;
  }
//£££££££££££££££££££££££££££££££££££££££££££££££££££££££££££££££££££££££//MP
  Dosh=float(h*2); 
  H2=float(h*h);
  Fourh2=float(h*h*4); 
  Eta2=float((h*0.1)*(h*0.1));
  if(simulate2d){
    if(TKernel==KERNEL_Wendland){
      Awen=float(0.557/(h*h));
      Bwen=float(-2.7852/(h*h*h));
    }
    else if(TKernel==KERNEL_Cubic){
      const double a1=10./(PI*7.);
      const double a2=a1/(h*h);
      const double aa=a1/(h*h*h);
      const double deltap=1./1.5;
      const double wdeltap=a2*(1.-1.5*deltap*deltap+0.75*deltap*deltap*deltap);
      CubicCte.od_wdeltap=float(1./wdeltap);
      CubicCte.a1=float(a1);
      CubicCte.a2=float(a2);
      CubicCte.aa=float(aa);
      CubicCte.a24=float(0.25*a2);
      CubicCte.c1=float(-3.*aa);
      CubicCte.d1=float(9.*aa/4.);
      CubicCte.c2=float(-3.*aa/4.);
    }
  }
  else{
    if(TKernel==KERNEL_Wendland){
      Awen=float(0.41778/(h*h*h));
      Bwen=float(-2.08891/(h*h*h*h));
    }
    else if(TKernel==KERNEL_Cubic){
      const double a1=1./PI;
      const double a2=a1/(h*h*h);
      const double aa=a1/(h*h*h*h);
      const double deltap=1./1.5;
      const double wdeltap=a2*(1.-1.5*deltap*deltap+0.75*deltap*deltap*deltap);
      CubicCte.od_wdeltap=float(1./wdeltap);
      CubicCte.a1=float(a1);
      CubicCte.a2=float(a2);
      CubicCte.aa=float(aa);
      CubicCte.a24=float(0.25*a2);
      CubicCte.c1=float(-3.*aa);
      CubicCte.d1=float(9.*aa/4.);
      CubicCte.c2=float(-3.*aa/4.);
    }
  }
  //-Constants for Laminar viscosity + SPS turbulence model.
  if(TVisco==VISCO_LaminarSPS){  
    double dp_sps=(Simulate2D? sqrt(Dp*Dp*2.)/2.: sqrt(Dp*Dp*3.)/3.);  
    SpsSmag=float(pow((0.12*dp_sps),2));
    SpsBlin=float((2./3.)*0.0066*dp_sps*dp_sps); 
  }
//£££££££££££££££££££££££££££££££££££££££££££££££££££££££££££££££££££££££//MP
  //-Update particle mass for multiphase flows
  if(TPhase==FLOW_Multi)UpdateMassMP(MkListSize,simulate2d);
//£££££££££££££££££££££££££££££££££££££££££££££££££££££££££££££££££££££££//MP
  VisuConfig();
}

//==============================================================================
/// Prints out configuration of the case.
//==============================================================================
void JSph::VisuConfig()const{
  const char* met="VisuConfig";
  Log->Print(Simulate2D? "**2D-Simulation parameters:": "**3D-Simulation parameters:");
  Log->Print(fun::VarStr("CaseName",CaseName));
  Log->Print(fun::VarStr("RunName",RunName));
  if(Simulate2D)Log->Print(fun::VarStr("Simulate2DPosY",Simulate2DPosY));
  Log->Print(fun::VarStr("PosDouble",GetPosDoubleName(Psimple,SvDouble)));
//£££££££££££££££££££££££££££££££££££££££££££££££££££££££££££££££££££££££//MP
  Log->Print(fun::VarStr("FlowType",GetFlowType(TPhase)));
//£££££££££££££££££££££££££££££££££££££££££££££££££££££££££££££££££££££££//MP
  Log->Print(fun::VarStr("SvTimers",SvTimers));
  Log->Print(fun::VarStr("StepAlgorithm",GetStepName(TStep)));
  if(TStep==STEP_None)RunException(met,"StepAlgorithm value is invalid.");
  if(TStep==STEP_Verlet)Log->Print(fun::VarStr("VerletSteps",VerletSteps));
  Log->Print(fun::VarStr("Kernel",GetKernelName(TKernel)));
  Log->Print(fun::VarStr("Viscosity",GetViscoName(TVisco)));
  Log->Print(fun::VarStr("Visco",Visco));
  Log->Print(fun::VarStr("ViscoBoundFactor",ViscoBoundFactor));
  if(ViscoTime)Log->Print(fun::VarStr("ViscoTime",ViscoTime->GetFile()));
  Log->Print(fun::VarStr("DeltaSph",GetDeltaSphName(TDeltaSph)));
  if(TDeltaSph!=DELTA_None)Log->Print(fun::VarStr("DeltaSphValue",DeltaSph));
  Log->Print(fun::VarStr("Shifting",GetShiftingName(TShifting)));
  if(TShifting!=SHIFT_None){
    Log->Print(fun::VarStr("ShiftCoef",ShiftCoef));
    if(ShiftTFS)Log->Print(fun::VarStr("ShiftTFS",ShiftTFS));
//£££££££££££££££££££££££££££££££££££££££££££££££££££££££££££££££££££££££//MP
    if(TPhase==FLOW_Multi)Log->Print(fun::VarStr("ShiftNormalGrad",ShiftGrad));
//£££££££££££££££££££££££££££££££££££££££££££££££££££££££££££££££££££££££//MP
  }
  Log->Print(fun::VarStr("RigidAlgorithm",(!FtCount? "None": (UseDEM? "SPH+DEM": "SPH"))));
  Log->Print(fun::VarStr("FloatingCount",FtCount));
//£££££££££££££££££££££££££££££££££££££££££££££££££££££££££££££££££££££££//MP
  if(TPhase==FLOW_Multi)Log->Print(fun::VarStr("MPCoefficient",MPCoef));
  Log->Print(fun::VarStr("Use Surface Tension",(Surft? "Yes": "No")));
//£££££££££££££££££££££££££££££££££££££££££££££££££££££££££££££££££££££££//MP
  if(FtCount)Log->Print(fun::VarStr("FtPause",FtPause));
  Log->Print(fun::VarStr("LatticeBound",LatticeBound));
  Log->Print(fun::VarStr("LatticeFluid",LatticeFluid));
  Log->Print(fun::VarStr("CaseNp",CaseNp));
  Log->Print(fun::VarStr("CaseNbound",CaseNbound));
  Log->Print(fun::VarStr("CaseNfixed",CaseNfixed));
  Log->Print(fun::VarStr("CaseNmoving",CaseNmoving));
  Log->Print(fun::VarStr("CaseNfloat",CaseNfloat));
  Log->Print(fun::VarStr("CaseNfluid",CaseNfluid));
  Log->Print(fun::VarStr("PeriodicActive",PeriActive));
  if(PeriXY)Log->Print(fun::VarStr("PeriodicXY",PeriXY));
  if(PeriXZ)Log->Print(fun::VarStr("PeriodicXZ",PeriXZ));
  if(PeriYZ)Log->Print(fun::VarStr("PeriodicYZ",PeriYZ));
  if(PeriX)Log->Print(fun::VarStr("PeriodicXinc",PeriXinc));
  if(PeriY)Log->Print(fun::VarStr("PeriodicYinc",PeriYinc));
  if(PeriZ)Log->Print(fun::VarStr("PeriodicZinc",PeriZinc));
  Log->Print(fun::VarStr("Dx",Dp));
  Log->Print(fun::VarStr("H",H));
  Log->Print(fun::VarStr("CoefficientH",H/(Dp*sqrt(Simulate2D? 2.f: 3.f))));
  Log->Print(fun::VarStr("CteB",CteB));
  Log->Print(fun::VarStr("Gamma",Gamma));
  Log->Print(fun::VarStr("RhopZero",RhopZero));
  Log->Print(fun::VarStr("Cs0",Cs0));
  Log->Print(fun::VarStr("CFLnumber",CFLnumber));
  Log->Print(fun::VarStr("DtIni",DtIni));
  Log->Print(fun::VarStr("DtMin",DtMin));
  Log->Print(fun::VarStr("DtAllParticles",DtAllParticles));
  if(DtFixed)Log->Print(fun::VarStr("DtFixed",DtFixed->GetFile()));
  Log->Print(fun::VarStr("MassFluid",MassFluid));
  Log->Print(fun::VarStr("MassBound",MassBound));
  if(TKernel==KERNEL_Wendland){
    Log->Print(fun::VarStr("Bwen (wendland)",Bwen));
  }
  else if(TKernel==KERNEL_Cubic){
    Log->Print(fun::VarStr("CubicCte.a1",CubicCte.a1));
    Log->Print(fun::VarStr("CubicCte.aa",CubicCte.aa));
    Log->Print(fun::VarStr("CubicCte.a24",CubicCte.a24));
    Log->Print(fun::VarStr("CubicCte.c1",CubicCte.c1));
    Log->Print(fun::VarStr("CubicCte.c2",CubicCte.c2));
    Log->Print(fun::VarStr("CubicCte.d1",CubicCte.d1));
    Log->Print(fun::VarStr("CubicCte.od_wdeltap",CubicCte.od_wdeltap));
  }
  if(TVisco==VISCO_LaminarSPS){     
    Log->Print(fun::VarStr("SpsSmag",SpsSmag));
    Log->Print(fun::VarStr("SpsBlin",SpsBlin));
  }
  if(UseDEM)VisuDemCoefficients();
//£££££££££££££££££££££££££££££££££££££££££££££££££££££££££££££££££££££££//MP
  if(TPhase==FLOW_Multi)VisuMP();
//£££££££££££££££££££££££££££££££££££££££££££££££££££££££££££££££££££££££//MP
  if(CaseNfloat)Log->Print(fun::VarStr("FtPause",FtPause));
  Log->Print(fun::VarStr("TimeMax",TimeMax));
  Log->Print(fun::VarStr("TimePart",TimePart));
  Log->Print(fun::VarStr("Gravity",Gravity));
  Log->Print(fun::VarStr("NpMinimum",NpMinimum));
  Log->Print(fun::VarStr("RhopOut",RhopOut));
  if(RhopOut){
    Log->Print(fun::VarStr("RhopOutMin",RhopOutMin));
    Log->Print(fun::VarStr("RhopOutMax",RhopOutMax));
  }
//£££££££££££££££££££££££££££££££££££££££££££££££££££££££££££££££££££££££//MP
  if(CteB==0 && TPhase==FLOW_Single)RunException(met,"Constant \'b\' cannot be zero.\n\'b\' is zero when fluid height is zero (or fluid particles were not created)");
//£££££££££££££££££££££££££££££££££££££££££££££££££££££££££££££££££££££££//MP
}

//==============================================================================
/// Calcula celda de las particulas y comprueba que no existan mas particulas
/// excluidas de las previstas.
///
/// Computes cell particles and checks if there are more particles
/// excluded than expected.
//==============================================================================
void JSph::LoadDcellParticles(unsigned n,const word *code,const tdouble3 *pos,unsigned *dcell)const{
  const char met[]="LoadDcellParticles";
  for(unsigned p=0;p<n;p++){
    word codeout=CODE_GetSpecialValue(code[p]);
    if(codeout<CODE_OUTIGNORE){
      const tdouble3 ps=pos[p];
      if(ps>=DomRealPosMin && ps<DomRealPosMax){
        const double dx=ps.x-DomPosMin.x;
        const double dy=ps.y-DomPosMin.y;
        const double dz=ps.z-DomPosMin.z;
        unsigned cx=unsigned(dx/Scell),cy=unsigned(dy/Scell),cz=unsigned(dz/Scell);
        dcell[p]=PC__Cell(DomCellCode,cx,cy,cz);
      }
      else{ //-Particle out
        RunException(met,"Found new particles out."); //-No puede haber nuevas particulas excluidas. //-There can not be new particles excluded.
        dcell[p]=PC__CodeOut;
      }
    }
    else dcell[p]=PC__CodeOut;
  }
}

//==============================================================================
/// Configura CellOrder y ajusta orden de componentes en datos.
/// Configures CellOrder and adjusts order of components in data.
//==============================================================================
void JSph::ConfigCellOrder(TpCellOrder order,unsigned np,tdouble3* pos,tfloat4* velrhop){
  //-Stores initial periodic configuration in PeriodicConfig.  
  PeriodicConfig=StrPeriodic(PeriActive,PeriXinc,PeriYinc,PeriZinc);
  //-Applies CellOrder.  
  CellOrder=order;
  if(CellOrder==ORDER_None)CellOrder=ORDER_XYZ;
  if(Simulate2D&&CellOrder!=ORDER_XYZ&&CellOrder!=ORDER_ZYX)RunException("ConfigCellOrder","In 2D simulations the value of CellOrder must be XYZ or ZYX.");
  Log->Print(fun::VarStr("CellOrder",string(GetNameCellOrder(CellOrder))));
  if(CellOrder!=ORDER_XYZ){
    //-Modifica datos iniciales de particulas.
    //-Modifies initial particle data.
    OrderCodeData(CellOrder,np,pos);
    OrderCodeData(CellOrder,np,velrhop);
    //-Modifica otras constantes.
    //-Modifies other constants.
    Gravity=OrderCodeValue(CellOrder,Gravity);
    MapRealPosMin=OrderCodeValue(CellOrder,MapRealPosMin);
    MapRealPosMax=OrderCodeValue(CellOrder,MapRealPosMax);
    MapRealSize=OrderCodeValue(CellOrder,MapRealSize);
    Map_PosMin=OrderCodeValue(CellOrder,Map_PosMin);
    Map_PosMax=OrderCodeValue(CellOrder,Map_PosMax);
    Map_Size=OrderCodeValue(CellOrder,Map_Size);
    //-Modifica config periodica.
    //-Modifies periodic configuration. 
    bool perix=PeriX,periy=PeriY,periz=PeriZ;
    bool perixy=PeriXY,perixz=PeriXZ,periyz=PeriYZ;
    tdouble3 perixinc=PeriXinc,periyinc=PeriYinc,perizinc=PeriZinc;
    tuint3 v={1,2,3};
    v=OrderCode(v);
    if(v.x==2){ PeriX=periy; PeriXinc=OrderCode(periyinc); }
    if(v.x==3){ PeriX=periz; PeriXinc=OrderCode(perizinc); }
    if(v.y==1){ PeriY=perix; PeriYinc=OrderCode(perixinc); }
    if(v.y==3){ PeriY=periz; PeriYinc=OrderCode(perizinc); }
    if(v.z==1){ PeriZ=perix; PeriZinc=OrderCode(perixinc); }
    if(v.z==2){ PeriZ=periy; PeriZinc=OrderCode(periyinc); }
    if(perixy){
      PeriXY=(CellOrder==ORDER_XYZ||CellOrder==ORDER_YXZ);
      PeriXZ=(CellOrder==ORDER_XZY||CellOrder==ORDER_YZX);
      PeriYZ=(CellOrder==ORDER_ZXY||CellOrder==ORDER_ZYX);
    }
    if(perixz){
      PeriXY=(CellOrder==ORDER_XZY||CellOrder==ORDER_ZXY);
      PeriXZ=(CellOrder==ORDER_XYZ||CellOrder==ORDER_ZYX);
      PeriYZ=(CellOrder==ORDER_YXZ||CellOrder==ORDER_YZX);
    }
    if(periyz){
      PeriXY=(CellOrder==ORDER_YZX||CellOrder==ORDER_ZYX);
      PeriXZ=(CellOrder==ORDER_YXZ||CellOrder==ORDER_ZXY);
      PeriYZ=(CellOrder==ORDER_XYZ||CellOrder==ORDER_XZY);
    }
  }
  PeriActive=(PeriX? 1: 0)+(PeriY? 2: 0)+(PeriZ? 4: 0);
}

//==============================================================================
/// Convierte pos[] y vel[] al orden dimensional original.
/// Converts pos[] and vel[] to the original dimension order.
//==============================================================================
void JSph::DecodeCellOrder(unsigned np,tdouble3 *pos,tfloat3 *vel)const{
  if(CellOrder!=ORDER_XYZ){
    OrderDecodeData(CellOrder,np,pos);
    OrderDecodeData(CellOrder,np,vel);
  }
}

//==============================================================================
/// Modifica orden de componentes de un array de tipo tfloat3.
/// Modifies order of components of an array of type tfloat3.
//==============================================================================
void JSph::OrderCodeData(TpCellOrder order,unsigned n,tfloat3 *v){
  if(order==ORDER_XZY)for(unsigned c=0;c<n;c++)v[c]=ReOrderXZY(v[c]);
  if(order==ORDER_YXZ)for(unsigned c=0;c<n;c++)v[c]=ReOrderYXZ(v[c]);
  if(order==ORDER_YZX)for(unsigned c=0;c<n;c++)v[c]=ReOrderYZX(v[c]);
  if(order==ORDER_ZXY)for(unsigned c=0;c<n;c++)v[c]=ReOrderZXY(v[c]);
  if(order==ORDER_ZYX)for(unsigned c=0;c<n;c++)v[c]=ReOrderZYX(v[c]);
}

//==============================================================================
/// Modifica orden de componentes de un array de tipo tdouble3.
/// Modifies order of components of an array of type tdouble3.
//==============================================================================
void JSph::OrderCodeData(TpCellOrder order,unsigned n,tdouble3 *v){
  if(order==ORDER_XZY)for(unsigned c=0;c<n;c++)v[c]=ReOrderXZY(v[c]);
  if(order==ORDER_YXZ)for(unsigned c=0;c<n;c++)v[c]=ReOrderYXZ(v[c]);
  if(order==ORDER_YZX)for(unsigned c=0;c<n;c++)v[c]=ReOrderYZX(v[c]);
  if(order==ORDER_ZXY)for(unsigned c=0;c<n;c++)v[c]=ReOrderZXY(v[c]);
  if(order==ORDER_ZYX)for(unsigned c=0;c<n;c++)v[c]=ReOrderZYX(v[c]);
}

//==============================================================================
/// Modifica orden de componentes de un array de tipo tfloat4.
/// Modifies order of components of an array of type tfloat4.
//==============================================================================
void JSph::OrderCodeData(TpCellOrder order,unsigned n,tfloat4 *v){
  if(order==ORDER_XZY)for(unsigned c=0;c<n;c++)v[c]=ReOrderXZY(v[c]);
  if(order==ORDER_YXZ)for(unsigned c=0;c<n;c++)v[c]=ReOrderYXZ(v[c]);
  if(order==ORDER_YZX)for(unsigned c=0;c<n;c++)v[c]=ReOrderYZX(v[c]);
  if(order==ORDER_ZXY)for(unsigned c=0;c<n;c++)v[c]=ReOrderZXY(v[c]);
  if(order==ORDER_ZYX)for(unsigned c=0;c<n;c++)v[c]=ReOrderZYX(v[c]);
}

//==============================================================================
/// Configura division en celdas.
/// Configures cell division.
//==============================================================================
void JSph::ConfigCellDivision(){
  if(CellMode!=CELLMODE_2H && CellMode!=CELLMODE_H)RunException("ConfigCellDivision","The CellMode is invalid.");
  Hdiv=(CellMode==CELLMODE_2H? 1: 2);
  Scell=Dosh/Hdiv;
  MovLimit=Scell*0.9f;
  Map_Cells=TUint3(unsigned(ceil(Map_Size.x/Scell)),unsigned(ceil(Map_Size.y/Scell)),unsigned(ceil(Map_Size.z/Scell)));
  //-Prints configuration.
  Log->Print(fun::VarStr("CellMode",string(GetNameCellMode(CellMode))));
  Log->Print(fun::VarStr("Hdiv",Hdiv));
  Log->Print(string("MapCells=(")+fun::Uint3Str(OrderDecode(Map_Cells))+")");
  //-Creates VTK file with map cells.
  if(SvDomainVtk){
    const llong n=llong(Map_Cells.x)*llong(Map_Cells.y)*llong(Map_Cells.z);
    if(n<1000000)SaveMapCellsVtk(Scell);
    else Log->Print("\n*** Attention: File MapCells.vtk was not created because number of cells is too high.\n");
  }
}

//==============================================================================
/// Establece dominio local de simulacion dentro de Map_Cells y calcula DomCellCode.
/// Sets local domain of simulation within Map_Cells and computes DomCellCode.
//==============================================================================
void JSph::SelecDomain(tuint3 celini,tuint3 celfin){
  const char met[]="SelecDomain";
  DomCelIni=celini;
  DomCelFin=celfin;
  DomCells=DomCelFin-DomCelIni;
  if(DomCelIni.x>=Map_Cells.x || DomCelIni.y>=Map_Cells.y || DomCelIni.z>=Map_Cells.z )RunException(met,"DomCelIni is invalid.");
  if(DomCelFin.x>Map_Cells.x || DomCelFin.y>Map_Cells.y || DomCelFin.z>Map_Cells.z )RunException(met,"DomCelFin is invalid.");
  if(DomCells.x<1 || DomCells.y<1 || DomCells.z<1 )RunException(met,"The domain of cells is invalid.");
  //-Calcula limites del dominio local.
  //-Computes local domain limits.
  DomPosMin.x=Map_PosMin.x+(DomCelIni.x*Scell);
  DomPosMin.y=Map_PosMin.y+(DomCelIni.y*Scell);
  DomPosMin.z=Map_PosMin.z+(DomCelIni.z*Scell);
  DomPosMax.x=Map_PosMin.x+(DomCelFin.x*Scell);
  DomPosMax.y=Map_PosMin.y+(DomCelFin.y*Scell);
  DomPosMax.z=Map_PosMin.z+(DomCelFin.z*Scell);
  //-Ajusta limites finales.
  //-Adjusts final limits.
  if(DomPosMax.x>Map_PosMax.x)DomPosMax.x=Map_PosMax.x;
  if(DomPosMax.y>Map_PosMax.y)DomPosMax.y=Map_PosMax.y;
  if(DomPosMax.z>Map_PosMax.z)DomPosMax.z=Map_PosMax.z;
  //-Calcula limites reales del dominio local.
  //-Computes actual limits of local domain.
  DomRealPosMin=DomPosMin;
  DomRealPosMax=DomPosMax;
  if(DomRealPosMax.x>MapRealPosMax.x)DomRealPosMax.x=MapRealPosMax.x;
  if(DomRealPosMax.y>MapRealPosMax.y)DomRealPosMax.y=MapRealPosMax.y;
  if(DomRealPosMax.z>MapRealPosMax.z)DomRealPosMax.z=MapRealPosMax.z;
  if(DomRealPosMin.x<MapRealPosMin.x)DomRealPosMin.x=MapRealPosMin.x;
  if(DomRealPosMin.y<MapRealPosMin.y)DomRealPosMin.y=MapRealPosMin.y;
  if(DomRealPosMin.z<MapRealPosMin.z)DomRealPosMin.z=MapRealPosMin.z;
  //-Calcula codificacion de celdas para el dominio seleccionado.
  //-Computes cofification of cells for the selected domain.
  DomCellCode=CalcCellCode(DomCells+TUint3(1));
  if(!DomCellCode)RunException(met,string("Failed to select a valid CellCode for ")+fun::UintStr(DomCells.x)+"x"+fun::UintStr(DomCells.y)+"x"+fun::UintStr(DomCells.z)+" cells (CellMode="+GetNameCellMode(CellMode)+").");
  //-Prints configurantion.
  Log->Print(string("DomCells=(")+fun::Uint3Str(OrderDecode(DomCells))+")");
  Log->Print(fun::VarStr("DomCellCode",fun::UintStr(PC__GetSx(DomCellCode))+"_"+fun::UintStr(PC__GetSy(DomCellCode))+"_"+fun::UintStr(PC__GetSz(DomCellCode))));
}

//==============================================================================
/// Selecciona un codigo adecuado para la codificion de celda.
/// Selects an adequate code for cell configuration.
//==============================================================================
unsigned JSph::CalcCellCode(tuint3 ncells){
  unsigned sxmin=2; for(;ncells.x>>sxmin;sxmin++);
  unsigned symin=2; for(;ncells.y>>symin;symin++);
  unsigned szmin=2; for(;ncells.z>>szmin;szmin++);
  unsigned smin=sxmin+symin+szmin;
  unsigned ccode=0;
  if(smin<=32){
    unsigned sx=sxmin,sy=symin,sz=szmin;
    unsigned rest=32-smin;
    while(rest){
      if(rest){ sx++; rest--; }
      if(rest){ sy++; rest--; }
      if(rest){ sz++; rest--; }
    }
    ccode=PC__GetCode(sx,sy,sz);
  }
  return(ccode);
}

//==============================================================================
/// Calcula distancia maxima entre particulas y centro de cada floating.
/// Computes maximum distance between particles and center of floating.
//==============================================================================
void JSph::CalcFloatingRadius(unsigned np,const tdouble3 *pos,const unsigned *idp){
  const char met[]="CalcFloatingsRadius";
  const float overradius=1.2f; //-Porcentaje de incremento de radio. Percentage of ration increase.
  unsigned *ridp=new unsigned[CaseNfloat];
  //-Asigna valores UINT_MAX.
  //-Assigns values UINT_MAX. 
  memset(ridp,255,sizeof(unsigned)*CaseNfloat); 
  //-Calcula posicion segun id suponiendo que todas las particulas son normales (no periodicas).
  //-Computes position according to id assuming that all particles are not periodic.
  const unsigned idini=CaseNpb,idfin=CaseNpb+CaseNfloat;
  for(unsigned p=0;p<np;p++){
    const unsigned id=idp[p];
    if(idini<=id && id<idfin)ridp[id-idini]=p;
  }
  //-Comprueba que todas las particulas floating estan localizadas.
  //-Checks that all floating particles are located.  
  for(unsigned fp=0;fp<CaseNfloat;fp++){
    if(ridp[fp]==UINT_MAX)RunException(met,"There are floating particles not found.");
  }
  //-Calcula distancia maxima entre particulas y centro de floating (todas son validas).
  //-Calculates maximum distance between particles and center of the floating (all are valid).  
  float radiusmax=0;
  for(unsigned cf=0;cf<FtCount;cf++){
    StFloatingData *fobj=FtObjs+cf;
    const unsigned fpini=fobj->begin-CaseNpb;
    const unsigned fpfin=fpini+fobj->count;
    const tdouble3 fcen=fobj->center;
    double r2max=0;
    for(unsigned fp=fpini;fp<fpfin;fp++){
      const int p=ridp[fp];
      const double dx=fcen.x-pos[p].x,dy=fcen.y-pos[p].y,dz=fcen.z-pos[p].z;
      double r2=dx*dx+dy*dy+dz*dz;
      if(r2max<r2)r2max=r2;
    }
    fobj->radius=float(sqrt(r2max)*overradius);
    if(radiusmax<fobj->radius)radiusmax=fobj->radius;
  }
  //-Libera memoria.
  //-Release of memory.  
  delete[] ridp; ridp=NULL;
  //-Comprueba que el radio maximo sea menor que las dimensiones del dominio periodico.
  //-Checks maximum radius < dimensions of the periodic domain.
  if(PeriX && fabs(PeriXinc.x)<=radiusmax)RunException(met,fun::PrintStr("The floating radius (%g) is too large for periodic distance in X (%g).",radiusmax,abs(PeriXinc.x)));
  if(PeriY && fabs(PeriYinc.y)<=radiusmax)RunException(met,fun::PrintStr("The floating radius (%g) is too large for periodic distance in Y (%g).",radiusmax,abs(PeriYinc.y)));
  if(PeriZ && fabs(PeriZinc.z)<=radiusmax)RunException(met,fun::PrintStr("The floating radius (%g) is too large for periodic distance in Z (%g).",radiusmax,abs(PeriZinc.z)));
}

//==============================================================================
/// Devuelve la posicion corregida tras aplicar condiciones periodicas.
/// Returns the corrected position after applying periodic conditions.
//==============================================================================
tdouble3 JSph::UpdatePeriodicPos(tdouble3 ps)const{
  double dx=ps.x-MapRealPosMin.x;
  double dy=ps.y-MapRealPosMin.y;
  double dz=ps.z-MapRealPosMin.z;
  const bool out=(dx!=dx || dy!=dy || dz!=dz || dx<0 || dy<0 || dz<0 || dx>=MapRealSize.x || dy>=MapRealSize.y || dz>=MapRealSize.z);
  //-Ajusta posicion segun condiciones periodicas y vuelve a comprobar los limites del dominio.
  //-Adjusts position according to periodic conditions and checks again domain limtis.
  if(PeriActive && out){
    bool xperi=((PeriActive&1)!=0),yperi=((PeriActive&2)!=0),zperi=((PeriActive&4)!=0);
    if(xperi){
      if(dx<0)             { dx-=PeriXinc.x; dy-=PeriXinc.y; dz-=PeriXinc.z; }
      if(dx>=MapRealSize.x){ dx+=PeriXinc.x; dy+=PeriXinc.y; dz+=PeriXinc.z; }
    }
    if(yperi){
      if(dy<0)             { dx-=PeriYinc.x; dy-=PeriYinc.y; dz-=PeriYinc.z; }
      if(dy>=MapRealSize.y){ dx+=PeriYinc.x; dy+=PeriYinc.y; dz+=PeriYinc.z; }
    }
    if(zperi){
      if(dz<0)             { dx-=PeriZinc.x; dy-=PeriZinc.y; dz-=PeriZinc.z; }
      if(dz>=MapRealSize.z){ dx+=PeriZinc.x; dy+=PeriZinc.y; dz+=PeriZinc.z; }
    }
    ps=TDouble3(dx,dy,dz)+MapRealPosMin;
  }
  return(ps);
}

//==============================================================================
/// Muestra un mensaje con la memoria reservada para los datos basicos de las particulas.
/// Display a message with reserved memory for the basic data of particles.
//==============================================================================
void JSph::PrintSizeNp(unsigned np,llong size)const{
  Log->Printf("**Requested %s memory for %u particles: %.1f MB.",(Cpu? "cpu": "gpu"),np,double(size)/(1024*1024));
}

//==============================================================================
/// Visualiza cabeceras de PARTs.
/// Display headers of PARTs
//==============================================================================
void JSph::PrintHeadPart(){
  Log->Print("PART       PartTime      TotalSteps    Steps    Time/Sec   Finish time        ");
  Log->Print("=========  ============  ============  =======  =========  ===================");
  fflush(stdout);
}

//==============================================================================
/// Establece configuracion para grabacion de particulas.
/// Sets configuration for recordering of particles.
//==============================================================================
void JSph::ConfigSaveData(unsigned piece,unsigned pieces,std::string div){
  const char met[]="ConfigSaveData";
  word mkboundfirst=0;
  //-Stores basic information of simulation data.
  JPartDataHead parthead;
  parthead.ConfigBasic(RunCode,AppName,CaseName,CasePosMin,CasePosMax
    ,Simulate2D,Simulate2DPosY,pieces,PartBeginFirst);
  {//MkInfo->ConfigPartDataHead(&parthead);
    JSpaceParts parts;  parts.LoadFileXml(FileXml,"case.execution.particles");
    mkboundfirst=parts.GetMkBoundFirst();
    for(unsigned c=0;c<parts.CountBlocks();c++){
      const JSpacePartBlock &block=parts.GetBlock(c);
      TpParticlesNew type;
      switch(block.Type){
        case TpPartFixed:     type=TpPartFixed;     break;
        case TpPartMoving:    type=TpPartMoving;    break;
        case TpPartFloating:  type=TpPartFloating;  break;
        case TpPartFluid:     type=TpPartFluid;     break;
      }
      const unsigned mk=block.GetMk();
      const unsigned mktype=block.GetMkType();
      const unsigned begin=block.GetBegin();
      const unsigned count=block.GetCount();
      parthead.ConfigParticles(type,mk,mktype,begin,count);
    }
  }
  parthead.ConfigCtes(Dp,H,CteB,RhopZero,Gamma,MassBound,MassFluid,Gravity);
  parthead.ConfigSimNp(NpDynamic,ReuseIds);
  parthead.ConfigSimMap(OrderDecode(MapRealPosMin),OrderDecode(MapRealPosMax));
  parthead.ConfigSimPeri(PeriodicConfig.PeriMode,PeriodicConfig.PeriXinc,PeriodicConfig.PeriYinc,PeriodicConfig.PeriZinc);
  switch(TVisco){
    case VISCO_None:        parthead.ConfigVisco(JPartDataHead::VISCO_None      ,Visco,ViscoBoundFactor);  break;
    case VISCO_Artificial:  parthead.ConfigVisco(JPartDataHead::VISCO_Artificial,Visco,ViscoBoundFactor);  break;
    case VISCO_LaminarSPS:  parthead.ConfigVisco(JPartDataHead::VISCO_LaminarSPS,Visco,ViscoBoundFactor);  break;
    default: RunException(met,"Viscosity type is unknown.");
  }
  if(SvData&SDAT_Binx){
    parthead.SaveFile(DirOut);
  }
  //-Configura objeto para grabacion de particulas e informacion.
  //-Configures object to store particles and information.  
  if(SvData&SDAT_Info || SvData&SDAT_Binx){
    DataBi4=new JPartDataBi4();
    DataBi4->Config(piece,pieces,DirOut,&parthead);
    if(div.empty())DataBi4->ConfigSimDiv(JPartDataBi4::DIV_None);
    else if(div=="X")DataBi4->ConfigSimDiv(JPartDataBi4::DIV_X);
    else if(div=="Y")DataBi4->ConfigSimDiv(JPartDataBi4::DIV_Y);
    else if(div=="Z")DataBi4->ConfigSimDiv(JPartDataBi4::DIV_Z);
    else RunException(met,"The division configuration is invalid.");
  }
  //-Configura objeto para grabacion de particulas excluidas.
  //-Configures object to store excluded particles.  
  if(SvData&SDAT_Binx){
    DataOutBi4=new JPartOutBi4Save();
    DataOutBi4->ConfigBasic(piece,pieces,RunCode,AppName,Simulate2D,DirOut);
    DataOutBi4->ConfigParticles(CaseNp,CaseNfixed,CaseNmoving,CaseNfloat,CaseNfluid);
    DataOutBi4->ConfigLimits(OrderDecode(MapRealPosMin),OrderDecode(MapRealPosMax),(RhopOut? RhopOutMin: 0),(RhopOut? RhopOutMax: 0));
    DataOutBi4->SaveInitial();
  }
  //-Configura objeto para grabacion de datos de floatings.
  //-Configures object to store data of floatings.
  if(SvData&SDAT_Binx && FtCount){
    DataFloatBi4=new JPartFloatBi4Save();
    DataFloatBi4->Config(AppName,DirOut,mkboundfirst,FtCount);
    for(unsigned cf=0;cf<FtCount;cf++)DataFloatBi4->AddHeadData(cf,FtObjs[cf].mkbound,FtObjs[cf].begin,FtObjs[cf].count,FtObjs[cf].mass,FtObjs[cf].radius);
    DataFloatBi4->SaveInitial();
  }
  //-Crea objeto para almacenar las particulas excluidas hasta su grabacion.
  //-Creates object to store excluded particles until recordering. 
  PartsOut=new JPartsOut();
}

//==============================================================================
/// Almacena nuevas particulas excluidas hasta la grabacion del proximo PART.
/// Stores new excluded particles until recordering next PART.
//==============================================================================
void JSph::AddParticlesOut(unsigned nout,const unsigned *idp,const tdouble3* pos,const tfloat3 *vel,const float *rhop,unsigned noutrhop,unsigned noutmove){
  PartsOut->AddParticles(nout,idp,pos,vel,rhop,noutrhop,noutmove);
}

//==============================================================================
/// Devuelve puntero de memoria dinamica con los datos transformados en tfloat3.
/// Returns dynamic memory pointer with data transformed in tfloat3.
//==============================================================================
tfloat3* JSph::GetPointerDataFloat3(unsigned n,const tdouble3* v)const{
  tfloat3* v2=new tfloat3[n];
  for(unsigned c=0;c<n;c++)v2[c]=ToTFloat3(v[c]);
  return(v2);
}

//==============================================================================
/// Graba los ficheros de datos de particulas.
/// Stores files of particle data.
//==============================================================================
void JSph::SavePartData(unsigned npok,unsigned nout,const unsigned *idp,const tdouble3 *pos,const tfloat3 *vel,const float *rhop,unsigned ndom,const tdouble3 *vdom,const StInfoPartPlus *infoplus){
  //-Graba datos de particulas y/o informacion en formato bi4.
  //-Stores particle data and/or information in bi4 format.
  if(DataBi4){
    tfloat3* posf3=NULL;
    TimerPart.Stop();
    JBinaryData* bdpart=DataBi4->AddPartInfo(Part,TimeStep,npok,nout,Nstep,TimerPart.GetElapsedTimeD()/1000.,vdom[0],vdom[1],TotalNp);
    if(infoplus && SvData&SDAT_Info){
      bdpart->SetvDouble("dtmean",(!Nstep? 0: (TimeStep-TimeStepM1)/(Nstep-PartNstep)));
      bdpart->SetvDouble("dtmin",(!Nstep? 0: PartDtMin));
      bdpart->SetvDouble("dtmax",(!Nstep? 0: PartDtMax));
      if(DtFixed)bdpart->SetvDouble("dterror",DtFixed->GetDtError(true));
      bdpart->SetvDouble("timesim",infoplus->timesim);
      bdpart->SetvUint("nct",infoplus->nct);
      bdpart->SetvUint("npbin",infoplus->npbin);
      bdpart->SetvUint("npbout",infoplus->npbout);
      bdpart->SetvUint("npf",infoplus->npf);
      bdpart->SetvUint("npbper",infoplus->npbper);
      bdpart->SetvUint("npfper",infoplus->npfper);
      bdpart->SetvLlong("cpualloc",infoplus->memorycpualloc);
      if(infoplus->gpudata){
        bdpart->SetvLlong("nctalloc",infoplus->memorynctalloc);
        bdpart->SetvLlong("nctused",infoplus->memorynctused);
        bdpart->SetvLlong("npalloc",infoplus->memorynpalloc);
        bdpart->SetvLlong("npused",infoplus->memorynpused);
      }
    }
    if(SvData&SDAT_Binx){
      if(SvDouble)DataBi4->AddPartData(npok,idp,pos,vel,rhop);
      else{
        posf3=GetPointerDataFloat3(npok,pos);
        DataBi4->AddPartData(npok,idp,posf3,vel,rhop);
      }
      float *press=NULL;
      if(0){//-Example saving a new array (Pressure) in files BI4.
        press=new float[npok];
        for(unsigned p=0;p<npok;p++)press[p]=(idp[p]>=CaseNbound? CteB*(pow(rhop[p]/RhopZero,Gamma)-1.0f): 0.f);
        DataBi4->AddPartData("Pressure",npok,press);
      }
      DataBi4->SaveFilePart();
      delete[] press; press=NULL;//-Memory must to be deallocated after saving file because DataBi4 uses this memory space.
    }
    if(SvData&SDAT_Info)DataBi4->SaveFileInfo();
    delete[] posf3;
  }

  //-Graba ficheros VKT y/o CSV.
  //-Stores VTK nd/or CSV files.
  if((SvData&SDAT_Csv)||(SvData&SDAT_Vtk)){
    //-Genera array con posf3 y tipo de particula.
    //-Generates array with posf3 and type of particle.
    tfloat3* posf3=GetPointerDataFloat3(npok,pos);
    byte *type=new byte[npok];
    for(unsigned p=0;p<npok;p++){
      const unsigned id=idp[p];
      type[p]=(id>=CaseNbound? 3: (id<CaseNfixed? 0: (id<CaseNpb? 1: 2)));
    }
    //-Define campos a grabar.
    //-Defines fields to be stored.
    JFormatFiles2::StScalarData fields[8];
    unsigned nfields=0;
    if(idp){   fields[nfields]=JFormatFiles2::DefineField("Idp" ,JFormatFiles2::UInt32 ,1,idp);   nfields++; }
    if(vel){   fields[nfields]=JFormatFiles2::DefineField("Vel" ,JFormatFiles2::Float32,3,vel);   nfields++; }
    if(rhop){  fields[nfields]=JFormatFiles2::DefineField("Rhop",JFormatFiles2::Float32,1,rhop);  nfields++; }
    if(type){  fields[nfields]=JFormatFiles2::DefineField("Type",JFormatFiles2::UChar8 ,1,type);  nfields++; }
    if(SvData&SDAT_Vtk)JFormatFiles2::SaveVtk(DirOut+fun::FileNameSec("PartVtk.vtk",Part),npok,posf3,nfields,fields);
    if(SvData&SDAT_Csv)JFormatFiles2::SaveCsv(DirOut+fun::FileNameSec("PartCsv.csv",Part),npok,posf3,nfields,fields);
    //-libera memoria.
    //-release of memory.
    delete[] posf3;
    delete[] type; 
  }

  //-Graba datos de particulas excluidas.
  //-Stores data of excluded particles.
  if(DataOutBi4 && PartsOut->GetCount()){
    if(SvDouble)DataOutBi4->SavePartOut(Part,TimeStep,PartsOut->GetCount(),PartsOut->GetIdpOut(),PartsOut->GetPosOut(),PartsOut->GetVelOut(),PartsOut->GetRhopOut());
    else{
      const tfloat3* posf3=GetPointerDataFloat3(PartsOut->GetCount(),PartsOut->GetPosOut());
      DataOutBi4->SavePartOut(Part,TimeStep,PartsOut->GetCount(),PartsOut->GetIdpOut(),posf3,PartsOut->GetVelOut(),PartsOut->GetRhopOut());
      delete[] posf3;
    }
  }

  //-Graba datos de floatings.
  //-Stores data of floatings.
  if(DataFloatBi4){
    if(CellOrder==ORDER_XYZ)for(unsigned cf=0;cf<FtCount;cf++)DataFloatBi4->AddPartData(cf,FtObjs[cf].center,FtObjs[cf].fvel,FtObjs[cf].fomega);
    else                    for(unsigned cf=0;cf<FtCount;cf++)DataFloatBi4->AddPartData(cf,OrderDecodeValue(CellOrder,FtObjs[cf].center),OrderDecodeValue(CellOrder,FtObjs[cf].fvel),OrderDecodeValue(CellOrder,FtObjs[cf].fomega));
    DataFloatBi4->SavePartFloat(Part,TimeStep,(UseDEM? DemDtForce: 0));
  }

  //-Vacia almacen de particulas excluidas.
  //-Empties stock of excluded particles.
  PartsOut->Clear();
}

//==============================================================================
/// Graba los ficheros de datos de particulas.
/// Stores files of particle data for the multi-phase flow.
//==============================================================================
void JSph::SavePartDataMP(unsigned npok,unsigned nout,const unsigned *idp,const tdouble3 *pos,const tfloat3 *vel,const float *rhop,const word *code,unsigned ndom,const tdouble3 *vdom,const StInfoPartPlus *infoplus){
  //-Graba datos de particulas y/o informacion en formato bi4.
  //-Stores particle data and/or information in bi4 format.
  if(DataBi4){
    tfloat3* posf3=NULL;
    TimerPart.Stop();
    JBinaryData* bdpart=DataBi4->AddPartInfo(Part,TimeStep,npok,nout,Nstep,TimerPart.GetElapsedTimeD()/1000.,vdom[0],vdom[1],TotalNp);
    if(infoplus && SvData&SDAT_Info){
      bdpart->SetvDouble("dtmean",(!Nstep? 0: (TimeStep-TimeStepM1)/(Nstep-PartNstep)));
      bdpart->SetvDouble("dtmin",(!Nstep? 0: PartDtMin));
      bdpart->SetvDouble("dtmax",(!Nstep? 0: PartDtMax));
      if(DtFixed)bdpart->SetvDouble("dterror",DtFixed->GetDtError(true));
      bdpart->SetvDouble("timesim",infoplus->timesim);
      bdpart->SetvUint("nct",infoplus->nct);
      bdpart->SetvUint("npbin",infoplus->npbin);
      bdpart->SetvUint("npbout",infoplus->npbout);
      bdpart->SetvUint("npf",infoplus->npf);
      bdpart->SetvUint("npbper",infoplus->npbper);
      bdpart->SetvUint("npfper",infoplus->npfper);
      bdpart->SetvLlong("cpualloc",infoplus->memorycpualloc);
      if(infoplus->gpudata){
        bdpart->SetvLlong("nctalloc",infoplus->memorynctalloc);
        bdpart->SetvLlong("nctused",infoplus->memorynctused);
        bdpart->SetvLlong("npalloc",infoplus->memorynpalloc);
        bdpart->SetvLlong("npused",infoplus->memorynpused);
      }
    }
    if(SvData&SDAT_Binx){
      if(SvDouble)DataBi4->AddPartData(npok,idp,pos,vel,rhop);
      else{
        posf3=GetPointerDataFloat3(npok,pos);
        DataBi4->AddPartData(npok,idp,posf3,vel,rhop);
      }
      float *press=NULL;
      //if(0){//-Example saving a new array (Pressure) in files BI4.
        press=new float[npok];
		float cteb=0.f;
		const int n=int(npok); 
		#ifdef _WITHOMP
			#pragma omp parallel for schedule (static) if(n>LIMIT_COMPUTELIGHT_OMP)
		#endif
        for(int p=0;p<int(npok);p++){
			for(int iphase=0; iphase<int(MkListSize); iphase++){ //added selection for material properties
				if(CODE_GetTypeAndValue(code[p])==PhaseData[iphase].MkValue){
					cteb=PhaseData[iphase].PhaseCs0*PhaseData[iphase].PhaseCs0*PhaseData[iphase].PhaseRhop0/PhaseData[iphase].PhaseGamma;
					press[p]=cteb*(pow(rhop[p]/PhaseData[iphase].PhaseRhop0,PhaseData[iphase].PhaseGamma)-1.0f);
					break;
				}
			}
		}
        DataBi4->AddPartData("Press",npok,press);
      //}
      DataBi4->SaveFilePart();
      delete[] press; press=NULL;//-Memory must be deallocated after saving file because DataBi4 uses this memory space.
    }
    if(SvData&SDAT_Info)DataBi4->SaveFileInfo();
    delete[] posf3;
  }

  //-Graba ficheros VKT y/o CSV.
  //-Stores VTK nd/or CSV files.
  if((SvData&SDAT_Csv)||(SvData&SDAT_Vtk)){
    //-Genera array con posf3 y tipo de particula.
    //-Generates array with posf3 and type of particle.
    tfloat3* posf3=GetPointerDataFloat3(npok,pos);
    byte *type=new byte[npok];
    for(unsigned p=0;p<npok;p++){
      const unsigned id=idp[p];
      type[p]=(id>=CaseNbound? 3: (id<CaseNfixed? 0: (id<CaseNpb? 1: 2)));
    }
	//Add pressure array to saved files
	float *press=NULL;
    press=new float[npok];
	float cteb=0.f;
    for(unsigned p=0;p<npok;p++){
		for(int iphase=0; iphase<int(MkListSize); iphase++){ //added selection for material properties
			if(CODE_GetTypeAndValue(code[p])==PhaseData[iphase].MkValue){
				CteB=PhaseData[iphase].PhaseCs0*PhaseData[iphase].PhaseCs0*PhaseData[iphase].PhaseRhop0/PhaseData[iphase].PhaseGamma;
				press[p]=(idp[p]>=CaseNbound? cteb*(pow(rhop[p]/PhaseData[iphase].PhaseRhop0,PhaseData[iphase].PhaseGamma)-1.0f): 0.f);
				break;
			}
		}
	}

    //-Define campos a grabar.
    //-Defines fields to be stored.
    JFormatFiles2::StScalarData fields[8];
    unsigned nfields=0;
    if(idp){   fields[nfields]=JFormatFiles2::DefineField("Idp" ,JFormatFiles2::UInt32 ,1,idp);   nfields++; }
    if(vel){   fields[nfields]=JFormatFiles2::DefineField("Vel" ,JFormatFiles2::Float32,3,vel);   nfields++; }
    if(rhop){  fields[nfields]=JFormatFiles2::DefineField("Rhop",JFormatFiles2::Float32,1,rhop);  nfields++; }
    if(type){  fields[nfields]=JFormatFiles2::DefineField("Type",JFormatFiles2::UChar8 ,1,type);  nfields++; }
	if(press){ fields[nfields]=JFormatFiles2::DefineField("Pressure",JFormatFiles2::Float32,1,press);  nfields++; }
    if(SvData&SDAT_Vtk)JFormatFiles2::SaveVtk(DirOut+fun::FileNameSec("PartVtk.vtk",Part),npok,posf3,nfields,fields);
    if(SvData&SDAT_Csv)JFormatFiles2::SaveCsv(DirOut+fun::FileNameSec("PartCsv.csv",Part),npok,posf3,nfields,fields);
    //-libera memoria.
    //-release of memory.
    delete[] posf3;
    delete[] type; 
	delete[] press;
  }

  //-Graba datos de particulas excluidas.
  //-Stores data of excluded particles.
  if(DataOutBi4 && PartsOut->GetCount()){
    if(SvDouble)DataOutBi4->SavePartOut(Part,TimeStep,PartsOut->GetCount(),PartsOut->GetIdpOut(),PartsOut->GetPosOut(),PartsOut->GetVelOut(),PartsOut->GetRhopOut());
    else{
      const tfloat3* posf3=GetPointerDataFloat3(PartsOut->GetCount(),PartsOut->GetPosOut());
      DataOutBi4->SavePartOut(Part,TimeStep,PartsOut->GetCount(),PartsOut->GetIdpOut(),posf3,PartsOut->GetVelOut(),PartsOut->GetRhopOut());
      delete[] posf3;
    }
  }

  //-Graba datos de floatings.
  //-Stores data of floatings.
  if(DataFloatBi4){
    if(CellOrder==ORDER_XYZ)for(unsigned cf=0;cf<FtCount;cf++)DataFloatBi4->AddPartData(cf,FtObjs[cf].center,FtObjs[cf].fvel,FtObjs[cf].fomega);
    else                    for(unsigned cf=0;cf<FtCount;cf++)DataFloatBi4->AddPartData(cf,OrderDecodeValue(CellOrder,FtObjs[cf].center),OrderDecodeValue(CellOrder,FtObjs[cf].fvel),OrderDecodeValue(CellOrder,FtObjs[cf].fomega));
    DataFloatBi4->SavePartFloat(Part,TimeStep,(UseDEM? DemDtForce: 0));
  }

  //-Vacia almacen de particulas excluidas.
  //-Empties stock of excluded particles.
  PartsOut->Clear();
}

//==============================================================================
/// Genera los ficheros de salida de datos.
/// Generates data output files.
//==============================================================================
void JSph::SaveData(unsigned npok,const unsigned *idp,const tdouble3 *pos,const tfloat3 *vel,const float *rhop
  ,unsigned ndom,const tdouble3 *vdom,const StInfoPartPlus *infoplus)
{
  const char met[]="SaveData";
  string suffixpartx=fun::PrintStr("_%04d",Part);

  //-Contabiliza nuevas particulas excluidas.
  //-Counts new excluded particles.
  const unsigned noutpos=PartsOut->GetOutPosCount(),noutrhop=PartsOut->GetOutRhopCount(),noutmove=PartsOut->GetOutMoveCount();
  const unsigned nout=noutpos+noutrhop+noutmove;
  AddOutCount(noutpos,noutrhop,noutmove);

  //-Graba ficheros con datos de particulas.
  //-Stores data files of particles.
  SavePartData(npok,nout,idp,pos,vel,rhop,ndom,vdom,infoplus);
  
  //-Reinicia limites de dt.
  //-Reinitialises limits of dt.
  PartDtMin=DBL_MAX; PartDtMax=-DBL_MAX;

  //-Calculo de tiempo.
  //-Computation of time.
  if(Part>PartIni||Nstep){
    TimerPart.Stop();
    double tpart=TimerPart.GetElapsedTimeD()/1000;
    double tseg=tpart/(TimeStep-TimeStepM1);
    TimerSim.Stop();
    double tcalc=TimerSim.GetElapsedTimeD()/1000;
    double tleft=(tcalc/(TimeStep-TimeStepIni))*(TimeMax-TimeStep);
    Log->Printf("Part%s  %12.6f  %12d  %7d  %9.2f  %14s",suffixpartx.c_str(),TimeStep,(Nstep+1),Nstep-PartNstep,tseg,fun::GetDateTimeAfter(int(tleft)).c_str());
  }
  else Log->Printf("Part%s        %u particles successfully stored",suffixpartx.c_str(),npok);   
  
  //-Muestra info de particulas excluidas
  //-Shows info of the excluded particles
  if(nout){
    PartOut+=nout;
    Log->Printf("  Particles out: %u  (total: %u)",nout,PartOut);
  }

  if(SvDomainVtk)SaveDomainVtk(ndom,vdom);
}

//£££££££££££££££££££££££££££££££££££££££££££££££££££££££££££££££££££££££//MP
//==============================================================================
/// Genera los ficheros de salida de datos.
/// Generates data output files for the multi-phase flows. -££//MP
//==============================================================================
void JSph::SaveDataMP(unsigned npok,const unsigned *idp,const tdouble3 *pos,const tfloat3 *vel,const float *rhop,const word *code
  ,unsigned ndom,const tdouble3 *vdom,const StInfoPartPlus *infoplus)
{
  const char met[]="SaveData";
  string suffixpartx=fun::PrintStr("_%04d",Part);

  //-Contabiliza nuevas particulas excluidas.
  //-Counts new excluded particles.
  const unsigned noutpos=PartsOut->GetOutPosCount(),noutrhop=PartsOut->GetOutRhopCount(),noutmove=PartsOut->GetOutMoveCount();
  const unsigned nout=noutpos+noutrhop+noutmove;
  AddOutCount(noutpos,noutrhop,noutmove);

  //-Graba ficheros con datos de particulas.
  //-Stores data files of particles.
  SavePartDataMP(npok,nout,idp,pos,vel,rhop,code,ndom,vdom,infoplus);
  
  //-Reinicia limites de dt.
  //-Reinitialises limits of dt.
  PartDtMin=DBL_MAX; PartDtMax=-DBL_MAX;

  //-Calculo de tiempo.
  //-Computation of time.
  if(Part>PartIni||Nstep){
    TimerPart.Stop();
    double tpart=TimerPart.GetElapsedTimeD()/1000;
    double tseg=tpart/(TimeStep-TimeStepM1);
    TimerSim.Stop();
    double tcalc=TimerSim.GetElapsedTimeD()/1000;
    double tleft=(tcalc/(TimeStep-TimeStepIni))*(TimeMax-TimeStep);
    Log->Printf("Part%s  %12.6f  %12d  %7d  %9.2f  %14s",suffixpartx.c_str(),TimeStep,(Nstep+1),Nstep-PartNstep,tseg,fun::GetDateTimeAfter(int(tleft)).c_str());
  }
  else Log->Printf("Part%s        %u particles successfully stored",suffixpartx.c_str(),npok);   
  
  //-Muestra info de particulas excluidas
  //-Shows info of the excluded particles
  if(nout){
    PartOut+=nout;
    Log->Printf("  Particles out: %u  (total: %u)",nout,PartOut);
  }

  if(SvDomainVtk)SaveDomainVtk(ndom,vdom);
}
//£££££££££££££££££££££££££££££££££££££££££££££££££££££££££££££££££££££££//MP

//==============================================================================
/// Genera fichero VTK con el dominio de las particulas.
/// Generates VTK file with domain of the particles.
//==============================================================================
void JSph::SaveDomainVtk(unsigned ndom,const tdouble3 *vdom)const{ 
  if(vdom){
    string fname=fun::FileNameSec("Domain.vtk",Part);
    tfloat3 *vdomf3=new tfloat3[ndom*2];
    for(unsigned c=0;c<ndom*2;c++)vdomf3[c]=ToTFloat3(vdom[c]);
    JFormatFiles2::SaveVtkBoxes(DirOut+fname,ndom,vdomf3,H*0.5f);
    delete[] vdomf3;
  }
}

//==============================================================================
/// Genera fichero VTK con las celdas del mapa.
/// Generates VTK file with map cells.
//==============================================================================
void JSph::SaveMapCellsVtk(float scell)const{
  JFormatFiles2::SaveVtkCells(DirOut+"MapCells.vtk",ToTFloat3(OrderDecode(MapRealPosMin)),OrderDecode(Map_Cells),scell);
}

//==============================================================================
/// Añade la informacion basica de resumen a hinfo y dinfo.
/// Adds basic information of resume to hinfo & dinfo.
//==============================================================================
void JSph::GetResInfo(float tsim,float ttot,const std::string &headplus,const std::string &detplus,std::string &hinfo,std::string &dinfo){
//£££££££££££££££££££££££££££££££££££££££££££££££££££££££££££££££££££££££//MP
  hinfo=hinfo+"#RunName;RunCode;DateTime;Np;TSimul;TSeg;TTotal;MemCpu;MemGpu;Steps;PartFiles;PartsOut;MaxParticles;MaxCells;FlowType;MPCoef;Hw;StepAlgo;Kernel;Viscosity;ViscoValue;DeltaSPH;TMax;Nbound;Nfixed;H;RhopOut;PartsRhopOut;PartsVelOut;CellMode"+headplus;
//£££££££££££££££££££££££££££££££££££££££££££££££££££££££££££££££££££££££//MP  
  dinfo=dinfo+ RunName+ ";"+ RunCode+ ";"+ RunTimeDate+ ";"+ fun::UintStr(CaseNp);
  dinfo=dinfo+ ";"+ fun::FloatStr(tsim)+ ";"+ fun::FloatStr(tsim/float(TimeStep))+ ";"+ fun::FloatStr(ttot);
  dinfo=dinfo+ ";"+ fun::LongStr(MaxMemoryCpu)+ ";"+ fun::LongStr(MaxMemoryGpu);
  const unsigned nout=GetOutPosCount()+GetOutRhopCount()+GetOutMoveCount();
  dinfo=dinfo+ ";"+ fun::IntStr(Nstep)+ ";"+ fun::IntStr(Part)+ ";"+ fun::UintStr(nout);
  dinfo=dinfo+ ";"+ fun::UintStr(MaxParticles)+ ";"+ fun::UintStr(MaxCells);
//£££££££££££££££££££££££££££££££££££££££££££££££££££££££££££££££££££££££//MP
  dinfo=dinfo+ ";"+ GetFlowType(TPhase)+ ";"+ fun::FloatStr(MPCoef);
//£££££££££££££££££££££££££££££££££££££££££££££££££££££££££££££££££££££££//MP
  dinfo=dinfo+ ";"+ Hardware+ ";"+ GetStepName(TStep)+ ";"+ GetKernelName(TKernel)+ ";"+ GetViscoName(TVisco)+ ";"+ fun::FloatStr(Visco);
  dinfo=dinfo+ ";"+ fun::FloatStr(DeltaSph,"%G")+ ";"+ fun::FloatStr(float(TimeMax));
  dinfo=dinfo+ ";"+ fun::UintStr(CaseNbound)+ ";"+ fun::UintStr(CaseNfixed)+ ";"+ fun::FloatStr(H);
  std::string rhopcad;
  if(RhopOut)rhopcad=fun::PrintStr("(%G-%G)",RhopOutMin,RhopOutMax); else rhopcad="None";
  dinfo=dinfo+ ";"+ rhopcad+ ";"+ fun::UintStr(GetOutRhopCount())+ ";"+ fun::UintStr(GetOutMoveCount())+ ";"+ GetNameCellMode(CellMode)+ detplus;
}

//==============================================================================
/// Genera fichero Run.csv con resumen de ejecucion.
/// Generates file Run.csv with resume of execution.
//==============================================================================
void JSph::SaveRes(float tsim,float ttot,const std::string &headplus,const std::string &detplus){
  const char* met="SaveRes";
  string fname=DirOut+"Run.csv";
  ofstream pf;
  pf.open(fname.c_str());
  if(pf){
    string hinfo,dinfo;
    GetResInfo(tsim,ttot,headplus,detplus,hinfo,dinfo);
    pf << hinfo << endl << dinfo << endl;
    if(pf.fail())RunException(met,"Failed writing to file.",fname);
    pf.close();
  }
  else RunException(met,"File could not be opened.",fname);
}

//==============================================================================
/// Muestra resumen de ejecucion.
/// Shows resume of execution.
//==============================================================================
void JSph::ShowResume(bool stop,float tsim,float ttot,bool all,std::string infoplus){
  Log->Printf("\n[Simulation %s  %s]",(stop? "INTERRUPTED": "finished"),fun::GetDateTime().c_str());
  Log->Printf("Particles of simulation (initial): %u",CaseNp);
  if(NpDynamic)Log->Printf("Particles of simulation (total)..: %llu",TotalNp);
  if(all){
    Log->Printf("DTs adjusted to DtMin............: %d",DtModif);
    const unsigned nout=GetOutPosCount()+GetOutRhopCount()+GetOutMoveCount();
    Log->Printf("Excluded particles...............: %d",nout);
    if(GetOutRhopCount())Log->Printf("Excluded particles due to RhopOut: %u",GetOutRhopCount());
    if(GetOutMoveCount())Log->Printf("Excluded particles due to Velocity: %u",GetOutMoveCount());
  }
  Log->Printf("Total Runtime....................: %f sec.",ttot);
  Log->Printf("Simulation Runtime...............: %f sec.",tsim);
  if(all){
    float tseg=tsim/float(TimeStep);
    float nstepseg=float(Nstep)/tsim;
    Log->Printf("Time per second of simulation....: %f sec.",tseg);
    Log->Printf("Steps per second.................: %f",nstepseg);
    Log->Printf("Steps of simulation..............: %d",Nstep);
    Log->Printf("PART files.......................: %d",Part-PartIni);
    while(!infoplus.empty()){
      string lin=fun::StrSplit("#",infoplus);
      if(!lin.empty()){
        string tex=fun::StrSplit("=",lin);
        string val=fun::StrSplit("=",lin);
        while(tex.size()<33)tex=tex+".";
        Log->Print(tex+": "+val);
      }
    }
  }
  Log->Printf("Maximum number of particles......: %u",MaxParticles);
  Log->Printf("Maximum number of cells..........: %u",MaxCells);
  Log->Printf("CPU Memory.......................: %lld (%.2f MB)",MaxMemoryCpu,double(MaxMemoryCpu)/(1024*1024));
  if(MaxMemoryGpu)Log->Printf("GPU Memory.......................: %lld (%.2f MB)",MaxMemoryGpu,double(MaxMemoryGpu)/(1024*1024));
}

//==============================================================================
/// Devuelve texto sobre la configuracion de PosDouble.
/// Returns text about PosDouble configuration.
//==============================================================================
std::string JSph::GetPosDoubleName(bool psimple,bool svdouble){
  string tx;
  if(psimple && !svdouble)tx="0: Uses and stores in single precision";
  else if(!psimple && !svdouble)tx="1: Uses double and stores in single precision";
  else if(!psimple && svdouble)tx="2: Uses and stores in double precision";
  else tx="???";
  return(tx);
}

//==============================================================================
/// Devuelve el nombre del algoritmo en texto.
/// Returns the name of the time algorithm in text format.
//==============================================================================
std::string JSph::GetStepName(TpStep tstep){
  string tx;
  if(tstep==STEP_Verlet)tx="Verlet";
  else if(tstep==STEP_Symplectic)tx="Symplectic";
  else tx="???";
  return(tx);
}

//£££££££££££££££££££££££££££££££££££££££££££££££££££££££££££££££££££££££//MP
//==============================================================================
/// Returns the name of the flow type in text format. -££//MP
//==============================================================================
std::string JSph::GetFlowType(TpPhase tphase){
  string tx;
  if(tphase==FLOW_Single)tx="Singlephase";
  else if(tphase==FLOW_Multi)tx="Multiphase";
  else tx="???";
  return(tx);
}
//£££££££££££££££££££££££££££££££££££££££££££££££££££££££££££££££££££££££//MP

//==============================================================================
/// Devuelve el nombre del kernel en texto.
/// Returns the name of the kernel in text format.
//==============================================================================
std::string JSph::GetKernelName(TpKernel tkernel){
  string tx;
  if(tkernel==KERNEL_Cubic)tx="Cubic";
  else if(tkernel==KERNEL_Wendland)tx="Wendland";
  else tx="???";
  return(tx);
}

//==============================================================================
/// Devuelve el nombre de la viscosidad en texto.
/// Returns value of viscosity in text format.
//==============================================================================
std::string JSph::GetViscoName(TpVisco tvisco){
  string tx;
  if(tvisco==VISCO_Artificial)tx="Artificial";
  else if(tvisco==VISCO_LaminarSPS)tx="Laminar+SPS";
  else tx="???";
  return(tx);
}

//==============================================================================
/// Devuelve el valor de DeltaSPH en texto.
/// Returns value of DeltaSPH in text format.
//==============================================================================
std::string JSph::GetDeltaSphName(TpDeltaSph tdelta){
  string tx;
  if(tdelta==DELTA_None)tx="None";
  else if(tdelta==DELTA_Dynamic)tx="Dynamic";
  else if(tdelta==DELTA_DynamicExt)tx="DynamicExt";
  else tx="???";
  return(tx);
}

//==============================================================================
/// Devuelve el valor de Shifting en texto.
/// Returns value of Shifting in text format.
//==============================================================================
std::string JSph::GetShiftingName(TpShifting tshift){
  string tx;
  if(tshift==SHIFT_None)tx="None";
  else if(tshift==SHIFT_NoBound)tx="NoBound";
  else if(tshift==SHIFT_NoFixed)tx="NoFixed";
  else if(tshift==SHIFT_Full)tx="Full";
  else tx="???";
  return(tx);
}

//==============================================================================
/// Devuelve string con el nombre del temporizador y su valor.
/// Returns string with the name of timer and value.
//==============================================================================
std::string JSph::TimerToText(const std::string &name,float value){
  string ret=name;
  while(ret.length()<33)ret+=".";
  return(ret+": "+fun::FloatStr(value/1000)+" sec.");
}





