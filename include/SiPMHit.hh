#ifndef SIPMHIT_H_
#define SIPMHIT_H_

#include <G4Allocator.hh>
#include <G4THitsCollection.hh>
#include <G4ThreeVector.hh>
#include <G4Types.hh>
#include <G4VHit.hh>

class G4AttDef;

class SiPMHit : public G4VHit {
public:
  SiPMHit();
  virtual ~SiPMHit();

  SiPMHit(const SiPMHit& rhs);

  const SiPMHit& operator=(const SiPMHit& rhs);
  G4bool operator==(const SiPMHit& rhs) const;

  inline void* operator new(size_t);
  inline void operator delete(void* aHit);

  virtual const std::map<G4String, G4AttDef>* GetAttDefs() const;
  virtual std::vector<G4AttValue>* CreateAttValues() const;
  virtual void Print();

  virtual void Draw();

  void SetPos(const G4ThreeVector& xyz) { pos = xyz; };
  void SetTime(G4double t) { time = t; };
  void SetWavelength(G4double wl) { wavelength = wl; };

  G4ThreeVector GetPos() { return pos; };
  G4double GetTime() { return time; };
  G4double GetWavelength() { return wavelength; };

private:
  G4ThreeVector pos;   ///photon position on sipm
  G4double time;       ///photon arrival time on sipm
  G4double wavelength; ///photon wavelength

  static std::map<G4String, G4AttDef> fAttDefs;
};

using SiPMHitsCollection = G4THitsCollection<SiPMHit>;

extern G4ThreadLocal G4Allocator<SiPMHit>* SiPMHitAllocator;

inline void* SiPMHit::operator new(size_t) {
  if (!SiPMHitAllocator)
    SiPMHitAllocator = new G4Allocator<SiPMHit>;
  return (void*)SiPMHitAllocator->MallocSingle();
}

inline void SiPMHit::operator delete(void* aHit) {
  SiPMHitAllocator->FreeSingle((SiPMHit*)aHit);
}

#endif // SIPMHIT_H_
