
#ifndef HIST_H_GUARD
#define HIST_H_GUARD
#include "TCanvas.h"
#include "TF1.h"
#include "TFile.h"
#include "TGraph.h"
#include "TH1.h"
#include "TH2.h"
#include "THnSparse.h"
#include "TTree.h"
#include "TBranch.h"
#include <TChain.h>
#include <TSystem.h>
#include "TLegend.h"
#include "TLorentzVector.h"
#include "TPaveStats.h"
#include "TROOT.h"
#include "TStyle.h"
#include "TThread.h"
#include "colors.hpp"
#include "constants.hpp"
#include "cuts.hpp"
#include "deltat.hpp"
#include "reaction.hpp"
#include <mutex>
#include <TDirectory.h>
#include <sstream> // added this for w folders in mm2 w bins
#include <iomanip> // Include the <iomanip> header for std::setprecision

using namespace std;

using TH2D_ptr = std::shared_ptr<TH2D>;
using TH1D_ptr = std::shared_ptr<TH1D>;
using THnSparse_ptr = std::shared_ptr<THnSparse>;
using TGraph_ptr = std::shared_ptr<TGraph>;

class Histogram
{
protected:
    std::shared_ptr<TFile> RootOutputFile;
    std::shared_ptr<TCanvas> def;

    int bins = 500;
    double p_min = 0.0;
    double p_max = 20.0;
    double Dt_max = 10.0;
    double Dt_min = -Dt_max;
    double q2_max = 24.0;
    double w_max = 5.0;
    double zero = 0.0;

    // number of W and Q^2 bins
    static constexpr int w_nBins = 25;  
    static constexpr int q2_nBins = 17;

    // // number of bins for W and Q^2
    // int w_nBins = sizeof(w_bin_ranges) / sizeof(w_bin_ranges[0]) - 1;
    // int q2_nBins = sizeof(q2_bin_ranges) / sizeof(q2_bin_ranges[0]) - 1;

    // bin ranges for W and Q^2
    double w_bin_ranges[25] = {1.40000, 1.42500, 1.45000, 1.47500, 1.50000, 1.52500, 1.55000, 1.57500, 1.60000, 1.62500, 1.65000, 1.67500, 
                                1.70000, 1.72500, 1.75000, 1.77500, 1.80000, 1.82500, 1.85000, 1.87500, 1.90000, 1.92500, 1.95000, 
                                1.97500, 2.00000};
    double q2_bin_ranges[17] = {2.0, 2.4, 3.0, 3.5, 4.2, 5.0, 6.0, 7.0, 8.0, 9.0, 11.0, 13.0, 15.0, 18.0, 
                                21.0, 25.0, 30.0};

    static const short particle_num = 4; // 0-e 1-Pi 2-P 3-K
    std::string particle_name[particle_num] = {"e", "pi", "P", "K"};
    static const short charge_num = 2; // 0-pos 1-neg
    std::string charge_name[charge_num] = {"positive", "negative"};
    static const short with_id_num = 3; // 0-without 1-with 2-anti
    std::string id_name[with_id_num] = {"withoutID", "withID", "antiID"};
    static const short num_sectors = 6;
    std::string sec_name[num_sectors] = {"1", "2", "3", "4", "5", "6"};

    static const short CUTS = 2;
    enum cuts
    {
        before_cut,
        after_cut
    };
    std::mutex mutex;

    TH1D_ptr momentum;
    TH2D_ptr Mom_vs_Q2_hist[particle_num][charge_num][with_id_num];

    TH1D_ptr W;
    TH1D_ptr Q2;
    TH2D_ptr WvsQ2;

    TH1D_ptr MM2;
    TH2D_ptr W_vs_MM2;

    TH1D_ptr W_mc;
    TH1D_ptr Q2_mc;
    TH2D_ptr WvsQ2_mc;
    // TH1D_ptr MM2_mc;
    // TH2D_ptr W_vs_MM2_mc;

    TH1D_ptr W_sec[num_sectors];
    TH2D_ptr WvsQ2_sec[num_sectors];
    TH1D_ptr MM2_sec[num_sectors];
    TH2D_ptr W_vs_MM2_sec[num_sectors];

    TH1D_ptr W_mc_sec[num_sectors];
    TH2D_ptr WvsQ2_mc_sec[num_sectors];
    // TH1D_ptr MM2_mc_sec[num_sectors];
    // TH2D_ptr W_vs_MM2_mc_sec[num_sectors];

    std::vector<std::vector<TH1D_ptr>> MM2_hists;

    // TH1D_ptr W_det[3];
    // TH2D_ptr WQ2_det[3];

    // Mom vs Beta
    TH2D_ptr momvsbeta_hist[particle_num][charge_num][with_id_num];

    // Delta T
    TH2D_ptr delta_t_hist[particle_num][charge_num][with_id_num][2];

public:
    Histogram(const std::string &output_file);
    ~Histogram();

    // sectors
    void makeHists_sector();

    // W vs Q2
    void makeHists_WvsQ2();
    void Fill_WvsQ2(const std::shared_ptr<Reaction> &_e);
    void Fill_WvsQ2_mc(const std::shared_ptr<MCReaction> &_e);
    void Write_WvsQ2();

    // W vs MM2
    void makeHists_MM2();
    void Fill_MM2(const std::shared_ptr<Reaction> &_e);
    // void Fill_WvsMM2_mc(const std::shared_ptr<MCReaction> &_e);
    void Write_MM2();

    // MM2 with bins
    void makeHists_MM2withbins();
    void Fill_MM2withbins(const std::shared_ptr<Reaction> &_e);
    void Write_MM2withbins(TDirectory *Write_MM2_withbins_folder);

    // Mom vs Beta
    void makeHists_MomVsBeta();
    void Fill_MomVsBeta(const std::shared_ptr<Branches12> &data, int part, const std::shared_ptr<Reaction> &_e);
    void Write_MomVsBeta();

    // Mom vs Q2
    void makeHists_MomVsQ2();
    void Fill_MomVsQ2(const std::shared_ptr<Branches12> &data, int part, const std::shared_ptr<Reaction> &_e);
    void Write_MomVsQ2();

    // Delta T
    void makeHists_deltat();
    void Fill_deltat_pi(const std::shared_ptr<Branches12> &data,
                        const std::shared_ptr<Delta_T> &dt, int part, const std::shared_ptr<Reaction> &_e);
    void Fill_deltat_prot(const std::shared_ptr<Branches12> &data,
                          const std::shared_ptr<Delta_T> &dt, int part, const std::shared_ptr<Reaction> &_e);
    void Write_deltat(TDirectory *ctof_folder, TDirectory *ftof_folder, TDirectory *Write_deltat_folder);
    
    void Write();
};

#endif