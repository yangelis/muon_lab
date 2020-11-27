#ifndef PRIMARYGENERATORACTION_H_
#define PRIMARYGENERATORACTION_H_

#include <G4VUserPrimaryGeneratorAction.hh>
#include <globals.hh>

class G4GeneralParticleSource;
class G4Event;

/// The primary generator action class with particle gun.
class PrimaryGeneratorAction : public G4VUserPrimaryGeneratorAction {
public:
  PrimaryGeneratorAction();
  virtual ~PrimaryGeneratorAction();

  // method from the base class
  virtual void GeneratePrimaries(G4Event* event);

private:
  G4GeneralParticleSource* fParticleGun;
};

#endif // PRIMARYGENERATORACTION_H_
