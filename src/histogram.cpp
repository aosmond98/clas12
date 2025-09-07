
#include "histogram.hpp"
#include <TRandom.h> 
#include <TFile.h>
#include <TH2D.h>
#include <iostream>
#include <utility>
#include <sys/stat.h> // For creating directories
#include <string>

static bool contains(const std::string& str, const std::string& substr) {
    return str.find(substr) != std::string::npos;
}

std::string determineTopology(const std::string& output_filename) {
    if (contains(output_filename, "excl")) return "excl";
    if (contains(output_filename, "mProt")) return "mProt";
    if (contains(output_filename, "mPip")) return "mPip";
    if (contains(output_filename, "mPim")) return "mPim";
    return "unknown";
}

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
                        w_min = 0.9;                        
                        p_max = 3.0;
                }
                else if (atof(getenv("BEAM_E")) < 11)
                {
                        q2_min = 0.0;
                        q2_max = 13.0;
                        w_min = 0.9;
                        w_max = 2.6;
                        p_max = 20.0;
                }
                else if (atof(getenv("BEAM_E")) < 24)
                {
                        q2_min = 1.0;
                        q2_max = 31.0;
                        w_min = 0.9;
                        w_max = 2.6;
                        p_max = 20.0;
                }
        }

        if (output_file.find("excl") != std::string::npos) {
                topology = "excl";
        } else if (output_file.find("mProt") != std::string::npos) {
                topology = "mProt";
        } else if (output_file.find("mPip") != std::string::npos) {
                topology = "mPip";
        } else if (output_file.find("mPim") != std::string::npos) {
                topology = "mPim";
        } else {
                throw std::runtime_error("Unknown topology in output filename.");
        }

        // Detect topology from output filename
        std::string topology = determineTopology(output_file);

        // Set default MM² range
        mm2_min = -5.0;
        mm2_max = 5.0;

        // set wide mm2 range for background purposes
        mm2_min_wide = -5.0;
        mm2_max_wide = 5.0;

        // Set MM² range based on topology
        if (topology == "excl") {
                mm2_min = -0.02;
                mm2_max = 0.02;
        } else if (topology == "mProt") {
                mm2_min = 0.25;
                mm2_max = 1.5;
        } else if (topology == "mPip") {
                mm2_min = -0.1;
                mm2_max = 0.1;
        } else if (topology == "mPim") {
                mm2_min = -0.1;
                mm2_max = 0.1;
        } else {
                std::cerr << "Warning: Unknown topology detected. Using default MM² range.\n";
        }

        momentum = std::make_shared<TH1D>("mom", "mom", bins, p_min, p_max);

        ec_ecin_energy_0 = std::make_shared<TH1D>("ec_ecin_energy_0", "ec_ecin_energy_0", bins, zero, 10.0);
        pcal_vs_ecal = std::make_shared<TH2D>("pcal_vs_ecal", "pcal_vs_ecal", bins, zero, 2.0, bins, zero, 2.0);
        pcal_ecal_x_vs_y = std::make_shared<TH2D>("pcal_ecal_x_vs_y", "pcal_ecal_x_vs_y", bins, -10, 10,
                                                 bins, -10, 10);
        ec_tot_energy = std::make_shared<TH1D>("ec_tot_energy", "ec_tot_energy", bins, zero, 10.0);
        elec_energy = std::make_shared<TH1D>("elec_energy", "elec_energy", bins, zero, 10.0);
        elec_mom = std::make_shared<TH1D>("elec_mom", "elec_mom", bins, p_min, 10.0);
        vx_vs_vy = std::make_shared<TH2D>("vx_vs_vy", "vx_vs_vy", bins, -2, 2,
                                         bins, -2, 2);
        // corr_vx_vs_vy = std::make_shared<TH2D>("corr_vx_vs_vy", "corr_vx_vs_vy", bins, -2, 2,
        //                                  bins, -2, 2);
        vz = std::make_shared<TH1D>("vz", "vz", bins, -10.0, 10.0);
        cc_nphe_tot = std::make_shared<TH1D>("cc_nphe_tot", "cc_nphe_tot", bins, zero, 50.0);

        sf = std::make_shared<TH1D>("sf", "sf", bins, zero, 0.5);
        W_vs_sf = std::make_shared<TH2D>("W_vs_sf", "W_vs_sf", bins, zero, 3.5,
                                         bins, 0.15, 0.35);
        mom_vs_sf = std::make_shared<TH2D>("mom_vs_sf", "mom_vs_sf", bins, 1.0, 10.0,
                                         bins, 0.10, 0.35);

        mom_vs_theta = std::make_shared<TH2D>("mom_vs_theta", "mom_vs_theta", bins, p_min, p_max, 
                                                bins, zero, 40);
        mom_vs_phi = std::make_shared<TH2D>("mom_vs_phi", "mom_vs_phi", bins, p_min, p_max,
                                                bins, -5, 365);

        W = std::make_shared<TH1D>("W", "W", bins, w_min, w_max);
        Q2 = std::make_shared<TH1D>("Q2", "Q2", bins, q2_min, q2_max);
        WvsQ2 = std::make_shared<TH2D>("WvsQ2", "WvsQ2", bins, w_min, w_max,
                                         bins, q2_min, q2_max);

        MM2 = std::make_shared<TH1D>("MM2", "MM2", bins, mm2_min, mm2_max);
        // Mom_vs_MM2 = std::make_shared<TH2D>("Mom_vs_MM2", "Mom_vs_MM2", bins, p_min, p_max,
        //                                         bins, mm2_min, mm2_max);
        W_vs_MM2 = std::make_shared<TH2D>("W_vs_MM2", "W_vs_MM2", bins, w_min, w_max,
                                         bins, mm2_min, mm2_max);
        Q2_vs_MM2 = std::make_shared<TH2D>("Q2_vs_MM2", "Q2_vs_MM2", bins, q2_min, q2_max,
                                         bins, mm2_min, mm2_max);

        W_mc = std::make_shared<TH1D>("W_mc", "W_mc", bins, w_min, w_max);
        Q2_mc = std::make_shared<TH1D>("Q2_mc", "Q2_mc", bins, q2_min, q2_max);
        WvsQ2_mc = std::make_shared<TH2D>("WvsQ2_mc", "WvsQ2_mc", bins, w_min, w_max,
                                         bins, q2_min, q2_max);
        MM2_mc = std::make_shared<TH1D>("MM2_mc", "MM2_mc", bins, mm2_min, mm2_max);
        // Mom_vs_MM2_mc = std::make_shared<TH2D>("Mom_vs_MM2", "Mom_vs_MM2", bins, p_min, p_max,
        //                                         bins, mm2_min, mm2_max);
        // W_vs_MM2_mc = std::make_shared<TH2D>("W_vs_MM2", "W_vs_MM2", bins, w_min, w_max,
        //                                  bins, mm2_min, mm2_max);

        makeHists_deltat();
        makeHists_MomVsBeta();
        // makeHists_MomVsMM2();
        makeHists_sector();
        makeHists_MM2withbins();
}

