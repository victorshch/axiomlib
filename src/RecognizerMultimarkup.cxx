#include "RecognizerMultimarkup.h"
#include "MultiMarking/dtwmetric.h"
#include "Environment.h"
#include "AxiomSet.h"
#include "DataSet.h"

#include <boost/algorithm/string/case_conv.hpp>

namespace AxiomLib {


    void compare (std::vector<bool>& m, const std::vector<bool> &p){
        for (int i=0;i<m.size();i++){
         if (p[i])
             m[i]=true;
        }
    }
    double minimum (const std::vector<double> &r){
        double min=r[0];
        for(int i=0;i<r.size();i++){
            if (r[i]<min)
                min=r[i];
        }
        return min;
    }

    // Вычисление расстояний от участков разметок тестовых траекторий до разметок эталонных траекторий

    void RecognizerMultiMarkup::computeDistances(AxiomSet& axiomSet, DataSet& dataSet, std::vector<int>& params, // номера столбцов кторые надо использовать
                                                              // расстояния от участков разметок тестовых траекторий до разметок эталонных траекторий
                                                              std::vector<TrajectorySampleDistance>& result,
                                                              // какие аксиомы срабатывали при разметке эталонных траекторий
                                                              std::vector<bool>& refAxiomUsage,
                                                              // какие аксиомы срабатывали при разметке тестовых траекторий
                                                              std::vector<bool>& testAxiomUsage) {
        std::vector<bool> temp_testAxiomUsage;
        std::vector<bool> temp_refAxiomUsage;
        temp_testAxiomUsage.resize(axiomSet.size());
        temp_refAxiomUsage.resize(axiomSet.size());
        testAxiomUsage.resize(axiomSet.size());
        refAxiomUsage.resize(axiomSet.size());

        vector <MultiMarking::MultiMark> etalon(dataSet.getNumberOfClasses());
        std::vector<double> vec;
        for (int i=0;i<dataSet.getNumberOfClasses();i++){
            if (!(dataSet.getTSByIndexFromClass(vec, i , 0, params[0])))
                throw AxiomLibException("Can not create row");
            axiomSet.enter(etalon[i], vec , 0 , vec.size(), temp_refAxiomUsage);
            compare (refAxiomUsage,temp_refAxiomUsage);
        }
        std::vector<int> numOfTS;
        int k;
        dataSet.getTestSize(k,numOfTS);
        vector <MultiMarking::MultiMark> tests (k);
        for (int i=0;i<k;i++){
            if (!(dataSet.getTSByIndexFromTests(vec, i, params[0])))
                throw AxiomLibException("Can not create row");
            axiomSet.enter(tests[i], vec , 0 , vec.size(), temp_testAxiomUsage);
            compare (testAxiomUsage,temp_testAxiomUsage);
        }


       result.reserve(tests.size());
       for(int i = 0; i < tests.size(); ++i) {
           result.push_back(TrajectorySampleDistance(dataSet.getNumberOfClasses(), tests[i].size()));
       }
       std::string name_metric = boost::algorithm::to_lower_copy(name_metric);
       std::vector<double> temp_result;
        for (int j=0;j<tests.size();j++){
            for (int i=0;i<etalon.size();i++){
                for (int s=(int)stretch*(etalon[i].size());s<tests[j].size()+1;s++){
                   metric->computeDTWForMetric ( tests[j], s , (int)stretch*etalon[i].size(),(int )(1.0/stretch)*etalon[i].size(), etalon[i], temp_result);
                   result[j].setDistance(i, s,minimum(temp_result));
                }
            }
        }
    }

    int RecognizerMultiMarkup::initFromEnv (const Environment& env) {
        if (env.getIntParamValue(stretch, "stretch")!=0)
            throw AxiomLibException("Can not find stretch");
        if (env.getStringParamValue(name_metric, "metric")!=0)
            throw AxiomLibException("Can not find a name of metric");
        metric = MultiMarking::DTWMetric::getMetric(name_metric);
    }


    Recognizer* RecognizerMultiMarkup::copy (void){
        return new RecognizerMultiMarkup(*this);
    }

    RecognizerMultiMarkup::RecognizerMultiMarkup(const RecognizerMultiMarkup& other) {
        stretch = other.stretch;
        name_metric= other.name_metric;
        metric = MultiMarking::DTWMetric::getMetric(other.name_metric);
    }

    RecognizerMultiMarkup::~RecognizerMultiMarkup() {
        if(metric) delete metric;
    }
}

