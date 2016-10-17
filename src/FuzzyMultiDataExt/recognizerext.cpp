#include "recognizerext.h"
#include "../ReducedRecognizerFactory.h"
#include "../Logger.h"
#include "../TrajectorySampleDistance.h"

#include <string>

using namespace AxiomLib;

RecognizerExt::RecognizerExt(const AxiomExprSetPlus &aesp, double stretch, std::string reducedRecognizerName)
{
    mAesp = aesp;
    mStretch = stretch;
    ReducedRecognizerFactory factory;
    mReducedRecognizer.reset(factory.create(reducedRecognizerName));
    if (!mReducedRecognizer.get()) {
        exception() << "Invalid ReducedRecognizer name: " << reducedRecognizerName;
    }
}

TrajectorySampleDistance RecognizerExt::getDistances(const std::vector<std::vector<double> >& trajectory) const
{
    std::vector<int> marking;
    mAesp.enter(marking, trajectory, 0, trajectory.front().size());
    TrajectorySampleDistance result((int)mAesp.markUps.size(), (int) trajectory.front().size());
    for (unsigned classNo = 0; classNo < mAesp.markUps.size(); ++classNo) {
        std::vector<double> dist;
        mReducedRecognizer->run(marking, mAesp.markUps[classNo], dist);
        result.setDistancesForClass(classNo, dist);
    }

    return result;
}
