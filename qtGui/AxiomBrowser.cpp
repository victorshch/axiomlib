/****************************************************************************
*					Функции класса AxiomBrowser.h
*
*	Description:	Функции окна с описанием аксиом - реализация
*	Author:			dk
*	History:	
*
****************************************************************************/
#include "AxiomBrowser.h"

/****************************************************************************
*					AxiomBrowser::AxiomBrowser
*
*	Description:	Конструктор класса, задает необходимые начальные параметры
*					и настраивает внешний вид
*	Parameters:		as - структура аксиомы, которую отображать в данном окне
*	Returns:		-
*	Author:			dk
*	History:		
*
****************************************************************************/
AxiomBrowser::AxiomBrowser(AxiomStructure &as) {

	setupUi(this);
	
	// Устанавливаем назавание аксиомы
	QString str;
	str = "Axiom: ";
	str.append (as.axiomName.c_str());
	label_axiom->setText(str);
	
	// Заполняем структуру аксиомы
	str.clear();
	for (unsigned int ecNum = 0; ecNum < as.ecNames.size(); ecNum++) {
		if (ecNum > 0)
			str.append (" & ");
		str.append (as.ecNames[ecNum].c_str());
	}
	str.append (".");
	textEdit_axiom->setText (str);
	
	// Добавляем параметры аксиомы
	for (unsigned int ecNum = 0; ecNum < as.ecNames.size(); ecNum++) {
		str.clear();
		QTextStream (&str) << ecNum + 1 << ". " << as.ecNames[ecNum].c_str() << " ";
		gridLayout->addWidget(new QLabel(str), gridLayout->rowCount(), 0);

		QTextEdit *te_ecParam;
		te_ecParam = new QTextEdit(this);
		te_ecParam->setObjectName(QString::fromUtf8("TextEdit_ecParam"));
		te_ecParam->setReadOnly(true);
		te_ecParam->setTextInteractionFlags(Qt::TextSelectableByKeyboard|Qt::TextSelectableByMouse);

		str.clear();
		std::pair <c_iter, c_iter> runner = as.axiomParameters.equal_range(as.ecNames[ecNum]);
		for (c_iter i = runner.first; i != runner.second; ++i) {
			if (i != runner.first)
				str.append ("\n");
			for (std::map <std::string, std::string> ::const_iterator loc = (i->second).begin(); loc != (i->second).end(); ++loc) {
				if (loc != (i->second).begin()) 
					str.append ("\n");
				str.append (loc->first.c_str());
				str.append (":  ");
				str.append (loc->second.c_str());
			}
		}
		te_ecParam->setText(str);
		gridLayout->addWidget(te_ecParam, gridLayout->rowCount() - 1, 1);
	}
}


/****************************************************************************
*					AxiomBrowser::~AxiomBrowser
*
*	Description:	Пустой деструктор класса
*	Parameters:		-
*	Returns:		-
*	Author:			dk
*	History:		
*
****************************************************************************/
AxiomBrowser::~AxiomBrowser() {	

}
