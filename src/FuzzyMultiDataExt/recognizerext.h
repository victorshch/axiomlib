#ifndef RECOGNIZEREXT_H
#define RECOGNIZEREXT_H

#include "../AxiomExprSetPlus.h"
#include "../ReducedRecognizer.h"
#include "../TrajectorySampleDistance.h"

#include "boost/smart_ptr/shared_ptr.hpp"

#include <string>

namespace  AxiomLib {

class RecognizerExt
{
public:
    RecognizerExt(): mStretch(-1.0) {}
    RecognizerExt(const AxiomExprSetPlus& aesp, double stretch, std::string reducedRecognizerName);

    TrajectorySampleDistance getDistances(const std::vector<std::vector<double> > &trajectory) const;

private:
    AxiomExprSetPlus mAesp;
    double mStretch;
    boost::shared_ptr<ReducedRecognizer> mReducedRecognizer;
};

}

#endif // RECOGNIZEREXT_H
