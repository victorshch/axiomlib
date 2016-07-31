#pragma once

#ifndef FUZZYMULTIDATAEXT_EC_CLUSTERING_ELEM_COND_H
#define FUZZYMULTIDATAEXT_EC_CLUSTERING_ELEM_COND_H

#include "../elem_conditions/ElemCondition.h"
#include "ClusteringFeatures/ClusteringFeature.h"
#include "ClusteringRealizations/ClusteringModel.h"
#include "boost/serialization/list.hpp"

using namespace std;

namespace AxiomLib {
	namespace FuzzyMultiDataExt {
		class ElemCondClustering : public ElemCondition {
				
		public: 
			// бНГБПЮЫЮЕР ХЛЪ ДЮММНИ ЮЙЯХНЛШ.
			virtual std::string name (void) const {
				return "ElemCondClustering";
			}

			virtual ElemCondition* clone() const {
				ElemCondClustering* clone = new ElemCondClustering(*this);
				return clone;
			}

			ElemCondClustering(int clusterNumber, int length, const std::list<ClusteringFeature*>& clusteringFeatures, FuzzyMultiDataExt::ClusteringModel* model);

			ElemCondClustering() {
			};
				
			~ElemCondClustering(void) {};

			// оПНБЕПЙЮ БШОНКМЕМХЪ ДЮММНИ ЮЙЯХНЛШ. мЮ БУНДЕ - РНВЙЮ БПЕЛЕММНЦН ПЪДЮ k
			// Х ЯЮЛ БПЕЛЕММНИ ПЪД x. мЮ БШУНДЕ 1, ЕЯКХ БШОНКМЪЕРЯЪ, 0 ЕЯКХ МЕ
			// БШОНКМЪЕРЯЪ Х -1, ЕЯКХ МЕ ОПХЛЕМХЛН (БШУНД ЮЙЯХНЛШ ГЮ ЦПЮМХЖШ ПЪДЮ
			// ЯКЕБЮ ХКХ ЯОПЮБЮ).
			// аСДЕЛ ЯВХРЮРЭ ПЪДШ НДМНЛЕПМШЛХ, ХАН БЯЕ ЮЙЯХНЛШ ОНЙЮ С МЮЯ АСДСР НДМНЛЕПМШЛХ,
			// Ю ТЮЙР ЛМНЦНЛЕПМНЯРХ ХЯУНДМНЦН ПЪДЮ ЛШ АСДЕЛ ХЯОНКЭГНБР, МЮ АНКЕЕ
			// БШЯНЙНЛ СПНБМЕ.
			virtual signed int check (const unsigned long k, const std::vector<double>& x) const;

			// яПЮБМЕМХЕ ДЮММНИ ЮЙЯХНЛШ Я ЕЕ БМСРПЕММХЛХ ОЮПЮЛЕРПЮЛХ Я ДПСЦНИ ЮЙЯХНЛНИ РНЦН ФЕ РХОЮ.
			// бНГБПЮЫЮЕР 1, ЕЯКХ ОЕПБЮЪ ЮЙЯХНЛЮ ЪБКЪЕРЯЪ ВЮЯРМШЛ ЯКСВЮЕЛ БРНПНИ, 0 ЕЯКХ ЮЙЯХНЛШ
			// МЕ ЯПЮБМХЛШ ХКХ РНФДЕЯРБЕММШ, -1, ЕЯКХ БРНПЮЪ ЪБКЪЕРЯЪ ВЮЯРМШЛ ЯКСВЮЕЛ ОЕПБНИ
			virtual double operator< (const ElemCondition& second) const {
				return -1;
			}
    
