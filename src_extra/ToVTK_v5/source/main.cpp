/*
 <TOVTK4>  Copyright (c) 2020 by Dr Jose M. Dominguez et al. (see http://dual.sphysics.org/index.php/developers/). 

 EPHYSLAB Environmental Physics Laboratory, Universidade de Vigo, Ourense, Spain.
 School of Mechanical, Aerospace and Civil Engineering, University of Manchester, Manchester, U.K.

 This file is part of DualSPHysics. 

 DualSPHysics is free software: you can redistribute it and/or modify it under the terms of the GNU Lesser General Public License 
 as published by the Free Software Foundation; either version 2.1 of the License, or (at your option) any later version.
 
 DualSPHysics is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU Lesser General Public License for more details. 

 You should have received a copy of the GNU Lesser General Public License along with DualSPHysics. If not, see <http://www.gnu.org/licenses/>. 
*/

#include "Functions.h"
#include "JException.h"
#include "JCfgRun.h"
#include "TypesDef.h"
#include "JPartDataBi4.h"
#include "JVtkLib.h"
#include "JDataArrays.h"
#include "JOutputCsv.h"
#include "JXml.h"
#include "JSpaceCtes.h"
#include "JSpaceEParms.h"
#include "JSpaceParts.h"

#include <string>
#include <iostream>
#include <sstream>
#include <fstream>
#include <cmath>
#include <cstring>
#include <cstdio>

//using namespace std;
using std::string;
using std::exception;

const char *APP_NAME="ToVtk v5.0.027 (28-06-2020)";

//==============================================================================
// Invoca una excepcion referente a gestion de ficheros
//==============================================================================
void ExceptionFile(string msg,string file){
  msg=msg+"\nFile:["+file+"]\n";
  throw msg;
}

//==============================================================================
// Invoca una excepcion generica
//==============================================================================
void ExceptionText(string msg){
  throw msg;
}

