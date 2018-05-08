#include "pseudotree.h"
#include "Main.h"

namespace pseudotree {

PseudoTree::PseudoTree(int argc, char** argv){


    /* define to enable diagnostic output of memory stats */
    //#define MEMDEBUG

    bayesnet bn;

    bn.set_filename("test");
    daoopt::Main main;

    if (!main.start())
        exit(1);
    if (!main.parseOptions(argc, argv))
        exit(1);
    if (!main.outputInfo())
        exit(1);
    if (!main.loadProblem())
        exit(1);
    if (!main.preprocessHeuristic())
        exit(1);
    if (!main.runSLS())
        exit(1);
    if (!main.findOrLoadOrdering())
        exit(1);
    //if (!main.initDataStructs())
    //  exit(1);
    //if (!main.compileHeuristic())
    //  exit(1);
    //if (!main.runLDS())
    //  exit(1);
    //if (!main.finishPreproc())
    //  exit(1);
    //if (!main.runSearch())
    //  exit(1);
    //if (!main.outputStats())
    //  exit(1);


}
}