Histogram::~Histogram()
{
        this->Write();
}

void Histogram::Write()
{
        // std::cout << GREEN << "Writing" << DEF << std::endl;
        // Write_EC();

        std::cerr << BOLDBLUE << "Write_WvsQ2()" << DEF << std::endl;
        TDirectory *Write_WvsQ2_folder = RootOutputFile->mkdir("W vs Q2");
        Write_WvsQ2_folder->cd();
        Write_WvsQ2();

        std::cerr << BOLDBLUE << "Write_MM2()" << DEF << std::endl;
        TDirectory *Write_MM2_folder = RootOutputFile->mkdir("MM2");
        Write_MM2_folder->cd();
        Write_MM2();

        std::cerr << BOLDBLUE << "Write_MomVsBeta()" << DEF << std::endl;
        TDirectory *Write_MomVsBeta_folder = RootOutputFile->mkdir("Mom Vs Beta");
        Write_MomVsBeta_folder->cd();
        Write_MomVsBeta();

        // std::cerr << BOLDBLUE << "Write_MomVsMM2()" << DEF << std::endl;
        // TDirectory *Write_MomVsMM2_folder = RootOutputFile->mkdir("Mom Vs MM2");
        // Write_MomVsMM2_folder->cd();
        // Write_MomVsMM2();

        std::cerr << BOLDBLUE << "Write_MM2withbins()" << DEF << std::endl;
        TDirectory *Write_MM2_withbins_folder = RootOutputFile->mkdir("MM2 with bins");
        Write_MM2_withbins_folder->cd();
        Write_MM2withbins(Write_MM2_withbins_folder);

        std::cerr << BOLDBLUE << "Write_deltat()" << DEF << std::endl;
        TDirectory *Write_deltat_folder = RootOutputFile->mkdir("Delta_t");
        Write_deltat_folder->cd();
        TDirectory *ctof_folder = Write_deltat_folder->mkdir("ctof");
        TDirectory *ftof_folder = Write_deltat_folder->mkdir("ftof");
        Write_deltat(ctof_folder, ftof_folder, Write_deltat_folder);

        std::cerr << BOLDMAGENTA << "Done Writing!!" << DEF << std::endl;
}

void Histogram::Fill_WvsQ2(const std::shared_ptr<Reaction> &_e, const std::shared_ptr<Branches12> &data)
{
        // auto ec_energy = [&data](int part) {
        // return data->ec_pcal_energy(0) + data->ec_ecin_energy(0) + data->ec_ecout_energy(0);
        // };
        
        W->Fill(_e->W(), _e->weight());
        Q2->Fill(_e->Q2(), _e->weight());
        WvsQ2->Fill(_e->W(), _e->Q2(), _e->weight()); // normalized is w weight

        // Calculate ec_tot_energy for the first particle (index 0)
        double ec_energy = data->ec_pcal_energy(0) + data->ec_ecin_energy(0) + data->ec_ecout_energy(0);

        // Define and calculate sf_calc
        double sf_calc = ec_energy / _e->elec_mom();

        // calculate ecal+pcal x and y
        double ec_x = data->ec_ecin_x(0) + data->ec_ecout_x(0) + data->ec_pcal_x(0);
        double ec_y = data->ec_ecin_y(0) + data->ec_ecout_y(0) + data->ec_pcal_y(0);

        // corrected vx and vy
        // double corr_vx = data->vx(0) - 0.152;
        // double corr_vy = data->vy(0) - 0.232;

        ec_ecin_energy_0->Fill(data->ec_ecin_energy(0), _e->weight());
        pcal_vs_ecal->Fill(data->ec_pcal_energy(0), data->ec_ecin_energy(0) + data->ec_ecout_energy(0), _e->weight());
        pcal_ecal_x_vs_y->Fill(ec_x, ec_y);
        ec_tot_energy->Fill(data->ec_tot_energy(0), _e->weight());
        elec_energy->Fill(_e->elec_E(), _e->weight());
        elec_mom->Fill(_e->elec_mom(), _e->weight());
        vx_vs_vy->Fill(data->vx(0), data->vy(0), _e->weight());
        // corr_vx_vs_vy->Fill(corr_vx, corr_vy);
        vz->Fill(data->vz(0));//, _e->weight());
        cc_nphe_tot->Fill(data->cc_nphe_tot(0));//, _e->weight());

        sf_calc = (data->ec_ecin_energy(0) + data->ec_ecout_energy(0) + data->ec_pcal_energy(0)) / _e->elec_mom();
        sf->Fill(sf_calc, _e->weight());
        W_vs_sf->Fill(_e->W(), sf_calc, _e->weight());
        mom_vs_sf->Fill(_e->elec_mom(), sf_calc);

        mom_vs_theta->Fill(_e->elec_mom(), _e->elec_theta(), _e->weight());
        mom_vs_phi->Fill(_e->elec_mom(), _e->elec_phi(), _e->weight());

        short sec = _e->sec();
        if (sec > 0 && sec <= 6)
        {
                W_sec[sec - 1]->Fill(_e->W(), _e->weight());
                WvsQ2_sec[sec - 1]->Fill(_e->W(), _e->Q2(), _e->weight());
                momvstheta_sec[sec - 1]->Fill(_e->elec_mom(), _e->elec_theta(), _e->weight());
                momvsphi_sec[sec - 1]->Fill(_e->elec_mom(), _e->elec_phi(), _e->weight());
                W_vs_sf_sec[sec - 1]->Fill(_e->W(), sf_calc, _e->weight());
                vz_sec[sec - 1]->Fill(data->vz(0), _e->weight());
                momvssf_sec[sec - 1]->Fill(_e->elec_mom(), sf_calc);
        }
}