//==============================================================================
/// Processes files.
//==============================================================================
void RunFiles(const JCfgRun *cfg){
  const bool outtype=true,outmk=true,outpres=true;

  //-Reads XML file.
  JSpaceCtes* xmlctes=NULL;
  JSpaceEParms* xmleparms=NULL;
  JSpaceParts* xmlparts=NULL;
  if(outmk){
    if(!fun::FileExists(cfg->FileXml))ExceptionFile("XML file of case was not found.",(cfg->FileXml.empty()? string("???"): cfg->FileXml));
    JXml xml; xml.LoadFile(cfg->FileXml);
    xmlctes=new JSpaceCtes();     xmlctes->LoadXmlRun(&xml,"case.execution.constants");
    xmleparms=new JSpaceEParms(); xmleparms->LoadXml(&xml,"case.execution.parameters");
    xmlparts=new JSpaceParts();   xmlparts->LoadXml(&xml,"case.execution.particles");
  }

  //-Prepares input files.
  const bool onefile=!cfg->FileIn.empty();
  const string casein=cfg->FileIn;
  const string dirin=fun::GetDirWithSlash(cfg->DirIn);
  const int last=cfg->Last;
  int part=(onefile||cfg->First<0? 0: cfg->First);

  unsigned npiece=0;
  unsigned casenp=0,casenbound=0,casenfixed=0,casenmoving=0,casenfloat=0,casenfluid=0;
  double cteb=0,rhop0=0,gamma=0;

  double timestep=0;
  unsigned *idp=NULL;
  tfloat3 *pos=NULL,*vel=NULL;
  tdouble3 *posd=NULL;
  float *rhop=NULL;
  unsigned *ridp=NULL;
  byte *type=NULL,*mk=NULL;
  float *pres=NULL;

  //-Processes input files.
  byte npie=0;
  string file=JPartDataBi4::GetFileData(casein,dirin,part,npie);
  if(file.empty())ExceptionText("Error: Data files not found.");
  bool firstdata=true;
  while((last<0||part<=last) && fun::FileExists(file)){
    if(!npiece){//-Reads initial data.
      JPartDataBi4 pd;
      if(onefile)pd.LoadFileCase("",casein,0,npie);
      else pd.LoadFilePart(dirin,part,0,npie);
      npiece=pd.GetNpiece();
      if(npiece>1)ExceptionText("Error: The number of pieces is higher than 1.");
      casenp=(unsigned)pd.Get_CaseNp();
      if(pd.Get_CaseNp()!=casenp)ExceptionText("Error: The number of particles is too big.");
      if(!pd.Get_IdpSimple())ExceptionText("Error: Only Idp (32 bits) is valid at the moment.");
      casenfluid=(unsigned)pd.Get_CaseNfluid();
      casenfixed=(unsigned)pd.Get_CaseNfixed();
      casenmoving=(unsigned)pd.Get_CaseNmoving();
      casenfloat=(unsigned)pd.Get_CaseNfloat();
      casenbound=casenp-casenfluid;
      cteb=pd.Get_B();
      rhop0=pd.Get_Rhop0();
      gamma=pd.Get_Gamma();
      pos=new tfloat3[casenp];
      posd=new tdouble3[casenp];
      vel=new tfloat3[casenp];
      rhop=new float[casenp];
      idp=new unsigned[casenp];
      //-Allocates memory to other variables.
      if(outmk)mk=new byte[casenp];
      if(outtype)type=new byte[casenp];
      if(outpres)pres=new float[casenp];
      if(outmk)ridp=new unsigned[casenp];
    }

    //-Reads particle data.
    unsigned np=0;
    for(unsigned cp=0;cp<npiece && cp<1;cp++){
      file=(onefile? JPartDataBi4::GetFileNameCase(casein,cp,npiece): dirin+JPartDataBi4::GetFileNamePart(part,cp,npiece));
      if(!cp)printf("load> %s\n",file.c_str());
      JPartDataBi4 pd;
      if(onefile)pd.LoadFileCase("",casein,0,npiece);
      else pd.LoadFilePart(dirin,part,0,npiece);
      if(!cp)timestep=pd.Get_TimeStep();
      const bool possimple=pd.Get_PosSimple();
      const unsigned npok=pd.Get_Npok();
      if(npok){
        //-Loads data from PART.
        pd.Get_Idp(npok,idp);
        pd.Get_Vel(npok,vel);
        pd.Get_Rhop(npok,rhop);
        if(possimple){
          pd.Get_Pos(npok,pos);
        }
        else{ 
          pd.Get_Posd(npok,posd);
          for(unsigned p=0;p<npok;p++)pos[p]=ToTFloat3(posd[p]);
        }
      }
      np=npok;
    }

    //-Loads other vars.
    if(pres)for(unsigned c=0;c<np;c++)pres[c]=(float)(cteb*(pow(rhop[c]/rhop0,gamma)-1.));
    if(type){
      for(unsigned p=0;p<np;p++){
        const unsigned id=idp[p];
        type[p]=(id<casenfixed? 0: (id<casenmoving? 1: (id<casenfloat? 2: 3)));
      }
    }
    if(ridp)for(unsigned c=0;c<np;c++)ridp[idp[c]]=c;
    if(mk){
      for(unsigned c=0;c<xmlparts->CountBlocks();c++){
        const JSpacePartBlock &block=xmlparts->GetBlock(c);
        const byte mkblock=(byte)block.GetMk();
        const unsigned ipend=block.GetBegin()+block.GetCount();
        for(unsigned ip=block.GetBegin();ip<ipend;ip++)mk[ridp[ip]]=mkblock;
      }
    }

    //-Defines variables to save in VTk or CSV.
    JDataArrays arrays;
    arrays.AddArray("Pos"  ,np,pos ,false);
    arrays.AddArray("Idp"  ,np,idp ,false);
    arrays.AddArray("Vel"  ,np,vel ,false);
    arrays.AddArray("Rhop" ,np,rhop,false);
    arrays.AddArray("Type" ,np,type,false);
    arrays.AddArray("Press",np,pres,false);
    arrays.AddArray("Mk"   ,np,mk  ,false);

    //-Saves VTK files.
    if(!cfg->SaveVtk.empty()){
      string fileout=(onefile? cfg->SaveVtk: fun::FileNameSec(cfg->SaveVtk,part));
      if(fun::GetExtension(fileout).empty())fileout=fun::AddExtension(fileout,"vtk");
      printf("SaveVTK> %s\n",fun::ShortFileName(fileout,68).c_str());
      JVtkLib::SaveVtkData(fileout,arrays,"Pos");
    }
    //-Saves CSV files.
    if(!cfg->SaveCsv.empty()){
      string fileout=(onefile? cfg->SaveCsv: fun::FileNameSec(cfg->SaveCsv,part));
      if(fun::GetExtension(fileout).empty())fileout=fun::AddExtension(fileout,"csv");
      printf("SaveCSV> %s\n",fun::ShortFileName(fileout,68).c_str());
      JOutputCsv ocsv(false,true);
      ocsv.SaveCsv(fileout,arrays);
    }

    firstdata=false;
    if(!onefile){
      part++;
      file=dirin+JPartDataBi4::GetFileNamePart(part,0,npiece);
    }
    else break;
  }

  //-Free memory.
  delete[] pos;  pos=NULL;
  delete[] posd; posd=NULL;
  delete[] vel;  vel=NULL;
  delete[] rhop; rhop=NULL;
  delete[] idp; idp=NULL;
  delete[] ridp; ridp=NULL;
  delete[] mk;   mk=NULL;
  delete[] type; type=NULL;
  delete[] pres; pres=NULL;
  delete xmlctes;   xmlctes=NULL;
  delete xmleparms; xmleparms=NULL;
  delete xmlparts;  xmlparts=NULL;
}

