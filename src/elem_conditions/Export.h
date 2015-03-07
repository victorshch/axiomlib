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
#include "../FuzzyMultiDataExt/ElemCondClustering.h"
#include "../FuzzyMultiDataExt/ClusteringRealizations/KMeansClusteringModel.h"
#include "../FuzzyMultiDataExt/ClusteringFeatures/ClusteringFeature.h"
#include "../FuzzyMultiDataExt/ClusteringFeatures/MaxValueFeature.h"
#include "../FuzzyMultiDataExt/ClusteringFeatures/MeanValueFeature.h"
#include "../FuzzyMultiDataExt/ClusteringFeatures/MinValueFeature.h"
#include "../FuzzyMultiDataExt/ClusteringFeatures/RegressionAngleFeature.h"
#include "../FuzzyMultiDataExt/ClusteringFeatures/RegressionErrorFeature.h"
#include "../FuzzyMultiDataExt/ClusteringFeatures/StandardDeviationFeature.h"

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
BOOST_CLASS_EXPORT_KEY2(AxiomLib::FuzzyMultiDataExt::KMeansClusteringModel, "KMeansClustering");
BOOST_CLASS_EXPORT_KEY2(AxiomLib::FuzzyMultiDataExt::ClusteringFeature, "ClusteringFeature");
BOOST_CLASS_EXPORT_KEY2(AxiomLib::FuzzyMultiDataExt::MaxValueFeature, "MaxValueFeature");
BOOST_CLASS_EXPORT_KEY2(AxiomLib::FuzzyMultiDataExt::MeanValueFeature, "MeanValueFeature");
BOOST_CLASS_EXPORT_KEY2(AxiomLib::FuzzyMultiDataExt::MinValueFeature, "MinValueFeature");
BOOST_CLASS_EXPORT_KEY2(AxiomLib::FuzzyMultiDataExt::RegressionAngleFeature, "RegressionAngleFeature");
BOOST_CLASS_EXPORT_KEY2(AxiomLib::FuzzyMultiDataExt::RegressionErrorFeature, "RegressionErrorFeature");
BOOST_CLASS_EXPORT_KEY2(AxiomLib::FuzzyMultiDataExt::StandardDeviationFeature, "StandardDeviationFeature");

#endif // EXPORT_H
