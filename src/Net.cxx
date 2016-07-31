/****************************************************************************
*				Net.cxx
*
*	Description:	Функции класса Net - Реализация
*	Author:		dk
*	History:		
*
****************************************************************************/
#include "Net.h"

using namespace AxiomLib;


// Стандартный конструктор - задание значений по умолчанию
Net::Net (unsigned int newQ) {
	status = false;
	q = newQ;
	inNum = 0;
	outNum = 5;
	maxOutNum = 0;
	for (int i = 1; i <= (int) outNum; i++)
		maxOutNum += (int) pow ((double) q, i);
	outCoeff.resize (outNum);
	int curQ = 1;
	for (unsigned int j = 0; j < outNum; j++) {
		outCoeff[j] = curQ;
		curQ = curQ * q;
	}
	matrices.clear();
	distValues.clear();
	usedDistValues.clear();
}


// Стандартный деструктор - пустой
Net::~Net () {

}


/****************************************************************************
*				Net::codeRes
*
*	Description:	Функция кодирования числа в q-нарный вектор. Для входного числа
*					генерируется случайное уникальное соответствие, которое 
*					кодируется в виде вектора.
*	Parameters:		input - кодируемое число
*					res - результат кодирования
*	Returns:		- 
*	Throws:			AxiomLibException - если входное число закодировать не далось
*	Author:		dk
*	History:
*
****************************************************************************/
inline void Net::codeRes (int input, std::vector <int> &res) {
	if ((input < 0) || (input > (maxOutNum - 2)))
		throw AxiomLibException("Error in Net::codeRes: incorrect input param value.");
	
	int toAdd;
	// 1. Находим для input уникальное значение, генерируемое случайно, 
	//	  чтобы близким значениям input соответствовали различные числа, желательно удаленные
	std::map <int, int> :: const_iterator iter;
	iter = distValues.find(input);
	if (iter == distValues.end()) {
		// значит для данное входное число встретилось впервые - генеррируем для него новое случайное число
		bool flag = true;
		std::set <int> :: const_iterator iterSet;
		while (flag) {
			toAdd = 1 + ((int) ((double) rand() / (double) RAND_MAX)) * (maxOutNum - 2);
			iterSet = usedDistValues.find (toAdd);
			if (iterSet == usedDistValues.end()) {
				flag = false;
				usedDistValues.insert(toAdd);
				distValues[input] = toAdd;
			}
		}
	} else {
		// значит для данного числа уже генеривалось уникальное случайное значение
		toAdd = iter->second;
	}
	
	// теперь в toAdd - хранится уникальное значение, соответствующее кодируемому числу input
	// 2. Переводим значние, соответствующее input в вид q-нарного вектора
	res.resize(outNum);

	div_t div_result;

	for (unsigned int i = outNum - 1; i !=0 ; i--) {
		div_result = div( toAdd, outCoeff[i] );
		res [i] = div_result.quot;
		toAdd = div_result.rem;
	}
}


/****************************************************************************
*				Net::decodeRes
*
*	Description:	Функция собирает декодирует из q-нарного вектора число,
*					для которого затем ищет соответствующее ему представление.
*	Parameters:		output - возвращаемое число
*					res - q-нарный вектор, из которого следует декодировать число
*	Returns:		- 
*	Throws:			AxiomLibException - если для входного вектора нет соответствующего
*					числа в списке закодированных
*	Author:		dk
*	History:
*
****************************************************************************/
inline void Net::decodeRes (int &output, std::vector <int> &res) const {
	if ((res.size() != outNum) || (outNum < 1))
		throw AxiomLibException("Error in Net::decodeRes: incorrect input param value.");

	// 1. Собираем число из ега представления в виде q-нарного вектора
	int toRet = 0;
	for (unsigned int i = 0; i < outNum; i++)
		toRet += outCoeff[i]*res[i];
	
	// 2. Находим для найденного числа соответствующее исходное число, закодированное случайным образом
	std::set <int> :: const_iterator iterSet;
	iterSet = usedDistValues.find(toRet);
	if (iterSet != usedDistValues.end()) {
		// данное число есть в карте кодов - находим его
		bool flag = false;
		std::map <int, int> :: const_iterator iter;
		for (iter = distValues.begin(); iter != distValues.end(); iter++) {
			if (iter->second == toRet) {
				flag = true;
				output = iter->first;
				break;
			}
		}
		if (!flag)
			throw AxiomLibException("Error in Net::decodeRes: incorrect input param value - strange difference between set and map.");
	} else
		// Если соответствие не найдено - выдаем ошибку
		throw AxiomLibException("Error in Net::decodeRes: incorrect input param value - there is no decode number.");
}


