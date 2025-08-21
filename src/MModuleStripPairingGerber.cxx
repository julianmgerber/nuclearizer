/*
 * MModuleStripPairingChiSquare.cxx
 * Gerber Version (testing)
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
// MModuleStripPairingGerber
//
////////////////////////////////////////////////////////////////////////////////


// Include the header:
#include "MModuleStripPairingGerber.h"

// Standard libs:
#include <fstream>
#include <iostream>

// ROOT libs:
#include "TGClient.h"

// MEGAlib libs:
#include "MModule.h"
#include "MGUIOptionsStripPairing.h"


////////////////////////////////////////////////////////////////////////////////


#ifdef ___CLING___
ClassImp(MModuleStripPairingGerber)
#endif


////////////////////////////////////////////////////////////////////////////////


MModuleStripPairingGerber::MModuleStripPairingGerber() : MModule()
{
  // Construct an instance of MModuleStripPairingGerber

  // Set all module relevant information

  // Set the module name --- has to be unique
  m_Name = "Strip pairing - Gerber Testing Version";

  // Set the XML tag --- has to be unique --- no spaces allowed
  m_XmlTag = "XmlTagStripPairingGerber";

  // Set all modules, which have to be done before this module
  AddPreceedingModuleType(MAssembly::c_EventLoader);
  AddPreceedingModuleType(MAssembly::c_EnergyCalibration);
//  AddPreceedingModuleType(MAssembly::c_TACcut);

  // Set all types this modules handles
  AddModuleType(MAssembly::c_StripPairing);

  // Set all modules, which can follow this module
  AddSucceedingModuleType(MAssembly::c_NoRestriction);

  // Set if this module has an options GUI
  // Overwrite ShowOptionsGUI() with the call to the GUI!
  m_HasOptionsGUI = false;
  // If true, you have to derive a class from MGUIOptions (use MGUIOptionsTemplate)
  // and implement all your GUI options

  // Can the program be run multi-threaded
  m_AllowMultiThreading = true;

  // Can we use multiple instances of this class
  m_AllowMultipleInstances = true;
}


////////////////////////////////////////////////////////////////////////////////


MModuleStripPairingGerber::~MModuleStripPairingGerber()
{
  // Delete this instance of MModuleStripPairingGerber
}


////////////////////////////////////////////////////////////////////////////////


void MModuleStripPairingGerber::CreateExpos()
{
  // Create all expos

  if (HasExpos() == true) return;

  // Set the histogram display
  m_ExpoStripPairing = new MGUIExpoStripPairing(this);
  m_ExpoStripPairing->SetEnergiesHistogramParameters(1500, 0, 1500);
  m_Expos.push_back(m_ExpoStripPairing);

  m_ExpoStripPairingHits = new MGUIExpoStripPairingHits(this);
  m_ExpoStripPairingHits->SetHitsHistogramParameters(5, 0.5, 5.5);
  m_Expos.push_back(m_ExpoStripPairingHits);

  m_ExpoStripPairingStripHits = new MGUIExpoStripPairingStripHits(this);
  m_ExpoStripPairingStripHits->SetStripHitsHistogramParameters(10, 0.5, 10.5);
  m_Expos.push_back(m_ExpoStripPairingStripHits);
}


////////////////////////////////////////////////////////////////////////////////


bool MModuleStripPairingGerber::Initialize()
{
  // Initialize the module

  return MModule::Initialize();
}

////////////////////////////////////////////////////////////////////////////////

// following function returns a 3D vector of integers
// Input is 3D vector of ints called "Old Ones" and a vector of "Strip Hits" (pointing to an object of type StripHit, defined in MStripHit.cxx)
// StripHit is the literal readout in the .roa file
vector<vector<vector<unsigned int>>> MModuleStripPairingGerber::FindNewCombinations(vector<vector<vector<unsigned int>>> OldOnes, vector<MStripHit*> StripHits, bool RoundTwo)
{
    // define new vector of ints NewOnes
  vector<vector<vector<unsigned int>>> NewOnes; // <list> of <combinations> of <combined strips>
    
  for (unsigned int listspot = 0; listspot < OldOnes.size(); ++listspot) { // iterate over set of combinations
    // New single merges
    for (unsigned int combi1 = 0; combi1 < OldOnes[listspot].size(); ++combi1) { //iterate over individual combos within set
      for (unsigned int combi2 = combi1+1; combi2 < OldOnes[listspot].size(); ++combi2) { //iterate through all the combos AFTER combi1
        vector<unsigned int> NewCombinedStrips;
        NewCombinedStrips.insert(NewCombinedStrips.end(), OldOnes[listspot][combi1].begin(), OldOnes[listspot][combi1].end());
        NewCombinedStrips.insert(NewCombinedStrips.end(), OldOnes[listspot][combi2].begin(), OldOnes[listspot][combi2].end());
        sort(NewCombinedStrips.begin(), NewCombinedStrips.end());
        
        // the above combines each combination with all the subsequent combinations in order to produce new combinations of strips

        vector<unsigned int> NewCombinedAsIDs;
        for (unsigned int s = 0; s < NewCombinedStrips.size(); ++s) {
          NewCombinedAsIDs.push_back(StripHits[NewCombinedStrips[s]]->GetStripID()); //translates the hit number to the actual strip ID
        }
        sort(NewCombinedAsIDs.begin(), NewCombinedAsIDs.end());
        
        if (RoundTwo == false) {
            bool AllAdjacent = true;
            for (unsigned int c = 1; c < NewCombinedAsIDs.size(); ++c) {
              if (NewCombinedAsIDs[c-1] + 1 != NewCombinedAsIDs[c]) {
                AllAdjacent = false;
                break;
              }
            } //checks if the new combo is of adjacent strips

            if (AllAdjacent == true) {
              vector<vector<unsigned int>> NewCombo;
              for (unsigned int news = 0; news < OldOnes[listspot].size(); ++news) {
                if (news != combi1 && news != combi2) {
                  NewCombo.push_back(OldOnes[listspot][news]);
                }
                if (news == combi1) {
                  NewCombo.push_back(NewCombinedStrips); //Should this be NewCombinedStripsAsIDs????
                }
              }
              NewOnes.push_back(NewCombo);
            }
        } else {
            vector<vector<unsigned int>> NewCombo;
            for (unsigned int news = 0; news < OldOnes[listspot].size(); ++news) {
              if (news != combi1 && news != combi2) {
                NewCombo.push_back(OldOnes[listspot][news]);
              }
              if (news == combi1) {
                NewCombo.push_back(NewCombinedStrips); //Should this be NewCombinedStripsAsIDs????
              }
            }
            NewOnes.push_back(NewCombo);
        }
      }
    }
  }//only keeps the new combos that contain all adjacent strips
    // I don't believe there is any limit on the number of adjacent strips that can exist in a combination (but there is a limit on the number of combos)

  return NewOnes; //return all the new combos containing series of adjacent strips
}


////////////////////////////////////////////////////////////////////////////////


bool MModuleStripPairingGerber::AnalyzeEvent(MReadOutAssembly* Event)
{

  // Main data analysis routine, which updates the event to a new level

  mdebug<<"StripPairing started"<<endl;
  // MARK: NOTE - Limit on number of combos
  unsigned int MaxCombinations = 5; //sets limit on the number of strip combinations. why though?

  if (Event->GetNStripHits() == 0) { //check if there are actually any hits
    Event->SetStripPairingIncomplete(true, "No strip hits");
    Event->SetAnalysisProgress(MAssembly::c_StripPairing);
    return false;
  }

  // (1) Split hits by detector ID
  vector<unsigned int> DetectorIDs;
  vector<vector<vector<MStripHit*>>> StripHits; // list of detector ID, list of sides, list of hits

  for (unsigned int sh = 0; sh < Event->GetNStripHits(); ++sh) {
    MStripHit* SH = Event->GetStripHit(sh);
    unsigned int Side = (SH->IsLowVoltageStrip() == true) ? 0 : 1;

    // Check if detector is on list
    bool DetectorFound = false;
    unsigned int DetectorPos = 0;
    for (unsigned int d = 0; d < DetectorIDs.size(); ++d) {
      if (DetectorIDs[d] == SH->GetDetectorID()) {
        DetectorFound = true;
        DetectorPos = d;
      }
    }

    if (DetectorFound == true) {
      StripHits[DetectorPos][Side].push_back(SH);
    } else {
      vector<vector<MStripHit*>> List; // list of sides, list of hits
      List.push_back(vector<MStripHit*>()); // X
      List.push_back(vector<MStripHit*>()); // Y
      List[Side].push_back(SH);
      StripHits.push_back(List);
      DetectorIDs.push_back(SH->GetDetectorID());
    }
  }

  // Limit the strip hits
  const unsigned int MaxStripHits = 6; //where does this limit come from? --> 6 hits on any one side
  for (unsigned int d = 0; d < StripHits.size(); ++d) { // Detector loop
    for (unsigned int side = 0; side <=1; ++side) { // side loop
      if (StripHits[d][side].size() > MaxStripHits) {
        Event->SetStripPairingIncomplete(true, "More than 6 hit strips on one side");
        Event->SetAnalysisProgress(MAssembly::c_StripPairing);
        return false;
      }
    }
  }


      /*
      cout<<"Strip hits: "<<endl;
      for (unsigned int d = 0; d < StripHits.size(); ++d) { // Detector loop
        cout<<"Detector: "<<d<<endl;
        for (unsigned int side = 0; side <=1; ++side) { // side loop
          cout<<"  Side: "<<side<<endl;
          if (StripHits[d][side].size() > 0) {
            cout<<"    Hits: "<<endl;;
            for (unsigned int sh = 0; sh < StripHits[d][side].size(); ++sh) { // side loop
              cout<<StripHits[d][side][sh]->ToString();
            }
          } else {
            cout<<"no hits"<<endl;
          }
        }
      }
      */

  // (2) Check if we have enough strips and enough energy for each detector
  for (unsigned int d = 0; d < StripHits.size(); ++d) { // Detector loop

    if (StripHits[d][0].size() == 0 || StripHits[d][1].size() == 0) {
      Event->SetStripPairingIncomplete(true, "One detector side has no strip hits");
      Event->SetAnalysisProgress(MAssembly::c_StripPairing);
      return false;
    }

    double xEnergy = 0;
    double xEnergyRes = 0;
    for (unsigned int sh = 0; sh < StripHits[d][0].size(); ++sh) {
      xEnergy += StripHits[d][0][sh]->GetEnergy();
      xEnergyRes += StripHits[d][0][sh]->GetEnergyResolution()*StripHits[d][0][sh]->GetEnergyResolution();
    }
    double yEnergy = 0;
    double yEnergyRes = 0;
    for (unsigned int sh = 0; sh < StripHits[d][1].size(); ++sh) {
      yEnergy += StripHits[d][1][sh]->GetEnergy();
      yEnergyRes += StripHits[d][1][sh]->GetEnergyResolution()*StripHits[d][1][sh]->GetEnergyResolution();
    }

    //cout<<"Energies: "<<xEnergy<<":"<<xEnergyRes<<" -- "<<yEnergy<<":"<<yEnergyRes<<endl;
  }

  // (2.5) Create and populate charge trapping map for energy correction