void Histogram::Fill_WvsQ2_mc(const std::shared_ptr<MCReaction> &_e)
{
        W_mc->Fill(_e->W_mc(), _e->weight());
        Q2_mc->Fill(_e->Q2_mc(), _e->weight());
        WvsQ2_mc->Fill(_e->W_mc(), _e->Q2_mc(), _e->weight());
        
        short sec = _e->sec();
        if (sec > 0 && sec <= 6)
        {
                W_mc_sec[sec - 1]->Fill(_e->W_mc(), _e->weight());
                WvsQ2_mc_sec[sec - 1]->Fill(_e->W_mc(), _e->Q2_mc(), _e->weight());
        }
}

void Histogram::Write_WvsQ2()
{
        W->SetXTitle("W (GeV)");
        if (W->GetEntries())
                W->Write();

        Q2->SetXTitle("Q^{2}\u00A0(GeV^{2})");
        if (Q2->GetEntries())
                Q2->Write();

        WvsQ2->SetXTitle("W (GeV)");
        WvsQ2->SetYTitle("Q^{2}\u00A0(GeV^{2})");
        WvsQ2->SetTitle("W vs. Q^{2}");
        WvsQ2->SetOption("COLZ1");
        if (WvsQ2->GetEntries())
                WvsQ2->Write();

        ec_ecin_energy_0->SetXTitle("ec_ecin_energy_0");
        if (ec_ecin_energy_0->GetEntries())
                ec_ecin_energy_0->Write();

        ec_tot_energy->SetXTitle("ec_tot_energy");
        if (ec_tot_energy->GetEntries())
                ec_tot_energy->Write();

        pcal_vs_ecal->SetXTitle("pcal");
        pcal_vs_ecal->SetYTitle("ecal");
        if (pcal_vs_ecal->GetEntries())
                pcal_vs_ecal->Write();

        pcal_ecal_x_vs_y->SetXTitle("ec_x");
        pcal_ecal_x_vs_y->SetYTitle("ec_y");
        if (pcal_ecal_x_vs_y->GetEntries())
                pcal_ecal_x_vs_y->Write();

        elec_energy->SetXTitle("elec_energy");
        if (elec_energy->GetEntries())
                elec_energy->Write();

        elec_mom->SetXTitle("elec_mom");
        if (elec_mom->GetEntries())
                elec_mom->Write();

        vx_vs_vy->SetXTitle("vx");
        vx_vs_vy->SetYTitle("vy");
        if (vx_vs_vy->GetEntries())
                vx_vs_vy->Write();

        // corr_vx_vs_vy->SetXTitle("corr_vx");
        // corr_vx_vs_vy->SetYTitle("corr_vy");
        // if (corr_vx_vs_vy->GetEntries())
        //         corr_vx_vs_vy->Write();

        vz->SetXTitle("vz");
        if (vz->GetEntries())
                vz->Write();

        cc_nphe_tot->SetXTitle("cc_nphe_tot");
        if (cc_nphe_tot->GetEntries())
                cc_nphe_tot->Write();

        sf->SetXTitle("sf");
        // Create a canvas to draw the histogram and line
        TCanvas *sf_can = new TCanvas("sf_can", "Canvas", 800, 600);

        // Draw the histogram
        sf->Draw();

        // Create a vertical line at x = 0.25
        TLine* vline = new TLine(0.25, sf->GetMinimum(), 0.25, sf->GetMaximum());
        vline->SetLineColor(kRed);  // Set line color (optional)
        vline->SetLineWidth(1);     // Set line width (optional)
        vline->Draw("same");        // Draw the line on the same canvas as the histogram

        // Write the canvas to the ROOT file
        sf_can->Write();

        // Clean up
        delete sf_can;

        W_vs_sf->SetXTitle("W (GeV)");
        W_vs_sf->SetYTitle("sf");
        W_vs_sf->SetOption("COLZ1");
        if (W_vs_sf->GetEntries())
                W_vs_sf->Write();

        mom_vs_sf->SetXTitle("momentum");
        mom_vs_sf->SetYTitle("sf");
        mom_vs_sf->SetOption("COLZ1");
        if (mom_vs_sf->GetEntries())
                mom_vs_sf->Write();

        mom_vs_theta->SetXTitle("Momentum (GeV)");
        mom_vs_theta->SetYTitle("theta (degrees, probably)");
        if (mom_vs_theta->GetEntries())
                mom_vs_theta->Write();

        mom_vs_phi->SetXTitle("Momentum (GeV)");
        mom_vs_phi->SetYTitle("phi (degrees, probably)");
        if (mom_vs_phi->GetEntries())
                mom_vs_phi->Write();

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

        auto WvsQ2_can =
            std::make_unique<TCanvas>("WvsQ2_can", "W vs. Q^{2}\u00A0Sectors", 1920, 1080);
        WvsQ2_can->Divide(3, 2);
        for (short i = 0; i < num_sectors; i++)
        {
                WvsQ2_sec[i]->SetYTitle("Q^{2}\u00A0(GeV^{2})");
                WvsQ2_sec[i]->SetXTitle("W (GeV)");
                WvsQ2_sec[i]->SetOption("COLZ1");
                WvsQ2_can->cd(i + 1);
                gPad->SetLogz();
                WvsQ2_sec[i]->Draw("same");
        }
        WvsQ2_can->Write();

        auto momvstheta_can =
            std::make_unique<TCanvas>("momvstheta_can", "Momentum vs. theta Sectors", 1920, 1080);
        momvstheta_can->Divide(3, 2);
        for (short i = 0; i < num_sectors; i++)
        {
                momvstheta_sec[i]->SetYTitle("theta (degrees, probably)");
                momvstheta_sec[i]->SetXTitle("Momentum (GeV)");
                momvstheta_sec[i]->SetOption("COLZ1");
                momvstheta_can->cd(i + 1);
                gPad->SetLogz();
                momvstheta_sec[i]->Draw("same");
        }
        momvstheta_can->Write();

        auto momvsphi_can =
            std::make_unique<TCanvas>("momvsphi_can", "Momentum vs. phi Sectors", 1920, 1080);
        momvsphi_can->Divide(3, 2);
        for (short i = 0; i < num_sectors; i++)
        {
                momvsphi_sec[i]->SetYTitle("phi (degrees, probably)");
                momvsphi_sec[i]->SetXTitle("Momentum (GeV)");
                momvsphi_sec[i]->SetOption("COLZ1");
                momvsphi_can->cd(i + 1);
                gPad->SetLogz();
                momvsphi_sec[i]->Draw("same");
        }
        momvsphi_can->Write();

        auto W_vs_sf_can =
            std::make_unique<TCanvas>("W_vs_sf_can", "W vs. sf Sectors", 1920, 1080);
        W_vs_sf_can->Divide(3, 2);
        for (short i = 0; i < num_sectors; i++)
        {
                W_vs_sf_sec[i]->SetYTitle("sf");
                W_vs_sf_sec[i]->SetXTitle("W (GeV)");
                W_vs_sf_sec[i]->SetOption("COLZ1");
                W_vs_sf_can->cd(i + 1);
                gPad->SetLogz();
                W_vs_sf_sec[i]->Draw("same");
        }
        W_vs_sf_can->Write();

        auto vz_can = std::make_unique<TCanvas>("vz_can", "vz sectors", 1920, 1080);
        vz_can->Divide(3, 2);
        for (short i = 0; i < num_sectors; i++)
        {
                vz_sec[i]->SetXTitle("vz");
                vz_can->cd(i + 1);
                //  W_sec[i]->Fit("gaus", "QMR+", "QMR+", 0.85, 1.05);
                // gStyle->SetOptFit(01);
                vz_sec[i]->Draw("same");
        }
        vz_can->Write();

        auto momvssf_can =
            std::make_unique<TCanvas>("momvssf_can", "Momentum vs. sf sectors", 1920, 1080);
        momvssf_can->Divide(3, 2);
        for (short i = 0; i < num_sectors; i++)
        {
                momvssf_sec[i]->SetYTitle("sf");
                momvssf_sec[i]->SetXTitle("momentum");
                momvssf_sec[i]->SetOption("COLZ1");
                momvssf_can->cd(i + 1);
                gPad->SetLogz();
                momvssf_sec[i]->Draw("same");
        }
        momvssf_can->Write();

        W_mc->SetXTitle("W (GeV)");
        if (W_mc->GetEntries())
                W_mc->Write();

        Q2_mc->SetXTitle("Q^{2}\u00A0(GeV^{2})");
        if (Q2_mc->GetEntries())
                Q2_mc->Write();

        WvsQ2_mc->SetXTitle("W (GeV)");
        WvsQ2_mc->SetYTitle("Q^{2}\u00A0(GeV^{2})");
        WvsQ2_mc->SetTitle("W vs. Q^{2}\u00A0mc");
        WvsQ2_mc->SetOption("COLZ1");
        if (WvsQ2_mc->GetEntries())
                WvsQ2_mc->Write();
                // gPad->SetLogz(true); // Set the color scale to logarithmic
                // // Set log scale on the Z-axis
                // WvsQ2_gen->SetLogz(true);

        auto W_mc_can = std::make_unique<TCanvas>("W_mc_can", "W_mc sectors", 1920, 1080);
        W_mc_can->Divide(3, 2);
        for (short i = 0; i < num_sectors; i++)
        {
                W_mc_sec[i]->SetXTitle("W_mc (GeV)");
                W_mc_can->cd(i + 1);
                //  W_sec[i]->Fit("gaus", "QMR+", "QMR+", 0.85, 1.05);
                // gStyle->SetOptFit(01);
                W_mc_sec[i]->Draw("same");
        }
        W_mc_can->Write();

        auto WvsQ2_mc_can =
            std::make_unique<TCanvas>("WvsQ2_mc_can", "W vs. Q^{2}\u00A0mc Sectors", 1920, 1080);
        WvsQ2_mc_can->Divide(3, 2);
        for (short i = 0; i < num_sectors; i++)
        {
                WvsQ2_mc_sec[i]->SetYTitle("Q^{2}\u00A0(GeV^{2})");
                WvsQ2_mc_sec[i]->SetXTitle("W (GeV)");
                WvsQ2_mc_sec[i]->SetTitle("W vs. Q^{2}\u00A0mc");
                WvsQ2_mc_sec[i]->SetOption("COLZ1");
                WvsQ2_mc_can->cd(i + 1);
                gPad->SetLogz();
                WvsQ2_mc_sec[i]->Draw("same");
        }
        WvsQ2_mc_can->Write();
}

