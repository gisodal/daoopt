/*
 * SearchNode.h
 *
 *  Created on: Oct 9, 2008
 *      Author: lars
 */

#ifndef SEARCHNODE_H_
#define SEARCHNODE_H_

#include "_base.h"

class Problem;
class PseudotreeNode;

// some constants for aggregating the boolean flags
#define FLAG_LEAF 1 // node is a leaf node
#define FLAG_CACHABLE 2 // node is candidate for caching
#define FLAG_EXTERN 4 // subproblem was processed externally (in parallel setting)
#define FLAG_PRUNED 8 // subproblem below was pruned
#define FLAG_NOTOPT 16 // subproblem possibly not optimally solved (-> don't cache)

class SearchNode;

/*
struct SearchNodeComp {
  bool operator() (const SearchNode* a, const SearchNode* b) const;
};
*/

// data type to store child pointers
//typedef set<SearchNode*,SearchNodeComp> CHILDLIST;
typedef set<SearchNode*> CHILDLIST; // TODO


class SearchNode {
protected:
  unsigned char m_flags;             // for the boolean flags
  SearchNode* m_parent;              // pointer to the parent
  double m_nodeValue;                // node value (as in cost)
  double m_heurValue;                // heuristic estimate of the node's value
  CHILDLIST m_children;              // Child nodes
#ifndef NO_ASSIGNMENT
  vector<val_t> m_optAssignment;     // stores the optimal solution to the subproblem
#endif

public:
//  static size_t noNodes;

public:
  virtual int getType() const = 0;
  virtual int getVar() const = 0;
  virtual val_t getVal() const = 0;

  virtual void setValue(double) = 0;
  virtual double getValue() const = 0;
  virtual void setLabel(double) = 0;
  virtual double getLabel() const = 0;
  void setHeur(double d) { m_heurValue = d; }
  virtual double getHeur() const { return m_heurValue; }

  virtual void setCacheContext(const context_t&) = 0;
  virtual const context_t& getCacheContext() const = 0;

  virtual void setCacheInst(size_t i) = 0;
  virtual size_t getCacheInst() const = 0;

#ifdef PARALLEL_MODE
  virtual void setSubprobContext(const context_t&) = 0;
  virtual const context_t& getSubprobContext() const = 0;
  virtual void addPSTVal(std::pair<double,double>) = 0;
  virtual const std::list<std::pair<double,double> >& getPSTlist() const = 0;
#endif
  SearchNode* getParent() { return m_parent; }

  void addChild(SearchNode* node);
  const CHILDLIST& getChildren() const { return m_children; }
  bool hasChild(SearchNode* node) const;
  void eraseChild(SearchNode* node);
  void clearChildren();

#ifndef NO_ASSIGNMENT
  vector<val_t>& getOptAssig() { return m_optAssignment; }
  void setOptAssig(const vector<val_t>& assign) { m_optAssignment = assign; }
  void clearOptAssig() { vector<val_t> v; m_optAssignment.swap(v); }
#endif

  void setLeaf() { m_flags |= FLAG_LEAF; }
  bool isLeaf() const { return m_flags & FLAG_LEAF; }
  void setCachable() { m_flags |= FLAG_CACHABLE; }
  bool isCachable() const { return m_flags & FLAG_CACHABLE; }
  void setExtern() { m_flags |= FLAG_EXTERN; }
  bool isExtern() const { return m_flags & FLAG_EXTERN; }
  void setPruned() { m_flags |= FLAG_PRUNED; }
  bool isPruned() const { return m_flags & FLAG_PRUNED; }
  void setNotOpt() { m_flags |= FLAG_NOTOPT; }
  bool isNotOpt() const { return m_flags & FLAG_NOTOPT; }

  virtual void setHeurCache(double* d) = 0;
  virtual double* getHeurCache() const = 0;
  virtual void clearHeurCache() = 0;

protected:
  SearchNode(SearchNode* parent);

public:
  static bool heurLess(const SearchNode* a, const SearchNode* b);

public:
  virtual ~SearchNode();
};


class SearchNodeAND : public SearchNode {
protected:
  val_t m_val;          // Node value, assignment to OR parent variable
  double m_nodeLabel; // Label of arc <X_i,a>, i.e. instantiated function costs
  //static std::string emptyString;
  static context_t emptyCtxt;
  static std::list<std::pair<double,double> > emptyPSTList;

public:
  int getType() const { return NODE_AND; }
  int getVar() const { return (m_parent)?m_parent->getVar():UNKNOWN; }
  val_t getVal() const { return m_val; }

  void setValue(double d) { m_nodeValue = d; }
  double getValue() const { return m_nodeValue; }
  void setLabel(double d) { m_nodeLabel = d; }
  double getLabel() const { return m_nodeLabel; }

  // empty implementations, functions meaningless for AND nodes
  void setCacheContext(const context_t& c) { assert(false); }
  const context_t& getCacheContext() const { assert(false); return emptyCtxt; }
  void setCacheInst(size_t i) { assert(false); }
  size_t getCacheInst() const { assert(false); return 0; }
#ifdef PARALLEL_MODE
  void setSubprobContext(const context_t& t) { assert(false); }
  const context_t& getSubprobContext() const { assert(false); return emptyCtxt; }
  void addPSTVal(std::pair<double,double>) { assert(false); };
  const std::list<std::pair<double,double> >& getPSTlist() const {assert(false); return emptyPSTList;}
#endif

  // empty implementations, functions meaningless for AND nodes
  void setHeurCache(double* d) {}
  double* getHeurCache() const { return NULL; }
  void clearHeurCache() {}

public:
  SearchNodeAND(SearchNode* p, val_t val);
  ~SearchNodeAND() { /* empty */ }
};


