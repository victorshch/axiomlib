/****************************************************************************
*			ElemCondition
*
*	Description:	Данный файл содержит все макросы BOOST_CLASS_EXPORT_KEY2,
*				необходимые для сериализации полиморфных классов ЭУ.
*				Его необходимо включать в проект, использующий сериализацию ЭУ,
*				ровно в одном месте
*	Author:		wictor
*	History:
*
****************************************************************************/
#ifndef EXPORT_H
#define EXPORT_H

#include <boost/serialization/extended_type_info.hpp>
#include <boost/serialization/export.hpp>
#include "../all_axioms.h"

BOOST_CLASS_EXPORT_KEY2(AxiomLib::BetweenTwoLevels, "BetweenTwoLevels");
BOOST_CLASS_EXPORT_KEY2(AxiomLib::DecreaseLeftElemCondition, "DecreaseLeftElemCondition");
BOOST_CLASS_EXPORT_KEY2(AxiomLib::DecreaseRightElemCondition, "DecreaseRightElemCondition");
BOOST_CLASS_EXPORT_KEY2(AxiomLib::DerivativeDecreaseLeft, "DerivativeDecreaseLeft");
BOOST_CLASS_EXPORT_KEY2(AxiomLib::DerivativeDecreaseRight, "DerivativeDecreaseRight");
BOOST_CLASS_EXPORT_KEY2(AxiomLib::DerivativeIncreaseLeft, "DerivativeIncreaseLeft");
BOOST_CLASS_EXPORT_KEY2(AxiomLib::DerivativeIncreaseRight, "DerivativeIncreaseRight");
BOOST_CLASS_EXPORT_KEY2(AxiomLib::IncreaseLeftElemCondition, "IncreaseLeftElemCondition");
BOOST_CLASS_EXPORT_KEY2(AxiomLib::IncreaseRightElemCondition, "IncreaseRightElemCondition");
BOOST_CLASS_EXPORT_KEY2(AxiomLib::Integral, "Integral");
BOOST_CLASS_EXPORT_KEY2(AxiomLib::SecDerivativeDecreaseLeft, "SecDerivativeDecreaseLeft");
BOOST_CLASS_EXPORT_KEY2(AxiomLib::SecDerivativeDecreaseRight, "SecDerivativeDecreaseRight");
BOOST_CLASS_EXPORT_KEY2(AxiomLib::SecDerivativeIncreaseLeft, "SecDerivativeIncreaseLeft");
BOOST_CLASS_EXPORT_KEY2(AxiomLib::SecDerivativeIncreaseRight, "SecDerivativeIncreaseRight");
BOOST_CLASS_EXPORT_KEY2(AxiomLib::Volatility, "Volatility");
BOOST_CLASS_EXPORT_KEY2(AxiomLib::Variance, "Variance");
BOOST_CLASS_EXPORT_KEY2(AxiomLib::FuzzyMultiDataExt::ElemCondClustering, "ElemCondClustering");
BOOST_CLASS_EXPORT_GUID(AxiomLib::ElemCondMean, "ElemCondMean");

#endif // EXPORT_H
