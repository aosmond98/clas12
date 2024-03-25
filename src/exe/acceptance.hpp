
// #ifndef HIST_H_GUARD
// #define HIST_H_GUARD
// #include "TCanvas.h"
// #include "TF1.h"
// #include "TFile.h"
// #include "TGraph.h"
// #include "TH1.h"
// #include "TH2.h"
// #include "THnSparse.h"
// #include "TTree.h"
// #include "TBranch.h"
// #include <TChain.h>
// #include <TSystem.h>
// #include "TLegend.h"
// #include "TLorentzVector.h"
// #include "TPaveStats.h"
// #include "TROOT.h"
// #include "TStyle.h"
// #include "TThread.h"
// #include "colors.hpp"
// #include "constants.hpp"
// #include "cuts.hpp"
// #include "deltat.hpp"
// #include "reaction.hpp"
// #include <mutex>

// using namespace std;

// using TH2D_ptr = std::shared_ptr<TH2D>;
// using TH1D_ptr = std::shared_ptr<TH1D>;
// using THnSparse_ptr = std::shared_ptr<THnSparse>;
// using TGraph_ptr = std::shared_ptr<TGraph>;

// class Histogram
// {
// protected:
//     std::shared_ptr<TFile> RootOutputFile;
//     std::shared_ptr<TCanvas> def;

//     std::unique_ptr<TCanvas> WvsQ2_sec_rec_can;
//     std::unique_ptr<TCanvas> WvsQ2_sec_gen_can;

//     static const short num_sectors = 6;
//     std::string sec_name[num_sectors] = {"1", "2", "3", "4", "5", "6"};
//     TH2D *WvsQ2_Acceptance_sec[num_sectors]; // Array to store acceptance sector histograms

//     // TH2D_ptr acceptance_sec[num_sectors];


// public:
//     Histogram(const std::string &output_file);
//     // ~Histogram();

//     // Acceptance sectors
//     void Fill_Acceptance_sec(const std::shared_ptr<Reaction> &_e);
//     void Write_Acceptance_sec();

//     void SetWvsQ2_Acceptance_sec(TH2D *hist);
//     void Divide_WvsQ2_Sector_Histograms();
    
//     // void Write();
// };

// #endif
