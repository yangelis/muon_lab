#include "ScintillatorHit.hh"

#include <G4AttCheck.hh>
#include <G4AttDef.hh>
#include <G4AttValue.hh>
#include <G4Circle.hh>
#include <G4Colour.hh>
#include <G4UnitsTable.hh>
#include <G4VVisManager.hh>
#include <G4VisAttributes.hh>

G4ThreadLocal G4Allocator<ScintillatorHit>* ScintillatorHitAllocator = nullptr;

std::map<G4String, G4AttDef> ScintillatorHit::fAttDefs;

ScintillatorHit::ScintillatorHit()
    : G4VHit(), fEdep(0.), fPos(), fTime(), fParentID(), fTrID(), fTrLen(),
      fLocalTime() {}

ScintillatorHit::~ScintillatorHit() {}

ScintillatorHit::ScintillatorHit(const ScintillatorHit& rhs) : G4VHit() {
  fEdep      = rhs.fEdep;
  fPos       = rhs.fPos;
  fTime      = rhs.fTime;
  fParentID  = rhs.fParentID;
  fTrID      = rhs.fTrID;
  fTrLen     = rhs.fTrLen;
  fLocalTime = rhs.fLocalTime;
  fParName   = rhs.fParName;
  fScintName = rhs.fScintName;
}

const ScintillatorHit& ScintillatorHit::operator=(const ScintillatorHit& rhs) {
  fEdep      = rhs.fEdep;
  fPos       = rhs.fPos;
  fTime      = rhs.fTime;
  fParentID  = rhs.fParentID;
  fTrID      = rhs.fTrID;
  fTrLen     = rhs.fTrLen;
  fLocalTime = rhs.fLocalTime;
  fParName   = rhs.fParName;
  fScintName = rhs.fScintName;
  return *this;
}

G4bool ScintillatorHit::operator==(const ScintillatorHit& rhs) const {
  return (this == &rhs) ? true : false;
}

const std::map<G4String, G4AttDef>* ScintillatorHit::GetAttDefs() const {
  if (fAttDefs.empty()) {
    fAttDefs["HitType"] =
        G4AttDef("HitType", "Type of hit", "Physics", "", "G4String");
  }
  return &fAttDefs;
}

std::vector<G4AttValue>* ScintillatorHit::CreateAttValues() const {
  std::vector<G4AttValue>* attValues = new std::vector<G4AttValue>;
  attValues->emplace_back("HistType", "ScintillatorHit", "");
  return attValues;
}

void ScintillatorHit::Print() {}
