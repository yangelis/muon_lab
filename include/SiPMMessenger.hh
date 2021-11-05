#ifndef SIPMMESSENGER_H_
#define SIPMMESSENGER_H_

#include <G4UImessenger.hh>
#include <globals.hh>

/* #include "SiPM.hh" */

class SiPM;

class G4UIdirectory;
class G4UIcmdWithADouble;
class G4UIcmdWith3VectorAndUnit;
class G4UIcmdWith3Vector;
class G4UIcmdWithABool;
class G4UIcmdWithAString;

class SiPMMessenger : public G4UImessenger {
public:
  SiPMMessenger(SiPM*);
  virtual ~SiPMMessenger();

  void SetNewValue(G4UIcommand*, G4String);

private:
  SiPM* mySiPM;

  G4UIdirectory* Dir;
  G4UIdirectory* detDir;

  G4UIcmdWith3VectorAndUnit* sizeCmd;

  G4UIcmdWithADouble* pdeCmd;
  G4UIcmdWithADouble* gainCmd;
  G4UIcmdWithADouble* tdrCmd;
  G4UIcmdWithADouble* apsCmd;
  G4UIcmdWithADouble* tapsCmd;
  G4UIcmdWithADouble* apfCmd;
  G4UIcmdWithADouble* tapfCmd;
  G4UIcmdWithADouble* xtCmd;
  G4UIcmdWithADouble* enfCmd;
  G4UIcmdWithADouble* enCmd;
  G4UIcmdWithADouble* npixCmd;
  G4UIcmdWithADouble* recoveryCmd;
  G4UIcmdWith3Vector* shapeCmd;
  G4UIcmdWithADouble* signalAmplitudeCmd;
  G4UIcmdWithADouble* signalNoiseCmd;
  G4UIcmdWithADouble* gateCmd;
  G4UIcmdWithADouble* pregateCmd;
  G4UIcmdWithABool* writePhotonsCmd;
  G4UIcmdWithABool* writeGossipCmd;
  G4UIcmdWithAString* filePhotonsCmd;
  G4UIcmdWithAString* fileGossipCmd;
  G4UIcmdWithAString* parafileCmd;
};

#endif // SIPMMESSENGER_H_
