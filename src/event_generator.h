#ifndef PW_EVENT_GENERATOR_H
#define PW_EVENT_GENERATOR_H

// std/stl
#include <vector>

#include <random>

struct Event {
    double w;
    double w2;
    std::vector<bool> trigger_mask;
};

class EventGen {
    public :
        EventGen();
        ~EventGen() = default;

        double w();
        std::vector<bool> trigger_mask();

    private :
        std::default_random_engine _rng;
        std::normal_distribution<double> _w_dist;
        std::uniform_real_distribution<float> _trig_dist;
};

struct Lepton {
    float pt;
    float eta;
    float phi;
    unsigned flavor;
    std::vector<bool> trigger_match_mask;
    bool isLoose;
    bool isMedium;
    bool isTight;
};

class LeptonGen {
    public :
        LeptonGen(float max_pt);
        ~LeptonGen() = default;

        int nLeptons();
        float pt();
        float eta();
        float phi();
        unsigned flavor();
        std::vector<bool> trigger_match_mask();
        bool isLoose();
        bool isMedium();
        bool isTight();

    private :
        float _max_pt;

        std::default_random_engine _rng;
        std::uniform_int_distribution<int> _lep_eff_dist;
        std::uniform_real_distribution<float> _pt_dist;
        std::uniform_real_distribution<float> _eta_dist;
        std::uniform_real_distribution<float> _phi_dist;
        std::uniform_real_distribution<float> _trig_dist;
};

struct Jet {
    float pt;
    float eta;
    float phi;
    float mass;
    float truthHadronPt;
    unsigned truthHadronId;
    unsigned nTrk;
    bool isBJet;
    float bTagScore;
};

class JetGen {
    public :
        JetGen(float max_pt, int max_n);
        ~JetGen() = default;

        int nJets();
        float pt();
        float eta();
        float phi();
        float mass();
        unsigned truthHadronId();
        unsigned nTrk();
        float bTagScore();

    private :
        float _max_pt;
        int _max_n;

        std::default_random_engine _rng;
        std::uniform_int_distribution<int> _jet_eff_dist;
        std::uniform_real_distribution<float> _pt_dist;
        std::uniform_real_distribution<float> _eta_dist;
        std::uniform_real_distribution<float> _phi_dist;
        std::uniform_real_distribution<float> _trig_dist;
        std::uniform_int_distribution<int> _jet_ntrk_dist;

};

struct Met {
    float sumEt;
    float met;
    float phi;
    float electronTerm;
    float muonTerm;
    float jetTerm;
    float softTerm;
};

class MetGen {
    public :
        MetGen(float max_pt);
        ~MetGen() = default;

        float sumEt();
        float met();
        float phi();
    private :
        float _max_pt;
        std::default_random_engine _rng;
        std::uniform_real_distribution<float> _pt_dist;
        std::uniform_real_distribution<float> _phi_dist;
};

class EventGenerator {
    public :
        EventGenerator(float max_pt, unsigned max_lep, unsigned max_jet);
        ~EventGenerator() = default;

        Event event();
        std::vector<Lepton> leptons();
        std::vector<Jet> jets();
        Met met();

    private :

        std::shared_ptr<EventGen> _event_gen;
        std::shared_ptr<LeptonGen> _lepton_gen;
        std::shared_ptr<JetGen> _jet_gen;
        std::shared_ptr<MetGen> _met_gen;

        unsigned _max_nleptons;
        unsigned _max_njets;
        float _max_pt;
};


#endif
