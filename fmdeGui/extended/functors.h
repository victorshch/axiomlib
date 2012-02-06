/****************************************************************************
*			functors
*
*	Description:
*	Author:		wictor
*	History:	
*
****************************************************************************/


#ifndef FUNCTORS_H_
#define FUNCTORS_H_

#include "ElemCondPlusStat.h"
#include "algorithm_controller/ManagedFuzzyDataController.h"
#include "Index.h"
#include "Condition.h"
#include <QTreeWidgetItem>

template<class OutputIterator>
class GetCheckedItems {
public:
	OutputIterator outputIterator;

	GetCheckedItems(
			OutputIterator outputIterator): outputIterator(outputIterator) {}

	void operator()(QTreeWidgetItem *item) {
		if(item->checkState(0) == Qt::Checked && !item->isHidden()) {
			*outputIterator++=
					item->text(0).toStdString();
			//qDebug()<<"Selected item : "<<item->text(0)<<"\n";
		}
	}
};

template<class T>
GetCheckedItems<T> getCheckedItems(T 
								   t) { return GetCheckedItems<T>(t); }

template<Qt::CheckState checkState>
class CheckFunctor {
public:
	void operator ()(QTreeWidgetItem *item) {
		if(!item->isHidden()) {
			item->setCheckState(0, checkState);
		}
	}
};

#endif /* FUNCTORS_H_ */
