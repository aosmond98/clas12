
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

        // momentum = std::make_shared<TH1D>("mom", "mom", bins, p_min, p_max);
        // W_hist = std::make_shared<TH1D>("W", "W", bins, zero, w_max);
        // MM2_hist = std::make_shared<TH1D>("MM2", "MM2", bins, -0.04, 0.04);
        // Q2_hist = std::make_shared<TH1D>("Q2", "Q2", bins, zero, q2_max);
        // W_vs_q2 = std::make_shared<TH2D>("W_vs_q2", "W_vs_q2", bins, zero, w_max,
        //                                  bins, zero, q2_max);
        // Mom_vs_MM2 = std::make_shared<TH2D>("Mom_vs_MM2", "Mom_vs_MM2", bins, p_min, 1,
                                        //  bins, -0.04, 0.04);
        WvsQ2_gen = std::make_shared<TH2D>("WvsQ2_gen", "WvsQ2_gen", bins, zero, w_max,
                                         bins, zero, q2_max);
        WvsQ2_rec = std::make_shared<TH2D>("WvsQ2_rec", "WvsQ2_rec", bins, zero, w_max,
                                         bins, zero, q2_max);
        // acceptance_hist = std::make_shared<TH2D>("acceptance", "acceptance", bins, zero, w_max,
        //                                  bins, zero, q2_max);

        // from krishna
        // TH1D *w_vs_q2_th = (TH1D *)mc_data->Get("W vs Q2/W_vs_q2_thrown");
	// TH1D *w_vs_q2_simu = (TH1D *)mc_data->Get("W vs Q2/WQ2_det_2");
	// TH1D *w_vs_q2_exp = (TH1D *)exp_data->Get("W vs Q2/WQ2_det_2");
	// TH1D *acce_w_vs_q2 = (TH1D *)w_vs_q2_simu->Clone("Accepatnce_w_vs_q2");

        // makeHists_deltat();
        // makeHists_MomVsBeta();
        // makeHists_sector();
        // makeHists_WvsQ2();
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
        TDirectory *WvsQ2_folder = RootOutputFile->mkdir("W vs Q2");
        WvsQ2_folder->cd();
        Write_WvsQ2();

        // std::cerr << BOLDBLUE << "WvsQ2_gen()" << DEF << std::endl;
        // TDirectory *WvsQ2_gen_folder = RootOutputFile->mkdir("W vs Q2 gen");
        // WvsQ2_gen_folder->cd();
        // Write_WvsQ2_gen();

        // std::cerr << BOLDBLUE << "Write_MomVsBeta()" << DEF << std::endl;
        // TDirectory *Write_MomVsBeta_folder = RootOutputFile->mkdir("Mom Vs Beta");
        // Write_MomVsBeta_folder->cd();
        // Write_MomVsBeta();

        // std::cerr << BOLDBLUE << "Write_deltat()" << DEF << std::endl;
        // TDirectory *Write_deltat_folder = RootOutputFile->mkdir("Delta_t");
        // Write_deltat_folder->cd();
        // Write_deltat();

        std::cerr << BOLDBLUE << "Done Writing!!" << DEF << std::endl;
}

void Histogram::Fill_WvsQ2_rec(const std::shared_ptr<Reaction> &_e)
{
        // W_hist->Fill(_e->W(), _e->weight());
        // MM2_hist->Fill(_e->MM2_exclusive(), _e->weight());
        // Q2_hist->Fill(_e->Q2(), _e->weight());
        // W_vs_q2->Fill(_e->W(), _e->Q2(), _e->weight());
        // Mom_vs_MM2->Fill(_e->Mom_excl(), _e->MM2_exclusive(), _e->weight());
        WvsQ2_rec->Fill(_e->W(), _e->Q2(), _e->weight());

        // short sec = _e->sec();
        // if (sec > 0 && sec <= 6)
        // {
        //         W_vs_q2_sec[sec - 1]->Fill(_e->W(), _e->Q2(), _e->weight());
        //         W_sec[sec - 1]->Fill(_e->W(), _e->weight());
        //         MM2_sec[sec - 1]->Fill(_e->MM2_exclusive(), _e->weight());
        //         Mom_vs_MM2_sec[sec - 1]->Fill(_e->Mom_excl(), _e->MM2_exclusive(), _e->weight());
        // }
}

