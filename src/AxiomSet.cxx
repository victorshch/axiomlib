/****************************************************************************
*			Функции класса AxiomSet.h
*
*	Description:	Набор аксиом - Реализация
*	Author:		dk,gevor
*	History:	
*
****************************************************************************/
#include "AxiomSet.h"

// Дефолтные значения для элементов класса
#define str_default_axiomSet_name	"no_name_AxiomSet"
#define str_default_axiomSet_bank	"no_name_AxiomBank"

using namespace AxiomLib;
using namespace std;

// Пустой конструктор с заданием имени системы аксиом по default'у
AxiomSet::AxiomSet () {
	nameOfAxiomSet.assign (str_default_axiomSet_name);
	nameOfAxiomBank.assign (str_default_axiomSet_bank);
	axioms.resize(0);
	crossoverControlBit = 0;
}


// Пустой конструктор с заданием имени системы аксиом в параметре
AxiomSet::AxiomSet (const std::string sName) {
	nameOfAxiomSet.assign (sName);
	nameOfAxiomBank.assign (str_default_axiomSet_bank);
	axioms.resize(0);
	crossoverControlBit = 0;
}


// Деструктор - удаляет все созданные динамические объкты в системе аксиом
AxiomSet::~AxiomSet(void) {
	for (unsigned int i = 0; i < this->axioms.size(); i++) {
		(axioms[i])->clear();
		delete (axioms[i]);
	}
}


// Деструктор - удаляет все созданные динамические объкты в системе аксиом
void AxiomSet::clear(void) {
	for (unsigned int i = 0; i < this->axioms.size(); i++) {
		(axioms[i])->clear();
		delete (axioms[i]);
	}
	axioms.resize(0);
}


// Название аксиомы (одинаковое независимо от параметров, используется для задания библиотек аксиом)
std::string AxiomSet::name (void) const {
	//std::cout << "name() : \n";// << nameOfAxiomSet << "\n";
	return nameOfAxiomSet;
}


// Функция возвращает число аксиом в данной системе
int AxiomSet::size (void) const {
	return axioms.size();
}


// Функция установки значения контрольного бита скрещивания
int AxiomSet::setCrossoverControlBit (const int newCrossoverControlBit) {
	crossoverControlBit = newCrossoverControlBit;
	return newCrossoverControlBit;
}


// Функция возвращает текущее значение контрольного бита скрещивания
int AxiomSet::getCrossoverControlBit (void) const {
	return crossoverControlBit;
}


// Функция возвращает число элементов в векторе axioms данного набора аксиом
signed int AxiomSet::getNumOfAxioms(void) const {
	return (int) axioms.size();
}


/****************************************************************************
*				AxiomSet::getAxiom
*
*	Description:	Функция возвращает аксиому с заданным номером в векторе аксиом данной системы
*	Parameters:	int i - номер зарпашиваемой аксиомы в векторе систем
*	Returns:		Функция вовзращает запрашиваемою аксиому как копию той, что в векторе аксиом данной системы аксиом
*	Throws:		AxiomLibExeption - если значение индекса не удовлетворяет текущим граничным значениям
*	Author:		dk
*	History:
*
****************************************************************************/
Axiom& AxiomSet::getAxiom (int i) const {
	// проверка значения индекса запрашиваемой аксиомы
	if ( ( i <  0) || ( i  >= (int) axioms.size() ) ) {
		throw AxiomLibException("Error in AxiomSet::getAxiom : parameter value out of range.");
	}
	return (*axioms[i]);
}


/****************************************************************************
*				AxiomSet::setAxiom
*
*	Description:	Функция изменяет аксиому с заданным номером  векторе аксиом
*	Parameters:	axiom - аксиома - на которую заменяем i-ую в векторе аксиом
*				i - номер зарпашиваемой аксиомы в векторе систем
*	Returns:		0 - если все нормально
*	Throws:		AxiomLibExeption - если значение индекса не удовлетворяет текущим граничным значениям
*	Author:		dk
*	History:
*
****************************************************************************/
signed int AxiomSet::setAxiom (Axiom& axiom, const int i) {
	if ( ( i <  0) || ( i  > (int) axioms.size() ) ) {
		throw AxiomLibException("Error in AxiomSet::setAxiom : parameter value out of range.");
	}
	
	if ( i  < (int) axioms.size() ) {
		*(axioms[i]) = axiom;
        }
	else { // this mean: i == axioms.size()
		Axiom* addAxiom;
		addAxiom = new Axiom;
		*addAxiom = axiom;
		axioms.push_back (addAxiom);
	}
	return 0;
}


