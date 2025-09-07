#include <future>
#include <thread>
#include <iostream>
#include <chrono>
#include <string>
#include <vector>
#include "clas12_mc.hpp"
#include "histogram.hpp"
#include "TChain.h"


int main(int argc, char** argv) {
  // Ensures ROOT's thread safety
  ROOT::EnableThreadSafety();

  // Set the number of threads
  int NUM_THREADS = 4;
  if (getenv("NUM_THREADS") != NULL) NUM_THREADS = atoi(getenv("NUM_THREADS"));
  if (NUM_THREADS > argc - 2) NUM_THREADS = 1;  // Ensure thread count doesn't exceed input files

  // Vector of file names for each thread
  std::vector<std::vector<std::string>> infilenames(NUM_THREADS);
  // Output file name
  std::string outfilename;

  // Check if there are enough arguments
  if (argc >= 2) {
    // First argument is the output file
    outfilename = argv[1];
    // All other files are split evenly by the number of threads
    for (int i = 2; i < argc; i++) {
      infilenames[i % NUM_THREADS].push_back(argv[i]);
    }
  } else {
    std::cerr << "Not enough arguments! Usage: ./program outputfile inputfiles...\n";
    return 1;
  }

  // Ensure output filename specifies "gen" for generated data
  if (!contains(outfilename, "gen")) {
    std::cerr << "Output filename must specify 'gen' to indicate generated data." << std::endl;
    return 1;
  }

  // Define the shared Histogram object for all threads
  auto hists = std::make_shared<Histogram>(outfilename);

  // Function to run files on each thread
  auto run_files = [&hists, &outfilename](std::vector<std::string> inputs, int thread_id) {
    // Create a new TChain for this thread
    auto chain = std::make_shared<TChain>("clas12");
    // Add each file to the chain
    for (const auto& file : inputs) chain->Add(file.c_str());

    // Run the main processing function (gen data only in this case)
    return run<Cuts>(std::move(chain), hists, thread_id, outfilename);
  };

  // Start timing execution
  auto start = std::chrono::high_resolution_clock::now();
  size_t events = 0;

  // Run tasks asynchronously for each thread
  std::future<size_t> threads[NUM_THREADS];
  for (size_t i = 0; i < NUM_THREADS; i++) {
    threads[i] = std::async(run_files, infilenames[i], i);
  }

  // Retrieve event count from each thread
  for (size_t i = 0; i < NUM_THREADS; i++) {
    events += threads[i].get();
  }

  // Calculate and display elapsed time and processing rate
  std::chrono::duration<double> elapsed = std::chrono::high_resolution_clock::now() - start;
  std::cout << "Total time: " << elapsed.count() << " sec\n";
  std::cout << "Processing rate: " << events / elapsed.count() << " Hz\n";
  return 0;
}