void Histogram::Fill_WvsQ2_gen(const std::shared_ptr<MCReaction> &_e)
{
        WvsQ2_gen->Fill(_e->W_mc(), _e->Q2_mc(), _e->weight());
}

// void Histogram::Write_WvsQ2_rec()
// {
//        auto WvsQ2_can =
//             std::make_unique<TCanvas>("WvsQ2_can", "W vs Q2 sectors", 1920, 1080);
//         WvsQ2_can->Divide(3, 2);
//         for (short i = 0; i < num_sectors; i++)
//         {
//                 W_vs_q2_sec[i]->SetYTitle("Q^{2} (GeV^{2})");
//                 W_vs_q2_sec[i]->SetXTitle("W (GeV)");
//                 W_vs_q2_sec[i]->SetOption("COLZ1");
//                 WvsQ2_can->cd(i + 1);
//                 gPad->SetLogz();
//                 W_vs_q2_sec[i]->Draw("same");
//         }
//         WvsQ2_can->Write();

//         auto W_can = std::make_unique<TCanvas>("W_can", "W sectors", 1920, 1080);
//         W_can->Divide(3, 2);
//         for (short i = 0; i < num_sectors; i++)
//         {
//                 W_sec[i]->SetXTitle("W (GeV)");
//                 W_can->cd(i + 1);

//                 //  W_sec[i]->Fit("gaus", "QMR+", "QMR+", 0.85, 1.05);
//                 // gStyle->SetOptFit(01);
//                 W_sec[i]->Draw("same");
//         }
//         W_can->Write();

//         auto MM2_can = std::make_unique<TCanvas>("MM2_can", "MM2 sectors", 1920, 1080);
//         MM2_can->Divide(3, 2);
//         for (short i = 0; i < num_sectors; i++)
//         {
//                 MM2_sec[i]->SetXTitle("MM2 (GeV)");
//                 MM2_can->cd(i + 1);
//                 MM2_sec[i]->Draw("same");
//         }
//         MM2_can->Write();

//         auto MomvsMM2_can =
//             std::make_unique<TCanvas>("MomvsMM2_can", "Mom vs MM2 sectors", 1920, 1080);
//         MomvsMM2_can->Divide(3, 2);
//         for (short i = 0; i < num_sectors; i++)
//         {
//                 Mom_vs_MM2_sec[i]->SetYTitle("MM^{2} (GeV^{2})");
//                 Mom_vs_MM2_sec[i]->SetXTitle("Mom (GeV)");
//                 Mom_vs_MM2_sec[i]->SetOption("COLZ1");
//                 MomvsMM2_can->cd(i + 1);
//                 gPad->SetLogz();
//                 Mom_vs_MM2_sec[i]->Draw("same");
//         }
//         MomvsMM2_can->Write();

//         W_vs_q2->SetYTitle("Q^{2} (GeV^{2})");
//         W_vs_q2->SetXTitle("W (GeV)");
//         W_vs_q2->SetOption("COLZ1");
//         if (W_vs_q2->GetEntries())
//                 W_vs_q2->Write();

//         WvsQ2_rec->SetYTitle("Q^{2} (GeV^{2})");
//         WvsQ2_rec->SetXTitle("W (GeV)");
//         WvsQ2_rec->SetOption("COLZ1");
//         if (WvsQ2_rec->GetEntries())
//                 WvsQ2_rec->Write();

//         Mom_vs_MM2->SetYTitle("MM^{2} (GeV^{2})");
//         Mom_vs_MM2->SetXTitle("Mom (GeV)");
//         // Mom_vs_MM2->SetOption("COLZ1");
//         if (Mom_vs_MM2->GetEntries())
//                 Mom_vs_MM2->Write();
        
//         W_hist->SetXTitle("W (GeV)");
//         if (W_hist->GetEntries())
//                 W_hist->Write();

