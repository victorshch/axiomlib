#ifndef ASMODELONEMARKUP_H
#define ASMODELONEMARKUP_H

#include <vector>
#include <utility>

#include "AxiomExprSetPlus.h"

#include "asmodeltreeitemleaf.h"

class ASModelOneMarkup : public ASModelTreeItemLeaf
{
	Q_OBJECT
private:
	std::vector<int> m_markup;
	int m_maxAxiomNumber;
	int m_index;
	
	std::pair<int, int> m_errors;
	
	static QString markupToString(const std::vector<int> &markup);
public:
   ASModelOneMarkup(ASModelTreeItem *parent, const AxiomLib::AxiomExprSetPlus& as,
					int index);
   
   std::vector<int> &markup() { return m_markup; }
   int &index() { return m_index; }
   int &maxAxiomNumber() { return m_maxAxiomNumber; }
   
   virtual int columnCount() const;
   virtual QVariant data(int column) const;
public slots:
   void changeMarkup(const std::vector<int> &newMarkup);

};

#endif // ASMODELONEMARKUP_H