/****************************************************************************
*				AxiomSet::slide
*
*	Description:	Функция циклического сдвига аксиом в системе по часовой стрелке
*	Parameters:	i - начало сдвига
*				j - конец сдвига 
*	Returns:		0 - если все нормально
*	Throws:		AxiomLibExeption - если значение индекса не удовлетворяет текущим граничным значениям
*	Author:		dk
*	History:
*
****************************************************************************/
signed int AxiomSet::slide (const int i, const int j) {
	if ( ( i <  0) || ( i  > (int) axioms.size() ) || ( j <  0) || (j  > (int) axioms.size() ) ) {
		throw AxiomLibException("Error in AxiomSet::setAxiom : parameter value out of range.");
	}
	int start, finish;
	if ( j < i ) {
		start = j;
		finish = i;
	} else {
		start = i;
		finish = j;
	}
	Axiom* axiomTemp;
	axiomTemp = axioms[finish];
	for (int k = finish - 1; k >= start; k--) {
		axioms[k+1] = axioms[k];
	}
	axioms[start] = axiomTemp;
	return 0;
}


/****************************************************************************
*				AxiomSet::enter
*
*	Description:	Функция разметки ряда row начиная с отсчета begin до отсчета end
*	Parameters:	marked - вектор, куда кладем разметку
*				row    - ряд для разметки
*				begin - отсчет с которого начинать разметку
*				end    - отсчет до которого вести разметку
*	Returns:		Функция вовзращает размеченный ряд - ряд с номером аксиомы выполненной на каждом из отсчетов исходного ряда
*				-1 - означает что ни одна аксиома не выполнена - то есть нулевая аксиома записывается под этим значением
*	Throws:		AxiomLibExeption - если begin и end заданы не корректно
*	Author:		dk
*	History:
*
****************************************************************************/
int AxiomSet::enter (std::vector<int>& marked, const std::vector<double>& row, const unsigned long begin, const unsigned long end) {
	if ((end <= begin) || (row.size() < end)) {
		marked.resize(0);
		throw AxiomLibException("Error in AxiomSet::enter : wrong parameters.");
	}
	else {
		int curRes;
		marked.resize(end - begin);
		//std::cout << "\n New markuped row: \n	";
		for (unsigned long i = begin; i < end; i++) {
			marked[i - begin] = 0;
			curRes = 0;
			for (unsigned int j = 0; j < axioms.size(); j++ ) {
				curRes = (axioms[j])->check(i, row);
				if (curRes == 1) {
					marked[i - begin] = j + 1;
					break;
				}
			}
		}
	}
 	return 0;
}


/****************************************************************************
*				AxiomSet::enter
*
*	Description:	Функция разметки ряда row начиная с отсчета begin до отсчета end
*	Parameters:	marked - вектор, куда кладем разметку
*				row    - ряд для разметки
*				begin - отсчет с которого начинать разметку
*				end    - отсчет до которого вести разметку
*				stat - статистика по аксиомам системы, какие были использованы при разметке
*	Returns:		Функция вовзращает размеченный ряд - ряд с номером аксиомы выполненной на каждом из отсчетов исходного ряда
*				-1 - означает что ни одна аксиома не выполнена - то есть нулевая аксиома записывается под этим значением
*	Throws:		AxiomLibExeption - если begin и end заданы не корректно
*	Author:		dk
*	History:
*
****************************************************************************/
int AxiomSet::enter (std::vector<int>& marked, const std::vector<double>& row, const unsigned long begin, const unsigned long end, std::vector<bool> &stat) {
	if ((end <= begin) || (row.size() < end)) {
		marked.resize(0);
		throw AxiomLibException("Error in AxiomSet::enter : wrong parameters.");
	}
	else {
		int curRes;
		marked.resize(end - begin);
		stat.resize(axioms.size());
		for (unsigned int i = 0; i < axioms.size(); i++)
			stat[i] = false;
		//std::cout << "\n New markuped row: \n	";
		for (unsigned long i = begin; i < end; i++) {
			marked[i - begin] = 0;
			curRes = 0;
			for (unsigned int j = 0; j < axioms.size(); j++ ) {
				curRes = (axioms[j])->check(i, row);
				if (curRes == 1) {
					marked[i - begin] = j + 1;
					if (!stat[j]) stat[j] = true;
					break;
				}
			}
		}
	}
 	return 0;
}

