#ifndef DETECTORCONSTRUCTION_H_
#define DETECTORCONSTRUCTION_H_

#include "DetectorMessenger.hh"
#include "ScintillatorSD.hh"
#include "SiPMSD.hh"

#include <G4Cache.hh>
#include <G4Material.hh>
#include <G4OpticalSurface.hh>
#include <G4VUserDetectorConstruction.hh>
#include <globals.hh>

class G4MultiFunctionalDetector;
class G4VPhysicalVolume;
class G4LogicalVolume;
class G4Box;
class SiPM;
class SiPMMessenger;

/// Detector construction class to define materials and geometry.

class DetectorConstruction : public G4VUserDetectorConstruction {
public:
  DetectorConstruction();
  virtual ~DetectorConstruction();

  virtual G4VPhysicalVolume* Construct();
  virtual void ConstructSDandField(); // construct Sensitive Detector

  G4double GetWidth0x() { return fscintX; }
  void SetWidth0X(G4double myWidhtX);

private:
  // methods
  void DefineMaterials();
  G4VPhysicalVolume* DefineVolumes();

  DetectorMessenger* fDetectorMessenger;
  SiPMMessenger* fSiPMMessenger;

  // data members
  G4bool fCheckOverlaps;
  G4double fscintX;

  G4Box* fsolidWorld;
  G4LogicalVolume* flogicWorld;
  G4VPhysicalVolume* fphysWorld;

  G4Box* fsolidScintillator;
  G4LogicalVolume* flogicScintillator0;
  G4VPhysicalVolume* fphysScintillator0;
  G4LogicalVolume* flogicScintillator2;
  G4VPhysicalVolume* fphysScintillator2;

  G4Box* fsolidSmallScintillator;
  G4LogicalVolume* flogicScintillator1;
  G4VPhysicalVolume* fphysScintillator1;

  G4Box* fsolidAbsorber;
  G4LogicalVolume* flogicAbsorber;
  G4VPhysicalVolume* fphysAbsorber;

  // Sensitive Detector
  G4Cache<ScintillatorSD*> fscintSD;
  G4Cache<SiPMSD*> fsipmSD;

  G4Cache<G4MultiFunctionalDetector*> fscint0MFDet;
  G4Cache<G4MultiFunctionalDetector*> fscint1MFDet;
  G4Cache<G4MultiFunctionalDetector*> fscint2MFDet;
  G4Cache<G4MultiFunctionalDetector*> fsipm0MFDet;
  /* G4Cache<G4MultiFunctionalDetector*> fsipm1MFDet; */
  G4Cache<G4MultiFunctionalDetector*> fsipm2MFDet;

  SiPM* sipm0;
  SiPM* sipm1;
  SiPM* sipm2;
};

#endif // DETECTORCONSTRUCTION_H_