/*
  vector<array<array<array<double, 2>, 64>, 64>> ChargeTrappingMap; //64 x 64 grid
    for (unsigned int d = 0; d < StripHits.size(); ++d) { // Detector loop
        ChargeTrappingMap.push_back(array<array<array<double, 2>, 64>, 64>());
        for (unsigned int i = 0; i < 64; ++i) {
            for (unsigned int j = 0; j < 64; ++j) {
                ChargeTrappingMap[d][i][j][0] = 0;
                ChargeTrappingMap[d][i][j][1] = 0; //populate with 0s for now
 Have to make sure I'm indexing correctly!!! Do the Strip IDs start at 0???
 Also need to make sure the indexing on the detectors is consistent. Ie, what is detector 0, 1, etc.
            }
        }
    }
  
*/

  // (3) Find all possible combinations
  vector<vector<vector<vector<vector<unsigned int>>>>> Combinations;  // list of detector IDs, list of sides, list of combinations; combination with list of combined strips
  for (unsigned int d = 0; d < StripHits.size(); ++d) { // Detector loop
    Combinations.push_back(vector<vector<vector<vector<unsigned int>>>>()); //create 4D vectors for each detector
    for (unsigned int s = 0; s <= 1 ; ++s) {
      Combinations[d].push_back(vector<vector<vector<unsigned int>>>()); // X create 3D vectors within each detector vector for each side
      Combinations[d].push_back(vector<vector<vector<unsigned int>>>()); // Y
    }
    // Create the seed combinations --> the base case that will be used for the FindNewCombinations function (this is the start of the "OldOnes" vector
    for (unsigned int s = 0; s < StripHits[d].size(); ++s) { //is s just the side??? I think so...
      vector<vector<unsigned int>> Combination;
      for (unsigned int h = 0; h < StripHits[d][s].size(); ++h) {
        vector<unsigned int> CombinedStrips = { h };
        sort(CombinedStrips.begin(), CombinedStrips.end());
        Combination.push_back(CombinedStrips);
      }
      Combinations[d][s].push_back(Combination); //adds single strip "combinations" to a vector
        // so if there are hits 1,2, and 3 on a single side, Combinations[d][s] will have [[1],[2],[3]] (for each side)
    }
  }
    
