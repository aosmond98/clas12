
#include "histogram.hpp"
#include <TRandom.h> 

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

        W = std::make_shared<TH1D>("W", "W", bins, zero, w_max);
        Q2 = std::make_shared<TH1D>("Q2", "Q2", bins, zero, q2_max);
        WvsQ2 = std::make_shared<TH2D>("WvsQ2", "WvsQ2", bins, zero, w_max,
                                         bins, zero, q2_max);

        MM2 = std::make_shared<TH1D>("MM2", "MM2", bins, -0.04, 0.04);
        W_vs_MM2 = std::make_shared<TH2D>("W_vs_MM2", "W_vs_MM2", bins, zero, w_max,
                                         bins, -0.04, 0.04);

        W_mc = std::make_shared<TH1D>("W_mc", "W_mc", bins, zero, w_max);
        Q2_mc = std::make_shared<TH1D>("Q2_mc", "Q2_mc", bins, zero, q2_max);
        WvsQ2_mc = std::make_shared<TH2D>("WvsQ2_mc", "WvsQ2_mc", bins, zero, w_max,
                                         bins, zero, q2_max);
        
        // from krishna
        // TH1D *w_vs_q2_th = (TH1D *)mc_data->Get("W vs Q2/W_vs_q2_thrown");
	// TH1D *w_vs_q2_simu = (TH1D *)mc_data->Get("W vs Q2/WQ2_det_2");
	// TH1D *w_vs_q2_exp = (TH1D *)exp_data->Get("W vs Q2/WQ2_det_2");
	// TH1D *acce_w_vs_q2 = (TH1D *)w_vs_q2_simu->Clone("Accepatnce_w_vs_q2");

        makeHists_deltat();
        makeHists_MomVsBeta();
        makeHists_MomVsQ2();
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

        std::cerr << BOLDBLUE << "Write_MomVsQ2()" << DEF << std::endl;
        TDirectory *Write_MomVsQ2_folder = RootOutputFile->mkdir("Mom Vs Q2");
        Write_MomVsQ2_folder->cd();
        Write_MomVsQ2();

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

        std::cerr << BOLDBLUE << "Done Writing!!" << DEF << std::endl;
}