//         MM2_hist->SetXTitle("MM2 (GeV2)");
//         if (MM2_hist->GetEntries())
//                 MM2_hist->Write();        

//         Q2_hist->SetXTitle("Q^{2} (GeV^{2})");
//         if (Q2_hist->GetEntries())
//                 Q2_hist->Write();
// }

// void Histogram::Write_WvsQ2_gen()
// {
//         WvsQ2_gen->SetXTitle("W [Invariant Mass] (GeV/c^{2})");
//         WvsQ2_gen->SetYTitle("Q^{2} (GeV^{2})");
//         WvsQ2_gen->SetOption("COLZ1");
//         if (WvsQ2_gen->GetEntries()) WvsQ2_gen->Write();
// }

// void Histogram::Fill_WvsQ2(const std::shared_ptr<MCReaction> &_e_mc, const std::shared_ptr<Reaction> &_e){
// void Histogram::Fill_WvsQ2_gen(mc_event){
        // for (int i=0; i < W_nBins; i++){
        //         for (int j=0; j < Q2_nBins; j++){
                        // WvsQ2_gen[i][j] = data->Get<TH2D>("WvsQ2_gen/WvsQ2_gen");
                        // WvsQ2_gen[i][j] = (TH2D*)data->Get("WvsQ2_gen/WvsQ2_gen")
                        // WvsQ2_gen->Fill(_e->W_mc[i], _e->Q2_mc[j]);
                        // WvsQ2_gen->Fill(w_val(i), q2_val(j));
                        // WvsQ2_gen->Fill(_e_mc->W_mc(), _e_mc->Q2_mc(), _e_mc->weight());
                        // WvsQ2_rec->Fill(_e->W(), _e->Q2(), _e->weight());

                        // if (WvsQ2_gen->GetEntries() > 0 && WvsQ2_rec->GetEntries() > 0) {
                        //         if (WvsQ2_rec->GetNbinsX() == WvsQ2_gen->GetNbinsX() &&
                        //                 WvsQ2_rec->GetNbinsY() == WvsQ2_gen->GetNbinsY()) {
                        //                 TH2D *acceptance_hist = static_cast<TH2D*>(WvsQ2_rec->Clone("acceptance_hist"));
                        //                 acceptance_hist->Divide(WvsQ2_gen.get());
                        //         } else {
                        //                 std::cerr << "Error: Dimensions of WvsQ2_rec and WvsQ2_gen do not match." << std::endl;
                        //         }
                        // } else {
                        //         std::cerr << "Error: Empty histograms. Cannot perform division." << std::endl;
                        // }
                        // TH2D *acceptance_hist = static_cast<TH2D*>(WvsQ2_rec->Clone("acceptance_hist"));
                        // acceptance_hist->Divide(WvsQ2_gen.get());

        //         }
        // }
// }

void Histogram::Write_WvsQ2(){
        WvsQ2_gen->SetXTitle("W (GeV)");
        WvsQ2_gen->SetYTitle("Q2 (GeV^{2})");
        WvsQ2_gen->SetOption("COLZ1");
        if (WvsQ2_gen->GetEntries())
                WvsQ2_gen->Write();
                // gPad->SetLogz(); // Set the color scale to logarithmic

        WvsQ2_rec->SetXTitle("W (GeV)");
        WvsQ2_rec->SetYTitle("Q2 (GeV^{2})");
        WvsQ2_rec->SetOption("COLZ1"); 
        if (WvsQ2_rec->GetEntries())
                WvsQ2_rec->Write();
                // gPad->SetLogz(); // Set the color scale to logarithmic

        // acceptance_hist->SetXTitle("W (GeV)");
        // acceptance_hist->SetYTitle("Q2 (GeV^{2})");
        // if (acceptance_hist->GetEntries())
        //         acceptance_hist->Write();
}

