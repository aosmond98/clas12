
#include "histogram.hpp"

Histogram::Histogram(const std::string &output_file)
{
        RootOutputFile = std::make_shared<TFile>(output_file.c_str(), "RECREATE");
        def = std::make_shared<TCanvas>("def");
        if (getenv("BEAM_E") != NULL)
        {
                if (atof(getenv("BEAM_E")) < 3)
                {
                        q2_max = 1.0;
                        w_max = 3.5;
                        p_max = 3.0;
                }
                else if (atof(getenv("BEAM_E")) < 8)
                {
                        q2_max = 4.0;
                        w_max = 4.0;
                        p_max = 4.0;
                }
                else if (atof(getenv("BEAM_E")) < 9)
                {
                        q2_max = 7.0;
                        w_max = 7.0;
                        p_max = 7.0;
                }
        }

        momentum = std::make_shared<TH1D>("mom", "mom", bins, p_min, p_max);
        W_hist = std::make_shared<TH1D>("W", "W", bins, zero, w_max);
        MM2_hist = std::make_shared<TH1D>("MM2", "MM2", bins, -0.04, 0.04);
        Q2_hist = std::make_shared<TH1D>("Q2", "Q2", bins, zero, q2_max);
        W_vs_q2 = std::make_shared<TH2D>("W_vs_q2", "W_vs_q2", bins, zero, w_max,
                                         bins, zero, q2_max);
        Mom_vs_MM2 = std::make_shared<TH2D>("Mom_vs_MM2", "Mom_vs_MM2", bins, p_min, 1,
                                         bins, -0.04, 0.04);
        // W_vs_q2_mmsq = std::make_shared<THnSparse>("W_vs_q2_mmsq", "W_vs_q2_mmsq", bins, zero, w_max,
        //                                  bins, zero, q2_max)

        // W_vs_q2_acceptance = std::shared_ptr<THnSparseF>;



        makeHists_THnSparse();
        makeHists_deltat();
        makeHists_MomVsBeta();
        makeHists_sector();

}

Histogram::~Histogram()
{
        this->Write();
}

void Histogram::Write()
{
        // std::cout << GREEN << "Writting" << DEF << std::endl;

        // // // //  Write_EC();
        std::cerr << BOLDBLUE << "WvsQ2()" << DEF << std::endl;
        TDirectory *WvsQ2_folder = RootOutputFile->mkdir("W vs Q2");
        WvsQ2_folder->cd();
        Write_WvsQ2();

        std::cerr << BOLDBLUE << "Write_MomVsBeta()" << DEF << std::endl;
        TDirectory *Write_MomVsBeta_folder = RootOutputFile->mkdir("Mom Vs Beta");
        Write_MomVsBeta_folder->cd();
        Write_MomVsBeta();

        std::cerr << BOLDBLUE << "Write_deltat()" << DEF << std::endl;
        TDirectory *Write_deltat_folder = RootOutputFile->mkdir("Delta_t");
        Write_deltat_folder->cd();
        Write_deltat();

        std::cerr << BOLDBLUE << "Write_THnSparse()" << DEF << std::endl;
        TDirectory *Write_THnSparse_folder = RootOutputFile->mkdir("THnSparse");
        Write_THnSparse_folder->cd();
        Write_THnSparse();

        std::cerr << BOLDBLUE << "Done Writing!!" << DEF << std::endl;
}



void Histogram::Fill_WvsQ2(const std::shared_ptr<Reaction> &_e)
{
        short sec = _e->sec();

        W_hist->Fill(_e->W(), _e->weight());
        MM2_hist->Fill(_e->MM2_exclusive(), _e->weight());
        Q2_hist->Fill(_e->Q2(), _e->weight());
        W_vs_q2->Fill(_e->W(), _e->Q2(), _e->weight());
        Mom_vs_MM2->Fill(_e->Mom_excl(), _e->MM2_exclusive(), _e->weight());

        if (sec > 0 && sec <= 6)
        {
                W_vs_q2_sec[sec - 1]->Fill(_e->W(), _e->Q2(), _e->weight());
                W_sec[sec - 1]->Fill(_e->W(), _e->weight());
                MM2_sec[sec - 1]->Fill(_e->MM2_exclusive(), _e->weight());
                Mom_vs_MM2_sec[sec - 1]->Fill(_e->Mom_excl(), _e->MM2_exclusive(), _e->weight());
        }
}

