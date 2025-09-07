#include <iostream>
#include <memory>
#include <TFile.h>
#include <TH1D.h>
#include <TCanvas.h>
#include <TLegend.h>
#include <TF1.h>
#include <string>

// Helper function to check if a string contains a substring (case-sensitive)
static bool contains(const std::string& str, const std::string& substr) {
    return str.find(substr) != std::string::npos;
}

int main(int argc, char** argv) {
    if (argc < 3) {
        std::cerr << "Usage: ./mm2resolution <input_filename.root> <output_filename.root>" << std::endl;
        return 1;
    }

    const std::string input_filename = argv[1];
    const char* outputFileName = argv[2];

    // Determine the data type based on input filename
    bool is_rec_data = contains(input_filename, "rec");
    bool is_exp_data = contains(input_filename, "exp");

    // Open the input ROOT file
    TFile* input_file = new TFile(input_filename.c_str(), "READ");

    // Get the MM2 histogram
    TH1D* mm2_resolution = dynamic_cast<TH1D*>(input_file->Get("MM2/MM2"));

    // Check if the histogram is valid
    if (!mm2_resolution) {
        std::cerr << "Error: Unable to retrieve the MM2 histogram." << std::endl;
        return 1;
    }

    // Get the standard deviation from the histograms
    double std_dev_hist = mm2_resolution->GetStdDev();

    // Create a Gaussian fit
    TF1* gaus_fit = new TF1("gaus", "gaus", 0.005, 0.055);

    // Fit the histogram
    mm2_resolution->Fit(gaus_fit, "R");

    double std_dev_fit = gaus_fit->GetParameter(2);  // sigma of Gaussian

    // Create a canvas
    TCanvas* canvas = new TCanvas("canvas", "Missing Mass Squared Resolution", 800, 600);

    // Set line color based on data type
    if (is_rec_data) {
        mm2_resolution->SetLineColor(kGreen); // Green for reconstructed data
    } else if (is_exp_data) {
        mm2_resolution->SetLineColor(kGreen); // Red for experimental data
    } else {
        mm2_resolution->SetLineColor(kBlack); // Default to black if type is unknown
    }
    mm2_resolution->SetLineWidth(2);
    mm2_resolution->SetTitle("MM^{2}\u00A0 Resolution; MM^{2}\u00A0(GeV^{2}); Yields");

    // Set x-axis range
    mm2_resolution->GetXaxis()->SetRangeUser(-0.1, 0.1);

    // Draw the histogram
    mm2_resolution->Draw();

    // Customize Gaussian fit appearance
    // Set line color based on data type
    if (is_rec_data) {
        gaus_fit->SetLineColor(kBlue); // Green for reconstructed data
    } else if (is_exp_data) {
        gaus_fit->SetLineColor(kBlue); // Red for experimental data
    } else {
        gaus_fit->SetLineColor(kBlack); // Default to black if type is unknown
    }
    gaus_fit->Draw("SAME");

    // Add legend
    auto legend = new TLegend(0.6, 0.6, 0.9, 0.9);
    legend->AddEntry(mm2_resolution, Form("MM^{2}\u00A0 (#sigma = %.4f)", std_dev_hist), "l");
    legend->AddEntry(gaus_fit, Form("Gaussian Fit (#sigma = %.4f)", std_dev_fit), "l");
    legend->SetTextSize(0.06);
    legend->Draw();

    // Save the canvas to the output ROOT file
    TFile* outputFile = new TFile(outputFileName, "RECREATE");
    if (!outputFile || outputFile->IsZombie()) {
        std::cerr << "Error: Unable to create the output ROOT file." << std::endl;
        return 1;
    }

    canvas->Write();

    // Close files
    outputFile->Close();
    input_file->Close();

    return 0;
}



// #include <iostream>
// #include <memory>
// #include <TFile.h>
// #include <TH1D.h>
// #include <TCanvas.h>
// #include <TLegend.h>
// #include <TF1.h>

// int main(int argc, char** argv) {
//     if (argc < 4) {
//         std::cerr << "Usage: ./mm2resolution <exp_filename.root> <rec_filename.root> <output_filename.root>" << std::endl;
//         return 1;
//     }

//     const char* expFileName = argv[1];
//     const char* recFileName = argv[2];
//     const char* outputFileName = argv[3];

