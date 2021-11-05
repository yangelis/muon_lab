#ifndef DETECTORMESSENGER_H_
#define DETECTORMESSENGER_H_

#include <G4UImessenger.hh>
#include <globals.hh>

class DetectorConstruction;

class G4UIdirectory;
class G4UIcmdWithADouble;

class DetectorMessenger : public G4UImessenger {

public:
  DetectorMessenger(DetectorConstruction*);
  virtual ~DetectorMessenger();

  void SetNewValue(G4UIcommand*, G4String);

private:
  DetectorConstruction* fdetectorConstruction;

  G4UIdirectory* fDetDir;
  G4UIcmdWithADouble* fScintillator0WidthX;
};

#endif // DETECTORMESSENGER_H_