// void Histogram::Fill_WvsQ2_rec(const std::shared_ptr<Reaction> &_e){
// // void Histogram::Fill_WvsQ2_rec(event){
//         // for (int i=0; i < W_nBins; i++){
//         //         for (int j=0; j < Q2_nBins; j++){
//                         // WvsQ2_rec->Fill(_e->W[i], _e->Q2[j]);
//                         // WvsQ2_rec->Fill(w_val(i), q2_val(j));
//                         // WvsQ2_rec->Fill(_e->W(), _e->Q2(), _e->weight());

//         //         }
//         // }
// }

// void Histogram::Write_WvsQ2_rec(){
//         WvsQ2_rec->SetXTitle("W (GeV)");
//         WvsQ2_rec->SetYTitle("Q2 (GeV^{2})");
//         if (WvsQ2_rec->GetEntries())
//                 WvsQ2_rec->Write();
// }

// void Histogram::makeHists_sector()
// {

//         for (short i = 0; i < num_sectors; i++)
//         {
//                 W_vs_q2_sec[i] = std::make_shared<TH2D>(
//                     Form("wvsq2_sec_%d", i + 1), Form("W vs Q^{2} Sector: %d", i + 1), bins,
//                     zero, w_max, bins, zero, q2_max);

//                 W_sec[i] =
//                     std::make_shared<TH1D>(Form("w_sec_%d", i + 1),
//                                            Form("W Sector: %d", i + 1), bins, zero, w_max);

//                 MM2_sec[i] =
//                     std::make_shared<TH1D>(Form("mm2_sec_%d", i + 1),
//                                            Form("MM2 Sector: %d", i + 1), bins, -0.04, 0.04);

//                 Mom_vs_MM2_sec[i] = std::make_shared<TH2D>(
//                     Form("MomvsMM2_sec_%d", i + 1), Form("Mom vs MM^{2} Sector: %d", i + 1), bins,
//                     p_min, 1, bins, -0.04, 0.04);

//         }
// }

// void Histogram::makeHists_deltat()
// {
//         std::string tof = "";
//         for (short p = 0; p < particle_num; p++)
//         {
//                 for (short c = 0; c < charge_num; c++)
//                 {
//                         for (short i = 0; i < with_id_num; i++)
//                         {
//                                 tof = "ftof";
//                                 delta_t_hist[p][c][i][0] = std::make_shared<TH2D>(
//                                     Form("delta_t_%s_%s_%s_%s", tof.c_str(), particle_name[p].c_str(),
//                                          charge_name[c].c_str(), id_name[i].c_str()),
//                                     Form("#Deltat %s %s %s %s", tof.c_str(), particle_name[p].c_str(),
//                                          charge_name[c].c_str(), id_name[i].c_str()),
//                                     bins, p_min, 6, bins, Dt_min, Dt_max);

//                                 tof = "ctof";
//                                 delta_t_hist[p][c][i][1] = std::make_shared<TH2D>(
//                                     Form("delta_t_%s_%s_%s_%s", tof.c_str(), particle_name[p].c_str(),
//                                          charge_name[c].c_str(), id_name[i].c_str()),
//                                     Form("#Deltat %s %s %s %s", tof.c_str(), particle_name[p].c_str(),
//                                          charge_name[c].c_str(), id_name[i].c_str()),
//                                     bins, 0, 3.0, bins, -6.0, 6.0);
//                         }
//                 }
//         }
// }

// void Histogram::Fill_deltat_pi(const std::shared_ptr<Branches12> &data,
//                                const std::shared_ptr<Delta_T> &dt, int part,
//                                const std::shared_ptr<Reaction> &_e)
// {
//         auto _cuts = std::make_unique<Cuts>(data, dt);
//         int charge = data->charge(part);
//         // bool fc = dt->ctof();
//         bool cd_part = (dt->ctof_particle(part) && data->status(part) > 4000);
//         bool fd_part = (data->status(part) > 2000 && data->status(part) < 4000);

//         int pid = data->pid(part);
//         float mom = data->p(part);
//         float time = NAN;

//         if (cd_part)
//         {
//                 time = dt->dt_ctof_Pi();
//         }
//         else if (fd_part)
//                 time = dt->dt_Pi();

