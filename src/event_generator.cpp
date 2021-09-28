#include "event_generator.h"

EventGen::EventGen() {

  _w_dist = std::normal_distribution<double>(1.0, 0.3);
  _trig_dist = std::uniform_real_distribution<float>(0, 1.0);
}

double EventGen::w() { return _w_dist(_rng); }

std::vector<bool> EventGen::trigger_mask() {
  std::vector<bool> out;
  for (size_t i = 0; i < 15; i++) {
    float decision = _trig_dist(_rng);
    out.push_back(decision >= 0.5);
  }
  return out;
}

LeptonGen::LeptonGen(float max_pt) : _max_pt(max_pt) {
  _lep_eff_dist = std::uniform_int_distribution<int>(0, 3);
  _pt_dist = std::uniform_real_distribution<float>(0., _max_pt);
  _eta_dist = std::uniform_real_distribution<float>(-2.7, 2.7);
  _phi_dist = std::uniform_real_distribution<float>(-3.14, 3.14);
  _trig_dist = std::uniform_real_distribution<float>(0., 1.0);
}

int LeptonGen::nLeptons() { return _lep_eff_dist(_rng); }

float LeptonGen::pt() { return _pt_dist(_rng); }

float LeptonGen::eta() { return _eta_dist(_rng); }

float LeptonGen::phi() { return _phi_dist(_rng); }

unsigned LeptonGen::flavor() { return _lep_eff_dist(_rng); }

std::vector<bool> LeptonGen::trigger_match_mask() {
  std::vector<bool> out;
  for (size_t i = 0; i < 7; i++) {
    float decision = _trig_dist(_rng);
    out.push_back(decision >= 0.5);
  }
  return out;
}

bool LeptonGen::isLoose() { return _trig_dist(_rng) > 0.2; }

bool LeptonGen::isMedium() { return _trig_dist(_rng) > 0.5; }

bool LeptonGen::isTight() { return _trig_dist(_rng) > 0.8; }

JetGen::JetGen(float max_pt, int max_n) : _max_pt(max_pt), _max_n(max_n) {
  _jet_eff_dist = std::uniform_int_distribution<int>(0, _max_n);
  _pt_dist = std::uniform_real_distribution<float>(0., _max_pt);
  _eta_dist = std::uniform_real_distribution<float>(-2.7, 2.7);
  _phi_dist = std::uniform_real_distribution<float>(-3.14, 3.14);
  _trig_dist = std::uniform_real_distribution<float>(0., 1.0);
  _jet_ntrk_dist = std::uniform_int_distribution<int>(0, 30);
}

int JetGen::nJets() { return _jet_eff_dist(_rng); }

float JetGen::pt() { return _pt_dist(_rng); }

float JetGen::eta() { return _eta_dist(_rng); }

float JetGen::phi() { return _phi_dist(_rng); }

float JetGen::mass() { return _pt_dist(_rng) * 15.; }

unsigned JetGen::truthHadronId() { return _jet_eff_dist(_rng); }

unsigned JetGen::nTrk() { return _jet_ntrk_dist(_rng); }

float JetGen::bTagScore() { return _trig_dist(_rng) * 100.; }

MetGen::MetGen(float max_pt) : _max_pt(max_pt) {
  _pt_dist = std::uniform_real_distribution<float>(0, _max_pt);
  _phi_dist = std::uniform_real_distribution<float>(-3.14, 3.14);
}

float MetGen::sumEt() { return _pt_dist(_rng); }

float MetGen::met() { return _pt_dist(_rng); }

float MetGen::phi() { return _phi_dist(_rng); }

EventGenerator::EventGenerator(float max_pt, unsigned max_nlep,
                               unsigned max_njet)
    : _max_pt(max_pt), _max_nleptons(max_nlep), _max_njets(max_njet) {
  _event_gen = std::make_shared<EventGen>();
  _lepton_gen = std::make_shared<LeptonGen>(max_pt);
  _jet_gen = std::make_shared<JetGen>(max_pt, max_njet);
  _met_gen = std::make_shared<MetGen>(max_pt);
}

Event EventGenerator::event() {

  Event out;
  out.w = _event_gen->w();
  out.w2 = out.w * out.w;
  out.trigger_mask = _event_gen->trigger_mask();
  return out;
}

std::vector<Lepton> EventGenerator::leptons() {
  std::vector<Lepton> out;
  unsigned n = _lepton_gen->nLeptons();
  for (size_t i = 0; i < n; i++) {
    Lepton lep;
    lep.pt = _lepton_gen->pt();
    lep.eta = _lepton_gen->eta();
    lep.phi = _lepton_gen->phi();
    lep.flavor = _lepton_gen->flavor();
    lep.trigger_match_mask = _lepton_gen->trigger_match_mask();
    lep.isLoose = _lepton_gen->isLoose();
    lep.isMedium = _lepton_gen->isMedium();
    lep.isTight = _lepton_gen->isTight();
    out.push_back(lep);
  }
  return out;
}

std::vector<Jet> EventGenerator::jets() {
  std::vector<Jet> out;
  unsigned n = _jet_gen->nJets();
  for (size_t i = 0; i < n; i++) {
    Jet jet;
    jet.pt = _jet_gen->pt();
    jet.eta = _jet_gen->eta();
    jet.phi = _jet_gen->phi();
    jet.mass = _jet_gen->mass();
    jet.truthHadronPt = _event_gen->w() * jet.pt;
    jet.truthHadronId = _jet_gen->truthHadronId();
    jet.nTrk = _jet_gen->nTrk();
    jet.bTagScore = _jet_gen->bTagScore();
    jet.isBJet = jet.bTagScore > 0.7;
    out.push_back(jet);
  }
  return out;
}

Met EventGenerator::met() {
  Met out;
  out.sumEt = _met_gen->sumEt();
  out.met = _met_gen->met();
  out.phi = _met_gen->phi();
  out.electronTerm = 0.2 * out.sumEt;
  out.muonTerm = 0.05 * out.sumEt;
  out.jetTerm = 0.7 * out.sumEt;
  out.softTerm = 0.05 * out.sumEt;
  return out;
}
