#include "SiPMMessenger.hh"

#include "SiPM.hh"

#include <G4UIcmdWith3VectorAndUnit.hh>
#include <G4UIdirectory.hh>
#include <G4UnitsTable.hh>

SiPMMessenger::SiPMMessenger(SiPM* sipm) : mySiPM(sipm) {
  Dir = new G4UIdirectory("/sipm/");
  Dir->SetGuidance("UI commands for SiPM.");

  detDir = new G4UIdirectory("/sipm/");
  detDir->SetGuidance("Crystal Geometry.");

  sizeCmd = new G4UIcmdWith3VectorAndUnit("/sipm/size", this);
  sizeCmd->SetGuidance("Set size of the sipm in x,y,z");
  sizeCmd->SetUnitCategory("Length");
  sizeCmd->SetParameterName("sizeX", "sizeY", "sizeZ", false);
  sizeCmd->AvailableForStates(G4State_PreInit, G4State_Idle);
}

SiPMMessenger::~SiPMMessenger() {
  delete sizeCmd;
  delete detDir;
  delete Dir;
}

void SiPMMessenger::SetNewValue(G4UIcommand* command, G4String newValue) {
  if (command == sizeCmd) {
    G4ThreeVector dimensions = sizeCmd->GetNew3VectorValue(newValue);
    mySiPM->SetDimensions(dimensions);

    G4cout << ">>>>> SiPM size is now " << G4BestUnit(dimensions.x(), "Length")
           << " x " << G4BestUnit(dimensions.y(), "Length") << " x "
           << G4BestUnit(dimensions.z(), "Length") << " x "
           << "(X,Y,Z) <<<<<" << G4endl;
  }
}