// so a full set of combinations might be [ [ [1], [2], [3] ] , [ [1,2], [3] ], etc ] hence why you need a 3D vector
    
    int ChiSquareThreshold = 100;
    
  // Starting from this seed, find more new combinations
        for (unsigned int d = 0; d < StripHits.size(); ++d) { // Detector loop
            double BestChiSquare;
            vector<vector<unsigned int>> BestXSideCombo; //list of lists (ie. list of strip combos making up an event on either side)
            vector<vector<unsigned int>> BestYSideCombo;
            
            bool RoundTwo = false;
            //For loop allows for the possibility for a second round, but only if RoundTwo is set to true
            // Which will happen if BestChiSquare is not below a certain threshold (defined above)
            for (unsigned int round = 1; round < 3; ++round) { // round loop
                
                
                if (round == 1 or RoundTwo == true) {
                    for (unsigned int side = 0; side <=1; ++side) { // side loop (LV and HV)
                        
                        vector<vector<vector<unsigned int>>> NewCombinations;
                        
                        bool CombinationsAdded = true;
                        while (CombinationsAdded == true) {
                            CombinationsAdded = false;
                            
                            NewCombinations = FindNewCombinations(Combinations[d][side], StripHits[d][side], RoundTwo);
                            //cout<<"Size: "<<NewCombinations.size()<<endl;
                            
                            // Find equal combinations and eliminate them from the new list
                            for (unsigned int c = 0; c < Combinations[d][side].size(); ++c) {
                                auto Iter = NewCombinations.begin();
                                while (Iter != NewCombinations.end()) {
                                    if (Combinations[d][side][c] == (*Iter)) {
                                        bool Equal = true;
                                        for (unsigned int deep = 0; deep < Combinations[d][side][c].size(); ++deep) {
                                            if (Combinations[d][side][c][deep] != (*Iter)[deep]) {
                                                Equal = false;
                                                break;
                                            }
                                        }
                                        if (Equal == true) {
                                            Iter = NewCombinations.erase(Iter);
                                        } else {
                                            Iter++;
                                        }
                                    } else {
                                        Iter++;
                                    }
                                }
                            }
                            // If there are new combinations left, add them, and restart
                            if (NewCombinations.size() > 0) {
                                //cout<<NewCombinations.size()<<" new combinations found"<<endl;
                                for (auto C: NewCombinations) {
                                    Combinations[d][side].push_back(C);
                                }
                                CombinationsAdded = true; //keep going until no more combos added
                            }
                        } // combination search
                    } // side loop
                    
                    /*
                     cout<<"All combinations:"<<endl;
                     for (unsigned int xc = 0; xc < Combinations[d][0].size(); ++xc) {
                     cout<<"X - "<<xc<<": ";
                     for (unsigned h = 0; h < Combinations[d][0][xc].size(); ++h) {
                     cout<<" (";
                     for (unsigned int sh = 0; sh < Combinations[d][0][xc][h].size(); ++sh) {
                     cout<<Combinations[d][0][xc][h][sh]<<" ";
                     }
                     cout<<")";
                     }
                     cout<<endl;
                     }
                     for (unsigned int yc = 0; yc < Combinations[d][1].size(); ++yc) {
                     cout<<"Y - "<<yc<<": ";
                     for (unsigned h = 0; h < Combinations[d][1][yc].size(); ++h) {
                     cout<<" (";
                     for (unsigned int sh = 0; sh < Combinations[d][1][yc][h].size(); ++sh) {
                     cout<<Combinations[d][1][yc][h][sh]<<" ";
                     }
                     cout<<")";
                     }
                     cout<<endl;
                     }
                     */
                    
                    // (3) Evaluate all combinations
                    // All strip combinations for one side have been found, now check for the best x-y combinations
                    BestChiSquare = numeric_limits<double>::max();
                    //vector<vector<unsigned int>> BestXSideCombo; //list of lists (ie. list of strip combos making up an event on either side)
                    //vector<vector<unsigned int>> BestYSideCombo;
                    
                    for (unsigned int xc = 0; xc < Combinations[d][0].size(); ++xc) { // Loop over combinations of x-strips (xc represents a list of sets of strips,  and each set is a proposed Hit)
                        for (unsigned int yc = 0; yc < Combinations[d][1].size(); ++yc) { // Loop over combinations of y-strips (yc represents a list of sets of strips,  and each set is a proposed Hit)
                            
                            if (abs(long(Combinations[d][0][xc].size()) - long(Combinations[d][1][yc].size())) > 1) { // Skip this pair of combos if the x- and y-strip combos differ in size by more than one
                            // ie. if a certain set has 2 or more combos on one side than the other than it disregards the pair
                          
                            continue;
                        }
                        
                        unsigned int MinSize = min(Combinations[d][0][xc].size(), Combinations[d][1][yc].size());
                        
                        if (max(Combinations[d][0][xc].size(), Combinations[d][1][yc].size()) > MaxCombinations) { // Skip if either side has more than 5 sets of strips
                            continue;
                        }
                        
                        bool MorePermutations = true;
                        while (MorePermutations == true) {
                            //cout<<"New permutation..."<<endl;
                            //         if (Combinations[d][1][yc].size() > Combinations[d][0][xc].size()) {
                            //           PrintCombi(Combinations[d][1][yc]);
                            //         } else {
                            //           PrintCombi(NCombi[p]);
                            //         }
                            double ChiSquare = 0;
                            vector<double> HVtauList;
                            vector<double> LVtauList;
                            vector<double> HVtauResolutionList;
                            vector<double> LVtauResolutionList;
                            double HVtauMeanResolution = 0;
                            double LVtauMeanResolution = 0;
                            double HVtauMean = 0;
                            double LVtauMean = 0;
                            
                            for (unsigned int en = 0; en < MinSize; ++en) {
                                unsigned int ep = en;
                                // MARK: NOTE - The way this is written, doesn't this mean that if there are more sets of strips on one side than the other, they won't be counted in the chi^2? NO --> because of later if statement (line 535)
                                
                                double xEnergy = 0;
                                double xResolution = 0;
                                unsigned int dominantX;
                                double MaxEnergy = -numeric_limits<double>::max();
                                for (unsigned int entry = 0; entry < Combinations[d][0][xc][en].size(); ++entry) { // Sum up energy on xstrips in the set of strips, en (entry is on the strip level itself)

                                    double tempEnergy = StripHits[d][0][Combinations[d][0][xc][en][entry]]->GetEnergy();
                                    if (tempEnergy > MaxEnergy){
                                        dominantX = entry;
                                        MaxEnergy = tempEnergy; //keeps track of max energy on a single strip
                                    }
                                    xEnergy += StripHits[d][0][Combinations[d][0][xc][en][entry]]->GetEnergy();
                                    xResolution += pow(StripHits[d][0][Combinations[d][0][xc][en][entry]]->GetEnergyResolution(), 2);
                                }
                                
                                //repeats for y side
                                double yEnergy = 0;
                                double yResolution = 0;
                                unsigned int dominantY;
                                MaxEnergy = -numeric_limits<double>::max();
                                for (unsigned int entry = 0; entry < Combinations[d][1][yc][ep].size(); ++entry) { // Sum up energy on ystrips in the set of strips, ep
                                    
                                    double tempEnergy = StripHits[d][1][Combinations[d][1][yc][ep][entry]]->GetEnergy();
                                    if (tempEnergy > MaxEnergy){
                                        dominantY = entry;
                                        MaxEnergy = tempEnergy;
                                    }
                                    yEnergy += tempEnergy;
                                    yResolution += pow(StripHits[d][1][Combinations[d][1][yc][ep][entry]]->GetEnergyResolution(), 2);
                                }
                                // Here begins the timing bit, skipping for now.
                                double LVtau = StripHits[d][0][Combinations[d][0][xc][en][dominantX]]->GetTiming();
                                double HVtau = StripHits[d][1][Combinations[d][1][yc][ep][dominantY]]->GetTiming();
                                
                                // Charge trapping correction
                                // correction made based only on the dominant X and Y strip in a grouping of strips (eg neighboring strips)
    //                                        double CTDHVShift = LVtau - HVtau + 200;
    //                                        double slope = ChargeTrappingMap[d][Combinations[d][0][xc][en][dominantX]->GetStripID()][Combinations[d][1][yc][ep][dominantY]->GetStripID()][0];
    //                                        double intercept = ChargeTrappingMap[d][Combinations[d][0][xc][en][dominantX]->GetStripID()][Combinations[d][1][yc][ep][dominantY]->GetStripID()][1];
    //                                        yEnergy /= 1 - (slope*CTDHVShift - intercept)/100;
                                
    //Right now, adding up all the strips in the grouping's Yenergies then applying the CTD correction to the sum... is that correct?
                                
                                HVtauList.push_back(HVtau);
                                LVtauList.push_back(-LVtau);
                                HVtauMean += HVtau;
                                LVtauMean += LVtau;
                                
                                // !!! TODO: Fix timing resolution. Maybe put GetTimingResolution into MStripHit
                                double LVtauResolution = 3*60/StripHits[d][0][Combinations[d][0][xc][en][dominantX]]->GetEnergy();
                                double HVtauResolution = 3*60/StripHits[d][1][Combinations[d][1][yc][ep][dominantY]]->GetEnergy();
                                
                                HVtauResolutionList.push_back(HVtauResolution*HVtauResolution);
                                LVtauResolutionList.push_back(LVtauResolution*LVtauResolution);
                                
                                HVtauMeanResolution += HVtauResolution*HVtauResolution;
                                LVtauMeanResolution += LVtauResolution*LVtauResolution;
                                
                                //cout << "yEnergy: " << yEnergy << endl;
                                //cout << "  Sub - Test en=" << en << " (" << xEnergy << ") with ep="
                                //     << ep << " (" << yEnergy << "):" << endl;
                                //cout<<xResolution<<":"<<yResolution<<endl;
                                ChiSquare += (xEnergy - yEnergy)*(xEnergy - yEnergy) / (xResolution + yResolution); // Chi-squared is determined by how close the energies on either side match
                                // chi^2 is calculated for each individual pairing of x and y strip combos within each individual set of combos
                                // But the chi^2 is then added up over the total combination (ie. each xc/yc pair has a different chi^2)
                            }
                            
                            HVtauMean /= MinSize;
                            LVtauMean /= MinSize;
                            HVtauMeanResolution /= MinSize*MinSize;
                            LVtauMeanResolution /= MinSize*MinSize;
                            
                            vector<size_t> HVTauArgsort = Argsort(HVtauList);
                            vector<size_t> LVTauArgsort = Argsort(LVtauList);
                            bool TimesOrdered = true;
                            // for (unsigned int i=0; i<HVTauArgsort.size(); ++i) {
                            //   if (HVTauArgsort[i]!=LVTauArgsort[i]){
                            //     TimesOrdered = false;
                            //   }
                            // }
                            
                            // Calculate the distance between measurements and properly order lists of drift times --> time ordering!!
                            double HVTimeOrderDistance = 0;
                            double LVTimeOrderDistance = 0;
                            for (unsigned int i=0; i<HVTauArgsort.size(); ++i) {
                                if (HVTauArgsort[i]!=LVTauArgsort[i]){
                                    HVTimeOrderDistance += (HVtauList[HVTauArgsort[i]] - HVtauList[LVTauArgsort[i]])*(HVtauList[HVTauArgsort[i]] - HVtauList[LVTauArgsort[i]])/(HVtauResolutionList[HVTauArgsort[i]] + HVtauResolutionList[LVTauArgsort[i]]);
                                    LVTimeOrderDistance += (LVtauList[HVTauArgsort[i]] - LVtauList[LVTauArgsort[i]])*(LVtauList[HVTauArgsort[i]] - LVtauList[LVTauArgsort[i]])/(LVtauResolutionList[HVTauArgsort[i]] + LVtauResolutionList[LVTauArgsort[i]]);
                                }
                            }
                            
                            // if ((HVTimeOrderDistance < LVTimeOrderDistance) && (HVTimeOrderDistance > 50)) {
                            //   TimesOrdered = false;
                            // }
                            // else if ((LVTimeOrderDistance < HVTimeOrderDistance) && (LVTimeOrderDistance > 50)) {
                            //   TimesOrdered = false;
                            // }
                            
                            // !!! TODO: Do we need to take into account difference in hole and electron drift times?
                            // for( unsigned int h=0; h < HVtauList.size(); ++h ){
                            //   ChiSquare += ((HVtauList[h] - HVtauMean) + (LVtauList[h] - LVtauMean)) * ((HVtauList[h] - HVtauMean) + (LVtauList[h] - LVtauMean))/(HVtauMeanResolution + LVtauMeanResolution + HVtauResolutionList[h] + LVtauResolutionList[h]);
                            // }
                            
                            ChiSquare /= MinSize; // Chi-squared is normalized by the number of sets of strips in the smaller of the two combos xc and yc
                            // But doesn't this favor large combo sets? YES because larger combo sets correspond to less charge sharing pairs of strips (ie a simpler configuration)!
                            //cout<<"Chi square: "<<ChiSquare<<endl;
                            
                            if (ChiSquare < BestChiSquare) {
                                BestChiSquare = ChiSquare;
                                BestXSideCombo = Combinations[d][0][xc]; //best combo is chosen. But then how are the strips paired within each combo set?
                                BestYSideCombo = Combinations[d][1][yc];
                            }
                            
                            //cout<<"ChiSquare: "<<ChiSquare<<endl;
                            
                            if (Combinations[d][1][yc].size() > Combinations[d][0][xc].size()) {
                                MorePermutations = next_permutation(Combinations[d][1][yc].begin(), Combinations[d][1][yc].end());
                            } else {
                                MorePermutations = next_permutation(Combinations[d][0][xc].begin(), Combinations[d][0][xc].end());
                            }
                        }
                    }
                }
                
                if (BestChiSquare > ChiSquareThreshold) {
                    RoundTwo = true;
                } }
            }
                    /*
                     cout<<"Best combo:"<<endl;
                     cout<<"X: ";
                     for (unsigned h = 0; h < BestXSideCombo.size(); ++h) {
                     cout<<" (";
                     for (unsigned int sh = 0; sh < BestXSideCombo[h].size(); ++sh) {
                     cout<<StripHits[d][0][BestXSideCombo[h][sh]]->GetStripID()<<" ";
                     }
                     cout<<")";
                     }
                     cout<<endl;
                     cout<<"Y: ";
                     for (unsigned h = 0; h < BestYSideCombo.size(); ++h) {
                     cout<<" (";
                     for (unsigned int sh = 0; sh < BestYSideCombo[h].size(); ++sh) {
                     cout<<StripHits[d][1][BestYSideCombo[h][sh]]->GetStripID()<<" ";
                     }
                     cout<<")";
                     }
                     cout<<endl;
                     */
                    
                    
                    
                    // Now create hits:
                    if (BestChiSquare == numeric_limits<double>::max()) { //check if chi^2 was actually updated at all
                        Event->SetStripPairingIncomplete(true, "Pairing did not find a single match");
                        Event->SetAnalysisProgress(MAssembly::c_StripPairing);
                        return false;
                    }
            
                    Event->SetRedChiSquare(BestChiSquare);
                    
                    // Create the hits
                    double XPos = 0;
                    double YPos = 0;
                    double XEnergy = 0;
                    double XEnergyRes = 0;
                    double YEnergy = 0;
                    double YEnergyRes = 0;
                    double Energy = 0;
                    double EnergyResolution = 0;
                    
                    double XEnergyTotal = 0;
                    double YEnergyTotal = 0;
                    double EnergyTotal = 0;
                    double XEnergyResTotal = 0;
                    double YEnergyResTotal = 0;
            
                    double LVTau;
                    double HVTau;
            
                    
                    
                    // Create a list for plotting X and Y energies
                    vector<double> XEnergies;
                    vector<double> YEnergies;
                    
                    // But how do you know that the XSide and YSide Combos are ordered in exactly the right way such that h=0 on either side corresponds to the right pairing???
                    // This is done on the xc level: i.e. the set of combinations [ [1], [2] ] is not equal to [ [2], [1] ] which makes sure that the chi^2 picks out the correct pairing
    
                    for (unsigned int h = 0; h < min(BestXSideCombo.size(), BestYSideCombo.size()); ++h) {
                        
                        //Find dominant strip in each grouping
                        double MaxEnergy = -numeric_limits<double>::max();
                        double tempEnergy;
                        int dominantX;
                        for (unsigned int sh = 0; sh < BestXSideCombo[h].size(); ++sh) {
                            tempEnergy = StripHits[d][0][BestXSideCombo[h][sh]]->GetEnergy();
                            if (tempEnergy > MaxEnergy) {
                                MaxEnergy = tempEnergy;
                                dominantX = sh;
                            }
                        }
                        
                        MaxEnergy = -numeric_limits<double>::max();
                        int dominantY;
                        for (unsigned int sh = 0; sh < BestYSideCombo[h].size(); ++sh) {
                            double tempEnergy = StripHits[d][1][BestYSideCombo[h][sh]]->GetEnergy();
                            if (tempEnergy > MaxEnergy) {
                                MaxEnergy = tempEnergy;
                                dominantY = sh;
                            }
                        }
                        
                        bool AllAdjacentX = true;
                        bool AllAdjacentY = true;
                        bool AllAdjacent = true;
                        
                        XPos = 0;
                        YPos = 0;
                        XEnergy = 0;
                        YEnergy = 0;
                        XEnergyRes = 0;
                        YEnergyRes = 0;
                        
                        //Check if there are any non-adjacent strip groupings
                        for (unsigned int sh = 0; sh < BestXSideCombo[h].size() - 1; ++sh) {
                            if (StripHits[d][0][BestXSideCombo[h][sh]]->GetStripID() + 1 != StripHits[d][0][BestXSideCombo[h][sh+1]]->GetStripID()) {
                                AllAdjacentX = false;
                                AllAdjacent = false;
                                break;
                            }
                                
                        }
                        
                        for (unsigned int sh = 0; sh < BestYSideCombo[h].size() - 1; ++sh) {
                            if (StripHits[d][1][BestYSideCombo[h][sh]]->GetStripID() + 1 != StripHits[d][1][BestYSideCombo[h][sh+1]]->GetStripID()) {
                                AllAdjacentY = false;
                                AllAdjacent = false;
                                break;
                            }
                                
                        }
                        //If no non-adjacent strip groupings, continue pairing as normal
                        if (AllAdjacent) {
                            
                            for (unsigned int sh = 0; sh < BestXSideCombo[h].size(); ++sh) {
                                //cout<<"x-pos: "<<StripHits[d][0][BestXSideCombo[h][sh]]->GetNonStripPosition()<<endl;
                                XEnergy += StripHits[d][0][BestXSideCombo[h][sh]]->GetEnergy();
                                XEnergyRes += StripHits[d][0][BestXSideCombo[h][sh]]->GetEnergyResolution()*StripHits[d][0][BestXSideCombo[h][sh]]->GetEnergyResolution();
                            }
                            XEnergyResTotal += XEnergyRes;
                            XEnergyRes = sqrt(XEnergyRes);
                            XEnergyTotal += XEnergy;
                            
                            for (unsigned int sh = 0; sh < BestYSideCombo[h].size(); ++sh) {
                                YEnergy += StripHits[d][1][BestYSideCombo[h][sh]]->GetEnergy();
                                YEnergyRes += StripHits[d][1][BestYSideCombo[h][sh]]->GetEnergyResolution()*StripHits[d][1][BestYSideCombo[h][sh]]->GetEnergyResolution();
                            }
                            
                            LVTau = StripHits[d][0][BestXSideCombo[h][dominantX]]->GetTiming();
                            HVTau = StripHits[d][1][BestYSideCombo[h][dominantY]]->GetTiming();
                            
                            // Charge trapping correction --> only being applied to events with all adjacent strips (though maybe it should be applied to all since that's how it's going through the chi^2 calculation)
                            // correction made based only on the dominant X and Y strip in a grouping of strips (eg neighboring strips)
//                                        double CTDHVShift = LVtau - HVtau + 200;
//                                        double slope = ChargeTrappingMap[d][BestXSideCombo[h][dominantX]->GetStripID()][BestYSideCombo[h][dominantY]->GetStripID()][0];
//                                        double intercept = ChargeTrappingMap[d][BestXSideCombo[h][dominantX]->GetStripID()][BestYSideCombo[h][dominantY]->GetStripID()][1];
//                                        yEnergy /= 1 - (slope*CTDHVShift - intercept)/100;
                            
//Right now, adding up all the strips in the grouping's Yenergies then applying the CTD correction to the sum... is that correct?
                            
                            YEnergyResTotal += YEnergyRes;
                            YEnergyRes = sqrt(YEnergyRes);
                            YEnergyTotal += YEnergy;
                            
                            Energy = 0.0;
                            if (XEnergy > YEnergy + 3*YEnergyRes) {
                                Energy = XEnergy;
                                EnergyResolution = XEnergyRes;
                            } else if (YEnergy > XEnergy + 3*XEnergyRes) {
                                Energy = YEnergy;
                                EnergyResolution = YEnergyRes;
                            } else {
                                Energy = (XEnergy/(XEnergyRes*XEnergyRes) + YEnergy/(YEnergyRes*YEnergyRes)) / (1.0/(XEnergyRes*XEnergyRes) + 1.0/(YEnergyRes*YEnergyRes));
                                EnergyResolution = sqrt( 1.0 / (1.0/(XEnergyRes*XEnergyRes) + 1.0/(YEnergyRes*YEnergyRes)) );
                            }
                            EnergyTotal += Energy;
                            
                            XEnergies.push_back(XEnergy);
                            YEnergies.push_back(YEnergy);
                            
                            MHit* Hit = new MHit();
                            Hit->SetEnergy(Energy);
                            Hit->SetXEnergy(XEnergy);
                            Hit->SetYEnergy(YEnergy);
                            Hit->SetEnergyResolution(EnergyResolution);
                            Event->AddHit(Hit);
                            for (unsigned int sh = 0; sh < BestXSideCombo[h].size(); ++sh) {
                                Hit->AddStripHit(StripHits[d][0][BestXSideCombo[h][sh]]);
                                //cout<<"Paired Strip:"<<endl;
                                //cout<<"StripID: "<<StripHits[d][0][BestXSideCombo[h][sh]]->GetStripID()<<endl;
                                
                            }
                            for (unsigned int sh = 0; sh < BestYSideCombo[h].size(); ++sh) {
                                Hit->AddStripHit(StripHits[d][1][BestYSideCombo[h][sh]]);
                                
                            }
                            
                        }
                        //If there are non-adjacent strip groupings, then have to separate them out again to form multiple (physical) hits
                        if (AllAdjacentY == false && AllAdjacentX == true ) {
                            //cout<<"Multiple hits on single LV strip"<<endl;
                            bool MultipleHitsOnX = true;
                            
                            //Collect timing info for X strip based on dominantX strip found above
                            LVTau = StripHits[d][0][BestXSideCombo[h][dominantX]]->GetTiming();
                            
                            Event->SetMultipleHitsOnXStrip(MultipleHitsOnX);
                            for (unsigned int sh = 0; sh < BestYSideCombo[h].size(); ++sh) {
                                Energy = StripHits[d][1][BestYSideCombo[h][sh]]->GetEnergy();
                                EnergyTotal += Energy;
                                EnergyResolution = StripHits[d][1][BestYSideCombo[h][sh]]->GetEnergyResolution();
                                XEnergyRes = EnergyResolution;
                                YEnergyRes = EnergyResolution;
                                XEnergyResTotal += XEnergyRes * XEnergyRes; // add in quadrature, square root later
                                YEnergyResTotal += YEnergyRes * YEnergyRes;
                                MHit* Hit = new MHit();
                                Hit->SetEnergy(Energy);
                                Hit->SetEnergyResolution(EnergyResolution);
                                Event->AddHit(Hit);
                                Hit->AddStripHit(StripHits[d][1][BestYSideCombo[h][sh]]);
                                YEnergy = Energy;
                                XEnergy = Energy;
                                Hit->SetXEnergy(XEnergy);
                                Hit->SetYEnergy(YEnergy);
                                YEnergyTotal += YEnergy;
                                XEnergyTotal += XEnergy;
                                XEnergies.push_back(XEnergy);
                                YEnergies.push_back(YEnergy);
                                
                                //Collect timing info for Y strip
                             //   HVTau = StripHits[d][1][BestYSideCombo[h][sh]]->GetTiming();
                                
                                //Calculate CTD and record it to file
                             //   double CTD = LVTau = HVTau;
                             //   std::ofstream outFile("/Users/juliangerber/Desktop/Research/StripPairing/ChiSquareTesting/CTD.txt", std::ios::app);
                            //    outFile << CTD << endl;
                           //     outFile.close();
                                
                                for (unsigned int sh = 0; sh < BestXSideCombo[h].size(); ++sh) {
                                    Hit->AddStripHit(StripHits[d][0][BestXSideCombo[h][sh]]);
                                    }
                                }
                                
                            }
                        //And again for the other side...
                        else if (AllAdjacentX == false && AllAdjacentY == true) {
                           // cout<<"Multiple hits on single HV strip"<<endl;
                            bool MultipleHitsOnY = true;
                            Event->SetMultipleHitsOnYStrip(MultipleHitsOnY);
                            
                            //Collect timing info for Y strip based on dominantY strip found above
                            HVTau = StripHits[d][1][BestYSideCombo[h][dominantY]]->GetTiming();
                            
                            for (unsigned int sh = 0; sh < BestXSideCombo[h].size(); ++sh) {
                                Energy = StripHits[d][0][BestXSideCombo[h][sh]]->GetEnergy();
                                EnergyTotal += Energy;
                                EnergyResolution = StripHits[d][0][BestXSideCombo[h][sh]]->GetEnergyResolution();
                                XEnergyRes = EnergyResolution;
                                YEnergyRes = EnergyResolution;
                                XEnergyResTotal += XEnergyRes * XEnergyRes;
                                YEnergyResTotal += YEnergyRes * YEnergyRes;
                                MHit* Hit = new MHit();
                                Hit->SetEnergy(Energy);
                                Hit->SetEnergyResolution(EnergyResolution);
                                Event->AddHit(Hit);
                                Hit->AddStripHit(StripHits[d][0][BestXSideCombo[h][sh]]);
                                YEnergy = Energy;
                                XEnergy = Energy;
                                Hit->SetXEnergy(XEnergy);
                                Hit->SetYEnergy(YEnergy);
                                YEnergyTotal += YEnergy;
                                XEnergyTotal += XEnergy;
                                XEnergies.push_back(XEnergy);
                                YEnergies.push_back(YEnergy);
                                
                                //Collect timing info for X strip
                                LVTau = StripHits[d][0][BestXSideCombo[h][sh]]->GetTiming();
                                
                                //Calculate CTD and record it to file
                                double CTD = LVTau = HVTau;
                                std::ofstream outFile("/Users/juliangerber/Desktop/Research/StripPairing/ChiSquareTesting/CTD.txt", std::ios::app);
                                outFile << CTD << endl;
                                outFile.close();
                                
                                for (unsigned int sh = 0; sh < BestYSideCombo[h].size(); ++sh) {
                                    Hit->AddStripHit(StripHits[d][1][BestYSideCombo[h][sh]]);
                                    }
                                }
                        }
                        //If both HV and LV have multiple hits per strip, can't pair
                        else if (AllAdjacentX == false and AllAdjacentY == false) {
                            Event->SetStripPairingIncomplete(true, "Strips not pairable. Multiple hits per strip on LV and HV sides");
                            Event->SetAnalysisProgress(MAssembly::c_StripPairing);
                            return false;
                        }
                        
                        
                        // if (HasExpos() == true) {
                        //   m_ExpoStripPairing->AddEnergies(XEnergy, YEnergy);
                        // }
                        
                     //   MHit* Hit = new MHit();
                     //   Hit->SetEnergy(Energy);
                     //   Hit->SetEnergyResolution(EnergyResolution);
                     //   Event->AddHit(Hit);
                     //   for (unsigned int sh = 0; sh < BestXSideCombo[h].size(); ++sh) {
                     //       Hit->AddStripHit(StripHits[d][0][BestXSideCombo[h][sh]]);
                     //   }
                      //  for (unsigned int sh = 0; sh < BestYSideCombo[h].size(); ++sh) {
                      //      Hit->AddStripHit(StripHits[d][1][BestYSideCombo[h][sh]]);
                      //  }
                    }
                    XEnergyResTotal = sqrt(XEnergyResTotal);
                    YEnergyResTotal = sqrt(YEnergyResTotal);
                    
                    
                    if ((EnergyTotal > max(XEnergyTotal, YEnergyTotal) + 2.5*max(XEnergyResTotal, YEnergyResTotal) || EnergyTotal < min(XEnergyTotal, YEnergyTotal) - 2.5*max(XEnergyResTotal, YEnergyResTotal))) {
                        Event->SetStripPairingIncomplete(true, "Strips not pairable wihin 2.5 sigma of measured energy");
                        Event->SetAnalysisProgress(MAssembly::c_StripPairing);
                        return false;
                    }
                    else if (HasExpos() == true){
                        m_ExpoStripPairingHits->AddHits(Event->GetNHits());
                        for (unsigned int i = 0; i < XEnergies.size(); ++i){
                            m_ExpoStripPairing->AddEnergies(XEnergies[i], YEnergies[i]);
                        }
                        
                        for (unsigned int h = 0; h<Event->GetNHits(); h++){
                            double HVStrips = 0;
                            double LVStrips = 0;
                            for (unsigned int sh=0; sh<Event->GetHit(h)->GetNStripHits(); sh++){
                                if (Event->GetHit(h)->GetStripHit(sh)->IsLowVoltageStrip()==true){
                                    LVStrips++;
                                }
                                else{
                                    HVStrips++;
                                }
                            }
                            m_ExpoStripPairingStripHits->AddStripHits(LVStrips, HVStrips);
                        }
                    }
//std::ofstream outFile("/Users/juliangerber/Desktop/Research/StripPairing/ChiSquareTesting/XYEnergiesData.txt", std::ios::app);
//for (unsigned int i = 0; i < XEnergies.size(); ++i){
//    outFile << Event->GetID() << " "<<XEnergies[i]<<" "<<YEnergies[i] <<endl;
//      }
//outFile.close();

                    //
  
  } // detector loop

  Event->SetAnalysisProgress(MAssembly::c_StripPairing);
    
  //Print Strip Pairing Output
  /*
    for (unsigned int h = 0; h<Event->GetNHits(); h++){
        double PrintEnergy = Event->GetHit(h)->GetEnergy();
        vector<int> LVStripsPrint;
        vector<int> HVStripsPrint;
        for (unsigned int sh=0; sh<Event->GetHit(h)->GetNStripHits(); sh++){
            if (Event->GetHit(h)->GetStripHit(sh)->IsLowVoltageStrip()==true){
                LVStripsPrint.push_back(Event->GetHit(h)->GetStripHit(sh)->GetStripID());
            } else {
                HVStripsPrint.push_back(Event->GetHit(h)->GetStripHit(sh)->GetStripID());
            }
        }
        cout<<"Strip Paired Hit: "<<h<<endl;
        cout<<"Energy: "<<PrintEnergy<<endl;
        cout<<"LV Strip(s): ";
        for (unsigned int sh=0; sh<LVStripsPrint.size(); sh++){
            cout<<LVStripsPrint[sh]<<" ";
        }
        cout<<endl;
        cout<<"HV Strip(s): ";
        for (unsigned int sh=0; sh<HVStripsPrint.size(); sh++){
            cout<<HVStripsPrint[sh];
        }
        cout<<endl;
    }
   */
  return true;
}


