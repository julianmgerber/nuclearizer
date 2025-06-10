/*
 * MStripHit.cxx
 *
 *
 * Copyright (C) by Andreas Zoglauer.
 * All rights reserved.
 *
 *
 * This code implementation is the intellectual property of
 * Andreas Zoglauer.
 *
 * By copying, distributing or modifying the Program (or any work
 * based on the Program) you indicate your acceptance of this statement,
 * and all its terms.
 *
 */


////////////////////////////////////////////////////////////////////////////////
//
// MStripHit
//
////////////////////////////////////////////////////////////////////////////////


// Include the header:
#include "MStripHit.h"

// Standard libs:
#include <iomanip>
#include <algorithm>
using namespace std;

// ROOT libs:

// MEGAlib libs:
#include "MStreams.h"

////////////////////////////////////////////////////////////////////////////////


#ifdef ___CLING___
ClassImp(MStripHit)
#endif


////////////////////////////////////////////////////////////////////////////////


MStripHit::MStripHit()
{
  // Construct an instance of MStripHit

  m_ReadOutElement = new MReadOutElementDoubleStrip();
  
  Clear();
}


////////////////////////////////////////////////////////////////////////////////


MStripHit::~MStripHit()
{
  // Delete this instance of MStripHit
  
  delete m_ReadOutElement;
}


////////////////////////////////////////////////////////////////////////////////


void MStripHit::Clear()
{
  // Reset all data

  m_ReadOutElement->Clear();
  m_HasTriggered = false;
  m_UncorrectedADCUnits = 0;
  m_ADCUnits = 0;
  m_Energy = 0;
  m_EnergyResolution = 0;
  m_TAC = 0;
  m_TACResolution = 0;
  m_Timing = 0;
  m_TimingResolution = 0;
  m_PreampTemp = 0;
  m_IsGuardRing = false;
  m_IsNearestNeighbor = false;
  m_HasGoodTiming = false;
  m_HasFastTiming = false;
  m_Origins.clear();
}


////////////////////////////////////////////////////////////////////////////////


bool MStripHit::Parse(MString& Line, int Version)
{
	const char* line = Line.Data();
	if( line[0] == 'S' && line[1] == 'H' ){
		int det_id, strip_id, has_triggered, timing, un_adc, adc;
		float energy, energy_res;
		char pos_strip;
    unsigned int flags;
		sscanf(&line[3],"%d %c %d %d %d %d %d %f %f %u",&det_id,
			  															   &pos_strip,
																			&strip_id,
																			&has_triggered,
																			&timing,
																			&un_adc,
																			&adc,
																			&energy,
																			&energy_res,
                                      &flags);
		SetDetectorID(det_id);
		pos_strip == 'l' ? IsLowVoltageStrip(true) : IsLowVoltageStrip(false);
		SetStripID(strip_id);
		has_triggered == 0 ? HasTriggered(false) : HasTriggered(true);
		SetTiming((double)timing);
		SetUncorrectedADCUnits((double)un_adc);
		SetADCUnits((double)adc);
		SetEnergy(energy);
		SetEnergyResolution(energy_res);
    ParseFlags(flags);
		return true;
	} else {
		return false;
	}

  // to be written later 
/*
  vector<MString> tokens = Line.Tokenize(" ");
  if( tokens.size() >= 10 ){
	  SetDetectorID(tokens.at(1).ToInt());
	  tokens.at(2) == "p" ? IsLowVoltageStrip(true) : IsLowVoltageStrip(false);
	  SetStripID(tokens.at(3).ToInt());
	  tokens.at(4) == "0" ? HasTriggered(false) : HasTriggered(true);
	  SetTiming( tokens.at(5).ToDouble() );
	  SetUncorrectedADCUnits( tokens.at(6).ToDouble() );
	  SetADCUnits( tokens.at(7).ToDouble() );
	  SetEnergy( tokens.at(8).ToDouble() );
	  SetEnergyResolution( tokens.at(9).ToDouble() );
	  int det_id, pos_strip, strip_id
	  return true;
  } else {
	  return false;
  }
  */
}


////////////////////////////////////////////////////////////////////////////////


//! Set the origins from the simulations (take care of duplicates)
void MStripHit::AddOrigins(vector<int> Origins)
{
  m_Origins.insert(m_Origins.end(), Origins.begin(), Origins.end());
  sort(m_Origins.begin(), m_Origins.end());
  m_Origins.erase(unique(m_Origins.begin(), m_Origins.end()), m_Origins.end());
}

  
////////////////////////////////////////////////////////////////////////////////


bool MStripHit::StreamDat(ostream& S, int Version)
{
  //! Stream the content to an ASCII file 
  
  S<<"SH "
   <<m_ReadOutElement->GetDetectorID()<<" "
   <<((m_ReadOutElement->IsLowVoltageStrip() == true) ? "l" : "h")<<" "
   <<m_ReadOutElement->GetStripID()<<" "
   <<m_HasTriggered<<" "
   <<setprecision(9)<<m_Timing<<" "
   <<m_UncorrectedADCUnits<<" "
   <<m_ADCUnits<<" "
   <<m_Energy<<" "
   <<m_EnergyResolution<<" "
   <<MakeFlags()<<endl;
 
  return true;
}




////////////////////////////////////////////////////////////////////////////////


void MStripHit::StreamRoa(ostream& S)
{
  //! Stream the content in MEGAlib's evta format 

  S<<"UH " 
   <<m_ReadOutElement->GetDetectorID()<<" "
   <<m_ReadOutElement->GetStripID()<<" "
   <<((m_ReadOutElement->IsLowVoltageStrip() == true) ? "l" : "h")<<" "
   <<m_ADCUnits<<" "
   <<m_TAC<<" "
   <<m_PreampTemp<<" "
   <<MakeFlags()<<" ";
  for (unsigned int i = 0; i < m_Origins.size(); ++i) {
    if (i != 0) S<<";";
    S<<m_Origins[i]; 
  }
  S<<endl;
}


////////////////////////////////////////////////////////////////////////////////


unsigned int MStripHit::MakeFlags()
{
  //! Return flags to indicate the type of strip hit
  //! Currently, 2 bits:
  //!   v = Is a nearest neighbor
  //!    v = Is a guard ring
  //! 0b11u

  unsigned int Flags = 0b00u;
  if (m_IsGuardRing == true) {
    Flags = Flags | 0b01u;
  }
  if (m_IsNearestNeighbor == true) {
    Flags = Flags | 0b10u;
  }

  return Flags;
}


////////////////////////////////////////////////////////////////////////////////


void MStripHit::ParseFlags(unsigned int Flags)
{
  //! Set internal booleans according to flag
  //! Currently, 2 bits:
  //!   v = Is a nearest neighbor
  //!    v = Is a guard ring
  //! 0b11u

  IsGuardRing(Flags & 0b01u);
  IsNearestNeighbor(Flags & 0b10u);
}


// MStripHit.cxx: the end...
////////////////////////////////////////////////////////////////////////////////
