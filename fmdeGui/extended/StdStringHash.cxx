#include <QByteArray>

#include "StdStringHash.h"

uint qHash(const std::string value) {
	//return qHash(QByteArray(value.c_str()));
	return qChecksum(value.c_str(), value.size());
}
