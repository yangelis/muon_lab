#include "SiPMHit.hh"

#include <G4AttCheck.hh>
#include <G4AttDef.hh>
#include <G4AttValue.hh>
#include <G4Circle.hh>
#include <G4Colour.hh>
#include <G4UnitsTable.hh>
#include <G4VVisManager.hh>
#include <G4VisAttributes.hh>

G4ThreadLocal G4Allocator<SiPMHit>* SiPMHitAllocator = nullptr;

std::map<G4String, G4AttDef> SiPMHit::fAttDefs;

SiPMHit::SiPMHit() {}

SiPMHit::~SiPMHit() {}

SiPMHit::SiPMHit(const SiPMHit& right) : G4VHit() {
  pos        = right.pos;
  time       = right.time;
  wavelength = right.wavelength;
}

const SiPMHit& SiPMHit::operator=(const SiPMHit& right) {
  pos        = right.pos;
  time       = right.time;
  wavelength = right.wavelength;
  return *this;
}

G4bool SiPMHit::operator==(const SiPMHit& right) const {
  return (this == &right) ? 1 : 0;
}

void SiPMHit::Draw() {
  G4VVisManager* pVVisManager = G4VVisManager::GetConcreteInstance();
  if (pVVisManager) {
    G4Circle circle(pos);
    circle.SetScreenSize(4.);
    circle.SetFillStyle(G4Circle::filled);
    G4Colour colour(1., 0., 0.);
    G4VisAttributes attribs(colour);
    circle.SetVisAttributes(attribs);
    pVVisManager->Draw(circle);
  }
}

const std::map<G4String, G4AttDef>* SiPMHit::GetAttDefs() const {
  if (fAttDefs.empty()) {
    fAttDefs["HitType"] =
        G4AttDef("HitType", "Type of hit", "Physics", "", "G4String");
  }
  return &fAttDefs;
}

std::vector<G4AttValue>* SiPMHit::CreateAttValues() const {
  std::vector<G4AttValue>* attValues = new std::vector<G4AttValue>;
  attValues->emplace_back("HistType", "SiPMHit", "");
  return attValues;
}

void SiPMHit::Print() {}