			// нОЕПЮЖХЪ ЛСРЮЖХХ: ХГЛЕМЕМХЕ ЙЮЙХУ-РН БМСРПЕММХУ ОЮПЮЛЕРПНБ СЯКНБХЪ (ВРН ХЛЕММН Х
			// ЙЮЙ ЛЕМЪЕРЯЪ НР МЮАКЧДЮРЕКЪ ЯЙПШРН). p НОПЕДЕКЪЕР ЯРЕОЕМЭ ХГЛЕМЕМХЪ (ДНКФМН МЮУНДХРЭЯЪ
			// МЮ НРПЕГЙЕ [0,1]). бНГБПЮЫЮЕР 0 ЕЯКХ ХГЛЕМЕМХЪ МЕ ОПНХГНЬКН, -1, ЕЯКХ ХГЛЕМЕМХЪ МЕ ОНКСВХКНЯЭ (МЮОПХЛЕП,
			// С ЩРНИ ЮЙЯХНЛШ МЕР ОЮПЮЛЕРПНБ), 1, ЕЯКХ ХГЛЕМЕМХЕ ОПНХГНЬКН
			virtual signed int transmute (const double p) {
				return -1;
			}
    
			// гЮДЮЕР ОЮПЮЛЕРПШ ЩКЕЛЕМРЮПМНЦН СЯКНБХЪ
			// бНГБПЮЫЮЕР 0, ЕЯКХ БЯЕ ok, -1 ХМЮВЕ
			virtual signed int setParams (const std::map<std::string,std::string>& newParams) {
				return 0;
			}
    
			// гЮДЮЕР ОЮПЮЛЕРПШ ЩКЕЛЕМРЮПМНЦН СЯКНБХЪ
			// бНГБПЮЫЮЕР 0, ЕЯКХ БЯЕ ok, -1 ХМЮВЕ
			virtual signed int getParams (std::map<std::string,std::string> &newParams) const {
				return 0;
			}
    
			// тСМЙЖХЪ СЯРЮМЮБКХБЮЕР ГМЮВЕМХЕ НДМНЦН ОЮПЮЛЕРПЮ ЩКЕЛЕМРЮПМНЦН СЯКНБХЪ,
			// ЕЯКХ РЮЙНИ ОЮПЮЛЕРП ОПХЯСРЯРБСЕР Б ДЮММНЛ ЩКЕЛЕМРЮПМНЛ СЯКНБХХ
			virtual signed int setParamValue (const double param, const std::string &paramName) {
				return 0;
			}
   
			// тСМЙЖХЪ ГЮОХЯШБЮЕР ГМЮВЕМХЕ НДМНЦН ОЮПЮЛЕРПЮ ЩКЕЛЕМРЮПМНЦН СЯКНБХЪ,
			// ЕЯКХ РЮЙНИ ОЮПЮЛЕРП ОПХЯСРЯРБСЕР Б ДЮММНЛ ЩКЕЛЕМРЮПМНЛ СЯКНБХХ
			virtual signed int getParamValue (double &param, const std::string &paramName) const {
				return -1;
			}
   
			// тСМЙЖХЪ ГЮОНКМЪЕР БУНДМНИ БЕЙРНП ЯРПНЙЮЛХ Я МЮГБЮМХЪЛХ ОЮПЮЛЕРПНБ ДЮММНИ ЩКЕЛЕМРЮПМНИ ЮЙЯХНЛШ
			virtual signed int getECParamNames (std::vector<std::string> &ecParamNames) const {
				return -1;
			}
     
			  // оПНБЕПЙЮ ЯНБЛЕЯРХЛНЯРХ Я ДПСЦХЛ ЩКЕЛЕМРЮПМШЛ СЯКНБХЕЛ МЮ ЯБЪГЙС "х"
			  // 0 - ЕЯКХ МЕ ЯНБЛЕЯРХЛЮ
			  // 1 - ЕЯКХ ЯНБЛЕЯРХЛЮ
			  // -1 - ЕЯКХ undefined
			virtual signed int checkAndCompatibility (const ElemCondition& toCheck) const {
				return -1;
			}

			  // яЙПЕЫХБЮМХЕ ДБСУ ЩКЕЛЕМРЮПМШУ СЯКНБХИ (ЛЕМЪЕР ХУ ЯНЯРНЪМХЕ)
			void crossEC(ElemCondition& second, const double crossProb, const double crossParam) {
				
			}

