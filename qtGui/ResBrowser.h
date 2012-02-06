/***************************************************************************
*					ResBrowser
*
*	Description:	Класс, отражающий функциональность окна с отображением
*					результатов распознавания
*	Author:			dk
*	History:
*
****************************************************************************/
#ifndef RES_BROWSER_H
#define RES_BROWSER_H

#include "ui_ResBrowser.h"
#include <stdio.h>
#include <stdlib.h>
#include <string>
#include <vector>
#include <map>
#include "boost/filesystem/operations.hpp"
#include "boost/filesystem/path.hpp"
#include "boost/filesystem/fstream.hpp"
#include "boost/tokenizer.hpp"
#include <QWidget>
#include <QListWidget>
#include <QLineEdit>
#include <QString>
#include <QProcess>
#include <QTextStream>
#ifdef Q_OS_WIN32
	#include <windows.h>
#endif

class ResBrowser : public QWidget, public Ui::resBrowser {

	Q_OBJECT

 private:

	 // Путь к папке, содержащей отображаемые результаты
	 std::string pathToResults;
	 // Пути к системам аксиом, описания которых отображаются
	 std::map <int, std::string> axiomSetDirs;

	 // Обозначние для сокращения и лучшего понимания
	 typedef boost::tokenizer<boost::char_separator<char> > token_separator;

	 // Функции класса: 
	 // Функция пробегает по директории в поиске систем аксиом и вызывает их отображение
	 void findAxiomSets(const std::string pathToDir);
	 // Функция добавляет найденную систему аксиом в окно
	 void addAxiomSetResults (std::string axiomSetPath, std::string &axiomSetName);
	 // Функция удаляет все данные, отображаемые в текущем окне
	 void clearForm ();
	 // Функция очищает поля таблицы
	 void clearTableFields();
	 // Функция делает окно затемненным если в нем нет данных или светлым в противном случае
	 void setFormEnabledOrNot();
	 // Функция открывает файл с текстовым представлением траектории
	 void openFile (std::string &path);
	 // Функция вызывает графическое отображение ряда
	 void showFile (std::string &path, int whatToShow);

 public:
	
	 // Конструктор класса	
	 ResBrowser(const std::string pathToDir);
	 // Деструктор класса
	 ~ResBrowser();

 public slots:
	
	 // Функции, вызываемые внешними воздействиями, например нажатием кнопки: 
	 void refreshForm (); // Вызывается при нажатии на кнопку обновления, вызывает обновление содержимого окна
	 void showRow(); // Соответствует кнопке отображения ряда, вызывает его графическое отображение
	 void showResults(); // Соответствует кнопке отображения результатов распознавания, вызывает их графический вид
	 void loadContents (int index); // Функция заполянет содержимое результатов для выбранной системы аксиом
	 void muCellCalled (int row, int column); // Соответствует вызову файла с описанием разметки эталонной траектории
	 void resCellCalled (int row, int column); // Соответствует вызову файла с описанием результатов распознавания
	
}; // end of class

#endif // RES_BROWSER_H
