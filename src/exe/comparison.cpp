#include <iostream>
#include <memory>
#include <TFile.h>
#include <TH2D.h>
#include <TCanvas.h>

int main(int argc, char** argv) {
    if (argc < 4) {
        std::cerr << "Usage: ./comparison <exp_filename.root> <sim_filename.root> <output_filename.root>" << std::endl;
        return 1;
    }

    const char* expFileName = argv[1];
    const char* simFileName = argv[2];
    const char* outputFileName = argv[3];

    TFile *sim_file = new TFile(simFileName, "READ");
    TFile *exp_file = new TFile(expFileName, "READ");

    // Clone histograms
    TH2D *vx_vs_vy_exp = dynamic_cast<TH2D*>(exp_file->Get("W vs Q2/vx_vs_vy")->Clone("vx_vs_vy"));
    TH2D *vx_vs_vy_sim_corrected = dynamic_cast<TH2D*>(sim_file->Get("W vs Q2/corr_vx_vs_vy")->Clone("corr_vx_vs_vy"));

    // Check if histograms are valid
    if (!vx_vs_vy_sim_corrected || !vx_vs_vy_exp) {
        std::cerr << "Error: Unable to clone histograms." << std::endl;
        return 1;
    }

    // Create comparison histograms
    TH2D *comparison_hist = dynamic_cast<TH2D*>(vx_vs_vy_exp->Clone("comparison_hist"));
    comparison_hist->Divide(vx_vs_vy_sim_corrected);

    // Save to a new ROOT file
    TFile *outputFile = new TFile(outputFileName, "RECREATE");

    // // Save to a new ROOT file
    // TFile *outputFile = new TFile("acceptance_output.root", "RECREATE");
    comparison_hist->SetTitle("Comparison of vx vs vy (exp / sim_corr)");
    comparison_hist->SetOption("COLZ");
    comparison_hist->Write();
    // outputFile->Close();

    // Check if the output file is created successfully
    if (!outputFile || outputFile->IsZombie()) {
        std::cerr << "Error: Unable to create the output ROOT file." << std::endl;
        return 1;
    }

    // Histogram *histogram = new Histogram("output_file.root");

    // histogram->SetWvsQ2_Acceptance_sec(WvsQ2_rec_can);
    // histogram->Divide_WvsQ2_Sector_Histograms();
    // histogram->Write_Acceptance_sec();

    // Close the ROOT file
    sim_file->Close();
    exp_file->Close();

    return 0;
}

// #include <iostream>
// #include <memory>
// #include <TFile.h>
// #include <TH2D.h>

// int main(int argc, char** argv) {
//     if (argc < 4) {
//         std::cerr << "Usage: ./comparison <exp_filename.root> <sim_filename.root> <output_filename.root>" << std::endl;
//         return 1;
//     }

//     const char* expFileName = argv[1];
//     const char* simFileName = argv[2];
//     const char* outFileName = argv[3];

//     // Open experimental file
//     std::unique_ptr<TFile> exp_file(TFile::Open(expFileName, "READ"));
//     if (!exp_file || exp_file->IsZombie()) {
//         std::cerr << "Error: Could not open experimental file " << expFileName << std::endl;
//         return 1;
//     }

//     // Open simulation file
//     std::unique_ptr<TFile> sim_file(TFile::Open(simFileName, "READ"));
//     if (!sim_file || sim_file->IsZombie()) {
//         std::cerr << "Error: Could not open simulation file " << simFileName << std::endl;
//         return 1;
//     }

//     // Retrieve histograms
//     TH2D* vx_vs_vy_exp = dynamic_cast<TH2D*>(exp_file->Get("W vs Q2/vx_vs_vy"));
//     TH2D* vx_vs_vy_sim_corrected = dynamic_cast<TH2D*>(sim_file->Get("W vs Q2/corr_vx_vs_vy"));

//     if (!vx_vs_vy_exp || !vx_vs_vy_sim_corrected) {
//         std::cerr << "Error: Unable to retrieve histograms from files." << std::endl;
//         return 1;
//     }

//     // Clone the experimental histogram
//     std::unique_ptr<TH2D> comparison_hist(dynamic_cast<TH2D*>(vx_vs_vy_exp->Clone("comparison_hist")));
//     comparison_hist->Divide(vx_vs_vy_sim_corrected);

//     // Open output file
//     std::unique_ptr<TFile> outputFile(TFile::Open(outFileName, "RECREATE"));
//     if (!outputFile || outputFile->IsZombie()) {
//         std::cerr << "Error: Unable to create the output ROOT file." << std::endl;
//         return 1;
//     }

//     // Write the comparison histogram to the output file
//     comparison_hist->SetTitle("Comparison of vx vs vy (exp / sim_corr)");
//     comparison_hist->SetOption("COLZ");
//     comparison_hist->Write();
//     outputFile->Close();

//     return 0;
// }