void Histogram::Write_WvsQ2()
{
       auto WvsQ2_can =
            std::make_unique<TCanvas>("WvsQ2_can", "W vs Q2 sectors", 1920, 1080);
        WvsQ2_can->Divide(3, 2);
        for (short i = 0; i < num_sectors; i++)
        {
                W_vs_q2_sec[i]->SetYTitle("Q^{2} (GeV^{2})");
                W_vs_q2_sec[i]->SetXTitle("W (GeV)");
                W_vs_q2_sec[i]->SetOption("COLZ1");
                WvsQ2_can->cd(i + 1);
                gPad->SetLogz();
                W_vs_q2_sec[i]->Draw("same");
        }
        WvsQ2_can->Write();

        auto W_can = std::make_unique<TCanvas>("W_can", "W sectors", 1920, 1080);
        W_can->Divide(3, 2);
        for (short i = 0; i < num_sectors; i++)
        {
                W_sec[i]->SetXTitle("W (GeV)");
                W_can->cd(i + 1);

                //  W_sec[i]->Fit("gaus", "QMR+", "QMR+", 0.85, 1.05);
                // gStyle->SetOptFit(01);
                W_sec[i]->Draw("same");
        }
        W_can->Write();

        auto MM2_can = std::make_unique<TCanvas>("MM2_can", "MM2 sectors", 1920, 1080);
        MM2_can->Divide(3, 2);
        for (short i = 0; i < num_sectors; i++)
        {
                MM2_sec[i]->SetXTitle("MM2 (GeV)");
                MM2_can->cd(i + 1);
                MM2_sec[i]->Draw("same");
        }
        MM2_can->Write();

        auto MomvsMM2_can =
            std::make_unique<TCanvas>("MomvsMM2_can", "Mom vs MM2 sectors", 1920, 1080);
        MomvsMM2_can->Divide(3, 2);
        for (short i = 0; i < num_sectors; i++)
        {
                Mom_vs_MM2_sec[i]->SetYTitle("MM^{2} (GeV^{2})");
                Mom_vs_MM2_sec[i]->SetXTitle("Mom (GeV)");
                Mom_vs_MM2_sec[i]->SetOption("COLZ1");
                MomvsMM2_can->cd(i + 1);
                gPad->SetLogz();
                Mom_vs_MM2_sec[i]->Draw("same");
        }
        MomvsMM2_can->Write();

        // // Create a canvas to draw the histogram
        // auto canvas = std::make_unique<TCanvas>("WvsQ2", "W vs Q2", 1920, 1080);
        // for (short i = 0; i < num_sectors; i++){
        W_vs_q2->SetYTitle("Q^{2} (GeV^{2})");
        W_vs_q2->SetXTitle("W (GeV)");
        // W_vs_q2->SetOption("COLZ1");

        // // Show the color bar on the right side
        // gPad->SetLogz();

        // // Draw the histogram on the canvas
        // W_vs_q2->Draw("same");  // Use "colz" to specify the option
        // }
        if (W_vs_q2->GetEntries())
                W_vs_q2->Write();

        // Write the THnSparse histogram for acceptance over W and Q^2
        if (W_vs_q2_acceptance->GetEntries())
                W_vs_q2_acceptance->Write();

        Mom_vs_MM2->SetYTitle("MM^{2} (GeV^{2})");
        Mom_vs_MM2->SetXTitle("Mom (GeV)");
        // Mom_vs_MM2->SetOption("COLZ1");
        if (Mom_vs_MM2->GetEntries())
                Mom_vs_MM2->Write();
        
        W_hist->SetXTitle("W (GeV)");
        if (W_hist->GetEntries())
                W_hist->Write();

        MM2_hist->SetXTitle("MM2 (GeV2)");
        if (MM2_hist->GetEntries())
                MM2_hist->Write();        

        Q2_hist->SetXTitle("Q^{2} (GeV^{2})");
        if (Q2_hist->GetEntries())
                Q2_hist->Write();
}
void Histogram::makeHists_sector()
{

        for (short i = 0; i < num_sectors; i++)
        {
                W_vs_q2_sec[i] = std::make_shared<TH2D>(
                    Form("wvsq2_sec_%d", i + 1), Form("W vs Q^{2} Sector: %d", i + 1), bins,
                    zero, w_max, bins, zero, q2_max);

                W_sec[i] =
                    std::make_shared<TH1D>(Form("w_sec_%d", i + 1),
                                           Form("W Sector: %d", i + 1), bins, zero, w_max);

                MM2_sec[i] =
                    std::make_shared<TH1D>(Form("mm2_sec_%d", i + 1),
                                           Form("MM2 Sector: %d", i + 1), bins, -0.04, 0.04);

                Mom_vs_MM2_sec[i] = std::make_shared<TH2D>(
                    Form("MomvsMM2_sec_%d", i + 1), Form("Mom vs MM^{2} Sector: %d", i + 1), bins,
                    p_min, 1, bins, -0.04, 0.04);

        }
}