/****************************************************************************
*				Net::code
*
*	Description:	Функция преводит число от 0 до q-1 вклюдчительно в вектор 
*					из q элементов, с бинарными занчениями
*	Parameters:		input - кодируемое число
*					res - результат кодирования, вектор из 0 и 1
*	Returns:		- 
*	Throws:			AxiomLibException - если входное число выходит за определенные параметры
*	Author:		dk
*	History:
*
****************************************************************************/
inline void Net::code (int input, std::vector <char> &res) const {
	res.resize(q);
	if ((input < 0) || (input >= (int) q))
		throw AxiomLibException("Error in Net::code: incorrect param 'q' is set.");
	for (unsigned int i = 0; i < q; i++)
		res[i] = 0;
	res[input] = 1;
}


/****************************************************************************
*				Net::code
*
*	Description:	Функция кодирования вектора значений в q-нарном виде
*					в вид матрицы с бинарными элементами
*	Parameters:		input - входной вектор кодируемых значений
*					res - выходная матрица, закодированный вид
*	Returns:		-
*	Throws:			-
*	Author:		dk
*	History:
*
****************************************************************************/
inline void Net::code (std::vector <int> &input, std::vector <std::vector <char> > &res) const {
	res.resize(input.size());
	for (unsigned int i = 0; i < input.size(); i++)
		code (input[i], res[i]);
}


/****************************************************************************
*				Net::mult
*
*	Description:	Функция векторного перемножения 2-х векторов
*	Parameters:		x, y - входные вектора
*					res - выходная матрица, результат перемножения
*	Returns:		-
*	Throws:			-
*	Author:		dk
*	History:
*
****************************************************************************/
inline void Net::mult (std::vector <char> &x, std::vector <char> &y, std::vector <std::vector <char> > &res) const {
	res.resize(x.size());
	for (unsigned int i = 0; i < x.size(); i++) {
		res[i].resize(y.size());
		for (unsigned int j = 0; j < y.size(); j++)
			res[i][j] = x[i]*y[j];
	}
}


/****************************************************************************
*				Net::prepare
*
*	Description:	Функция обучает сеть
*	Parameters:		in - входной вектор
*					out - закодированное выходное значение
*	Returns:		0
*	Throws:		AxiomLibException - в случае ошибки
*	Author:		dk
*	History:
*
****************************************************************************/
int Net::prepare (std::vector <int> &in, int out) {
	std::vector <std::vector <char> > inCode; 
	std::vector <char> outCode;
	code (in, inCode);
	if (!status){
		// Значит первое обучение. Матрицы для коэффициентов еще не созданы
		inNum = (unsigned int) in.size();
		matrices.resize (inNum);
		for (unsigned int i = 0; i < inNum; i++) {
			matrices[i].resize(outNum);
			for (unsigned int j = 0; j < outNum; j++){
				mult (outCode, inCode[i], matrices[i][j]);


			}

		}
	}

	status = true;
	return 0;
}

 
/****************************************************************************
*				Net::check
*
*	Description:	Функция проверки, есть ли к отсчету i в ряду row одна из
*					разметок, запомненных на обучении. При этом возвращается 
*					достоверность наиболее подходящей разметки.
*	Parameters:		resNum - индекс класса нештатного поведения, заполняется
*							в данной функцие
*					row - анализируемый ряд разметки
*					i - отсчет, до которого искать вхождения разметок
*	Returns:		double (от 0 до 1) - достоверность значения, записываемого
*					в параметр resNum
*	Throws:		AxiomLibException - в случае ошибки
*	Author:		dk
*	History:
*
****************************************************************************/
double Net::check (int &resNum, std::vector <int> &row, long int &i) const {

	return 0.0;
}