/****************************************************************************
*				AxiomSet::enter
*
*	Description:	Функция разметки ряда для случая разметки множеством аксиом
*                       row начиная с отсчета begin до отсчета end
*	Parameters:	marked -куда кладем разметку
*				row    - ряд для разметки
*				begin - отсчет с которого начинать разметку
*				end    - отсчет до которого вести разметку
*	Returns:		Функция вовзращает размеченный ряд - ряд с номером аксиомы выполненной на каждом из отсчетов исходного ряда
*				-1 - означает что ни одна аксиома не выполнена - то есть нулевая аксиома записывается под этим значением
*	Throws:		AxiomLibExeption - если begin и end заданы не корректно
*	Author:		armkor
*	History:
*
****************************************************************************/

int AxiomSet::enter (MultiMarking::MultiMark& marked, const std::vector<double>& row, const unsigned long begin, const unsigned long end) {
        if ((end <= begin) || (row.size() < end)) {
                marked.resize(0);
                throw AxiomLibException("Error in AxiomSet::enter : wrong parameters.");
        }
        else {
                int curRes;
                marked.resize(end - begin);

                for (unsigned long i=begin;i<end;i++){
                    marked[i-begin].resize(axioms.size());
                    for (int j=0;j<axioms.size();j++){
                        marked[i-begin][j]=false;
                    }
                }

                for (unsigned long i = begin; i < end; i++) {
                        curRes = 0;
                        for (unsigned int j = 0; j < axioms.size(); j++ ) {
                                curRes = (axioms[j])->check(i, row);
                                if (curRes == 1) {
                                        marked[i - begin][j] = true;
                                }
                        }
                }
        }
        return 0;
}

/****************************************************************************
*				AxiomSet::enter
*
*	Description:	Функция разметки ряда для случая разметки множеством аксиом
*                       row начиная с отсчета begin до отсчета end
*	Parameters:	marked -куда кладем разметку
*				row    - ряд для разметки
*				begin - отсчет с которого начинать разметку
*				end    - отсчет до которого вести разметку
*				stat - статистика по аксиомам системы, какие были использованы при разметке
*	Returns:		Функция вовзращает размеченный ряд - ряд с номером аксиомы выполненной на каждом из отсчетов исходного ряда
*				-1 - означает что ни одна аксиома не выполнена - то есть нулевая аксиома записывается под этим значением
*	Throws:		AxiomLibExeption - если begin и end заданы не корректно
*	Author:		armkor
*	History:
*
****************************************************************************/


int AxiomSet::enter (MultiMarking::MultiMark& marked, const std::vector<double>& row, const unsigned long begin, const unsigned long end, std::vector<bool> &stat){
  //  std::cout <<"AxiomSet::enter";
    if ((end <= begin) || (row.size() < end)) {
            marked.resize(0);
            throw AxiomLibException("Error in AxiomSet::enter : wrong parameters.");
    }
    else {
            int curRes;
            marked.resize(end - begin);
            stat.resize(axioms.size());
            for (unsigned int i = 0; i < axioms.size(); i++)
                                    stat[i] = false;
            for (unsigned long i=begin;i<end;i++){
                marked[i-begin].resize(axioms.size());

                for (int j=0;j<axioms.size();j++){
                    marked[i-begin][j]=false;
                }
            }

            for (unsigned long i = begin; i < end; i++) {
                //std::cout << "S";
                    curRes = 0;
                    for (unsigned int j = 0; j < axioms.size(); j++ ) {
                            curRes = (axioms[j])->check(i, row);
         //                   std::cout << "!"<< curRes << "!";
                            if (curRes != 0) {
                  //              std::cout << "E";
                                    marked[i - begin][j] = true;
       //                             std::cout << "@"<< marked[i - begin][j] << "@";
                                    if (!stat[j]) stat[j] = true;
                            }
                    }
            }
    }/*
    for(int k=0;k<marked.size();k++){
        for (int t=0;t<marked[k].size();t++){
            std::cout << marked[k][t] << "\n";
        }
    }*/
    return 0;
}