void Histogram::makeHists_deltat()
{
        std::string tof = "";
        for (short p = 0; p < particle_num; p++)
        {
                for (short c = 0; c < charge_num; c++)
                {
                        for (short i = 0; i < with_id_num; i++)
                        {
                                tof = "ftof";
                                delta_t_hist[p][c][i][0] = std::make_shared<TH2D>(
                                    Form("delta_t_%s_%s_%s_%s", tof.c_str(), particle_name[p].c_str(),
                                         charge_name[c].c_str(), id_name[i].c_str()),
                                    Form("#Deltat %s %s %s %s", tof.c_str(), particle_name[p].c_str(),
                                         charge_name[c].c_str(), id_name[i].c_str()),
                                    bins, p_min, 6, bins, Dt_min, Dt_max);

                                tof = "ctof";
                                delta_t_hist[p][c][i][1] = std::make_shared<TH2D>(
                                    Form("delta_t_%s_%s_%s_%s", tof.c_str(), particle_name[p].c_str(),
                                         charge_name[c].c_str(), id_name[i].c_str()),
                                    Form("#Deltat %s %s %s %s", tof.c_str(), particle_name[p].c_str(),
                                         charge_name[c].c_str(), id_name[i].c_str()),
                                    bins, 0, 3.0, bins, -6.0, 6.0);
                        }
                }
        }
}

void Histogram::Fill_deltat_pi(const std::shared_ptr<Branches12> &data,
                               const std::shared_ptr<Delta_T> &dt, int part,
                               const std::shared_ptr<Reaction> &_e)
{
        auto _cuts = std::make_unique<Cuts>(data, dt);
        int charge = data->charge(part);
        // bool fc = dt->ctof();
        bool cd_part = (dt->ctof_particle(part) && data->status(part) > 4000);
        bool fd_part = (data->status(part) > 2000 && data->status(part) < 4000);

        int pid = data->pid(part);
        float mom = data->p(part);
        float time = NAN;

        if (cd_part)
        {
                time = dt->dt_ctof_Pi();
        }
        else if (fd_part)
                time = dt->dt_Pi();

        if (charge == 1)
        {
                if (cd_part)
                        delta_t_hist[1][0][0][1]->Fill(mom, time, _e->weight());
                else if (fd_part)
                        delta_t_hist[1][0][0][0]->Fill(mom, time, _e->weight());
                
                if (_cuts->IsPip(part))
                {
                        if (cd_part)
                                delta_t_hist[1][0][1][1]->Fill(mom, time, _e->weight());
                        else if (fd_part&& abs(time) < 0.5)
                                delta_t_hist[1][0][1][0]->Fill(mom, time, _e->weight());
                }
        }
        else if (charge == -1)
        {
                if (cd_part)
                        delta_t_hist[1][1][0][1]->Fill(mom, time, _e->weight());
                else if (fd_part)
                        delta_t_hist[1][1][0][0]->Fill(mom, time, _e->weight());
                
                if (_cuts->IsPim(part))
                {
                        if (cd_part)
                                delta_t_hist[1][1][1][1]->Fill(mom, time, _e->weight());
                        else if (fd_part && abs(time) < 0.5)
                                delta_t_hist[1][1][1][0]->Fill(mom, time, _e->weight());
                }
        }
}

