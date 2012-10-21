#ifndef TAGGEDDATASET_H
#define TAGGEDDATASET_H

#include <string>
#include <vector>
#include <set>
#include <map>

#include "DataSetBase.h"

namespace AxiomLib {

class TaggedDataSet : public DataSetBase
{
public:
    TaggedDataSet();

    void load(const std::string& configFileName);

    std::vector<const MultiTS*> getTimeSeriesWithTags(const std::set<std::string>& tags) const;
    const MultiTS* getTimeSeriesWithTags(const std::set<std::string> &tags, int tsNo) const;

private:
    std::vector<MultiTS*> mTimeSeries;
    typedef std::vector<std::set<std::string> > TagVector;
    TagVector mTags;
};

}

#endif // TAGGEDDATASET_H
