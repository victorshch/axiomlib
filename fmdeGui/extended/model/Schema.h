#ifndef SCHEMA_H
#define SCHEMA_H

#include <QString>
#include <QHash>

class Schema
{
public:
	// Число колонок в представлении
	int columnCount() const;
    const QString& getColumnId(int column, int role) const;
	QList<int> getRoles(int column) const;
	
	static Schema* create();
	
	void addColumnIdMapping(const QString& columnId, int column, int role);
private:
	Schema();
	Schema(const Schema& other);
	void operator=(const Schema& other);
	
	QHash<int, QHash<int, QString> > m_mapping;
	
	static QString m_empty;
};

#endif // SCHEMA_H