//         if (charge == 1)
//         {
//                 if (cd_part)
//                         delta_t_hist[1][0][0][1]->Fill(mom, time, _e->weight());
//                 else if (fd_part)
//                         delta_t_hist[1][0][0][0]->Fill(mom, time, _e->weight());
                
//                 if (_cuts->IsPip(part))
//                 {
//                         if (cd_part)
//                                 delta_t_hist[1][0][1][1]->Fill(mom, time, _e->weight());
//                         else if (fd_part&& abs(time) < 0.5)
//                                 delta_t_hist[1][0][1][0]->Fill(mom, time, _e->weight());
//                 }
//         }
//         else if (charge == -1)
//         {
//                 if (cd_part)
//                         delta_t_hist[1][1][0][1]->Fill(mom, time, _e->weight());
//                 else if (fd_part)
//                         delta_t_hist[1][1][0][0]->Fill(mom, time, _e->weight());
                
//                 if (_cuts->IsPim(part))
//                 {
//                         if (cd_part)
//                                 delta_t_hist[1][1][1][1]->Fill(mom, time, _e->weight());
//                         else if (fd_part && abs(time) < 0.5)
//                                 delta_t_hist[1][1][1][0]->Fill(mom, time, _e->weight());
//                 }
//         }
// }

// void Histogram::Fill_deltat_prot(const std::shared_ptr<Branches12> &data,
//                                  const std::shared_ptr<Delta_T> &dt, int part,
//                                  const std::shared_ptr<Reaction> &_e)
// {
//         auto _cuts = std::make_unique<Cuts>(data, dt);
//         int status = abs(data->status(part));
//         int charge = data->charge(part);
//         // bool fc = dt->ctof();
//         bool cd_part = (dt->ctof_particle(part) && data->status(part) > 4000);
//         bool fd_part = (data->status(part) > 2000 && data->status(part) < 4000);

//         int pid = data->pid(part);
//         float mom = data->p(part);
//         float time = NAN;
//         float time1 = NAN;

//         if (cd_part)
//         {
//                 time = dt->dt_ctof_P();
//         }
//         else if (fd_part)
//         {
//                 time1 = dt->dt_P();
//         }
//         if (charge == 1)
//         {
//                 if (cd_part)
//                         delta_t_hist[2][0][0][1]->Fill(mom, time, _e->weight());
//                 else if (fd_part)
//                         delta_t_hist[2][0][0][0]->Fill(mom, time1, _e->weight());

//                 if (_cuts->IsProton(part))
//                 {
//                         if (fd_part && abs(time1) < 0.5)
//                                 delta_t_hist[2][0][1][0]->Fill(mom, time1, _e->weight());
//                         else if (cd_part)
//                                 delta_t_hist[2][0][1][1]->Fill(mom, time, _e->weight());
//                 }
//         }
// }

// // do
// // pid == 11 at first;
// // skim vs pid at 0 is 11. compare.

// // pid 11 is electron
// void Histogram::Write_deltat()
// {
//         // TDirectory *ftof_folder = RootOutputFile->mkdir("ftof");
//         // ftof_folder->cd();
//         for (short p = 0; p < particle_num; p++)
//         {
//                 for (short c = 0; c < charge_num; c++)
//                 {
//                         for (short i = 0; i < with_id_num; i++)
//                         {
//                                 delta_t_hist[p][c][i][0]->SetXTitle("Momentum (GeV)");
//                                 delta_t_hist[p][c][i][0]->SetYTitle("#Deltat");
//                                 delta_t_hist[p][c][i][0]->Draw("COLZ1");
//                                 gPad->SetLogz();
//                                 if (delta_t_hist[p][c][i][0]->GetEntries() > 1)
//                                         delta_t_hist[p][c][i][0]->Write();
//                         }
//                 }
//         }
//         // TDirectory *ctof_folder = RootOutputFile->mkdir("ctof");
//         // ctof_folder->cd();
//         for (short p = 0; p < particle_num; p++)
//         {
//                 for (short c = 0; c < charge_num; c++)
//                 {
//                         for (short i = 0; i < with_id_num; i++)
//                         {
//                                 delta_t_hist[p][c][i][1]->SetXTitle("Momentum (GeV)");
//                                 delta_t_hist[p][c][i][1]->SetYTitle("#Deltat");
//                                 delta_t_hist[p][c][i][1]->Draw("COLZ1");
//                                 gPad->SetLogz();
//                                 if (delta_t_hist[p][c][i][1]->GetEntries() > 1)
//                                         delta_t_hist[p][c][i][1]->Write();
//                         }
//                 }
//         }
// }

