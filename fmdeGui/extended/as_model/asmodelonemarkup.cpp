#include <QStringList>
#include <QDebug>

#include <iostream>

#include <boost/lexical_cast.hpp>

#include "AxiomExprSetPlus.h"

#include "../relaxed_transform.h"

#include "asmodelonemarkup.h"

std::ostream &operator<<(std::ostream &ostr, const QString &str) {
	ostr<<str.toStdString();
	return ostr;
}

std::istream &operator>>(std::istream& istr, QString &str) {
	std::string result;
	istr>>result;
	str = QString::fromStdString(result);
	return istr;
}

ASModelOneMarkup::ASModelOneMarkup(ASModelTreeItem *parent, const AxiomLib::AxiomExprSetPlus& as, int index)
	: ASModelTreeItemLeaf(parent), m_markup(as.markUps[index]), m_maxAxiomNumber(as.axioms.size()), m_index(index),
      m_errors(as.errors[index])
{ }

int ASModelOneMarkup::columnCount() const {
	return 5;
}

QVariant ASModelOneMarkup::data(int column) const {
	switch(column) {
	case 0: return QVariant::fromValue(m_index + 1);
	case 2: return QVariant::fromValue(m_errors.first);
	case 3: return QVariant::fromValue(m_errors.second);
	//case 1:case 2:case 3: return QVariant::fromValue(QString(""));
	case 4: return QVariant::fromValue(markupToString(m_markup));
	default: return QVariant();
	}
}

QString ASModelOneMarkup::markupToString(const std::vector<int> &markup) {

	QStringList markupStr;
	//markupStr.reserve(markup.size());
	
	relaxed_transform(markup.begin(),
				   markup.end(),
				   std::back_inserter(markupStr),
					boost::lexical_cast<QString, int>
						   );
	
	QString result = markupStr.join(" ");
	
	qDebug()<<"Markup: "<<result;
	
	return result;	
}

void ASModelOneMarkup::changeMarkup(const std::vector<int> &newMarkup) {
	m_markup = newMarkup;
	propagateChanges(this);
	emitItemChanged(this);
}