////////////////////////////////////////////////////////////////////////////////


vector<size_t> MModuleStripPairingGerber::Argsort(vector<double> &list)
{
  // Return the order of indices resulting from list sorting
  // initialize original index locations
  vector<size_t> idx(list.size());
  iota(idx.begin(), idx.end(), 0);

  // sort indexes based on comparing values in v
  // using std::stable_sort instead of std::sort
  // to avoid unnecessary index re-orderings
  // when v contains elements of equal values
  stable_sort(idx.begin(), idx.end(),
       [&list](size_t i1, size_t i2) {return list[i1] < list[i2];});

  return idx;
}


////////////////////////////////////////////////////////////////////////////////


void MModuleStripPairingGerber::Finalize()
{
  // Finalize the analysis - do all cleanup, i.e., undo Initialize()

  MModule::Finalize();
}


////////////////////////////////////////////////////////////////////////////////


void MModuleStripPairingGerber::ShowOptionsGUI()
{
  //! Show the options GUI --- has to be overwritten!

  MGUIOptionsStripPairing* Options = new MGUIOptionsStripPairing(this);
  Options->Create();
  gClient->WaitForUnmap(Options);
}


////////////////////////////////////////////////////////////////////////////////


bool MModuleStripPairingGerber::ReadXmlConfiguration(MXmlNode* Node)
{
  //! Read the configuration data from an XML node

  /*
  MXmlNode* SomeTagNode = Node->GetNode("SomeTag");
  if (SomeTagNode != 0) {
    m_SomeTagValue = SomeTagNode->GetValue();
  }
  */

  return true;
}


////////////////////////////////////////////////////////////////////////////////


MXmlNode* MModuleStripPairingGerber::CreateXmlConfiguration()
{
  //! Create an XML node tree from the configuration

  MXmlNode* Node = new MXmlNode(0, m_XmlTag);
  
  /*
  MXmlNode* SomeTagNode = new MXmlNode(Node, "SomeTag", "SomeValue");
  */

  return Node;
}


// MModuleStripPairingGerber.cxx: the end...
////////////////////////////////////////////////////////////////////////////////
