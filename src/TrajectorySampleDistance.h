/****************************************************************************
*					TrajectorySampleDistance
*
*	Description:	Структура для хранения расстояний от участков траектории
*					до эталонных траекторий
*	Author:			wictor
*	History:	
*
****************************************************************************/
#ifndef TRAJECTORYDISTANCELABELING_H
#define TRAJECTORYDISTANCELABELING_H

#include <vector>

namespace AxiomLib {

class TrajectorySampleDistance
{
public:
	// Конструктор, в котором задается число классов нештатного поведения
	// и длина траектории
	explicit TrajectorySampleDistance(int numClasses, int length = 0);
	
	// Длина траектории
	int length() const;
	
	void setLength(int newValue);
	
	// Число классов нештатного поведения
	int numClasses() const;
	
	// Запись расстояния от участка траектории с концом в endPoint до эталонной траектории класса classNo
	void setDistance(int classNo, int endPoint, double distance);
	
	void setDistancesForClass(int classNo, const std::vector<double>& distances);
	
	//void setDistancesForClass(int classNo, const std::vector<double>& distances);
	
	// Расстояние от участка траектории с концом в endPoint до эталонной траектории класса classNo
	double distance(int classNo, int endPoint) const;
	
private:
	std::vector<std::vector<double> > m_dist;
	int m_length;
	int m_nClasses;
};

}

#endif // TRAJECTORYDISTANCELABELING_H