void Histogram::Fill_WvsQ2(const std::shared_ptr<Reaction> &_e)
{
        W->Fill(_e->W(), _e->weight());
        Q2->Fill(_e->Q2(), _e->weight());
        WvsQ2->Fill(_e->W(), _e->Q2(), _e->weight());

        short sec = _e->sec();
        if (sec > 0 && sec <= 6)
        {
                W_sec[sec - 1]->Fill(_e->W(), _e->weight());
                WvsQ2_sec[sec - 1]->Fill(_e->W(), _e->Q2(), _e->weight());
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

        Q2->SetXTitle("Q^{2} (GeV^{2})");
        if (Q2->GetEntries())
                Q2->Write();

        WvsQ2->SetXTitle("W (GeV)");
        WvsQ2->SetYTitle("Q2 (GeV^{2})");
        WvsQ2->SetOption("COLZ1");
        if (WvsQ2->GetEntries())
                WvsQ2->Write();

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
            std::make_unique<TCanvas>("WvsQ2_can", "W vs Q2 sectors", 1920, 1080);
        WvsQ2_can->Divide(3, 2);
        for (short i = 0; i < num_sectors; i++)
        {
                WvsQ2_sec[i]->SetYTitle("Q^{2} (GeV^{2})");
                WvsQ2_sec[i]->SetXTitle("W (GeV)");
                WvsQ2_sec[i]->SetOption("COLZ1");
                WvsQ2_can->cd(i + 1);
                gPad->SetLogz();
                WvsQ2_sec[i]->Draw("same");
        }
        WvsQ2_can->Write();

        W_mc->SetXTitle("W (GeV)");
        if (W_mc->GetEntries())
                W_mc->Write();

        Q2_mc->SetXTitle("Q^{2} (GeV^{2})");
        if (Q2_mc->GetEntries())
                Q2_mc->Write();

        WvsQ2_mc->SetXTitle("W (GeV)");
        WvsQ2_mc->SetYTitle("Q2 (GeV^{2})");
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
            std::make_unique<TCanvas>("WvsQ2_mc_can", "W vs Q2 mc sectors", 1920, 1080);
        WvsQ2_mc_can->Divide(3, 2);
        for (short i = 0; i < num_sectors; i++)
        {
                WvsQ2_mc_sec[i]->SetYTitle("Q^{2} (GeV^{2})");
                WvsQ2_mc_sec[i]->SetXTitle("W (GeV)");
                WvsQ2_mc_sec[i]->SetOption("COLZ1");
                WvsQ2_mc_can->cd(i + 1);
                gPad->SetLogz();
                WvsQ2_mc_sec[i]->Draw("same");
        }
        WvsQ2_mc_can->Write();
}

void Histogram::Fill_MM2(const std::shared_ptr<Reaction> &_e)
{
        MM2->Fill(_e->MM2_exclusive(), _e->weight());
        W_vs_MM2->Fill(_e->W(), _e->MM2_exclusive(), _e->weight());

        short sec = _e->sec();
        if (sec > 0 && sec <= 6)
        {
                MM2_sec[sec - 1]->Fill(_e->MM2_exclusive(), _e->weight());
                W_vs_MM2_sec[sec - 1]->Fill(_e->W(), _e->MM2_exclusive(), _e->weight());
        }
}

void Histogram::Write_MM2()
{
        MM2->SetXTitle("MM2 (GeV2)");
        if (MM2->GetEntries())
                MM2->Write(); 

        W_vs_MM2->SetYTitle("MM^{2} (GeV^{2})");
        W_vs_MM2->SetXTitle("W (GeV)");
        // W_vs_MM2->SetOption("COLZ1");
        if (W_vs_MM2->GetEntries())
                W_vs_MM2->Write();  

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
            std::make_unique<TCanvas>("WvsMM2_can", "W vs MM2 sectors", 1920, 1080);
        WvsMM2_can->Divide(3, 2);
        for (short i = 0; i < num_sectors; i++)
        {
                W_vs_MM2_sec[i]->SetYTitle("MM^{2} (GeV^{2})");
                W_vs_MM2_sec[i]->SetXTitle("W (GeV)");
                W_vs_MM2_sec[i]->SetOption("COLZ1");
                WvsMM2_can->cd(i + 1);
                gPad->SetLogz();
                W_vs_MM2_sec[i]->Draw("same");
        }
        WvsMM2_can->Write();
}

void Histogram::makeHists_MM2withbins() {
        MM2_hists.resize(w_nBins);
        for (int w_bin = 0; w_bin < w_nBins; ++w_bin) 
        {
                MM2_hists[w_bin].resize(q2_nBins);
                for (int q2_bin = 0; q2_bin < q2_nBins; ++q2_bin) 
                {
                        std::string hist_name = "MM2_W_" + std::to_string(w_bin_ranges[w_bin]) +
                                                "_to_" + std::to_string(w_bin_ranges[w_bin + 1]) +
                                                "_Q2_" + std::to_string(q2_bin_ranges[q2_bin]) +
                                                "_to_" + std::to_string(q2_bin_ranges[q2_bin + 1]);

                        MM2_hists[w_bin][q2_bin] = std::make_shared<TH1D>(hist_name.c_str(), hist_name.c_str(), bins, -0.04, 0.04);
                }
        }
}

void Histogram::Fill_MM2withbins(const std::shared_ptr<Reaction> &_e)
{
        double w_val = _e->W();
        double q2_val = _e->Q2();
        double mm2_val = _e->MM2_exclusive();

        for (int w_bin = 0; w_bin < w_nBins; ++w_bin)
        {
                for (int q2_bin = 0; q2_bin < q2_nBins; ++q2_bin)
                {
                        double w_min = w_bin_ranges[w_bin];
                        double w_max = w_bin_ranges[w_bin + 1];
                        double q2_min = q2_bin_ranges[q2_bin];
                        double q2_max = q2_bin_ranges[q2_bin + 1];

                        // put w and q2 data values in bin ranges for plotting purposes
                        if (w_val >= w_min && w_val < w_max && q2_val >= q2_min && q2_val < q2_max)
                        {
                                if (w_bin < MM2_hists.size() && q2_bin < MM2_hists[w_bin].size()) {
                                        MM2_hists[w_bin][q2_bin]->Fill(mm2_val, _e->weight());
                                }
                        }
                }
        }        
}

void Histogram::Write_MM2withbins(TDirectory *Write_MM2_withbins_folder) {
        for (int w_bin = 0; w_bin < w_nBins; ++w_bin) 
        {
                // Create a folder for each w range
                std::stringstream folder_name;
                folder_name << "W_" << std::fixed << std::setprecision(3) << w_bin_ranges[w_bin]
                        << "_to_" << std::fixed << std::setprecision(3) << w_bin_ranges[w_bin + 1];
                TDirectory *w_bin_folder = Write_MM2_withbins_folder->mkdir(folder_name.str().c_str());
                w_bin_folder->cd();

                for (int q2_bin = 0; q2_bin < q2_nBins; ++q2_bin) 
                {
                        if (MM2_hists[w_bin][q2_bin] && MM2_hists[w_bin][q2_bin]->GetEntries()) 
                                MM2_hists[w_bin][q2_bin]->GetXaxis()->SetTitle("MM2 (GeV^2)");
                                MM2_hists[w_bin][q2_bin]->Write();
                }
        }
}

void Histogram::makeHists_sector()
{
        for (short i = 0; i < num_sectors; i++)
        {
                W_sec[i] =
                    std::make_shared<TH1D>(Form("w_sec_%d", i + 1),
                                           Form("W Sector: %d", i + 1), bins, zero, w_max);

                WvsQ2_sec[i] = std::make_shared<TH2D>(
                    Form("wvsq2_sec_%d", i + 1), Form("W vs Q^{2} Sector: %d", i + 1), bins,
                    zero, w_max, bins, zero, q2_max);

                MM2_sec[i] =
                    std::make_shared<TH1D>(Form("mm2_sec_%d", i + 1),
                                           Form("MM2 Sector: %d", i + 1), bins, -0.04, 0.04);

                W_vs_MM2_sec[i] = std::make_shared<TH2D>(
                    Form("WvsMM2_sec_%d", i + 1), Form("W vs MM^{2} Sector: %d", i + 1), bins,
                    p_min, 1, bins, -0.04, 0.04);
                
                W_mc_sec[i] =
                    std::make_shared<TH1D>(Form("w_mc_sec_%d", i + 1),
                                           Form("W mc Sector: %d", i + 1), bins, zero, w_max);

                WvsQ2_mc_sec[i] = std::make_shared<TH2D>(
                    Form("wvsq2_mc_sec_%d", i + 1), Form("W vs Q^{2} mc Sector: %d", i + 1), bins,
                    zero, w_max, bins, zero, q2_max);

                // MM2_mc_sec[i] =
                //     std::make_shared<TH1D>(Form("mm2_mc_sec_%d", i + 1),
                //                            Form("MM2_mc Sector: %d", i + 1), bins, -0.04, 0.04);

                // W_vs_MM2_mc_sec[i] = std::make_shared<TH2D>(
                //     Form("WvsMM2_mc_sec_%d", i + 1), Form("W vs MM^{2} mc Sector: %d", i + 1), bins,
                //     p_min, 1, bins, -0.04, 0.04);
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

void Histogram::makeHists_MomVsQ2()
{
        for (short p = 0; p < particle_num; p++)
        {
                for (short c = 0; c < charge_num; c++)
                {
                        for (short i = 0; i < with_id_num; i++)
                        {
                                Mom_vs_Q2_hist[p][c][i] = std::make_shared<TH2D>(
                                    Form("Mom_vs_Q2_%s_%s_%s", particle_name[p].c_str(),
                                         charge_name[c].c_str(), id_name[i].c_str()),
                                    Form("Momentum vs Q2 %s %s %s", particle_name[p].c_str(),
                                         charge_name[c].c_str(), id_name[i].c_str()),
                                    bins, p_min, p_max, bins, zero, q2_max);
                        }
                }
        }
}

void Histogram::Fill_MomVsQ2(const std::shared_ptr<Branches12> &data, int part, const std::shared_ptr<Reaction> &_e)
{
        int good_ID = 0;
        // float Q2 = data->Q2(part);
        float mom = data->p(part);
        int charge = data->charge(part);
        int pid = data->pid(part);
        if (_e->Q2() != 0)
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
                         
                        Mom_vs_Q2_hist[p][0][0]->Fill(mom, _e->Q2(), _e->weight());

                        if (good_ID == pid)
                        {
                                Mom_vs_Q2_hist[p][0][1]->Fill(mom, _e->Q2(), _e->weight());
                        }
                        else
                        {
                                Mom_vs_Q2_hist[p][0][2]->Fill(mom, _e->Q2(), _e->weight());
                        }

                        if (charge == -1)
                        {

                                Mom_vs_Q2_hist[p][1][0]->Fill(mom, _e->Q2(), _e->weight());
                                // if (good_ID == 11)
                                if (-good_ID == pid)
                                        Mom_vs_Q2_hist[p][1][1]->Fill(mom, _e->Q2(), _e->weight());
                                else // if (-good_ID == pid)
                                        Mom_vs_Q2_hist[p][1][2]->Fill(mom, _e->Q2(), _e->weight());     
                        }
                        else if (charge == 1)
                        {
                                Mom_vs_Q2_hist[p][0][0]->Fill(mom, _e->Q2(), _e->weight());
                                if (good_ID == pid)
                                        Mom_vs_Q2_hist[p][0][1]->Fill(mom, _e->Q2(), _e->weight());
                                else  // if (-good_ID == pid)
                                        Mom_vs_Q2_hist[p][0][2]->Fill(mom, _e->Q2(), _e->weight());       
                        }       
                }
        }
}

void Histogram::Write_MomVsQ2()
{
        momentum->SetXTitle("Momentum (GeV)");
        momentum->Write();
        for (short p = 0; p < particle_num; p++)
        {
                for (short c = 0; c < charge_num; c++)
                {
                        for (short i = 0; i < with_id_num; i++)
                        {
                                Mom_vs_Q2_hist[p][c][i]->SetXTitle("Momentum (GeV)");
                                Mom_vs_Q2_hist[p][c][i]->SetYTitle("Q2");
                                Mom_vs_Q2_hist[p][c][i]->SetOption("COLZ1");
                                gPad->SetLogz();
                                Mom_vs_Q2_hist[p][c][i]->Write();
                        }
                }
        }
}