			  // тСМЙЖХЪ СЯРЮМЮБКХБЮЕР ЛЮЙЯХЛЮКЭМШЕ ГМЮВЕМХЪ ДКЪ ЦПЮМХЖ left Х right,
			  // ЙНРНПШЕ НОПЕДЕКЪЧР НЙПЕЯРМНЯРЭ РЕЙСЫЕИ РНВЙХ Б ЙНРНПНИ ОПНБЕПЪЕРЯЪ ЩКЕЛЕМРЮПМНЕ СЯКНБХЕ
			virtual signed int setLimits (const int leftLimit, const int rightLimit) {
				return 0;
			}

			  // тСМЙЖХЪ СЯРЮМЮБКХБЮЕР ЛЮЙЯХЛЮКЭМШЕ ГМЮВЕМХЪ ДКЪ ОЮПЮЛЕРПНБ ЩКЕЛЕМРЮПМНЦН СЯКНБХЪ,
			  // НРКХВМШУ НР left Х right
			virtual signed int setLimits (const std::vector<double>& row, const double reserve, const unsigned int numOfLevels) {
				return 0;
			}

			  // тСМЙЖХЪ ХГЛЕМЪЕР ОЮПЮЛЕРПШ ЩКЕЛЕМРЮПМНЦН СЯКНБХЪ Б ГЮДЮММШУ ОПЕДЕКЮУ МЮ ВХЯКН ЬЮЦНБ,
			  // СЙЮГЮММНЕ Б ОЮПЮЛЕРПЮУ. хГЛЕМЕМХЕ МЮ НДХМ ЬЮЦ - ЩРН ХГЛЕМЕМХЕ МЮ НДМН ГМЮВЕМХЕ delta.
			  // гМЮВЕМХЕ delta НОПЕДЕКЪЕРЯЪ ОПХ СЯРЮМНБЙЕ КХЛХРНБ ХГЛЕМЕМХЪ.
			virtual signed int changeParams (const int numOfSteps) {
				return -1;
			}

			// тСМЙЖХЪ ОПХЯБЮХБЮЕР ДЮММНЛС щс щс other Х БНГБПЮЫЮЕР true, ЕЯКХ С МХУ ЯНБОЮДЮЕР РХО
			// хМЮВЕ БНГБПЮЫЮЕР false
			// б ПЕЮКХГЮЖХХ МЕ АСДСР ХЯОНКЭГНБЮРЭЯЪ ДНПНЦХЕ НОЕПЮЖХХ,
			// РЮЙХЕ, ЙЮЙ new
			virtual bool assign(const ElemCondition* other) {
				return false;
			}
			

			private: 			
				//НАЗЪБКЕМХЪ, МЕНАУНДХЛШЕ ДКЪ ЯЕПХЮКХГЮЖХХ
				friend class boost::serialization::access;

				//ТСМЙЖХЪ ЯЕПХЮКХГЮЖХХ - Б МЕИ ДНКФМШ АШРЭ СЙЮГЮМШ ВКЕМШ-ДЮММШЕ,
				//УЮПЮЙРЕПХГСЧЫХЕ ЯНЯРНЪМХЕ НАЗЕЙРЮ
				template<class Archive>
				void serialize(Archive & archive, const unsigned int /*version*/)
				{
					archive & BOOST_SERIALIZATION_BASE_OBJECT_NVP(ElemCondition);
					archive & BOOST_SERIALIZATION_NVP(clusterNumber);
					archive & BOOST_SERIALIZATION_NVP(length);
					archive & BOOST_SERIALIZATION_NVP(clusteringModel);
                    archive & BOOST_SERIALIZATION_NVP(clusteringFeatures);
				}

				int clusterNumber;

				int length;

				std::list<ClusteringFeature*> clusteringFeatures;

				ClusteringModel* clusteringModel;
		};
	};
};

#endif //FUZZYMULTIDATAEXT_EC_CLUSTERING_ELEM_COND_H
