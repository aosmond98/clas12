#include <TFile.h>
#include <TH2D.h>
#include <iostream>
#include <utility>

std::pair<double, double> findPeak(TH2D* hist) {
    int nBinsX = hist->GetNbinsX();
    int nBinsY = hist->GetNbinsY();
    double maxContent = 0;
    double peakX = 0;
    double peakY = 0;

    for (int i = 1; i <= nBinsX; ++i) {
        for (int j = 1; j <= nBinsY; ++j) {
            double binContent = hist->GetBinContent(i, j);
            if (binContent > maxContent) {
                maxContent = binContent;
                peakX = hist->GetXaxis()->GetBinCenter(i);
                peakY = hist->GetYaxis()->GetBinCenter(j);
            }
        }
    }

    return {peakX, peakY};
}

// void applyOffset(TH2D* simHist, double offsetX, double offsetY, TH2D* correctedHist) {
//     int nBinsX = simHist->GetNbinsX();
//     int nBinsY = simHist->GetNbinsY();

//     for (int i = 1; i <= nBinsX; ++i) {
//         for (int j = 1; j <= nBinsY; ++j) {
//             double binContent = simHist->GetBinContent(i, j);
//             if (binContent > 0) {
//                 double x = simHist->GetXaxis()->GetBinCenter(i) - offsetX;
//                 double y = simHist->GetYaxis()->GetBinCenter(j) - offsetY;
//                 correctedHist->Fill(x, y, binContent);
//             }
//         }
//     }
// }

int main(int argc, char* argv[]) {
    if (argc != 2) {
        std::cerr << "Usage: ./offset_center <exp_root_file.root>" << std::endl;
        return 1;
    }

    const char* expFileName = argv[1];
    // const char* simFileName = argv[2];

    // Open and load experimental histogram
    TFile* expFile = TFile::Open(expFileName);
    if (!expFile || expFile->IsZombie()) {
        std::cerr << "Error opening experimental file: " << expFileName << std::endl;
        return -1;
    }
    TH2D* vx_vs_vy_exp = dynamic_cast<TH2D*>(expFile->Get("W vs Q2/vx_vs_vy"));
    if (!vx_vs_vy_exp) {
        std::cerr << "Experimental histogram 'vx_vs_vy' not found in file: " << expFileName << std::endl;
        expFile->Close();
        return -1;
    }
    auto peak_exp = findPeak(vx_vs_vy_exp);
    double offsetX = peak_exp.first;
    double offsetY = peak_exp.second;
    std::cout << "Peak of Experimental Histogram: (" << offsetX << ", " << offsetY << ")" << std::endl;
    expFile->Close();

    // // Open and load simulated histogram
    // TFile* simFile = TFile::Open(simFileName);
    // if (!simFile || simFile->IsZombie()) {
    //     std::cerr << "Error opening simulated file: " << simFileName << std::endl;
    //     return -1;
    // }
    // TH2D* vx_vs_vy_sim = dynamic_cast<TH2D*>(simFile->Get("W vs Q2/vx_vs_vy"));
    // if (!vx_vs_vy_sim) {
    //     std::cerr << "Simulated histogram 'vx_vs_vy' not found in file: " << simFileName << std::endl;
    //     simFile->Close();
    //     return -1;
    // }

    // // Create corrected simulated histogram
    // TH2D* vx_vs_vy_sim_corrected = new TH2D("vx_vs_vy_sim_corrected", "Corrected Simulated vx vs vy", 50, -4, 4, 50, -4, 4);
    // applyOffset(vx_vs_vy_sim, offsetX, offsetY, vx_vs_vy_sim_corrected);

    // // Write corrected histogram to output file
    // TFile* outFile = new TFile("10_2sim_corrected.root", "RECREATE");
    // if (!outFile || outFile->IsZombie()) {
    //     std::cerr << "Error creating output file." << std::endl;
    //     delete vx_vs_vy_sim_corrected;
    //     simFile->Close();
    //     return -1;
    // }
    // vx_vs_vy_sim_corrected->Write();
    // outFile->Close();
    // simFile->Close();

    // delete vx_vs_vy_sim_corrected; // Clean up dynamically allocated histogram

    return 0;
}
