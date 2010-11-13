/*
 * ProgramOptions.h
 *
 *  Created on: Nov 15, 2008
 *      Author: lars
 */

#ifndef PROGRAMOPTIONS_H_
#define PROGRAMOPTIONS_H_

#include "boost/program_options.hpp"
#include "_base.h"

namespace po = boost::program_options;

#include <string>
#include <iostream>

struct ProgramOptions {
public:
  bool nosearch; // abort before starting the actual search
  bool autoCutoff; // enable automatic cutoff
  int ibound; // bucket elim. i-bound
  int cbound; // cache context size bound
  int cbound_worker; // cache bound for worker processes
  int threads; // max. number of parallel subproblems
  int order_iterations; // no. of randomized order finding iterations
  int cutoff_depth; // fixed cutoff depth for central search
  int cutoff_width; // fixed width for central cutoff
  int nodes_init; // number of nodes for local initialization (times 10^6)
  int memlimit; // memory limit (in MB)
  int cutoff_size; // fixed cutoff subproblem size (times 10^6)
  int local_size; // lower bound for problem size to be solved locally (times 10^6)
  int maxSubprob; // only generate this many subproblems, then abort (for testing)
  int lds;  // run initial LDS with this limit (-1: enabled)
  int seed; // the seed for the random number generator
  int stackLimit; // how many nodes to expand per subproblem stack before rotating

  double initialBound; // initial lower bound

  std::string executableName; // name of the executable
  std::string problemName; // name of the problem
  std::string in_problemFile; // problem file path
  std::string in_evidenceFile; // evidence file path
  std::string in_orderingFile; // ordering file path
  std::string in_subproblemFile; // subproblem file path
  std::string in_boundFile; // file with initial lower bound (from SLS, e.g.)
  std::string out_solutionFile; // file path to write solution to
  std::string out_reducedFile; // file to save reduced network to

public:
  ProgramOptions();
};

ProgramOptions parseCommandLine(int argc, char** argv);

inline ProgramOptions::ProgramOptions() :
		      nosearch(false), autoCutoff(true), ibound(0), cbound(0), cbound_worker(0),
		      threads(0), order_iterations(0), cutoff_depth(NONE), cutoff_width(NONE),
		      nodes_init(NONE), memlimit(NONE),
		      cutoff_size(NONE), local_size(NONE), maxSubprob(NONE),
		      lds(NONE), seed(NONE), stackLimit(0), initialBound(ELEM_NAN) {}

#endif /* PROGRAMOPTIONS_H_ */