void Histogram::Fill_deltat_prot(const std::shared_ptr<Branches12> &data,
                                 const std::shared_ptr<Delta_T> &dt, int part,
                                 const std::shared_ptr<Reaction> &_e)
{
        auto _cuts = std::make_unique<Cuts>(data, dt);
        int status = abs(data->status(part));
        int charge = data->charge(part);
        // bool fc = dt->ctof();
        bool cd_part = (dt->ctof_particle(part) && data->status(part) > 4000);
        bool fd_part = (data->status(part) > 2000 && data->status(part) < 4000);

        int pid = data->pid(part);
        float mom = data->p(part);
        float time = NAN;
        float time1 = NAN;

        if (cd_part)
        {
                time = dt->dt_ctof_P();
        }
        else if (fd_part)
        {
                time1 = dt->dt_P();
        }
        if (charge == 1)
        {
                if (cd_part)
                        delta_t_hist[2][0][0][1]->Fill(mom, time, _e->weight());
                else if (fd_part)
                        delta_t_hist[2][0][0][0]->Fill(mom, time1, _e->weight());

                if (_cuts->IsProton(part))
                {
                        if (fd_part && abs(time1) < 0.5)
                                delta_t_hist[2][0][1][0]->Fill(mom, time1, _e->weight());
                        else if (cd_part)
                                delta_t_hist[2][0][1][1]->Fill(mom, time, _e->weight());
                }
        }
}

// do
// pid == 11 at first;
// skim vs pid at 0 is 11. compare.

// pid 11 is electron
void Histogram::Write_deltat()
{
        // TDirectory *ftof_folder = RootOutputFile->mkdir("ftof");
        // ftof_folder->cd();
        for (short p = 0; p < particle_num; p++)
        {
                for (short c = 0; c < charge_num; c++)
                {
                        for (short i = 0; i < with_id_num; i++)
                        {
                                delta_t_hist[p][c][i][0]->SetXTitle("Momentum (GeV)");
                                delta_t_hist[p][c][i][0]->SetYTitle("#Deltat");
                                delta_t_hist[p][c][i][0]->Draw("COLZ1");
                                gPad->SetLogz();
                                if (delta_t_hist[p][c][i][0]->GetEntries() > 1)
                                        delta_t_hist[p][c][i][0]->Write();
                        }
                }
        }
        // TDirectory *ctof_folder = RootOutputFile->mkdir("ctof");
        // ctof_folder->cd();
        for (short p = 0; p < particle_num; p++)
        {
                for (short c = 0; c < charge_num; c++)
                {
                        for (short i = 0; i < with_id_num; i++)
                        {
                                delta_t_hist[p][c][i][1]->SetXTitle("Momentum (GeV)");
                                delta_t_hist[p][c][i][1]->SetYTitle("#Deltat");
                                delta_t_hist[p][c][i][1]->Draw("COLZ1");
                                gPad->SetLogz();
                                if (delta_t_hist[p][c][i][1]->GetEntries() > 1)
                                        delta_t_hist[p][c][i][1]->Write();
                        }
                }
        }
}

