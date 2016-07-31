#ifndef STRATEGYFRAME_H
#define STRATEGYFRAME_H

#include <string>
#include "StdStringHash.h"

#include <QWidget>
#include <QVariant>
#include <QItemEditorFactory>
#include <QStackedLayout>

#include "ParamsForm.h"

namespace Ui {
    class StrategyFrame;
}


class StrategyFrame : public AbstractParamsForm
{
    Q_OBJECT

public:
    explicit StrategyFrame(const std::string& strategyParamName, const QString& strategyLabel, QWidget *parent = 0);
    ~StrategyFrame();
	
	void addStrategy(const QList<QPair<StrategyParamSpec, QString> >& paramSpecs, const std::string& strategyName, const QString& strategyTitle);

	// Выбранная стратегия
	std::string currentStrategy() const;
	//void setCurrentStrategy(const std::string& name);
	
	// Выбранное значение параметра для выбранной стратегии
	std::string strValue(const std::string &paramName) const;
	
	void setStrValue(const std::string &paramName, const std::string &newValue);
	std::list<std::string> paramNames() const;
public slots:
	void setCurrentStrategy(int index);
private:
    Ui::StrategyFrame *ui;
	
	QStackedLayout* stackedLayout;
	
	QItemEditorFactory factory;
	
        //typedef QHash<std::string, int> ParamsContainer;
	
        //ParamsContainer m_params;
	std::string m_currentStrategy;
	std::string m_strategyParamName;
	
	void setCurrentStrategy(const std::string &strategyName);
	ParamsForm* getCurrentParamsForm() const;
};

#endif // STRATEGYFRAME_H
