#include "Schema.h"

QString Schema::m_empty = "";

Schema::Schema()
{
}

Schema* Schema::create() {
	return new Schema();
}

int Schema::columnCount() const {
	return m_mapping.size();
}

const QString& Schema::getColumnId(int column, int role) const {
	if(!m_mapping.count(column)) {
		return m_empty;
	}
	if(!m_mapping[column].count(role)) {
		return m_empty;
	}
	return m_mapping[column][role];
}

QList<int> Schema::getRoles(int column) const {
	if(!m_mapping.count(column)) {
		return QList<int>();
	} else {
		return m_mapping[column].keys();
	}
}

void Schema::addColumnIdMapping(const QString &columnId, int column, int role) {
	m_mapping[column][role] = columnId;
}
