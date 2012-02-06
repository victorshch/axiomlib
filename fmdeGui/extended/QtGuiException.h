#ifndef QTGUIEXCEPTION_H
#define QTGUIEXCEPTION_H

#include <stdexcept>
#include <string>
#include <QString>

class QtGuiException : public std::logic_error
{
public:
	QtGuiException(const std::string &message): logic_error(message) {}
	QtGuiException(const QString &message): logic_error(message.toStdString()) {}
	QtGuiException(const char *message): logic_error(message) {}
	
	static void assertCondition(bool condition, const std::string &errorText) {
		if(!condition) {
			throw QtGuiException(errorText);
		}
	}
};

#endif // QTGUIEXCEPTION_H
