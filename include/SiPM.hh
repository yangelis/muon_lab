#ifndef SIPM_H_
#define SIPM_H_

#include "SiPMMessenger.hh"
#include "SiPMSD.hh"

#include <G4Box.hh>
#include <G4Cache.hh>
#include <G4LogicalVolume.hh>
#include <G4Material.hh>
#include <G4PVPlacement.hh>

class SiPMMessenger;

class SiPM {
public:
  SiPM(G4RotationMatrix* pRot, const G4ThreeVector& tlate,
       G4LogicalVolume* pMother, G4bool pMany, G4int pCopyNo, G4bool pSurfChk,
       const G4String& LVname, const G4String& PVname);

  ~SiPM();

  void SetTranslation(const G4ThreeVector& translate);
  void SetDimensions(G4double sipmX, G4double sipmY, G4double sipmZ);
  void SetDimensions(const G4ThreeVector& dimensions);
  G4ThreeVector GetDimensions();
  SiPMSD* GetSD() { return fsipmSD.Get(); };
  G4LogicalVolume* GetLogVol() { return sipm_log; };

private:
  void BuildSurface();
  void BuildSi();
  void BuildSiO2();

  G4double coatingWidth;
  G4String log_name;
  G4String phys_name;

  ///sipm
  G4Box* sipm_box;
  G4LogicalVolume* sipm_log;
  G4PVPlacement* sipm_phys;

  ///coating
  G4Box* coating_box;
  G4LogicalVolume* coating_log;
  G4PVPlacement* coating_phys;

  ///materials
  G4Material* Si;
  G4Material* SiO2;

  ///sensitive detector
  G4Cache<SiPMSD*> fsipmSD;

  ///messenger
  SiPMMessenger* sipmMessenger;
};
#endif // SIPM_H_