/****************************************************************************
*				AxiomSet::initAxiomSetByRand
*
*	Description:	Функция задания системы аксиом случайным образом
*	Parameters:	numOfAxioms  - число случано выбириаемых аксиом
*				maxECinAxiom - максимальное чилсо элементарных условий которые могут составлять одну аксиому
*	Returns:		0 - если все нормальо, в противном случае выход по throw 
*	Throws:		-
*	Author:		dk,gevor
*	History:
*
****************************************************************************/
signed int AxiomSet::initAxiomSetByRand (const int numOfAxioms, const int maxECinAxiom) {
	int numOfEC;
	Axiom* axiom;
	axioms.resize(numOfAxioms);
	for (int i = 0; i < numOfAxioms; i++) {
		axiom = new Axiom();
		numOfEC = round(((double) rand() / (double) RAND_MAX) * (maxECinAxiom-1) + 1);
		axiom->initAxiomByRand(numOfEC);
		while (axiom->size() == 0) {
		  numOfEC--;
		  cout << "numOfEC too large, setting '" << numOfEC << "'\n";
		  axiom->initAxiomByRand(numOfEC);
		}
//                cout << "numOfEC = '" << numOfEC << "', num of ecs in axiom = '" << axiom->size() << "'\n";
		axioms[i] = axiom;
	}
	
	return 0;
}


/****************************************************************************
*				AxiomSet::initAxiomSetByNames
*
*	Description:	Функция задающая систему аксиом как набор строго определенных аксиом
*	Parameters:	axiomEC - вектор "конструкций"
*				"конструкция" - вектор сторк определяюищх аксиому как набор элементарных условий с именами заданными в строках
*				axiomNames - вектор имен создаваемых аксиом (в порядке уменьшения приоритета)
*	Returns:		0 - если все нормально, в противном случае выход по throw
*	Throws:		AxiomLibExeption - если вектор описания элементарных условий пустой
*	Author:		dk
*	History:
*
****************************************************************************/
signed int AxiomSet::initAxiomSetByNames (const std::vector < std::vector < std::string > >& axiomEC, const std::vector <std::string>& axiomNames) {
	if (axiomEC.size() < 1) {
		throw AxiomLibException("Error in AxiomSet::initAxiomSetByNames : wrong first input parameter - it is empty.");
		return -1;
	}
	else {
		Axiom* axiom;
		for (unsigned int i = 0; i < axiomEC.size(); i++) {
			if (i < axiomNames.size()) {
				axiom = new Axiom(axiomNames[i]);
			}
			else {
				axiom = new Axiom();
			}
			axiom->initAxiomByNames(axiomEC[i]);
			axioms.push_back(axiom);
		}
	}
	return 0;
}


/****************************************************************************
*				AxiomSet::initAxiomSetByNums
*
*	Description:	Функция задающая систему аксиом как набор строго определенных аксиом
*	Parameters:	axiomEC - вектор "конструкция"
*				"конструкция" - вектор номеров определяюищх аксиому как набор элементарных условий с номерами заданными в векторе
*				axiomNames - вектор имен создаваемых аксиом (в порядке уменьшения приоритета)
*	Returns:		0 - если все нормально, в противном случае выход по throw
*	Throws:		AxiomLibExeption  - если вектор описания элементарных условий пустой
*	Author:		dk
*	History:
*
****************************************************************************/
signed int AxiomSet::initAxiomSetByNums (const std::vector < std::vector < int > >& axiomEC, const std::vector <std::string>& axiomNames) {
	if (axiomEC.size() < 1) {
		throw AxiomLibException("Error in AxiomSet::initAxiomSetByNums : wrong first input parameter - it is empty.");
		return -1;
	}
	else {
		Axiom* axiom;
		for (unsigned int i = 0; i < axiomEC.size(); i++) {
			if (i < axiomNames.size()) {
				axiom = new Axiom(axiomNames[i]);
			}
			else {
				axiom = new Axiom();
			}
			axiom->initAxiomByNums(axiomEC[i]);
			axioms.push_back(axiom);
		}
	}
	return 0;
}