void Histogram::makeHists_MomVsBeta()
{
        for (short p = 0; p < particle_num; p++)
        {
                for (short c = 0; c < charge_num; c++)
                {
                        for (short i = 0; i < with_id_num; i++)
                        {
                                momvsbeta_hist[p][c][i] = std::make_shared<TH2D>(
                                    Form("mom_vs_beta_%s_%s_%s", particle_name[p].c_str(),
                                         charge_name[c].c_str(), id_name[i].c_str()),
                                    Form("Momentum vs #beta %s %s %s", particle_name[p].c_str(),
                                         charge_name[c].c_str(), id_name[i].c_str()),
                                    bins, p_min, p_max, bins, zero, 1.2);
                        }
                }
        }
}
// added line after int part to hopefully make the weight work
void Histogram::Fill_MomVsBeta(const std::shared_ptr<Branches12> &data, int part, const std::shared_ptr<Reaction> &_e)
{
        int good_ID = 0;
        float beta = data->beta(part);
        float mom = data->p(part);
        int charge = data->charge(part);
        int pid = data->pid(part);
        if (beta != 0)
        {
                momentum->Fill(mom);
                for (short p = 0; p < particle_num; p++)
                {
                        switch (p)
                        {
                        case 0:
                                good_ID = ELECTRON;
                                break;
                        case 1:
                                good_ID = PIP;
                                break;
                        case 2:
                                good_ID = PROTON;
                                break;
                        case 3:
                                good_ID = KP;
                                break;
                         }
                         
                        momvsbeta_hist[p][0][0]->Fill(mom, beta, _e->weight());

                        // trying to make the x axis a log scale
                        // gStyle->SetOptLogx(1); // Enable logarithmic scale on the x-axis.
                        // gStyle->SetOptLogy(1); // Enable logarithmic scale on the y-axis.

                        // also trying with log color pallette
                        // gStyle->SetPalette(53); // 53 corresponds to the logarithmic color palette.

                        if (good_ID == pid)
                        {
                                momvsbeta_hist[p][0][1]->Fill(mom, beta, _e->weight());
                        }
                        else
                        {
                                momvsbeta_hist[p][0][2]->Fill(mom, beta, _e->weight());
                        }

                        if (charge == -1)
                        {

                                momvsbeta_hist[p][1][0]->Fill(mom, beta, _e->weight());
                                // if (good_ID == 11)
                                if (-good_ID == pid)
                                        momvsbeta_hist[p][1][1]->Fill(mom, beta, _e->weight());
                                else // if (-good_ID == pid)
                                        momvsbeta_hist[p][1][2]->Fill(mom, beta, _e->weight());     
                        }
                        else if (charge == 1)
                        {
                                momvsbeta_hist[p][0][0]->Fill(mom, beta, _e->weight());
                                if (good_ID == pid)
                                        momvsbeta_hist[p][0][1]->Fill(mom, beta, _e->weight());
                                else  // if (-good_ID == pid)
                                        momvsbeta_hist[p][0][2]->Fill(mom, beta, _e->weight());       
                        }       
                }
        }
}

void Histogram::Write_MomVsBeta()
{
        momentum->SetXTitle("Momentum (GeV)");
        momentum->Write();
        for (short p = 0; p < particle_num; p++)
        {
                for (short c = 0; c < charge_num; c++)
                {
                        for (short i = 0; i < with_id_num; i++)
                        {
                                momvsbeta_hist[p][c][i]->SetXTitle("Momentum (GeV)");
                                momvsbeta_hist[p][c][i]->SetYTitle("#beta");
                                momvsbeta_hist[p][c][i]->SetOption("COLZ1");
                                gPad->SetLogz();
                                momvsbeta_hist[p][c][i]->Write();
                        }
                }
        }
}

// // attempt at THnSparse

// void Fill_THnSparse(const std::shared_ptr<Reaction> &_e)
// {
//         short sec = _e->sec();
//         W_vs_q2_mmsq->Fill(_e->W(), _e->Q2(), _e->MM2_exclusive(), _e->weight());

//         // // Fill the THnSparse histogram for acceptance over W and Q^2
//         // Double_t sparseData[3];
//         // sparseData[0] = _e->W();      // W
//         // sparseData[1] = _e->Q2();     // Q^2
//         // sparseData[2] = _e->weight(); // Weight

//         // W_vs_q2_acceptance->Fill(sparseData);

// }

// void Histogram::Write_THnSparse()
// {
//         // Define the axis titles and binning
//         const char* axisTitles[kNdimensions] = {"W (GeV)", "Q^2 (GeV^2)", "MM^2 (GeV^2)"};
//         Int_t nBins[kNdimensions] = {bins, bins, bins};
//         Double_t minValues[kNdimensions] = {0.0, 0.0, -0.1};
//         Double_t maxValues[kNdimensions] = {5.0, 24.0, 0.1};

//         // Create the THnSparse histogram
//         THnSparseF* W_vs_q2_mmsq = new THnSparseF("WvsQ2vsMM2", "W vs Q^2 vs MM^2",
//             kNdimensions, nBins, minValues, maxValues);

