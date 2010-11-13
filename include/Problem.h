/*
 * Problem.h
 *
 *  Created on: Oct 10, 2008
 *      Author: lars
 */

#ifndef PROBLEM_H_
#define PROBLEM_H_

#include "Function.h"
#include "Graph.h"
#include "MiniBucket.h"
#include "_base.h"
#include "gzstream.h"

/* holds a problem instance with variable domains and function tables */
class Problem {

protected:

  int m_prob;            // Problem class (multiplication or summation of costs)
  int m_task;            // Type of problem (Minim. or maxim. task)


  int m_n;               // No. of variables
  int m_nOrg;            // No. of variables (before evidence was removed)
  val_t m_k;             // Max. domain size
  int m_e;               // No. of evidence

  int m_c;               // No. of functions
  int m_r;               // Max. function arity

  double m_globalConstant;        // Global constant modifier for objective function

  double m_curCost;               // Cost of current solution

  string m_name;                  // Problem name

  vector<val_t> m_domains;        // Domain sizes of variables

  vector<Function*> m_functions;  // List of functions

  map<int,val_t> m_evidence;      // List of evidence as <index,value>

  map<int,int> m_old2new;         // Translation of variable names after removing evidence

  vector<val_t> m_curSolution;       // Current best solution

public:
  val_t getDomainSize(int i) const;
  double getGlobalConstant() const;

  int getN() const { return m_n; }
  int getNOrg() const { return m_nOrg; }
  val_t getK() const { return m_k; }
  int getE() const { return m_e; }
  int getC() const { return m_c; }
  int getR() const { return m_r; }

  const string& getName() const { return m_name; }

  const vector<Function*>& getFunctions() const { return m_functions; }
  const vector<val_t>& getDomains() const { return m_domains; }

public:

  // parses a UAI format input file
  bool parseUAI(const string& prob, const string& evid);

  // writes the current problem to a UAI file
  void writeUAI(const string& prob) const;

  // parses an ordering from file 'file' and stores it in 'elim'
  bool parseOrdering(const string& file, vector<int>& elim) const;
  // stores ordering from 'elim' in file 'file'
  void saveOrdering(const string& file, const vector<int>& elim) const;

  // removes evidence and unary-domain variables
  void removeEvidence();

  // report an updated solution
  void updateSolution(double cost,
#ifndef NO_ASSIGNMENT
      const vector<val_t>& sol,
#endif
      pair<size_t,size_t> nodes,
      bool output = true);

  // outputs the solution to the screen and, if file!="", writes it to file
  //  - cost is the MPE tuple value
  //  - sol is the optimal solution tuple
  //  - noNodes is the number of OR/AND nodes
  //  - nodeProf and leafProf are the full and leaf node profiles
  // if subprobOnly is true, only the variables from sol will be output to file (for subproblem solving)
  void outputAndSaveSolution(const string& file, pair<count_t,count_t> noNodes,
                             const vector<count_t>& nodeProf, const vector<count_t>& leafProf,
                             bool subprobOnly = false) const;

#ifndef NO_ASSIGNMENT
  /* returns true iff the index variable from the full set has been eliminated
   * as evidence or unary */
  bool isEliminated(int i) const;
#endif

  // adds the dummy variable to connect disconnected pseudo tree components
  void addDummy();

public:
  Problem();
  virtual ~Problem();
};


/* Inline definitions */

inline val_t Problem::getDomainSize(int i) const {
  assert (i<m_n);
  return m_domains[i];
}

inline double Problem::getGlobalConstant() const {
  return m_globalConstant;
}

inline void Problem::addDummy() {
  m_n += 1;
  m_domains.push_back(1); // unary domain
}


inline Problem::Problem() :
    m_prob(UNKNOWN),
    m_task(UNKNOWN),
    m_n(UNKNOWN),
    m_nOrg(UNKNOWN),
    m_k(UNKNOWN),
    m_e(UNKNOWN),
    m_c(UNKNOWN),
    m_r(UNKNOWN),
    m_globalConstant(ELEM_NAN),
    m_curCost(ELEM_NAN) {}

inline Problem::~Problem() {
  // delete functions
  for (vector<Function*>::iterator it = m_functions.begin(); it!= m_functions.end(); ++it) {
    if (*it) delete (*it);
  }
}



#endif /* PROBLEM_H_ */
