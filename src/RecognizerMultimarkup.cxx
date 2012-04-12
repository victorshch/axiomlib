#include "RecognizerMultimarkup.h"
#include "MultiMarking/dtwmetric.h"
#include "Environment.h"
#include "AxiomSet.h"
#include "DataSet.h"
namespace AxiomLib {


    void compare (std::vector<bool>& m, std::vector<bool> p){
        for (int i=0;i<m.size();i++){
         if (p[i])
             m[i]=true;
        }
    }
    double minimum (std::vector<double> r){
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
        // Вектор, в котором хранятся промежуточные значения используемых аксиом
        std::vector<bool> temp_testAxiomUsage;
        std::vector<bool> temp_refAxiomUsage;
        temp_testAxiomUsage.resize(axiomSet.size());
        temp_refAxiomUsage.resize(axiomSet.size());
        testAxiomUsage.resize(axiomSet.size());
        refAxiomUsage.resize(axiomSet.size());

        // Разметки траекторий

        // Разметка эталонных траекторий
        // Вектор, в котором будут храниться разметки эталонных траекторий
        vector <MultiMarking::MultiMark> etalon(dataSet.getNumberOfClasses()); // ЧЕМ ОТЛИЧАЕТСЯ getClassSize ОТ getNumberOfClasses()
        // Вектор(временный), в котором храниться временной ряд
        std::vector<double> vec;  // ЗНАЮ ЛИ Я РАЗМЕР ВЕКТОРА
        // Заполнение этого вектора
        for (int i=0;i<dataSet.getNumberOfClasses();i++){
            if (!(dataSet.getTSByIndexFromClass(vec, i , 0, params[0])))
                throw AxiomLibException("Can not create row");
            axiomSet.enter(etalon[i], vec , 0 , vec.size(), temp_refAxiomUsage);
            compare (&refAxiomUsage,temp_refAxiomUsage);
        }

        // Разметка тестовых траекторий
        // Вектор, в котором хранятся разметки тестовых траекторий
        // Вектор, в котором будут храниться разметки эталонных траекторий
        vector <MultiMarking::MultiMark> tests (dataSet.getTestSize());
        for (int i=0;i<dataSet.getTestSize();i++){
            if (!(dataSet.getTSByIndexFromTests(vec, i, params[0])))
                throw AxiomLibException("Can not create row");
            axiomSet.enter(tests[i], vec , 0 , vec.size(), temp_refAxiomUsage);
            compare (&refAxiomUsage,temp_refAxiomUsage);
        }

        // DTW
        std::vector<double> temp_result;
        for (int j=0;j<tests.size();j++){
            for (int i=0;i<etalon.size();i++){ // Откуда брать с какого отчета начинать?
                // тут еще цикл по началу отсчета в тестовой траектории + создать вектор, в котором для каждого отсечта - минимальное расстояние
                MultiMarking::DTWMetric::computeDTW (metric, tests[j], 0 , stretch*etalon[i],etalon[i]*(1.0/stretch), etalon[i], &temp_result); // МИНИМАЛЬНАЯ МАКСИМАЛЬНАЯ ДЛИНА ОКНА
                // тут ищем минимум среди result и пишем его в вектор, создаваемый выше
                // пишем значение в TrajectorySampleDistance
            }
        }


    }




    // Задание параметров класса-потомка считываю имя создаю метрику
    int RecognizerMultiMarkup::initFromEnv (const Environment& env) {
        // Чтение параметра размера окна   Проверука ноль - если не ноль, параметр не указан кидаем ошибку+тест на англ
        if (env.getIntParamValue(stretch, "stretch")!=0)
            throw AxiomLibException("Can not find stretch");
        // Чтение параметра - метрики
        std::string name_metric;
        if (env.getStringParamValue(name_metric, "metric")!=0)
            throw AxiomLibException("Can not find a name of metric");
        metric = MultiMarking::DTWMetric::getMetric(name_metric);  // +namepace
    }

    // Функция возвращает копию распознавателя
    // Рекомендуется реализовывать вызовом конструктора копирования

    Recognizer* RecognizerMultiMarkup::copy (void){
        return new RecognizerMultiMarkup(*this);
    }

    RecognizerMultiMarkup::RecognizerMultiMarkup(const RecognizerMultiMarkup& other) {
        stretch = other.stretch;
        metric_name = other.metric_name;
        metric = DTWMetric::getMetric(other.metric_name);
    }

    ~RecognizerMultiMarkup() {
        if(metric) delete metric;
    }
}

