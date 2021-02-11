#include "DetectorMessenger.hh"

#include "DetectorConstruction.hh"

#include <G4RunManager.hh>
#include <G4UIcmdWithADouble.hh>
#include <G4UIdirectory.hh>

DetectorMessenger::DetectorMessenger(DetectorConstruction* detCon)
    : fdetectorConstruction(detCon) {

  fDetDir              = new G4UIdirectory("/det/");
  fScintillator0WidthX = new G4UIcmdWithADouble("/det/Scintillator", this);
  fScintillator0WidthX->SetGuidance("Select width X of first scintillator.");
  fScintillator0WidthX->SetParameterName("widthX", false);
  fScintillator0WidthX->AvailableForStates(G4State_PreInit, G4State_Idle);
}

DetectorMessenger::~DetectorMessenger() {
  delete fScintillator0WidthX;
  delete fDetDir;
}

void DetectorMessenger::SetNewValue(G4UIcommand* command, G4String newValue) {
  if (command == fScintillator0WidthX) {
    fdetectorConstruction->SetWidth0X(
        fScintillator0WidthX->GetNewDoubleValue(newValue));
  }
}
