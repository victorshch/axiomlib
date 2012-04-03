#include "RecognizerMultimarkup.h"

/*
    int stretch; // используется при указании размеров окна
    DTWMetric* metric;

    // Вычисление расстояний от участков разметок тестовых траекторий до разметок эталонных траекторий

    void computeDistances(AxiomSet& axiomSet, DataSet& dataSet, std::vector<int>& params, // номера столбцов кторые надо использовать
                                                              // расстояния от участков разметок тестовых траекторий до разметок эталонных траекторий
                                                              std::vector<TrajectorySampleDistance>& result,
                                                              // какие аксиомы срабатывали при разметке эталонных траекторий
                                                              std::vector<bool>& refAxiomUsage,
                                                              // какие аксиомы срабатывали при разметке тестовых траекторий
                                                              std::vector<bool>& testAxiomUsage) {

        // Разметка эталонных траекторий.
        vector <MultiMarking::MultiMark> etalon(dataSet.getNumberOfClasses());
        for (int i=0;i<dataSet.getNumberOfClasses();i++){
             axiomSet.enter(etalon[i], dataSet.referenceClassesTS[i] , 0 , dataSet.referenceClassesTS[i].size(), &testAxiomUsage)
        }







        //Иницализация переменной, в которой будет храниться временной ряд
        std::vector<double> row;
        // Получение временного ряда
        dataSet.getTSByIndexFromClass(&row,params[0],params[1],params[2]);
        // Инициализация разметки рассматриваемой траектории
        MultiMarking::MultiMark marked_in;
        MultiMarking::MultiMark marked_etalon;
        axiomSet.enter(&marked_in, &row , 0 , row.size(), &testAxiomUsage)

    }

/*
    int enter (MultiMarking::MultiMark& marked, const std::vector<double>& row, const unsigned long begin, const unsigned long end, std::vector<bool> &stat);
bool getTSByIndexFromClass (std::vector<double> &vec, int indexClass, int indexMultiTS, int indexTS) const;
*/
    // Задание параметров класса-потомка считываю имя создаю метрику
/*
    int RecognizerMultiMarkup::initFromEnv (const Environment& env) {
        // Чтение параметра размера окна
        env.getIntParamValue(stretch, "stretch");
        // Чтение параметра - метрики
        std::string name_metric;
        env.getStringParamValue(name_metric, "metric");
        metric = metric.getMetric(name_metric);
    }

    // Функция возвращает копию распознавателя
    // Рекомендуется реализовывать вызовом конструктора копирования

    Recognizer* RecognizerMultiMarkup::copy (void){
        Recognizer* m;
        m.stretch= this->stretch;
        m.metric = this->metric;
    }
*/
