/****************************************************************************
*					Logger
*
*	Description:	Класс для логирования в cout и файл
*	Author:			wictor
*	History:	
*
****************************************************************************/
#ifndef LOGGER_H
#define LOGGER_H

#include <string>
#include <sstream>
#include <boost/scoped_ptr.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/thread/once.hpp>

#include "AxiomLibException.h"

namespace AxiomLib {

// singleton для логирования
// инстанс автоматически удаляется в конце работы программы 
// (когда выполняются деструкторы статических объектов)
class Logger
{
public:
	static Logger* getInstance();
	
	bool isWritingComments() const;
	bool isWritingDebug() const;
	bool useLogFile() const;
	bool timeStamp() const;
	
	// не нитебезопасные
	void setComments(bool value);
	void setDebug(bool value);
	void setLogFile(const std::string& fileName);
	void setTimeStamp(bool value);
	
	// Если комментарии включены, то пишет в cout строку, \n и flush'ит буфер
	// формально не нитебезопасная из-за cout
	void writeComment(const std::string& str) const;
	void writeDebug(const std::string& str) const;

	static void comment(const std::string& str);
	static void debug(const std::string& str);
	
private:
	// Эти функции вынесены в private намеренно
	Logger();
	Logger(const Logger&);
	Logger& operator=(const Logger&);
	
	bool m_comments;
	bool m_debug;
	bool m_file;
	bool m_timestamp;
	std::string m_logFileName;
	
	std::string getTimeStamp() const;
	
	void write(const std::string& str) const;
	
	static boost::scoped_ptr<Logger> m_self;
	static void reinit();
	static boost::once_flag m_once_flag;
};

// logging stream (modeled after QDebug)
class LoggerStream {
public:
	enum Type {
		Comment,
		Debug,
		Exception
	};

private:
	class LoggerStreamInternal {
	public:
		LoggerStreamInternal(LoggerStream::Type type_): type(type_), ref(1) {}

		LoggerStream::Type type;
		std::ostringstream ostr;
		int ref;
	};

	LoggerStreamInternal* mInternal;
public:
	LoggerStream(Type type) : mInternal(new LoggerStreamInternal(type)) {}
	LoggerStream(const LoggerStream& other) : mInternal(other.mInternal) {
		++mInternal->ref;
	}

	~LoggerStream() {
		if(!--mInternal->ref) {
			std::string str = mInternal->ostr.str();
			LoggerStream::Type type = mInternal->type;
			delete mInternal;
			switch(type) {
			case LoggerStream::Comment: Logger::comment(str); break;
			case LoggerStream::Debug: Logger::debug(str); break;
			case LoggerStream::Exception: throw AxiomLibException(str); break;
			}
		}
	}

	template<class T>
	LoggerStream operator<<(const T& value) {
		mInternal->ostr << value;
		return *this;
	}
};

inline LoggerStream debug() {
	return LoggerStream(LoggerStream::Debug);
}

inline LoggerStream comment() {
	return LoggerStream(LoggerStream::Comment);
}

/// Throws exception from LoggerStream's destructor -- use with care!
inline LoggerStream exception() {
	return LoggerStream(LoggerStream::Exception);
}

}

#endif // LOGGER_H