class SearchNodeOR : public SearchNode {
protected:
  int m_var;             // Node variable
#ifdef PARALLEL_MODE
  size_t m_cacheInst;    // Cache instance counter
#endif
  double* m_heurCache;   // Stores the precomputed heuristic values of the AND children
  context_t m_cacheContext; // stores the context (for caching)

#ifdef PARALLEL_MODE
  context_t m_subprobContext;     // Stores the context values to this subproblem
  // For external subproblems: Holds the labels and values needed for
  // more effective pruning (i.e, from the central part of the search tree)
  std::list<pair<double,double> > m_PSTlist;
#endif

public:
  int getType() const { return NODE_OR; }
  int getVar() const { return m_var; }
  val_t getVal() const { assert(false); return NONE; } // no val for OR nodes!

  void setValue(double d) { m_nodeValue = d; }
  double getValue() const { return m_nodeValue; }
  void setLabel(double d) { assert(false); } // no label for OR nodes!
  double getLabel() const { assert(false); return 0; } // no label for OR nodes!

  double getHeur() const;

  void setCacheContext(const context_t& t) { m_cacheContext = t; }
  const context_t& getCacheContext() const { return m_cacheContext; }

#ifdef PARALLEL_MODE
  void setCacheInst(size_t i) { m_cacheInst = i; }
  size_t getCacheInst() const { return m_cacheInst; }
#else
  void setCacheInst(size_t i) { }
  size_t getCacheInst() const { return 0; }
#endif

#ifdef PARALLEL_MODE
  void setSubprobContext(const context_t& c) { m_subprobContext = c; }
  const context_t& getSubprobContext() const { return m_subprobContext; }
  void addPSTVal(std::pair<double,double> p) { m_PSTlist.push_front(p); }
  const std::list<std::pair<double,double> >& getPSTlist() const { return m_PSTlist; }
#endif

  void setHeurCache(double* d) { m_heurCache = d; }
  double* getHeurCache() const { return m_heurCache; }
  void clearHeurCache();

public:
  SearchNodeOR(SearchNode* parent, int var);
  ~SearchNodeOR();
};



// cout function
ostream& operator << (ostream&, const SearchNode&);


// Inline definitions
inline SearchNode::SearchNode(SearchNode* parent) :
  m_flags(0), m_parent(parent), m_nodeValue(ELEM_NAN), m_heurValue(INFINITY)
  // TODO? , m_nodeValue(ELEM_ZERO)
  { /* ++noNodes; */ }

inline SearchNode::~SearchNode() {
  for (CHILDLIST::iterator it = m_children.begin(); it!=m_children.end(); ++it)
    delete *it;
  //--noNodes;
  //  cout << "  deleting node " << endl;
}

inline void SearchNode::addChild(SearchNode* node) {
  assert(node);
  //m_children.push_back(node);
  m_children.insert(node);
}

inline bool SearchNode::hasChild(SearchNode* node) const {
  CHILDLIST::const_iterator it=m_children.begin();
  for (; it!=m_children.end(); ++it) {
    if (node == *it)
      return true;
  }
  return false;
}

inline ostream& operator << (ostream& os, const CHILDLIST& set) {
  os << '{' ;
  for (CHILDLIST::const_iterator it=set.begin(); it!=set.end();) {
    os << *it;
    if (++it != set.end()) os << ',';
  }
  cout << '}';
  return os;
}

inline void SearchNode::eraseChild(SearchNode* node) {
  CHILDLIST::iterator it = m_children.find(node);
  if (it != m_children.end()) {
    m_children.erase(it);
    delete node;
  }
}

inline void SearchNode::clearChildren() {
  CHILDLIST::iterator it = m_children.begin();
  for (;it!=m_children.end();++it)
    delete *it;
  m_children.clear();
}


inline void SearchNodeOR::clearHeurCache() {
  if (m_heurCache) {
    delete[] m_heurCache;
    m_heurCache = NULL;
  }
}


inline double SearchNodeOR::getHeur() const {
  if (m_children.empty()) return m_heurValue;
  double h = m_nodeValue;
  for (CHILDLIST::const_iterator it=m_children.begin(); it!=m_children.end(); ++it) {
    if ((*it)->getHeur() > h) h = (*it)->getHeur();
  }
  return h;
}


inline SearchNodeAND::SearchNodeAND(SearchNode* parent, val_t val) :
    SearchNode(parent), m_val(val), m_nodeLabel(ELEM_ONE)
{
  m_nodeValue = ELEM_ONE;
}


inline SearchNodeOR::SearchNodeOR(SearchNode* parent, int var) :
  SearchNode(parent), m_var(var), m_heurCache(NULL) //, m_cacheContext(NULL)
#ifdef PARALLEL_MODE
  //, m_subprobContext(NULL)
#endif // PARALLEL_MODE
  {}


inline SearchNodeOR::~SearchNodeOR() {
//  if (m_cacheContext) delete m_cacheContext;
#ifdef PARALLEL_MODE
//  if (m_subprobContext) delete m_subprobContext;
#endif
}

/*
inline bool SearchNodeComp::operator ()(const SearchNode* a, const SearchNode* b) const {
//  return a->getHeur() < b->getHeur();
  if (a->getHeur() == b->getHeur())
    return a<b; // TODO better criterion needed
  else
    return a->getHeur() < b->getHeur();
}
*/

inline bool SearchNode::heurLess(const SearchNode* a, const SearchNode* b) {
  assert(a && b);
  return a->getHeur() < b->getHeur();
}


#endif /* SEARCHNODE_H_ */
