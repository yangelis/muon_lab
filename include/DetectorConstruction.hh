#ifndef DETECTORCONSTRUCTION_H_
#define DETECTORCONSTRUCTION_H_

#include <G4Material.hh>
#include <G4OpticalSurface.hh>
#include <G4VUserDetectorConstruction.hh>
#include <globals.hh>

class G4VPhysicalVolume;

/// Detector construction class to define materials and geometry.

class DetectorConstruction : public G4VUserDetectorConstruction {
public:
  DetectorConstruction();
  virtual ~DetectorConstruction();

  virtual G4VPhysicalVolume* Construct();
  virtual void ConstructSDandField(); // construct Sensitive Detector

private:
  // methods
  void DefineMaterials();
  G4VPhysicalVolume* DefineVolumes();

  // data members
  G4bool fCheckOverlaps;
};

#endif // DETECTORCONSTRUCTION_H_
