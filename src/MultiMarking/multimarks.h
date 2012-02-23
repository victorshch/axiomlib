/****************************************************************************
*			MultiMark
*
*	Description:	MultiMark - тип, описывающий разметку с множеством аксиом.
*
*	Author:		armkor
*	History:
*
****************************************************************************/
#ifndef MULTIMARK_H
#define MULTIMARK_H


#include "iostream"
#include "vector"

namespace AxiomLib {

namespace MultiMarking {

    // declaration of type MultiMark
    typedef std::vector<bool> tempb;
    typedef std::vector<tempb>  MultiMark;

};//  end of namespace MultiMarking

};//  end of namespace AxiomLib

#endif // MULTIMARK_H
