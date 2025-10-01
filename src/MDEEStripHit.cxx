/*
 * MDEEStripHit.cxx
 *
 *
 * Copyright (C) by YOUR NAME HERE.
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
// MDEEStripHit
//
////////////////////////////////////////////////////////////////////////////////


// Include the header:
#include "MDEEStripHit.h"

// Standard libs:

// ROOT libs:

// MEGAlib libs:


////////////////////////////////////////////////////////////////////////////////


#ifdef ___CLING___
ClassImp(MDEEStripHit)
#endif


////////////////////////////////////////////////////////////////////////////////


MDEEStripHit::MDEEStripHit() : m_SimulatedEventID(0), m_SimulatedPosition(0,0,0), m_SimulatedDepth(0), m_SimulatedEnergy(0), m_SimulatedIsGuardRing(false), m_SimulatedHitIndex(0), m_Energy(0), m_ADC(0), m_TAC(0), m_Temperature(0)
{
  // Construct an instance of MDEEStripHit
}


// MDEEStripHit.cxx: the end...
////////////////////////////////////////////////////////////////////////////////
