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

// Helper function to check if a string contains a substring (case-sensitive)
bool contains(const std::string& str, const std::string& substr) 
{
  return str.find(substr) != std::string::npos;
}

template <class CutType>
size_t run(std::shared_ptr<TChain> _chain, const std::shared_ptr<Histogram>& _hists, int thread_id, const std::string& output_filename) 
{
  // Get the number of events in this thread
  size_t num_of_events = (int)_chain->GetEntries();

  // Determine the beam energy from the environment variable or set a default value
  float beam_energy = 10.6; // Default value
  if (getenv("BEAM_E") != NULL) beam_energy = atof(getenv("BEAM_E"));

  // Verify this is a generated data run based on filename
  bool is_gen_data = contains(output_filename, "gen");

  // Confirm output filename is for gen data
  if (!is_gen_data) {
    std::cerr << "Error: clas12_mc should only be used for generated data (expected 'gen' in filename).\n";
    return 0;
  }

  // Print some information for each thread
  std::cout << "=============== " << RED << "Thread " << thread_id << DEF << " =============== " << BLUE
            << num_of_events << " Events " << DEF << "===============\n";

  // Make a data object where all branches can be accessed (true for generated data)
  auto data = std::make_shared<Branches12>(_chain, true);

  // Total number of events processed
  size_t total = 0;
  size_t total_twopion_events = 0;

  // For each event
  for (size_t current_event = 0; current_event < num_of_events; current_event++) {
    // Get current event
    _chain->GetEntry(current_event);

    // Print progress for the 0th thread every 1000 events
    if (thread_id == 0 && current_event % 1000 == 0)
      std::cout << "\t" << (100 * current_event / num_of_events) << " %\r" << std::flush;

    // Ensure there are particles in generated data; skip events with no particles
    if (data->mc_npart() < 1) continue; 

    // If we pass electron cuts the event is processed
    total++;  // Increment processed events

    // Create a reaction class for generated data
    auto mc_event = std::make_shared<MCReaction>(data, beam_energy, "gen");

    // Check particle IDs and fill the reaction class
    for (int part = 1; part < data->mc_npart(); part++) {
      if (data->mc_pid(part) == PIP) {
        mc_event->SetMCPip(part);
      } else if (data->mc_pid(part) == PROTON) {
        mc_event->SetMCProton(part);
      } else if (data->mc_pid(part) == PIM) {
        mc_event->SetMCPim(part);
      }
    }
    _hists->Fill_WvsQ2_mc(mc_event);
  }

  std::cout << "Percent = " << 100.0 * total / num_of_events << std::endl;
  std::cout << " total number of events = " << total << std::endl;
  std::cout << " total number of two-pion events = " << total_twopion_events << std::endl;

  return num_of_events;
}

#endif
