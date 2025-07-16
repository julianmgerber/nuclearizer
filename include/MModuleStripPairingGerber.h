/*
 * MModuleStripPairingGerber.h
 *
 * Copyright (C) by Andreas Zoglauer.
 * All rights reserved.
 *
 * Please see the source-file for the copyright-notice.
 *
 */


#ifndef __MModuleStripPairingGerber__
#define __MModuleStripPairingGerber__


////////////////////////////////////////////////////////////////////////////////


// Standard libs:
#include<iostream>
#include<vector>
#include<cmath>
#include<algorithm>
#include<map>
#include<limits>
#include <numeric>

// ROOT libs:

// MEGAlib libs:
#include "MGlobal.h"
#include "MModule.h"
#include "MStripHit.h"
#include "MGUIExpoStripPairing.h"
#include "MGUIExpoStripPairingHits.h"
#include "MGUIExpoStripPairingStripHits.h"

// Forward declarations:


////////////////////////////////////////////////////////////////////////////////


class MModuleStripPairingGerber : public MModule
{
  // public interface:
 public:
  //! Default constructor
  MModuleStripPairingGerber();
  //! Default destructor
  virtual ~MModuleStripPairingGerber();
  
  //! Create a new object of this class 
  virtual MModuleStripPairingGerber* Clone() { return new MModuleStripPairingGerber(); }

  //! Create the expos
  virtual void CreateExpos();

  //! Initialize the module
  virtual bool Initialize();

  //! Finalize the module
  virtual void Finalize();

  //! Main data analysis routine, which updates the event to a new level 
  virtual bool AnalyzeEvent(MReadOutAssembly* Event);

  //! Show the options GUI
  virtual void ShowOptionsGUI();

  //! Read the configuration data from an XML node
  virtual bool ReadXmlConfiguration(MXmlNode* Node);
  //! Create an XML node tree from the configuration
  virtual MXmlNode* CreateXmlConfiguration();

  // protected methods:
 protected:
  // Find a new set of combinations giving the existing gone
    vector<vector<vector<unsigned int>>> FindNewCombinations(vector<vector<vector<unsigned int>>> OldOnes, vector<MStripHit*> StripHits, bool RoundTwo);
    
    //! Return the order of indices resulting from sorting a vector
    vector<size_t> Argsort(vector<double> &list);

  // private methods:
 private:



  // protected members:
 protected:
  //! The display of debugging data
  MGUIExpoStripPairing* m_ExpoStripPairing;
  MGUIExpoStripPairingHits* m_ExpoStripPairingHits;
  MGUIExpoStripPairingStripHits* m_ExpoStripPairingStripHits;


  // private members:
 private:




#ifdef ___CLING___
 public:
  ClassDef(MModuleStripPairingGerber, 0) // no description
#endif

};

#endif


////////////////////////////////////////////////////////////////////////////////
