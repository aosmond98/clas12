#include <iostream>
#include <memory>
#include <TFile.h>
#include <TH2D.h>
#include <TCanvas.h>

int main(int argc, char** argv) {
    if (argc < 3) {
        std::cerr << "Usage: ./acceptance <output_filename.root> <input_root_file_pattern>" << std::endl;
        return 1;
    }
// int main() {
    // Open the ROOT file
    TFile *WvsQ2 = new TFile("24GeV_excl_31.root", "READ");

    // Clone histograms
    TH2D *WvsQ2_gen = dynamic_cast<TH2D*>(WvsQ2->Get("W vs Q2/WvsQ2_gen")->Clone("WvsQ2_gen"));
    TH2D *WvsQ2_rec = dynamic_cast<TH2D*>(WvsQ2->Get("W vs Q2/WvsQ2_rec")->Clone("WvsQ2_rec"));

    // Check if histograms are valid
    if (!WvsQ2_gen || !WvsQ2_rec) {
        std::cerr << "Error: Unable to clone histograms." << std::endl;
        return 1;
    }

    // Create acceptance histogram
    TH2D *acceptance_hist = dynamic_cast<TH2D*>(WvsQ2_rec->Clone("acceptance_hist"));
    acceptance_hist->Divide(WvsQ2_gen);

    // change for log scale z
    auto acceptance_can = std::make_unique<TCanvas>("acceptance_can", "Acceptance", 1920, 1080);

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