/****************************************************************************
*				AxiomSet::initAxiomSetByCopy - простейшая версия - без упорядочивания - можно использовать при проверке кода
*
*	Description:	Функция задающая систему аксиом путем копирования структуры вектора аксиом из входного параметра
*	Parameters:	axiomVec - набор аксиом, которые ожидаем увидеть в создаваемом axiomSet'е
*				levelOfSub - уровень по которому ведется сортировка вложенности одних аксиом в дургие
*	Returns:		0 - если все нормально, в противном случае выход по throw
*	Throws:		AxiomLibExeption - если вектор описания элементарных условий 
*	Author:		dk
*	History:
*
****************************************************************************/
/*signed int AxiomSet::initAxiomSetByCopy (const std::vector <Axiom*> &axiomVec, double levelOfSub) {
	this->axioms.clear();
	this->axioms.resize(axiomVec.size());
//std::cout << "\n 			Copying \n";
	for (int i = 0; i < axiomVec.size(); i++) {
		this->axioms[i] = new Axiom;
		*(this->axioms[i]) = *(axiomVec[i]);
//std::cout << "\n 			this->axioms[i]->name() is " << this->axioms[i]->name();
//std::cout << "\n 			this->axioms[i]->size() is " << this->axioms[i]->size() << "\n";
	}
//std::cout << "\n";
	return 0;
}*/


/****************************************************************************
*				AxiomSet::initAxiomSetByCopy
*
*	Description:	Функция задающая систему аксиом путем копирования структуры вектора аксиом из входного параметра
*	Parameters:	axiomVec - набор аксиом, которые ожидаем увидеть в создаваемом axiomSet'е
*				levelOfSub - уровень по которому ведется сортировка вложенности одних аксиом в дургие
*	Returns:		0 - если все нормально, в противном случае выход по throw
*	Throws:		AxiomLibExeption - если вектор описания элементарных условий 
*	Author:		dk
*	History:
*
****************************************************************************/
signed int AxiomSet::initAxiomSetByCopy (const std::vector <Axiom*> &axiomVec, double levelOfSub) {
	// Проверка корректности входного параметра - не пуст ли вектор
	if (axiomVec.size() < 1) {
		throw AxiomLibException("Error in AxiomSet::initAxiomSetByCopy : wrong first input vector of axioms- it is empty.");
	}
	if ( (levelOfSub > 1) || (levelOfSub < 0) ) {
		throw AxiomLibException("Error in AxiomSet::initAxiomSetByCopy : wrong first input level Of Sub- it is out of range.");
	}
	// Сортировка аксиом для того, чтобы в системе аксиом аксиомы имели корректный приоритет
	double temp;
	unsigned int cur;
	std::vector<int> mask;
	mask.resize(axiomVec.size());
	for (unsigned int i = 0; i < axiomVec.size(); i++) {
		cur = i;
		for (unsigned int j = 0; j < i; j++) {
			temp = (*(axiomVec[i])) < (*(axiomVec[mask[j]]));
			if (temp > levelOfSub) {
				cur = j;
				break;
			}
		}
		// смщение если необходимо части вектора 
		for (unsigned int h = i; h > cur; h--) {
			mask[h] = mask[h-1];
		}
		mask[cur] = i;
	}
	// запись аксиом в системе
	this->axioms.clear();
	this->axioms.resize(axiomVec.size());
	//std::cout << "\n 		Init::Maks is \n		";
	for (unsigned int i = 0; i < axiomVec.size(); i++) {
		this->axioms[i] = new Axiom;
		*(this->axioms[i]) = *(axiomVec[mask[i]]);
		//std::cout << "  " << mask[i];
	}
	//std::cout << "\n";
	return 0;
}


