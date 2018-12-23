#include "mclOntology.h"
#include <vector>

/** \file
 *  \brief code for constructing the ontologies.
 */

typedef vector<mclOntology *> ontologyVector;

class mclFrame;

ontologyVector *mclGenerateOntologies(mclFrame *f);
