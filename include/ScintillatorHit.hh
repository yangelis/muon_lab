#ifndef SCINTILLATORHIT_H
#define SCINTILLATORHIT_H

#include <G4Allocator.hh>
#include <G4THitsCollection.hh>
#include <G4ThreeVector.hh>
#include <G4Types.hh>
#include <G4VHit.hh>

class G4AttDef;

class ScintillatorHit : public G4VHit {
public:
  ScintillatorHit();

  virtual ~ScintillatorHit();

  ScintillatorHit(const ScintillatorHit& rhs);

  // operators
  const ScintillatorHit& operator=(const ScintillatorHit& rhs);
  G4bool operator==(const ScintillatorHit& rhs) const;
  inline void* operator new(size_t);
  inline void operator delete(void* aHit);

  virtual const std::map<G4String, G4AttDef>* GetAttDefs() const;
  virtual std::vector<G4AttValue>* CreateAttValues() const;
  virtual void Print();

  // Setters and Getters
  inline void SetEdep(G4double ed) { fEdep = ed; }
  inline G4double GetEdep() { return fEdep; }

  inline void SetE(G4double en) { fEnergy = en; }
  inline G4double GetE() { return fEnergy; }

  inline void SetPos(G4ThreeVector vec3) { fPos = vec3; }
  inline G4ThreeVector GetPos() { return fPos; }

  inline void SetTime(G4double ti) { fTime = ti; }
  inline G4double GetTime() { return fTime; }

  inline void SetParentId(G4int parentID) { fParentID = parentID; }
  inline G4int GetParentId() { return fParentID; }

  inline void SetTrId(G4double tid) { fTrID = tid; }
  inline G4double GetTrId() { return fTrID; }

  inline void SetTrLen(G4double tl) { fTrLen = tl; }
  inline G4double GetTrLen() { return fTrLen; }

  inline void SetLocalTime(G4double ti) { fLocalTime = ti; }
  inline G4double GetLocalTime() { return fLocalTime; }

  inline void SetParticleName(const G4String& par_name) { fParName = par_name; }
  inline G4String GetParticleName() { return fParName; }

  inline void SetScintName(const G4String& scint_name) {
    fScintName = scint_name;
  }
  inline G4String GetScintName() { return fScintName; }

private:
  G4double fEdep;      // energy deposition
  G4double fEnergy;    // Particle energy
  G4ThreeVector fPos;  // Position Vec3
  G4double fTime;      // time
  G4int fParentID;     // Particle ID
  G4int fTrID;         // Track ID
  G4double fTrLen;     // Track Length
  G4double fLocalTime; // LocalTIme
  G4String fParName;   // Particle Name
  G4String fScintName; // Scintillator Name

  static std::map<G4String, G4AttDef> fAttDefs;
};

using ScintillatorHitsCollection = G4THitsCollection<ScintillatorHit>;

extern G4ThreadLocal G4Allocator<ScintillatorHit>* ScintillatorHitAllocator;

inline void* ScintillatorHit::operator new(size_t) {
  if (!ScintillatorHitAllocator)
    ScintillatorHitAllocator = new G4Allocator<ScintillatorHit>;
  return (void*)ScintillatorHitAllocator->MallocSingle();
}

inline void ScintillatorHit::operator delete(void* aHit) {
  ScintillatorHitAllocator->FreeSingle((ScintillatorHit*)aHit);
}

#endif // SCINTILLATORHIT_H_