/****************************************************************************
*				AxiomSet::initAxiomSetFromFile
*
*	Description:	Функция задающая систему аксиом как набор строго определенных аксиом
*	Parameters:	axiomBaseDir - директория, где находится запрашиваемый файл описания системы аксиом
*				axiomSetName - имя системы аксиом
*				axiomBase - используемый банк аксиом
*	Returns:		0 - если все нормально, в противном случае выход по throw
*	Throws:		AxiomLibExeption - если файла не нашлось
*	Author:		dk
*	History:
*
****************************************************************************/
signed int AxiomSet::initAxiomSetFromFile (std::string axiomSetBaseDir, std::string axiomSetName, AxiomBase &axiomBase) {
	AxiomSetBase axiomSetBase;
	AxiomSetStructure axiomSetStructure;
	// Формируем полный путь к файлу
	std::string axiomSetPath = axiomSetBaseDir;
	axiomSetPath.append("/");
	axiomSetPath.append(axiomSetName);
	// Считываем информацию о системе аксиом в axiomSetStructure
	axiomSetBase.readFromAS (axiomSetPath, axiomSetStructure);
	// Заполняем поля класса в соответствии со считанными значениями
	if (axiomSetStructure.axiomSetName.size() > 0)
		nameOfAxiomSet = axiomSetStructure.axiomSetName;
	if (axiomSetStructure.axiomBankDir.size() > 0)
		nameOfAxiomBank = axiomSetStructure.axiomBankDir;
	std::vector<std::string> axiomECs;
	axiomECs.resize(0);
	Axiom* axiom;
	for (unsigned int i = 0; i < axiomSetStructure.axioms.size(); i++) {
		axiom = new Axiom(axiomSetStructure.axioms[i].axiomName);
		if (axiomSetStructure.axioms[i].axiomName == "no_name") {
			axiomSetStructure.axioms[i].getVectorOfEC (axiomECs);
		}
		else {
			axiomBase.getVectorOfEC(axiomECs, axiomSetStructure.axioms[i].axiomName);
		}
		if (axiomECs.size() > 0) {
			axiom->initAxiomByNames(axiomECs);
			if (axiomSetStructure.axioms[i].axiomParameters.size() != 0)
				axiom->setECParameters (axiomSetStructure.axioms[i].axiomParameters);
			axioms.push_back(axiom);
		}
		else {
			delete (axiom);
			//std::cout << "\n 	i is " << i << "\n";
			//std::cout << "	axiomSetStructure.axioms[i].axiomName is " << axiomSetStructure.axioms[i].axiomName << "\n";
			throw AxiomLibException ("Error in AxiomSet::initAxiomSetFromFile: in Axiom Set found axiom with a name that is not in the AxiomBank.");
		}
	}
	
	return 0;
}


/****************************************************************************
*					AxiomSet::createAxiomSetStructure
*
*	Description:	Функция сохраняет структуру данной системы аксиом в AxiomSetStructures
*	Parameters:		ass - структура, в которую записывает описание системы акисом
*	Returns:		0
*	Throws:			-
*	Author:			dk
*	History:
*
****************************************************************************/
signed int AxiomSet::createAxiomSetStructure (AxiomSetStructure &ass) const {
	// Заполняем поля стуктуры
	ass.axiomSetName = this->nameOfAxiomSet;
	ass.axiomBankDir = this->nameOfAxiomBank;
	// Вызываем заполнение полей с описанием аксиом
	ass.axioms.resize (this->axioms.size());
	for (unsigned int i = 0; i < this->axioms.size(); i++) {
		this->axioms[i]->createAxiomStructure (ass.axioms[i]);		
	}
	return 0;
}


/****************************************************************************
*				AxiomSet::saveAxiomSetToFile
*
*	Description:	Функция сохранения системы аксиом в файл
*	Parameters:		baseDir - директория, где находится запрашиваемый файл описания системы аксиом
*					axiomSetName - имя системы аксиом
*	Returns:		0
*	Throws:			-
*	Author:			dk
*	History:
*
****************************************************************************/
signed int AxiomSet::saveAxiomSetToFile (std::string baseDir, std::string axiomSetName, int first, int second) {
	AxiomSetStructure ass;
	createAxiomSetStructure (ass);
	ass.axiomSetName = axiomSetName;
	AxiomSetBase axiomSetBase;
	axiomSetBase.saveToAS (baseDir, ass, first, second);
	return 0;
}


