#include <iostream>
#include <memory>
#include <TFile.h>
#include <TH2D.h>
#include <TCanvas.h>
// #include "acceptance.hpp"

// Histogram::Histogram(const std::string &output_file);
// {
//     WvsQ2_sec_rec_can = std::make_unique<TCanvas>("WvsQ2_sec_rec_can", "W vs Q2 rec sectors", 1920, 1080);
//     WvsQ2_sec_rec_can->Divide(3, 2);

//     WvsQ2_sec_gen_can = std::make_unique<TCanvas>("WvsQ2_sec_gen_can", "W vs Q2 gen sectors", 1920, 1080);
//     WvsQ2_sec_gen_can->Divide(3, 2);
// }

// void Histogram::Fill_Acceptance_sec(const std::shared_ptr<Reaction> &_e)
// {
//     short sec = _e->sec();
//     if (sec > 0 && sec <= num_sectors)
//     {
//         Acceptance_sec[sec - 1]->Fill_WvsQ2_Acceptance_sec(_e);
//     }
// }

// void Histogram::Write_Acceptance_sec()
// {
//     auto acceptance_can =
//         std::make_unique<TCanvas>("Acceptance_can", "Acceptance sectors", 1920, 1080);
//     acceptance_can->Divide(3, 2);

//     for (short i = 0; i < num_sectors; i++)
//     {
//         acceptance_sec[i]->SetYTitle("Q^{2} (GeV^{2})");
//         acceptance_sec[i]->SetXTitle("W (GeV)");
//         acceptance_sec[i]->SetOption("COLZ1");
//         acceptance_can->cd(i + 1);
//         gPad->SetLogz();
//         acceptance_sec[i]->Draw("same");
//     }

//     acceptance_can->Write();
// }

// void Histogram::SetWvsQ2_Acceptance_sec(TH2D *hist)
// {
//     for (short i = 0; i < num_sectors; i++)
//     {
//         WvsQ2_Acceptance_sec[i] = dynamic_cast<TH2D *>(hist->Clone(Form("WvsQ2_Acceptance_sec_%d", i + 1)));
//         // Additional setup for WvsQ2_Acceptance_sec[i] if needed
//     }
// }

// void Histogram::Divide_WvsQ2_Sector_Histograms()
// {
//     for (short i = 0; i < num_sectors; i++)
//     {
//         WvsQ2_Acceptance_sec[i]->Divide(WvsQ2_sec_rec[i], WvsQ2_sec_gen[i], 1.0, 1.0);
//         // Additional setup for the divided histograms if needed
//     }
// }

int main(int argc, char** argv) {
    if (argc < 3) {
        std::cerr << "Usage: ./acceptance <output_filename.root> <input_root_file_pattern>" << std::endl;
        return 1;
    }
// int main() {
    // Open the ROOT file
    TFile *WvsQ2 = new TFile("10_2sim_excl_jlab_02_01_2024.root", "READ");

    // Clone histograms
    TH2D *WvsQ2_gen = dynamic_cast<TH2D*>(WvsQ2->Get("W vs Q2/WvsQ2_gen")->Clone("WvsQ2_gen"));
    TH2D *WvsQ2_rec = dynamic_cast<TH2D*>(WvsQ2->Get("W vs Q2/WvsQ2_rec")->Clone("WvsQ2_rec"));

    // TCanvas *WvsQ2_gen_can = dynamic_cast<TCanvas*>(WvsQ2->Get("W vs Q2/WvsQ2_gen_can")->Clone("WvsQ2_gen_can"));
    // TCanvas *WvsQ2_rec_can = dynamic_cast<TCanvas*>(WvsQ2->Get("W vs Q2/WvsQ2_rec_can")->Clone("WvsQ2_rec_can"));

    // Check if histograms are valid
    if (!WvsQ2_gen || !WvsQ2_rec) {
        std::cerr << "Error: Unable to clone histograms." << std::endl;
        return 1;
    }

    // if (!WvsQ2_gen_can || !WvsQ2_rec_can) {
    //     std::cerr << "Error: Unable to clone sector histograms." << std::endl;
    //     return 1;
    // }

    // Create acceptance histograms
    TH2D *acceptance_hist = dynamic_cast<TH2D*>(WvsQ2_rec->Clone("acceptance_hist"));
    acceptance_hist->Divide(WvsQ2_gen);

    // TH2D *acceptance_sec = dynamic_cast<TH2D*>(WvsQ2_rec_can->Clone("acceptance_sec"));
    // acceptance_sec->Divide(WvsQ2_gen_can);

    // change for log scale z
    // auto acceptance_can = std::make_unique<TCanvas>("acceptance_can", "Acceptance", 1920, 1080);

    // Save to a new ROOT file
    TFile *outputFile = new TFile(argv[1], "RECREATE");

    // // Save to a new ROOT file
    // TFile *outputFile = new TFile("acceptance_output.root", "RECREATE");
    acceptance_hist->SetTitle("Acceptance");
    acceptance_hist->SetOption("COLZ");
    acceptance_hist->Write();
    outputFile->Close();

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
    WvsQ2->Close();

    return 0;
}


// #include <iostream>
// #include <memory>
// #include <TFile.h>
// #include <TH2D.h>

// int main(int argc, char** argv) {
//     if (argc < 3) {
//         std::cerr << "Usage: ./acceptance <output_filename.root> <input_root_file_pattern>" << std::endl;
//         return 1;
//     }

//     // Open the ROOT file
//     TFile *WvsQ2 = new TFile(argv[2], "READ");

//     // Check if the file is opened successfully
//     if (!WvsQ2 || WvsQ2->IsZombie()) {
//         std::cerr << "Error: Unable to open the input ROOT file." << std::endl;
//         return 1;
//     }

//     // Clone histograms
//     TH2D *WvsQ2_gen = dynamic_cast<TH2D*>(WvsQ2->Get("W vs Q2/WvsQ2_gen")->Clone("WvsQ2_gen"));
//     TH2D *WvsQ2_rec = dynamic_cast<TH2D*>(WvsQ2->Get("W vs Q2/WvsQ2_rec")->Clone("WvsQ2_rec"));

//     // Check if the histograms are cloned successfully
//     if (!WvsQ2_gen || !WvsQ2_rec) {
//         std::cerr << "Error: Unable to clone histograms." << std::endl;
//         return 1;
//     }

//     // Create acceptance histogram
//     TH2D *acceptance_hist = dynamic_cast<TH2D*>(WvsQ2_rec->Clone("acceptance_hist"));

//     // Check if the cloning for the acceptance histogram is successful
//     if (!acceptance_hist) {
//         std::cerr << "Error: Unable to clone acceptance histogram." << std::endl;
//         return 1;
//     }

//     // Divide to create the acceptance histogram
//     acceptance_hist->Divide(WvsQ2_gen);

//     // Save to a new ROOT file
//     TFile *outputFile = new TFile(argv[1], "RECREATE");

//     // Check if the output file is created successfully
//     if (!outputFile || outputFile->IsZombie()) {
//         std::cerr << "Error: Unable to create the output ROOT file." << std::endl;
//         return 1;
//     }

//     acceptance_hist->SetTitle("Acceptance");
//     acceptance_hist->SetOption("COLZ1");
//     acceptance_hist->Write();
//     outputFile->Close();
//     WvsQ2->Close();

//     return 0;
// }