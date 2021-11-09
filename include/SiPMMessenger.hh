#ifndef SIPMMESSENGER_H_
#define SIPMMESSENGER_H_

#include <G4UImessenger.hh>
#include <globals.hh>

class SiPM;

class G4UIdirectory;
class G4UIcmdWith3VectorAndUnit;

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
};

#endif // SIPMMESSENGER_H_