/****************************************************************************
*				AxiomSet::saveAxiomSetToFile
*
*	Description:	Функция сохранения системы аксиом в файл
*	Parameters:	env - класс с данными из основного конфига
*				envDataSet - класс с данными из конфига набора данных
*	Returns:		0 - если все нормально, в противном случае выход по throw
*	Throws:		AxiomLibExeption - если файла не нашлось
*	Author:		gevor
*	History:
*
****************************************************************************/
signed int AxiomSet::saveAxiomSetToFile (Environment &env, EnvDataSet &envDataSet, int first, int second){
  // Запись лучшой системы аксиом с учетом оверрида имени файла в общем конфиги 
  // (тогда как первоначально задается в конфиге набора данных)

  // получаем базовый каталог axiomset-ов
  std::string axiomSetBaseDir;
  if (env.getStringParamValue(axiomSetBaseDir, "AxiomSetBaseDir") < 0) {
    throw AxiomLibException("AxiomSet::saveAxiomSetToFile : can not get axiom base dir");
  }

  // получаем из основного файла имя - куда сохранять
  std::string whereToSave;
  if (env.getStringParamValue(whereToSave, "saveTo") < 0) {
    // упс, в основном не задано, посмотрим, что в dataset-е
    if (envDataSet.getWhereToSave (whereToSave) < 0) {
      throw AxiomLibException("AxiomSet::saveAxiomSetToFile : can not get axiom set name to save to");
    }
  }

  // сохраняем
  saveAxiomSetToFile (axiomSetBaseDir, whereToSave, first, second);

  return 0;
}


/****************************************************************************
*				AxiomSet::setAxiomParameters
*
*	Description:	Функция заданяи параметров системы аксиом
*	Parameters:	Параметр определят отображение из имен аксиом на "структуру"
*				где "структура" - это отображение из имен элемнтарных условий в "низкую структуру"
*				где "низкая структура" определяет отображени из имен параметров в значния параметров
*	Returns:		0 - если хотя бы одмн параметр был изменен
*				-1 - если что-то произошло не так, присваивание параметров не началось
*				-2 - если ни один из параметров не был изменен
*	Throws:		AxiomLibExeption - если входной параметр пуст
*	Author:		dk
*	History:
*
****************************************************************************/
signed int AxiomSet::setAxiomParameters (const std::map < std::string , std::map <std::string, std::map <std::string, std::string> > >& axiomParams) {
	signed int toReturn=-2;

	// проверяем, пустая ли карта?
	if (axiomParams.size() == 0) {
		toReturn = -1;
		throw AxiomLibException("Error in AxiomSet::setAxiomParameters : zero parameter map.");
	}

	// общий метод: ходим в цикле, выставляя параметры
	// при этом, не проверяется, был ли уже выставлен такой параметр в этом цикле, или нет
	for (std::map <std::string, std::map <std::string, std::map <std::string, std::string> > >::const_iterator i = axiomParams.begin(); i != axiomParams.end(); i++) {
		std::string axiomName = i->first;
		std::map <std::string, std::map <std::string, std::string> > paramMap = i->second;
		for (unsigned int j = 0; j < this->axioms.size(); j++) {
			// Ищем, какому параметру соответствует i
			if (axiomName == this->axioms[j]->name()) {
				// ok, нашли, находим value
				toReturn = 0;
				this->axioms[j]->setECParameters(paramMap);
			}
		}
	}

  	return toReturn;
}
  

/****************************************************************************
*				AxiomSet::getAxiomParameters
*
*	Description:	Функция возвращает значения установленных параметров системы аксиом
*	Parameters:	axiomParams - заполняемый параметр
*				Он определят отображение из имен аксиом на "структуру"
*				где "структура" - это отображение из имен элемнтарных условий в "низкую структуру"
*				где "низкая структура" определяет отображени из имен параметров в значния параметров
*	Returns:		0 
*	Throws:		-
*	Author:		dk
*	History:
*
****************************************************************************/
signed int AxiomSet::getAxiomParameters (std::map < std::string , std::map <std::string, std::map <std::string, std::string> > > &axiomParams) {
	std::string curName;
	axiomParams.clear();
	for (unsigned int j = 0; j < this->axioms.size(); j++) {
		std::map <std::string, std::map <std::string, std::string> > newMap;
		(this->axioms[j])->getECParameters(newMap);
		curName = this->axioms[j]->name();
		axiomParams[curName] = newMap;
	}
	return 0;
}


/****************************************************************************
*				AxiomSet::setParamValue
*
*	Description:	Функция устанавливает значение параметра по номеру аксиомы в векторе аксиом, 
*				по номеру элементарного условия в наборе аксиом, по имени параметра
*	Parameters:	param - устанавливаемое значение параметра
*				axiomNum - номер аксиомы в системе аксиом 
*				ecNum - номер элементарного условия в аксиоме
*				paramName - строка с именем параметра
*	Returns:		0 - если значение параметра обновлено
*				-1 - если значение не установлено
*	Throws:		-
*	Author:		dk
*	History:
*
****************************************************************************/
signed int AxiomSet::setParamValue (double param, int axiomNum, int ecNum, std::string &paramName) {
	// Проверка корректности входных параметров
	if ( (axiomNum >= 0) && (axiomNum < (int) axioms.size()) ) {
		return ( (axioms[axiomNum])->setParamValue(param, ecNum, paramName) );
	}
	return -1;
}
    

