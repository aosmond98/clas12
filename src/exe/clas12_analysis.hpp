#ifndef MAIN_H_GUARD
#define MAIN_H_GUARD

#include <iostream>
#include <string>
#include "TFile.h"
#include "TH1.h"
#include "branches.hpp"
#include "colors.hpp"
#include "cuts.hpp"
#include "histogram.hpp"
#include "reaction.hpp"

// check if a string contains a substring (case-sensitive)
static bool contains(const std::string& str, const std::string& substr) 
{
  return str.find(substr) != std::string::npos;
}

template <class CutType>
size_t run(std::shared_ptr<TChain> _chain, const std::shared_ptr<Histogram>& _hists, int thread_id, const std::string& output_filename) 
{
  // Get the number of events in this thread
  size_t num_of_events = (int)_chain->GetEntries();

  // Determine the beam energy from the environment variable or set default value
  float beam_energy = 10.6;  // default value

  // a check for cut type from Krishna's code
  if (std::is_same<CutType, Pass2_Cuts>::value)
  {
    if (thread_id == 0)
      std::cout << GREEN << "Using Pass2 RGA Cuts" << DEF << std::endl;
      
    beam_energy = 10.6041; // use this beam energy for pass2 cuts (applies even if a env var is provided later)
  }

  // get beam energy from environment variable if set (could put this before pass2 cuts above?)
  if (getenv("BEAM_E") != NULL) beam_energy = atof(getenv("BEAM_E"));

  // Determine the data type (rec or exp) based on output filename
  bool is_gen_data = contains(output_filename, "gen");
  bool is_rec_data = contains(output_filename, "rec");
  bool is_exp_data = contains(output_filename, "exp");

  // Ensure only one data type is selected
  if ((is_gen_data + is_rec_data + is_exp_data) > 1) {
    throw std::invalid_argument("Output filename must specify exactly one data type: gen, rec, or exp.");
  }

  // Determine the topology based on output filename
  bool is_topology_excl = contains(output_filename, "excl");
  bool is_topology_mProt = contains(output_filename, "mProt");
  bool is_topology_mPip = contains(output_filename, "mPip");
  bool is_topology_mPim = contains(output_filename, "mPim");

  // Ensure only one topology is selected
  if ((is_topology_excl + is_topology_mProt + is_topology_mPip + is_topology_mPim) > 1) {
    throw std::invalid_argument("Output filename must specify exactly one topology: excl, mProt, mPip, or mPim.");
  }

  // Print some information for each thread
  std::cout << "==========" << RED << " Thread " << thread_id << DEF 
            << " ========== " << BLUE << num_of_events << " Events " << DEF 
            << "==========" << GREEN << " Beam Energy " << beam_energy << DEF << " ==========\n";

  // Make a data object which all the branches can be accessed from
  auto data = (is_rec_data || is_gen_data)
                // ? std::make_shared<Branches12>(_chain, true)  // For rec and gen
                ? std::make_shared<Branches12>(_chain, true)  // For rec and gen; this is where Krishna's code defines _mc i think
                : std::make_shared<Branches12>(_chain);       // For exp

  // Total number of events "Processed"
  size_t total = 0;
  size_t total_twopion_events = 0;

  // For each event
  for (size_t current_event = 0; current_event < num_of_events; current_event++) 
  {
    // Get current event
    _chain->GetEntry(current_event);

    // Print progress for the 0th thread every 1000 events
    if (thread_id == 0 && current_event % 1000 == 0)
      std::cout << "\t" << (100 * current_event / num_of_events) << " %\r" << std::flush;

    /////// for generated (mc) events only 
    if (is_gen_data)
    {
      if (data->mc_weight() <= 0 || data->mc_npart() < 1)
        continue;
      
      // Create a reaction class for generated data
      auto mc_event = std::make_shared<MCReaction>(data, beam_energy, "gen");

      // Check particle IDs and fill the reaction class // krishna has part = 0 and for some reason i had part = 1?
      for (int part = 0; part < data->mc_npart(); part++)
      {
        if (data->mc_pid(part) == PIP)
        {
          mc_event->SetMCPip(part);
        } 
        if (data->mc_pid(part) == PROTON) 
        {
          mc_event->SetMCProton(part);
        } 
        if (data->mc_pid(part) == PIM) 
        {
          mc_event->SetMCPim(part);
        }
      }

      // krishna has the following cut but i haven't been using it to this point (5/7/25)
      // if (mc_event->W_mc() < 3.0 && mc_event->W_mc() > 1.0 && mc_event->Q2_mc() < 12.0 && mc_event->Q2_mc() > 1.0)
      // {
        _hists->Fill_WvsQ2_mc(mc_event);
      // }
    }

    //////// for rec and exp

    // commented out the following 2 lines bc krishna doesnt use them? also rearranging some of the following code
    // // Skip events for rec data with no particles
    // if (is_rec_data && data->mc_npart() < 1) continue;

    // Make a reaction class from the data given
    auto event = std::make_shared<Reaction>(data, beam_energy, is_rec_data ? "rec" : "exp");

    total++; // Increment processed events

    // Make cuts; changed make_shared to make_unique (bc Krishna has it that way)
    auto cuts = std::make_unique<Pass2_Cuts>(data);
    auto dt = std::make_shared<Delta_T>(data);

    if (!cuts->ElectronCuts()) continue;

    /////////////// particle loop //////////////////////

    // For each particle in the event
    for (int part = 1; part < data->gpart(); part++) 
    {
      dt->dt_calc(part);

      if ((data->charge(part) != ELECTRON) && (data->charge(part) != 0))
      {
        _hists->Fill_MomVsBeta(data, part, event);

        if (data->charge(part) == POSITIVE)
        {
          // insert proton and pip pid cuts (see krishna's code)

          if (cuts->IsProton(part))
          {
            event->SetProton(part);
          }
          else if (cuts->IsPip(part)) 
          {
            event->SetPip(part);
          }
        } 
        else 
        {
          if (cuts->IsPim(part))
          {
            event->SetPim(part);
          }
        }
      }
      _hists->Fill_deltat_pi(data, dt, part, event);
      _hists->Fill_deltat_prot(data, dt, part, event);
      // _hists->Fill_MomVsMM2(data, part, event); // test moving these?
    }

    ///////// this part is very different from krishna's. im keeping it as is for now
    ////// since i think it helps the code work with the naming convention i use but
    ////// need to confirm this still works fine logically

    double q2_min_analysis = -1.0, q2_max_analysis = 30.0;
    double w_min_analysis = 1.0, w_max_analysis = 2.5;

    // Dynamically set W and Q2 limits based on BEAM_E
    if (getenv("BEAM_E") != NULL) {
      double beam_energy = atof(getenv("BEAM_E"));
      if (beam_energy < 3) {
        q2_max_analysis = 1.0;
        w_max_analysis = 3.5;
        w_min_analysis = 0.9;
      } else if (beam_energy < 11) {
        q2_min_analysis = 0.0;
        q2_max_analysis = 12.0;
        w_min_analysis = 1.0;
        w_max_analysis = 2.5;
      } else if (beam_energy < 24) {
        q2_min_analysis = 2.0;
        q2_max_analysis = 30.0;
        w_min_analysis = 1.0;
        w_max_analysis = 2.5;
      }
    }

    // Update the condition to use the dynamically set limits
    if ((is_topology_excl && event->TwoPion_exclusive()) ||
        (is_topology_mProt && event->TwoPion_missingProt()) ||
        (is_topology_mPip && event->TwoPion_missingPip()) ||
        (is_topology_mPim && event->TwoPion_missingPim())) {
      if (event->W() > w_min_analysis && event->W() < w_max_analysis && 
          event->Q2() > q2_min_analysis && event->Q2() < q2_max_analysis && 
          event->weight() > 0.0) {
        _hists->Fill_WvsQ2(event, data);
        _hists->Fill_MM2(event, data);
        _hists->Fill_MM2withbins(event);
        // _hists->Fill_Q2binsvsMM2(event);
        total_twopion_events++;
      }
    }
  }

  std::cout << "Percent = " << 100.0 * total / num_of_events << std::endl;
  std::cout << " total no of events = " << total << std::endl;
  std::cout << " total no of twopion events = " << total_twopion_events << std::endl;

  return num_of_events;
}

#endif