/***************************************************************************
*					AxiomSetBrowser
*
*	Description:	Класс, отражающий функциональность окна с описанием 
*					систем аксиом и статистикой по ним
*	Author:			dk
*	History:
*
****************************************************************************/
#ifndef AXIOM_SET_BROWSER_H
#define AXIOM_SET_BROWSER_H

#include "ui_AxiomSetBrowser.h"
#include "AxiomSetStructure.h"
#include "AxiomStructure.h"
#include "AxiomBrowser.h"
#include "Environment.h"
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <string>
#include <map>
#include <vector>
#include <iostream>
#include "boost/filesystem/operations.hpp"
#include "boost/filesystem/path.hpp"
#include "boost/filesystem/fstream.hpp"
#include "boost/tokenizer.hpp"
#include <QWidget>
#include <QListWidget>
#include <QLineEdit>
#include <QString>
#include <QTextStream>

class AxiomSetBrowser : public QWidget, public Ui::asBrowser {

	Q_OBJECT

 private:

	// Путь к директории, содержащей отражаемые системы аксиом
	std::string axiomSetsPath;

	// Вектор считанных описаний систем аксиом
	std::vector <AxiomSetStructure> assVec;

	// Обозначние для сокращения и лучшего понимания
	typedef boost::tokenizer<boost::char_separator<char> > token_separator;

	// Функция отыскания описаний систем аксиом и статистики по ним в указанной директории
	void findAxiomSets(const std::string pathToAxiomSets);
	// Функция очищает содержимое основной таблицы с описанием систем аксиом
	void clearTable ();
	// Функция добавляет описание системы аксиом в таблицу
	void addAxiomSet (std::string &path);
	// Функция чтения данных из файла описания системы аксиом во внутреннюю структуру
	int readFromAS (std::string &filePath, AxiomSetStructure &ass);
	// Функция вызова описания выбранной аксиомы
	void showAxiomDescription (AxiomSetStructure &ass, const int axiomNum) const;

 public:
	
	// Конструктор класса
	AxiomSetBrowser(const std::string pathToAxiomSets);
	// Деструктор класса
	~AxiomSetBrowser();

 public slots:

	// Функции, вызываемые внешними воздействиями, например нажатием кнопки: 
	void cellCalled(int , int ); // Соответствует вызову выбранной для отображения аксиомы, вызывает окно с отображением выбранной аксиомы
	void refreshForm(); // Соответствует кнопке обновления формы, вызывает новое считывание всего содержимого директории и составления таблицы с системами аксиом
	
}; // end of class

#endif // AXIOM_SET_BROWSER_H
