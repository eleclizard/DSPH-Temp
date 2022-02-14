//HEAD_DSCODES
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

//#############################################################################
//# Cambios:
//# =========
//# - Nuevo movimiento (PredefX) que lee de un fichero la posición en distintos
//#   instantes e interpola para el instante deseado. (14-10-2010)
//# - Nuevos metodos SetDirIn() y GetDirIn() para poder establecer un directorio
//#   de entrada para ficheros de datos. (19-10-2010)
//# - El movimiento PredefX se ha sustituido por Predef que permite definir el 
//#   formato del fichero de datos (numero de campos por linea y posicion de
//#   cada campo). (28-12-2010)
//# - El movimiento PredefX se ha sustituido por Predef que permite definir el 
//#   formato del fichero de datos (numero de campos por linea y posicion de
//#   cada campo). (28-12-2010)
//# - Pequeños reajustes para aumentar la idependencia entre clases de JMotion
//#   y con otras clases externas. (04-03-2011)
//# - Traduccion de algunos comentarios al ingles. (10-02-2012)
//# - Todas las variables pasaron a doble precision. (02-08-2013)
//# - Nuevo movimiento MovNull, simplemente para separar particulas. (13-05-2014)
//# - Nuevo movimiento MovRotFile. (15-12-2015)
//# - Cambio de nombre de mov MovPrdef por MovRectFile. (15-12-2015)
//# - Se usa JReadDatafile para leer datos movimiento en ficheros. (16-12-2015)
//# - Se permite indicar unidades de angulos a usar: grados o radianes. (16-12-2015)
//# - Se añaden unidades a los datos de movimientos. (16-12-2015)
//# - Se añadio el alias mvfile para mvrectfile (o mvpredef). (29-01-2016)
//#############################################################################

/// \file JMotion.h \brief Declares the class \ref JMotion.

#ifndef _JMotion_
#define _JMotion_

#include "TypesDef.h"
#include "JObject.h"
#include "JMatrix4.h"
#include <string>
#include <cstdlib>
#include <ctime>
#include <vector>

class JMotionObj;
class JMotionMov;
class JMotionEvent;
class JMotionAxis;
class JXml;
class TiXmlNode;

class JMotion : protected JObject
{
private:
  std::vector<JMotionObj*> Objs;
  std::vector<JMotionEvent*> Events;

  JMotionObj** LisMov;    //-Objects that move in the last ProcesTime()
  JMotionObj** LisStop;   //-Objetos that stop in the last ProcesTime()
  unsigned LisMovCount;
  unsigned LisStopCount;
  unsigned ObjCount;

  bool Prepared;
  int EventNext;
  bool ObjsActive;

  std::string Dirin; //-Input directory.

  JMotionObj* ObjGetPointer(unsigned id)const;
  JMotionObj* ObjGetPointerByRef(int ref)const;
  bool ExistsObj(JMotionObj* obj)const;
  void MovAdd(unsigned objid,JMotionMov* mov);
  JMotionAxis* AxisAdd(unsigned objid,const tdouble3 &p1,const tdouble3 &p2);
  void ReadXml(JXml *jxml,TiXmlNode* node,unsigned &id,unsigned idp);

  void MovAddCopy(JMotionMov *mv)const;

public:

  JMotion();
  ~JMotion();
  void Reset();

  void SetDirIn(const std::string &dirin);
  std::string GetDirIn()const{ return(Dirin); };

  void ObjAdd(unsigned id,unsigned idparent,int ref);
  void EventAdd(unsigned objid,unsigned movid,double timestart,double timefinish=-1);

  void MovAddWait(unsigned objid,unsigned id,unsigned nextid,double time);
  void MovAddTeleport(unsigned objid,unsigned id,unsigned nextid,const tdouble3 &mpos);
  void MovAddRectilinear(unsigned objid,unsigned id,unsigned nextid,double time,const tdouble3 &vel);
  void MovAddRectilinearAce(unsigned objid,unsigned id,unsigned nextid,double time,const tdouble3 &ace,const tdouble3 &vel,bool velpre);
  void MovAddRotation(unsigned objid,unsigned id,unsigned nextid,double time,bool angdegrees,const tdouble3 &axisp1,const tdouble3 &axisp2,double velang,bool useangdegrees=true);
  void MovAddRotationAce(unsigned objid,unsigned id,unsigned nextid,double time,bool angdegrees,const tdouble3 &axisp1,const tdouble3 &axisp2,double aceang,double velang,bool velpre,bool useangdegrees=true);
  void MovAddCircular(unsigned objid,unsigned id,unsigned nextid,double time,bool angdegrees,const tdouble3 &axisp1,const tdouble3 &axisp2,const tdouble3 &ref,double velang,bool useangdegrees=true);
  void MovAddCircularAce(unsigned objid,unsigned id,unsigned nextid,double time,bool angdegrees,const tdouble3 &axisp1,const tdouble3 &axisp2,const tdouble3 &ref,double aceang,double velang,bool velpre,bool useangdegrees=true);
  void MovAddRecSinu(unsigned objid,unsigned id,unsigned nextid,double time,bool angdegrees,const tdouble3 &freq,const tdouble3 &ampl,tdouble3 phase,bool phaseprev,bool useangdegrees=true);
  void MovAddRotSinu(unsigned objid,unsigned id,unsigned nextid,double time,bool angdegrees,const tdouble3 &axisp1,const tdouble3 &axisp2,double freq,double ampl,double phase,bool phaseprev,bool useangdegrees=true);
  void MovAddCirSinu(unsigned objid,unsigned id,unsigned nextid,double time,bool angdegrees,const tdouble3 &axisp1,const tdouble3 &axisp2,const tdouble3 &ref,double freq,double ampl,double phase,bool phaseprev,bool useangdegrees=true);
  void MovAddRectilinearFile(unsigned objid,unsigned id,unsigned nextid,double time,const std::string &file,int fields,int fieldtime,int fieldx,int fieldy,int fieldz);
  void MovAddRotationFile(unsigned objid,unsigned id,unsigned nextid,double time,bool angdegrees
    ,const tdouble3 &axisp1,const tdouble3 &axisp2,const std::string &file);
  void MovAddNull(unsigned objid,unsigned id);

  bool ExistsRef(int ref)const{ return(ObjGetPointerByRef(ref)!=NULL); }
  void CheckLinkMovs()const;
  void Prepare();

  bool ProcesTime(double timestep,double dt);
  unsigned GetMovCount()const{ return(LisMovCount); }
  unsigned GetStopCount()const{ return(LisStopCount); }
  bool GetMov(unsigned pos,unsigned &ref,tdouble3 &mvsimple,JMatrix4d &mvmatrix)const;
  unsigned GetStopRef(unsigned pos)const;
  int GetMaxRef()const;

  void CopyConfig(JMotion &mot)const;
  void CopyChangeRef(JMotion &mot,const int* ref,const int* refnew,unsigned refcount)const;
  void Optimize();

  void WriteXml(JXml *jxml,const std::string &path)const;
  void ReadXml(JXml *jxml,const std::string &path,bool checkexists=true);
  void LoadFileXml(const std::string &file,const std::string &path);
  void SaveFileXml(const std::string &file,const std::string &path,bool newfile=true)const;
};

#endif