void Histogram::Fill_MM2(const std::shared_ptr<Reaction> &_e, const std::shared_ptr<Branches12> &data)
{
        double MM2_val = 0.0;

        // Select MM2 calculation based on topology
        if (topology == "excl") {
                MM2_val = _e->MM2_exclusive();
        } else if (topology == "mProt") {
                MM2_val = _e->MM2_mProt();
        } else if (topology == "mPip") {
                MM2_val = _e->MM2_mPip();
        } else if (topology == "mPim") {
                MM2_val = _e->MM2_mPim();
        }
        
        MM2->Fill(MM2_val, _e->weight());
        W_vs_MM2->Fill(_e->W(), MM2_val, _e->weight());
        Q2_vs_MM2->Fill(_e->Q2(), MM2_val, _e->weight());
        // Mom_vs_MM2->Fill(data->p(0), MM2_val, _e->weight());
        // W_vs_sf->Fill(_e->W(), _e->sf(), _e->weight());

        short sec = _e->sec();
        if (sec > 0 && sec <= 6)
        {
                MM2_sec[sec - 1]->Fill(MM2_val, _e->weight());
                W_vs_MM2_sec[sec - 1]->Fill(_e->W(), MM2_val, _e->weight());
                Q2_vs_MM2_sec[sec - 1]->Fill(_e->Q2(), MM2_val, _e->weight());
        }
}

