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

/// \file JWaveGen.h \brief Declares the class \ref JWaveGen.

#ifndef _JWaveGenUndef_
#define _JWaveGenUndef_

class JXml;
class JLog2;
class JWavePaddles;


//##############################################################################
//# JWaveGen
//##############################################################################
/// \brief Implements wave generation for regular and irregular waves.

#ifdef DISABLE_WAVEGEN
class JWaveGen
{
public:

  //==============================================================================
  /// Constructor.
  //==============================================================================
  JWaveGen(JLog2* log,std::string dirdata,JXml *sxml,const std::string &place){}

  //==============================================================================
  /// Destructor.
  //==============================================================================
  ~JWaveGen(){}

  //==============================================================================
  /// Returns true when this feature is available.
  //==============================================================================
  static bool Available(){ return(true); }

  //==============================================================================
  /// Configura paddle con datos de las particulas.
  /// Set paddle with the particle data.
  //==============================================================================
  bool ConfigPaddle(word mkbound,word paddleid,unsigned idbegin,unsigned np){ return(false); }

  //==============================================================================
  /// Prepara movimiento de paddles.
  /// Prepares paddle movement.
  //==============================================================================
  void Init(double timemax,tfloat3 gravity,bool simulate2d,TpCellOrder cellorder,float massfluid,double dp,float dosh,float scell,int hdiv,tdouble3 domposmin,tdouble3 domrealposmin,tdouble3 domrealposmax){}

  //==============================================================================
  /// Shows object configuration using Log.
  //==============================================================================
  void VisuConfig(std::string txhead,std::string txfoot){}

  //==============================================================================
  /// Devuelve si es un movimiento lineal y los datos de movimiento para el intervalo indicado.
  /// Returns if it is a linear motion and the motion data for the specified interval.
  //==============================================================================
  bool GetMotion(unsigned cp,double timestep,double dt,tdouble3 &mvsimple,tmatrix4d &mvmatrix,unsigned &np,unsigned &idbegin){ return(false); }

  unsigned GetCount()const{ return(0); }
  bool UseAwas()const{ return(false); }
  bool UseAwasZsurf()const{ return(false); }

};
#endif

#endif