//==============================================================================
/// GPL License.
//==============================================================================
std::string getlicense_lgpl(const std::string &name){
  std::string tx="";
  tx=tx+"\n\n <"+fun::StrUpper(name)+">  Copyright (c) 2020 by Dr Jose M. Dominguez";
  tx=tx+"\n (see http://dual.sphysics.org/index.php/developers/)\n";
  tx=tx+"\n EPHYSLAB Environmental Physics Laboratory, Universidade de Vigo";
  tx=tx+"\n School of Mechanical, Aerospace and Civil Engineering, University of Manchester\n";
  tx=tx+"\n DualSPHysics is free software: you can redistribute it and/or"; 
  tx=tx+"\n modify it under the terms of the GNU Lesser General Public License";
  tx=tx+"\n as published by the Free Software Foundation, either version 2.1 of"; 
  tx=tx+"\n the License, or (at your option) any later version.\n"; 
  tx=tx+"\n DualSPHysics is distributed in the hope that it will be useful,"; 
  tx=tx+"\n but WITHOUT ANY WARRANTY; without even the implied warranty of"; 
  tx=tx+"\n MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the"; 
  tx=tx+"\n GNU Lesser General Public License for more details.\n";
  tx=tx+"\n You should have received a copy of the GNU Lesser General Public License"; 
  tx=tx+"\n along with DualSPHysics. If not, see <http://www.gnu.org/licenses/>.\n\n";
  return(tx);
}

//==============================================================================
//==============================================================================
int main(int argc, char** argv){
  int errcode=1;
  printf("%s",getlicense_lgpl("TOVTK4").c_str());
  printf("\n%s\n",APP_NAME);
  for(unsigned c=0;c<=strlen(APP_NAME);c++)printf("="); printf("\n");
  JCfgRun cfg;
  try{
    cfg.LoadArgv(argc,argv);
    if(!cfg.PrintInfo){
      cfg.ValidaCfg();
      RunFiles(&cfg);
    }
    errcode=0;
  }
  catch(const char *cad){
    printf("\n*** Exception: %s\n",cad);
  }
  catch(const string &e){
    printf("\n*** Exception: %s\n",e.c_str());
  }
  catch (const exception &e){
    printf("\n*** %s\n",e.what());
  }
  catch(...){
    printf("\n*** Attention: Unknown exception...\n");
  }
  return(errcode);
}