// void Histogram::Fill_MM2_mc(const std::shared_ptr<MCReaction> &_e, const std::shared_ptr<MCBranches12> &data)
// {
//        double MM2_val = _e->MM2_exclusive_mc();       
//         MM2_mc->Fill(MM2_val, _e->weight());
//         W_vs_MM2_mc->Fill(_e->W_mc(), MM2_val, _e->weight());
//         // Mom_vs_MM2_->Fill(data->p(0), MM2_val, _e->weight());
//         // W_vs_sf->Fill(_e->W(), _e->sf(), _e->weight());
//         short sec = _e->sec();
//         if (sec > 0 && sec <= 6)
//         {
//                 MM2_mc_sec[sec - 1]->Fill(MM2_val, _e->weight());
//                 W_vs_MM2_mc_sec[sec - 1]->Fill(_e->W_mc(), MM2_val, _e->weight());
//         }
// }

void Histogram::Write_MM2()
{
        MM2->SetXTitle("MM2 (GeV2)");
        if (MM2->GetEntries())
                MM2->Write(); 

        W_vs_MM2->SetYTitle("MM^{2}\u00A0(GeV^{2})");
        W_vs_MM2->SetXTitle("W (GeV)");
        W_vs_MM2->SetTitle("W vs. MM^{2}");
        // W_vs_MM2->SetOption("COLZ1");
        if (W_vs_MM2->GetEntries())
                W_vs_MM2->Write();

        Q2_vs_MM2->SetYTitle("MM^{2}\u00A0(GeV^{2})");
        Q2_vs_MM2->SetXTitle("Q^{2}\u00A0(GeV^{2})");
        Q2_vs_MM2->SetTitle("Q^{2}\u00A0vs. MM^{2}");
        // Q2_vs_MM2->SetOption("COLZ1");
        if (Q2_vs_MM2->GetEntries())
                Q2_vs_MM2->Write();

        // Mom_vs_MM2->SetYTitle("MM^{2}\u00A0(GeV^{2})");
        // Mom_vs_MM2->SetXTitle("Mom (GeV)");
        // // Mom_vs_MM2->SetOption("COLZ1");
        // if (Mom_vs_MM2->GetEntries())
        //         Mom_vs_MM2->Write();   

        // W_vs_sf->SetYTitle("SF");
        // W_vs_sf->SetXTitle("W (GeV)");
        // // W_vs_sf->SetOption("COLZ1");
        // if (W_vs_sf->GetEntries())
        //         W_vs_sf->Write();  

        MM2_mc->SetXTitle("MM2_mc (GeV2)");
        if (MM2_mc->GetEntries())
                MM2_mc->Write(); 

        // W_vs_MM2_mc->SetYTitle("MM^{2}_mc\u00A0(GeV^{2})");
        // W_vs_MM2_mc->SetXTitle("W_mc (GeV)");
        // // W_vs_MM2->SetOption("COLZ1");
        // if (W_vs_MM2_mc->GetEntries())
        //         W_vs_MM2_mc->Write();

        // Mom_vs_MM2_mc->SetYTitle("MM^{2}_mc\u00A0(GeV^{2})");
        // Mom_vs_MM2_mc->SetXTitle("Mom_mc (GeV)");
        // // Mom_vs_MM2->SetOption("COLZ1");
        // if (Mom_vs_MM2_mc->GetEntries())
        //         Mom_vs_MM2_mc->Write();

        auto MM2_can = std::make_unique<TCanvas>("MM2_can", "MM2 sectors", 1920, 1080);
        MM2_can->Divide(3, 2);
        for (short i = 0; i < num_sectors; i++)
        {
                MM2_sec[i]->SetXTitle("MM2 (GeV)");
                MM2_can->cd(i + 1);
                MM2_sec[i]->Draw("same");
        }
        MM2_can->Write();

        auto WvsMM2_can =
            std::make_unique<TCanvas>("WvsMM2_can", "W vs. MM2 sectors", 1920, 1080);
        WvsMM2_can->Divide(3, 2);
        for (short i = 0; i < num_sectors; i++)
        {
                W_vs_MM2_sec[i]->SetYTitle("MM^{2}\u00A0(GeV^{2})");
                W_vs_MM2_sec[i]->SetXTitle("W (GeV)");
                W_vs_MM2_sec[i]->SetOption("COLZ1");
                WvsMM2_can->cd(i + 1);
                gPad->SetLogz();
                W_vs_MM2_sec[i]->Draw("same");
        }
        WvsMM2_can->Write();

        auto Q2vsMM2_can =
            std::make_unique<TCanvas>("Q2vsMM2_can", "Q^{2}\u00A0vs. MM2 sectors", 1920, 1080);
        Q2vsMM2_can->Divide(3, 2);
        for (short i = 0; i < num_sectors; i++)
        {
                Q2_vs_MM2_sec[i]->SetYTitle("MM^{2}\u00A0(GeV^{2})");
                Q2_vs_MM2_sec[i]->SetXTitle("Q^{2}\u00A0(GeV^{2})");
                Q2_vs_MM2_sec[i]->SetOption("COLZ1");
                Q2vsMM2_can->cd(i + 1);
                gPad->SetLogz();
                Q2_vs_MM2_sec[i]->Draw("same");
        }
        Q2vsMM2_can->Write();

        auto MM2_mc_can = std::make_unique<TCanvas>("MM2_mc_can", "MM2 sectors", 1920, 1080);
        MM2_mc_can->Divide(3, 2);
        for (short i = 0; i < num_sectors; i++)
        {
                MM2_mc_sec[i]->SetXTitle("MM2 (GeV)");
                MM2_mc_can->cd(i + 1);
                MM2_mc_sec[i]->Draw("same");
        }
        MM2_mc_can->Write();

        // auto WvsMM2_mc_can =
        //     std::make_unique<TCanvas>("WvsMM2_mc_can", "W vs. MM2 sectors", 1920, 1080);
        // WvsMM2_mc_can->Divide(3, 2);
        // for (short i = 0; i < num_sectors; i++)
        // {
        //         W_vs_MM2_mc_sec[i]->SetYTitle("MM^{2}\u00A0(GeV^{2})");
        //         W_vs_MM2_mc_sec[i]->SetXTitle("W (GeV)");
        //         W_vs_MM2_mc_sec[i]->SetOption("COLZ1");
        //         WvsMM2_mc_can->cd(i + 1);
        //         gPad->SetLogz();
        //         W_vs_MM2_mc_sec[i]->Draw("same");
        // }
        // WvsMM2_mc_can->Write();

        
}