// void Histogram::makeHists_MomVsBeta()
// {
//         for (short p = 0; p < particle_num; p++)
//         {
//                 for (short c = 0; c < charge_num; c++)
//                 {
//                         for (short i = 0; i < with_id_num; i++)
//                         {
//                                 momvsbeta_hist[p][c][i] = std::make_shared<TH2D>(
//                                     Form("mom_vs_beta_%s_%s_%s", particle_name[p].c_str(),
//                                          charge_name[c].c_str(), id_name[i].c_str()),
//                                     Form("Momentum vs #beta %s %s %s", particle_name[p].c_str(),
//                                          charge_name[c].c_str(), id_name[i].c_str()),
//                                     bins, p_min, p_max, bins, zero, 1.2);
//                         }
//                 }
//         }
// }
// // added line after int part to hopefully make the weight work
// void Histogram::Fill_MomVsBeta(const std::shared_ptr<Branches12> &data, int part, const std::shared_ptr<Reaction> &_e)
// {
//         int good_ID = 0;
//         float beta = data->beta(part);
//         float mom = data->p(part);
//         int charge = data->charge(part);
//         int pid = data->pid(part);
//         if (beta != 0)
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
                         
//                         momvsbeta_hist[p][0][0]->Fill(mom, beta, _e->weight());

//                         // trying to make the x axis a log scale
//                         // gStyle->SetOptLogx(1); // Enable logarithmic scale on the x-axis.
//                         // gStyle->SetOptLogy(1); // Enable logarithmic scale on the y-axis.

//                         // also trying with log color pallette
//                         // gStyle->SetPalette(53); // 53 corresponds to the logarithmic color palette.

//                         if (good_ID == pid)
//                         {
//                                 momvsbeta_hist[p][0][1]->Fill(mom, beta, _e->weight());
//                         }
//                         else
//                         {
//                                 momvsbeta_hist[p][0][2]->Fill(mom, beta, _e->weight());
//                         }

//                         if (charge == -1)
//                         {

//                                 momvsbeta_hist[p][1][0]->Fill(mom, beta, _e->weight());
//                                 // if (good_ID == 11)
//                                 if (-good_ID == pid)
//                                         momvsbeta_hist[p][1][1]->Fill(mom, beta, _e->weight());
//                                 else // if (-good_ID == pid)
//                                         momvsbeta_hist[p][1][2]->Fill(mom, beta, _e->weight());     
//                         }
//                         else if (charge == 1)
//                         {
//                                 momvsbeta_hist[p][0][0]->Fill(mom, beta, _e->weight());
//                                 if (good_ID == pid)
//                                         momvsbeta_hist[p][0][1]->Fill(mom, beta, _e->weight());
//                                 else  // if (-good_ID == pid)
//                                         momvsbeta_hist[p][0][2]->Fill(mom, beta, _e->weight());       
//                         }       
//                 }
//         }
// }

// void Histogram::Write_MomVsBeta()
// {
//         momentum->SetXTitle("Momentum (GeV)");
//         momentum->Write();
//         for (short p = 0; p < particle_num; p++)
//         {
//                 for (short c = 0; c < charge_num; c++)
//                 {
//                         for (short i = 0; i < with_id_num; i++)
//                         {
//                                 momvsbeta_hist[p][c][i]->SetXTitle("Momentum (GeV)");
//                                 momvsbeta_hist[p][c][i]->SetYTitle("#beta");
//                                 momvsbeta_hist[p][c][i]->SetOption("COLZ1");
//                                 gPad->SetLogz();
//                                 momvsbeta_hist[p][c][i]->Write();
//                         }
//                 }
//         }
// }