/****************************************************************************
*				AxiomSet::getParamValue
*
*	Description:	Функция возвращает значение параметра по номеру аксиомы в векторе аксиом, 
*				по номеру элементарного условия в наборе аксиом, по имени параметра
*				Если параметр перечислимого типа - то записывается соответсвующая числовая 
*				интерпретация данного значения и возвращается 1.
*				Если значение не числовое и не перечислимого типа - возвращается 2. 
*				(возможно введение такого рода параметров потребует дописания функции скрещивания - 
*				в данном варианте работаем с числовой интерпертацией параметров - 
*				все остальные параметры пропускаются при скрещивании)
*	Parameters:	param - заполняемое значения параметра
*				axiomNum - номер аксиомы в системе аксиом
*				ecNum - номер элементарного условия в аксиоме
*				paramName - срока с именем параметра, значение которого требуется получить
*	Returns:		0 - если удалось прочитать параметр
*				-1 - если не удалось считать параметр (возможно его просто нет в данном элементарном условии)
*				1 - если значение перечислимого типа
*				2 - если значение не числового и не перечислимго типа
*	Throws:		-
*	Author:		dk
*	History:
*
****************************************************************************/
signed int AxiomSet::getParamValue (double &param, int axiomNum, int ecNum, std::string &paramName) {
	// Проверка корректности входных параметров
	if ( (axiomNum >= 0) && (axiomNum < (int) axioms.size()) ) {
		return ( (axioms[axiomNum])->getParamValue(param, ecNum, paramName) );
	}
	return -1;
}


/****************************************************************************
*				AxiomSet::operator=
*
*	Description:	Оператор присваивания системы аксиом
*	Parameters:	second - присваиваемая система аксиом
*	Returns:		*this
*	Throws:		-
*	Author:		dk
*	History:
*
****************************************************************************/
AxiomSet & AxiomSet::operator= (const AxiomSet &second) {
	// Копируем значение контрольного бита
	crossoverControlBit = second.getCrossoverControlBit ();
	// Копируем имя системы аксиом
	nameOfAxiomSet = second.name ();
	// Копируем входящие в систему аксиомы
	for (unsigned int i = 0; i < axioms.size(); i++) {
		(axioms[i])->clear();
		delete (axioms[i]);
	}
	int num;
	num = second.getNumOfAxioms ();
	axioms.resize(num);
	for (int i = 0; i < num; i++) {
		axioms[i] = new Axiom;
		*axioms[i] = second.getAxiom (i);
	}
	return *this;
}

/****************************************************************************
*                               AxiomSet::checkWeakEquivalence
*
*       Description:    Проверка (сильной) структурной идентичности двух систем аксиом:
*                       множества аксиом совпадают.
*       Parameters:     const AxiomSet& second - набор аксиом, с которым надо проводить сравнение
*       Returns:        0 если структурной эквивалентности нет
*                       1 если структурная эквивалентность есть
*       Throws:         -
*       Author:         gevor
*       History:
*
****************************************************************************/
signed int AxiomSet::checkWeakEquivalence(const AxiomSet& second) const
{
  // последовательно сравниваем
  for(vector<Axiom*>::const_iterator ax = axioms.begin(); ax != axioms.end(); ax++) {
  }
  return 0;
}


/****************************************************************************
*                               AxiomSet::transmuteAxiom
*
*       Description:    Transmute axiom in the axiomSet with given number
*       Parameters:     axiomNum - axiom number to transmute
*       Returns:        result of transmute operation
*       Throws:         AxiomLibException - if number of axiom is wrong or params are out of boundaries
*       Author:         dk
*       History:
*
****************************************************************************/
signed int AxiomSet::transmuteAxiom (const int axiomNum, double ectl, double atl) {
	if ((axiomNum < 0) || (axiomNum >= (int) axioms.size())) 
		throw AxiomLibException("Error in AxiomSet::transmuteAxiom: wrong parameter value.");
	int res = axioms[axiomNum]->transmute(ectl, atl);
	return res;
}