//         for (Int_t i = 0; i < kNdimensions; i++) {
//             W_vs_q2_mmsq->GetAxis(i)->SetTitle(axisTitles[i]);
//         }
//         W_vs_q2_mmsq->Write();
// }

// void Histogram::makeHists_THnSparse(){}

// void CreateTHnSparseHistogram() {
//     // Define the dimensions and binning of the histogram
//     const Int_t kNdimensions = 3;
//     TString axisTitles[kNdimensions] = {"W (GeV)", "Q^2 (GeV^2)", "Acceptance"};
//     Int_t nBins[kNdimensions] = {16, 17, 1}; // Define the number of bins for each dimension
//     Double_t minValues[kNdimensions] = {1.475, 2.0, 0.0}; // Minimum values for each dimension
//     Double_t maxValues[kNdimensions] = {2.0, 30.0, 1.0}; // Maximum values for each dimension

//     // Create the THnSparse histogram
//     THnSparseF *hist = new THnSparseF("WvsQ2vsAcceptance", "W vs Q^2 vs Acceptance",
//                                       kNdimensions, nBins, minValues, maxValues);

//     // Define the branches containing the data (hist_24_gev_sim.w_val, hist_24_gev_sim.q2_val, hist_24_gev_sim.acceptance)
//     TBranch *branchW = hist_24_gev_sim->GetBranch("w_val");
//     TBranch *branchQ2 = hist_24_gev_sim->GetBranch("q2_val");
//     TBranch *branchAcceptance = hist_24_gev_sim->GetBranch("acceptance");

//     // Loop over entries and fill the histogram
//     Long64_t nEntries = hist_24_gev_sim->GetEntries();
//     for (Long64_t iEntry = 0; iEntry < nEntries; ++iEntry) {
//         hist_24_gev_sim->GetEntry(iEntry);
//         Double_t data[kNdimensions] = {branchW->GetValue(), branchQ2->GetValue(), branchAcceptance->GetValue()};
//         hist->Fill(data);
//     }

//     // Draw the histogram (you can customize this part)
//     hist->Draw("colz"); // colz option for a 2D color plot
//     hist->GetXaxis()->SetTitle(axisTitles[0]);
//     hist->GetYaxis()->SetTitle(axisTitles[1]);

//     // Customize plot appearance as needed

//     // Save or display the plot
//     // For example:
//     TCanvas *c1 = new TCanvas("c1", "THnSparse Plot", 800, 600);
//     hist->Draw("colz");
//     c1->SaveAs("output_plot.png");
//     c1->Close();
// }

// THnSparseF* mm2Sparse = new THnSparseF("MM2Sparse", "MM^2 vs W vs Q^2", kNdimensions, nBins, minValues, maxValues);

// TChain chain("MyTree"); // The name of the TTree in your ROOT files

// // Load all ROOT files from a folder
// TSystemDirectory dir("/osmondal/nuclearPhys/root/24gev", "/osmondal/nuclearPhys/root/24gev");
// TList* files = dir.GetListOfFiles();

// if (files) {
//     TSystemFile* file;
//     TIter next(files);

//     while ((file = (TSystemFile*)next())) {
//         TString filename = file->GetName();

//         // Check if the file is a ROOT file
//         if (filename.EndsWith(".root")) {
//             TString filePath = TString::Format("/osmondal/nuclearPhys/root/24gev/%s", filename.Data());
//             chain.Add(filePath);
//         }
//     }
// }

// // Create variables to hold data from the TTree branches
// Double_t w, q2, mm2;

// // Set branch addresses
// chain.SetBranchAddress("W", &w);
// chain.SetBranchAddress("Q2", &q2);
// chain.SetBranchAddress("MM2", &mm2);

// // Loop through the chain and fill the histogram
// for (Long64_t i = 0; i < chain.GetEntries(); i++) {
//     chain.GetEntry(i);

//     // Fill the THnSparse histogram
//     Double_t values[kNdimensions] = {w, q2, mm2};
//     mm2Sparse->Fill(values);
// }

// // Write the histogram to a ROOT file
// TFile* outputFile = new TFile("sparse_output.root", "RECREATE");
// mm2Sparse->Write();
// outputFile->Close();