void Histogram::makeHists_MM2withbins() 
{
        MM2_hists.resize(w_nBins);
        for (int w_bin = 0; w_bin < w_nBins; ++w_bin) 
        {
                MM2_hists[w_bin].resize(q2_nBins);
                for (int q2_bin = 0; q2_bin < q2_nBins; ++q2_bin) 
                {
                        // Create an output string stream
                        std::ostringstream oss;

                        // Set precision for the output stream
                        oss << std::setprecision(3) << std::fixed;

                        // Use bin edge values for the histogram name
                        oss << "MM2_W[" << w_bin_lower[w_bin] << "-" << w_bin_upper[w_bin]
                                << ")_Q^{2}[" << q2_bin_lower[q2_bin] << "-" << q2_bin_upper[q2_bin] << ")";

                        // Convert the output stream to a string
                        std::string hist_name = oss.str();

                        // Create the histogram with the new name
                        MM2_hists[w_bin][q2_bin] = std::make_shared<TH1D>(hist_name.c_str(), hist_name.c_str(), bins, mm2_min, mm2_max);
                }
        }

        MM2_hists_wide.resize(w_nBins);
        for (int w_bin = 0; w_bin < w_nBins; ++w_bin) 
        {
                MM2_hists_wide[w_bin].resize(q2_nBins);
                for (int q2_bin = 0; q2_bin < q2_nBins; ++q2_bin) 
                {
                        // Create an output string stream
                        std::ostringstream oss;

                        // Set precision for the output stream
                        oss << std::setprecision(3) << std::fixed;

                        // Use bin edge values for the histogram name
                        oss << "wide_MM2_W[" << w_bin_lower[w_bin] << "-" << w_bin_upper[w_bin]
                                << ")_Q^{2}[" << q2_bin_lower[q2_bin] << "-" << q2_bin_upper[q2_bin] << ")";

                        // Convert the output stream to a string
                        std::string hist_name_wide = oss.str();

                        // Create the histogram with the new name
                        MM2_hists_wide[w_bin][q2_bin] = std::make_shared<TH1D>(hist_name_wide.c_str(), hist_name_wide.c_str(), bins, mm2_min_wide, mm2_max_wide);
                }
        }
}

void Histogram::Fill_MM2withbins(const std::shared_ptr<Reaction> &_e) 
{
        double w_val = _e->W();
        double q2_val = _e->Q2();
        double MM2_val = 0.0;

        // Select the MM2 calculation based on topology
        if (topology == "excl") {
                MM2_val = _e->MM2_mProt();
        } else if (topology == "mProt") {
                MM2_val = _e->MM2_mProt();
        } else if (topology == "mPip") {
                MM2_val = _e->MM2_mPip();
        } else if (topology == "mPim") {
                MM2_val = _e->MM2_mPim();
        }

        // Loop over W bins
        for (int w_bin = 0; w_bin < w_nBins; ++w_bin) {
                // Check if w_val falls into the current W bin
                if (w_val >= w_bin_lower[w_bin] && (w_bin == w_nBins - 1 ? w_val <= w_bin_upper[w_bin] : w_val < w_bin_upper[w_bin])) {
                        // Loop over Q² bins
                        for (int q2_bin = 0; q2_bin < q2_nBins; ++q2_bin) {
                                // Check if q2_val falls into the current Q² bin
                                if (q2_val >= q2_bin_lower[q2_bin] && (q2_bin == q2_nBins - 1 ? q2_val <= q2_bin_upper[q2_bin] : q2_val < q2_bin_upper[q2_bin])) {
                                // Fill the correct histogram for this W and Q² bin
                                        if (w_bin < MM2_hists.size() && q2_bin < MM2_hists[w_bin].size()) {
                                                MM2_hists[w_bin][q2_bin]->Fill(MM2_val, _e->weight());
                                        }
                                        if (w_bin < MM2_hists_wide.size() && q2_bin < MM2_hists_wide[w_bin].size()) {
                                                MM2_hists_wide[w_bin][q2_bin]->Fill(MM2_val, _e->weight());
                                        }
                                }
                        }
                }
        }
}


