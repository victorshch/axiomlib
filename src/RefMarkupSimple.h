/****************************************************************************
*			RefMarkupSimple
*
*	Description:	Одна эталонная разметка
*	Author:		dk
*	History:	
*
****************************************************************************/

#ifndef __AXIOMS_LIB_REF_MARKUP_Simple_HXX
#define __AXIOMS_LIB_REF_MARKUP_Simple_HXX

#include "./elem_conditions/ElemCondition.h"
#include "ElemConditionsFactory.h"
#include "AxiomLibException.h"
#include "Environment.h"
#include "all_axioms.h"
#include "Axiom.h"
#include <stdlib.h>
#include <iostream>
#include <string>
#include <vector>
#include <map>

namespace AxiomLib {

// если максимальный размер установлен в -1, он не органичивается
#define MARKUPINF = -1
	
//#define min(a,b) (((a) < (b)) ? (a) : (b))
// Defines.h включен в .cxx

class RefMarkupSimple {

 private:
 
    // внутренние переменные класса - используются как вспомогательные при задействовании алгоритма на основе DTW, в противном случае - не используются
    float a1, a2, b1, b2;

 protected:

    // Одна разметка
    std::vector<int> axiomTypes;
    
    // Хранение значения целевой функции сравнения, от которого считать сравниваемые методом использования метрики участки рядов различными
    double maxDistance;
    
    // Цены за удаление, вставку, замену символа в строке эталонной разметки и в строке разметки ряда.
    std::vector <double> delPrice;
    std::vector <double> insPrice;
    std::vector <double> chgPrice;
    
    // Усреднения цен удаления, вставки и замены символа- используется в алгоритме DTW с алгоритмом поиска в ширину 
    float averDel;
    float averIns;
    float averChg;
    
    // Указания какие операции использовать
    bool delUse;
    bool insUse;
    bool chgUse;
    
    // коэффициенты по усечению области рассмотрения путей в матрице расстояний в алгоритме DTW
    float truncationAxioms;
    float truncationRow;
    // Кофицинты сжатия и растяжения в пределах которых искать соответствие эталонной разметке
    float coef1; 
    float coef2;
    
    // Функция вычисления расстояния между двумя символами разметки для алгоритма на основе DTW
    inline float distance (int symbA, int symbB);
    
    // Функция приверки ограницений на путь в матрице DTW
    inline bool checkInRange (const int i, const int j) const;
    
    // Указания какой алгоритм подсчета расстояния использовать - если true - то расстояние считается с использованием алгоритма DTW - иначе - на основе метрики
    bool dtwUse;
    // Переменные для указания какой из алгоритмов DTW использовать
    bool useDTWwideSearch;
    bool useDTWdeepSearch;
    
    // Функция запуска вычислений алгоритмом DTW с подготовкой некоторых переменных
    int runDistDTW (std::vector <double> &preResult, std::vector < std::vector <float> > &distMatrix);
    
    // Функция вычисления расстояния между двумя подстроками разметки в алгоритме DTW - метод поиска в глубину
    double distDTWdeepSearch (const double curDist, int &curDeep, std::vector < std::vector <float> > &distMatrix, const int i, const int j) const;
    
    // Функция вычисления расстояния между двумя подстроками разметки в алгоритме DTW - метод поиска в ширину
    double distDTWwideSearch (std::vector <double> &preResult, std::vector < std::vector <float> > &distMatrix) const;

 public:
 
    // тестовая функция - вывод результатов 
    int setOut ();
    
    // Конструктор с заданием начальных значений по умолчанию
    RefMarkupSimple(void);

    // Деструктор удаляющий все созданные в рамках данного класса динамические объекты
    ~RefMarkupSimple(void);

	// Выдает текущее состояние разметки
	int getCopy (std::vector<int> &axTypes) const;
    
    // Установка параметров из конфигурационного файла
    int setParamsFromEnv (const Environment& env);
    
    // Установка нового здачения переменной dtwUse
    void setDTWUse (bool newDTWUse);
    
    // Изменение цен на операцию удаления
    void setDelPrice (bool newUse, double delNull, double delNoNull);
    
    // Изменение цен на операцию вставки
    void setInsPrice (bool newUse, double insNull, double insNoNull);
    
    // Изменение цен на операцию замены 
    void setChgPrice (bool newUse, double chgNullToNoNull, double chgNoNullToNoNull, double chgNoNullToNull);

    // Добавление новой эталонной разметки данной системой аксиом
    void create (std::vector<signed int> &newAxiomTypes);
    
    // Очищение класса от всех добавленных элементов
    int clear ();
    
    // Функция проверки - подходит ли разметка данного класса к вектору расшиеренной разметки во вхоном параметре
    int check (const double newMaxDistance, std::vector<int> &rowAxiomTypes, long int upTo);
    
    // Вспомогательная функция сравенения с эталонной разметкой для check
    double subcheck (const double curDist, std::vector<int> &rowAxiomTypes, long int upTo, int curEnd);
    
    // Вспомогательаня функция расчета расстония между подстрокой анализируемого ряда и эталонной разметкой на основе алгоритма DTW
    double subcheckDTW (std::vector<int> &rowAxiomTypes, long int upTo);
    
    // Функция возвращает длину вектора с эталонной разметкой
    int size (void) const;

    // Функция четкого сравнения двух разметок
    bool operator == (const RefMarkupSimple& second) const;
    
    // Функция четкого сравнения разметки и входного ряда 
    bool operator == (std::vector <int>& second) const;
    
    // Вспомогательная функция - для организации хранения разметок в виде set'a, а не в виде vector'a
    bool operator < (const RefMarkupSimple second) const {return 0;};

	// Оператор присваивания
    RefMarkupSimple& operator= (const RefMarkupSimple& second);
        
}; // end of class

}; //  end of namespace

#endif /* __AXIOMS_LIB_REF_MARKUP_Simple_HXX */
