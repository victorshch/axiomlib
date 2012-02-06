/****************************************************************************
*			RecognizerMultiMarkup
*
*	Description: RecognizerMultiMarkup
*
*	Author:		armkor
*	History:
*
****************************************************************************/

#ifndef RECOGNIZERMULTIMARKUP_H
#define RECOGNIZERMULTIMARKUP_H

#include "Recognizer.h"

namespace AxiomLib {

class RecognizerMultiMarkup : public Recognizer {

private:

public:

    // run for MultiMark
    virtual signed int run (AxiomSetPop& pop) ;
    //
    virtual signed int run (AxiomSetPop& pop, std::vector <double> &precs) ;
    //
    virtual signed int run (AxiomSet& axiomSet, DataSet& dataSet, MultiMark& params, int& resFirst, int& resSecond, const int asIndex = -1) ;
    //
    virtual signed int run (AxiomSet& axiomSet, DataSet& dataSet, MultiMark& params, int& resFirst, int& resSecond, std::vector<double> &stat, const int asIndex = -1) ;

}; // end of class

}; //  end of namespace
#endif // RECOGNIZERMULTIMARKUP_H