void Histogram::Write_MM2withbins(TDirectory *Write_MM2_withbins_folder)
{
        // Loop over each Q² bin
        for (int q2_bin = 0; q2_bin < q2_nBins; ++q2_bin) 
        {
                // Create a subdirectory for each Q² bin based on the lower edge of the bin
                std::stringstream q2_folder_name;
                q2_folder_name << "Q2_[" << std::fixed << std::setprecision(3) << q2_bin_lower[q2_bin] << "-" <<
                        std::fixed << std::setprecision(3) << q2_bin_upper[q2_bin] << ")";
                TDirectory *q2_dir = Write_MM2_withbins_folder->mkdir(q2_folder_name.str().c_str());
                q2_dir->cd();  // Change to the Q²-specific subdirectory

                // Loop over each W bin and write the corresponding histogram into the Q² folder
                for (int w_bin = 0; w_bin < w_nBins; ++w_bin) 
                {
                        if (MM2_hists[w_bin][q2_bin] && MM2_hists[w_bin][q2_bin]->GetEntries()) 
                        {
                                MM2_hists[w_bin][q2_bin]->SetXTitle("MM2 (GeV^{2})");
                                MM2_hists[w_bin][q2_bin]->Write();  // Write the histogram into the subdirectory
                        }
                }

                // Create a subdirectory for each Q² bin based on the lower edge of the bin
                std::stringstream q2_folder_name_wide;
                q2_folder_name_wide << "wide_Q2_[" << std::fixed << std::setprecision(3) << q2_bin_lower[q2_bin] << "-" <<
                        std::fixed << std::setprecision(3) << q2_bin_upper[q2_bin] << ")";
                TDirectory *q2_dir_wide = Write_MM2_withbins_folder->mkdir(q2_folder_name_wide.str().c_str());
                q2_dir_wide->cd();  // Change to the Q²-specific subdirectory

                // Loop over each W bin and write the corresponding histogram into the Q² folder
                for (int w_bin = 0; w_bin < w_nBins; ++w_bin) 
                {
                        if (MM2_hists_wide[w_bin][q2_bin] && MM2_hists_wide[w_bin][q2_bin]->GetEntries()) 
                        {
                                MM2_hists_wide[w_bin][q2_bin]->SetXTitle("MM2 (GeV^{2})");
                                MM2_hists_wide[w_bin][q2_bin]->Write();  // Write the histogram into the subdirectory
                        }
                }
        }
}