//     // Open experimental and reconstructed ROOT files
//     TFile *rec_file = new TFile(recFileName, "READ");
//     TFile *exp_file = new TFile(expFileName, "READ");

//     // Get histograms for reconstructed and experimental MM2
//     TH1D *mm2_exp = dynamic_cast<TH1D*>(exp_file->Get("MM2/MM2"));
//     TH1D *mm2_rec = dynamic_cast<TH1D*>(rec_file->Get("MM2/MM2"));

//     // Check if histograms are valid
//     if (!mm2_exp || !mm2_rec) {
//         std::cerr << "Error: Unable to retrieve histograms for MM2." << std::endl;
//         return 1;
//     }

//     // Get the standard deviation from the histograms
//     double exp_std_dev_hist = mm2_exp->GetStdDev();
//     double rec_std_dev_hist = mm2_rec->GetStdDev();

//     // // Get the standard deviation from the histograms
//     // double exp_std_dev_hist = mm2_exp->GetRMS();
//     // double rec_std_dev_hist = mm2_rec->GetRMS();

//     // Create Gaussian fits for both histograms
//     TF1 *gaus_exp = new TF1("gaus_exp", "gaus", -0.02, 0.02);  // Gaussian for exp
//     TF1 *gaus_rec = new TF1("gaus_rec", "gaus", -0.02, 0.02);  // Gaussian for rec

//     // Fit the histograms with Gaussian functions
//     mm2_exp->Fit(gaus_exp, "R");  // Fit exp histogram
//     mm2_rec->Fit(gaus_rec, "R");  // Fit rec histogram

//     // Get the standard deviation (sigma) from the Gaussian fits
//     double exp_std_dev_fit = gaus_exp->GetParameter(2);  // sigma of Gaussian for exp
//     double rec_std_dev_fit = gaus_rec->GetParameter(2);  // sigma of Gaussian for rec

//     // Create a canvas to display the histograms
//     TCanvas *canvas = new TCanvas("canvas", "MM2 Comparison", 800, 600);

//     // Customize appearance of histograms
//     mm2_exp->SetLineColor(kRed);     // Set experimental MM2 histogram color to red
//     mm2_exp->SetLineWidth(2);        // Set line width
//     mm2_exp->SetTitle("MM2 Comparison; MM2; Events");

//     mm2_rec->SetLineColor(kBlue);    // Set reconstructed MM2 histogram color to blue
//     mm2_rec->SetLineWidth(2);        // Set line width

//     // Set x-axis range from -0.02 to 0.02
//     mm2_exp->GetXaxis()->SetRangeUser(-0.02, 0.02);
//     mm2_rec->GetXaxis()->SetRangeUser(-0.02, 0.02);

//     // Draw both histograms on the same canvas
//     mm2_exp->Draw();   // Draw experimental MM2 histogram
//     mm2_rec->Draw("SAME");  // Draw reconstructed MM2 histogram on the same canvas

//     // Change the color of the Gaussian fits
//     gaus_exp->SetLineColor(kMagenta);
//     gaus_rec->SetLineColor(kGreen);

//     // Draw the Gaussian fits
//     gaus_exp->Draw("SAME");
//     gaus_rec->Draw("SAME");

//     // Add legend to distinguish between the histograms and fits, including std devs
//     auto legend = new TLegend(0.6, 0.6, 0.9, 0.9);
//     legend->AddEntry(mm2_exp, Form("Exp MM2 (#sigma = %.4f)", exp_std_dev_hist), "l");
//     legend->AddEntry(mm2_rec, Form("Rec MM2 (#sigma = %.4f)", rec_std_dev_hist), "l");
//     legend->AddEntry(gaus_exp, Form("Exp MM2 (#sigma = %.4f)", exp_std_dev_fit), "l");
//     legend->AddEntry(gaus_rec, Form("Rec MM2 (#sigma = %.4f)", rec_std_dev_fit), "l");
//     legend->Draw();

//     // Save the output file
//     TFile *outputFile = new TFile(outputFileName, "RECREATE");
//     if (!outputFile || outputFile->IsZombie()) {
//         std::cerr << "Error: Unable to create the output ROOT file." << std::endl;
//         return 1;
//     }

//     // Write the canvas to the output file
//     canvas->Write();

//     // Close files
//     outputFile->Close();
//     rec_file->Close();
//     exp_file->Close();

//     return 0;
// }
