/****************************************************************************
*			ElemCondition
*
*	Description:	Данный файл содержит все макросы BOOST_CLASS_EXPORT_GUID,
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
#include "../all_axioms.h"

BOOST_CLASS_EXPORT_GUID(AxiomLib::BetweenTwoLevels, "BetweenTwoLevels");
BOOST_CLASS_EXPORT_GUID(AxiomLib::DecreaseLeftElemCondition, "DecreaseLeftElemCondition");
BOOST_CLASS_EXPORT_GUID(AxiomLib::DecreaseRightElemCondition, "DecreaseRightElemCondition");
BOOST_CLASS_EXPORT_GUID(AxiomLib::DerivativeDecreaseLeft, "DerivativeDecreaseLeft");
BOOST_CLASS_EXPORT_GUID(AxiomLib::DerivativeDecreaseRight, "DerivativeDecreaseRight");
BOOST_CLASS_EXPORT_GUID(AxiomLib::DerivativeIncreaseLeft, "DerivativeIncreaseLeft");
BOOST_CLASS_EXPORT_GUID(AxiomLib::DerivativeIncreaseRight, "DerivativeIncreaseRight");
BOOST_CLASS_EXPORT_GUID(AxiomLib::IncreaseLeftElemCondition, "IncreaseLeftElemCondition");
BOOST_CLASS_EXPORT_GUID(AxiomLib::IncreaseRightElemCondition, "IncreaseRightElemCondition");
BOOST_CLASS_EXPORT_GUID(AxiomLib::Integral, "Integral");
BOOST_CLASS_EXPORT_GUID(AxiomLib::SecDerivativeDecreaseLeft, "SecDerivativeDecreaseLeft");
BOOST_CLASS_EXPORT_GUID(AxiomLib::SecDerivativeDecreaseRight, "SecDerivativeDecreaseRight");
BOOST_CLASS_EXPORT_GUID(AxiomLib::SecDerivativeIncreaseLeft, "SecDerivativeIncreaseLeft");
BOOST_CLASS_EXPORT_GUID(AxiomLib::SecDerivativeIncreaseRight, "SecDerivativeIncreaseRight");
BOOST_CLASS_EXPORT_GUID(AxiomLib::Volatility, "Volatility");
BOOST_CLASS_EXPORT_GUID(AxiomLib::Variance, "Variance");

#endif // EXPORT_H