void Histogram::makeHists_sector()
{
        for (short i = 0; i < num_sectors; i++)
        {
                W_sec[i] =
                    std::make_shared<TH1D>(Form("w_sec_%d", i + 1),
                                           Form("W Sector: %d", i + 1), bins, w_min, w_max);

                WvsQ2_sec[i] = std::make_shared<TH2D>(
                    Form("wvsq2_sec_%d", i + 1), Form("W vs. Q^{2}\u00A0Sector: %d", i + 1), bins,
                    w_min, w_max, bins, q2_min, q2_max);

                momvstheta_sec[i] = std::make_shared<TH2D>(
                    Form("momvstheta_sec%d", i + 1), Form("Momentum vs. theta Sector: %d", i + 1), bins,
                    p_min, p_max, bins, zero, 40);

                momvsphi_sec[i] = std::make_shared<TH2D>(
                    Form("momvsphi_sec%d", i + 1), Form("Momentum vs. phi Sector: %d", i + 1), bins,
                    p_min, p_max, bins, zero, 180);

                W_vs_sf_sec[i] = std::make_shared<TH2D>(
                    Form("W_vs_sf_sec_%d", i + 1), Form("W vs. sf Sector: %d", i + 1), bins,
                    w_min, w_max, bins, 0.15, 0.35);

                momvssf_sec[i] = std::make_shared<TH2D>(
                    Form("momvssf_sec_%d", i + 1), Form("Momentum vs. sf Sector: %d", i + 1), bins,
                    1.0, 10.0, bins, 0.10, 0.35);

                vz_sec[i] =
                    std::make_shared<TH1D>(Form("vz_sec_%d", i + 1),
                                           Form("vz Sector: %d", i + 1), bins, -10.0, 10.0);

                MM2_sec[i] =
                    std::make_shared<TH1D>(Form("mm2_sec_%d", i + 1),
                                           Form("MM2 Sector: %d", i + 1), bins, mm2_min, mm2_max);

                W_vs_MM2_sec[i] = std::make_shared<TH2D>(
                    Form("WvsMM2_sec_%d", i + 1), Form("W vs. MM^{2}\u00A0Sector: %d", i + 1), bins,
                    w_min, w_max, bins, mm2_min, mm2_max);

                Q2_vs_MM2_sec[i] = std::make_shared<TH2D>(
                    Form("Q2vsMM2_sec_%d", i + 1), Form("Q^{2}\u00A0 vs. MM^{2}\u00A0Sector: %d", i + 1), bins,
                    q2_min, q2_max, bins, mm2_min, mm2_max);
                
                W_mc_sec[i] =
                    std::make_shared<TH1D>(Form("w_mc_sec_%d", i + 1),
                                           Form("W mc Sector: %d", i + 1), bins, w_min, w_max);

                WvsQ2_mc_sec[i] = std::make_shared<TH2D>(
                    Form("wvsq2_mc_sec_%d", i + 1), Form("W vs. Q^{2}\u00A0mc Sector: %d", i + 1), bins,
                    w_min, w_max, bins, q2_min, q2_max);

                MM2_mc_sec[i] =
                    std::make_shared<TH1D>(Form("mm2_mc_sec_%d", i + 1),
                                           Form("MM2_mc Sector: %d", i + 1), bins, mm2_min, mm2_max);

                // W_vs_MM2_mc_sec[i] = std::make_shared<TH2D>(
                //     Form("WvsMM2_mc_sec_%d", i + 1), Form("W vs. MM^{2}\u00A0mc Sector: %d", i + 1), bins,
                //     w_min, w_max, bins, mm2_min, mm2_max);
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
        auto _cuts = std::make_unique<Pass2_Cuts>(data, dt);
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
        auto _cuts = std::make_unique<Pass2_Cuts>(data, dt);
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
void Histogram::Write_deltat(TDirectory *ctof_folder, TDirectory *ftof_folder, TDirectory *Write_deltat_folder)
{
        ftof_folder->cd();
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
        Write_deltat_folder->cd();

        ctof_folder->cd();
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
        Write_deltat_folder->cd();
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
                                    Form("Momentum vs. #beta %s %s %s", particle_name[p].c_str(),
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

// void Histogram::makeHists_MomVsMM2()
// {
//         for (short p = 0; p < particle_num; p++)
//         {
//                 for (short c = 0; c < charge_num; c++)
//                 {
//                         for (short i = 0; i < with_id_num; i++)
//                         {
//                                 Mom_vs_MM2_hist[p][c][i] = std::make_shared<TH2D>(
//                                     Form("Mom_vs_MM2_%s_%s_%s", particle_name[p].c_str(),
//                                          charge_name[c].c_str(), id_name[i].c_str()),
//                                     Form("Momentum vs. MM^{2}\u00A0 %s %s %s", particle_name[p].c_str(),
//                                          charge_name[c].c_str(), id_name[i].c_str()),
//                                     bins, p_min, p_max, bins, mm2_min, mm2_max);
//                                 // Add check to confirm initialization
//                                 if (!Mom_vs_MM2_hist[p][c][i]) {
//                                         std::cerr << "Failed to initialize histogram for " << particle_name[p] << " " << charge_name[c] << " " << id_name[i] << std::endl;
//                                 }
//                         }
//                 }
//         }
// }

// void Histogram::Fill_MomVsMM2(const std::shared_ptr<Branches12> &data, int part, const std::shared_ptr<Reaction> &_e)
// {
//         int good_ID = 0;
//         // float Q2 = data->Q2(part);
//         float mom = data->p(part);
//         double MM2_val = _e->MM2_exclusive();
//         double weight = _e->weight();
//         int charge = data->charge(part);
//         int pid = data->pid(part);
//         if (MM2_val != 0)
//         {
//                 momentum->Fill(mom);
//                 for (short p = 0; p < particle_num; p++)
//                 {
//                         switch (p)
//                         {
//                         case 0:
//                                 good_ID = ELECTRON;
//                                 break;
//                         case 1:
//                                 good_ID = PIP;
//                                 break;
//                         case 2:
//                                 good_ID = PROTON;
//                                 break;
//                         case 3:
//                                 good_ID = KP;
//                                 break;
//                          }

//                         // // Add debug prints to verify values
//                         // std::cout << "Filling histogram for particle " << particle_name[p] << " with mom: " << mom << " MM2: " << MM2 << " weight: " << weight << std::endl;
                         
//                         Mom_vs_MM2_hist[p][0][0]->Fill(mom, MM2_val, weight);

//                         if (good_ID == pid)
//                         {
//                                 Mom_vs_MM2_hist[p][0][1]->Fill(mom, MM2_val, weight);
//                         }
//                         else
//                         {
//                                 Mom_vs_MM2_hist[p][0][2]->Fill(mom, MM2_val, weight);
//                         }

//                         if (charge == -1)
//                         {

//                                 Mom_vs_MM2_hist[p][1][0]->Fill(mom, MM2_val, weight);
//                                 // if (good_ID == 11)
//                                 // if (-good_ID == pid)
//                                 if(-good_ID == pid || pid == ELECTRON) //from chris 7/24/24
//                                         Mom_vs_MM2_hist[p][1][1]->Fill(mom, MM2_val, weight);
//                                 else // if (-good_ID == pid)
//                                         Mom_vs_MM2_hist[p][1][2]->Fill(mom, MM2_val, weight);     
//                         }
//                         else if (charge == 1)
//                         {
//                                 Mom_vs_MM2_hist[p][0][0]->Fill(mom, MM2_val, weight);
//                                 // if (good_ID == pid)
//                                 if(good_ID == pid || pid == -ELECTRON) //from chris
//                                         Mom_vs_MM2_hist[p][0][1]->Fill(mom, MM2_val, weight);
//                                 else  // if (-good_ID == pid)
//                                         Mom_vs_MM2_hist[p][0][2]->Fill(mom, MM2_val, weight);       
//                         }       
//                 }
//         }
// }

// void Histogram::Write_MomVsMM2()
// {
//         momentum->SetXTitle("Momentum (GeV)");
//         momentum->Write();
//         for (short p = 0; p < particle_num; p++)
//         {
//                 for (short c = 0; c < charge_num; c++)
//                 {
//                         for (short i = 0; i < with_id_num; i++)
//                         {
//                                 Mom_vs_MM2_hist[p][c][i]->SetXTitle("Momentum (GeV)");
//                                 Mom_vs_MM2_hist[p][c][i]->SetYTitle("MM2 (GeV^2)");
//                                 Mom_vs_MM2_hist[p][c][i]->SetOption("COLZ1");
//                                 gPad->SetLogz();
//                                 Mom_vs_MM2_hist[p][c][i]->Write();
//                                 // Add check to confirm histogram has entries
//                                 if (Mom_vs_MM2_hist[p][c][i]->GetEntries() == 0) {
//                                         std::cerr << "Histogram " << Mom_vs_MM2_hist[p][c][i]->GetName() << " is empty!" << std::endl;
//                                 }
//                         }
//                 }
//         }
// }