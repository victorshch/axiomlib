#include <iostream>
#include <fstream>

#include <boost/date_time.hpp>
#include <boost/format.hpp>
#include <boost/thread.hpp>

#include "Logger.h"

using namespace AxiomLib;

boost::scoped_ptr<Logger> Logger::m_self(0);
boost::once_flag Logger::m_once_flag = BOOST_ONCE_INIT;

Logger::Logger() : m_comments(false), m_debug(false), m_file(false), m_timestamp(false)
{
}

bool Logger::isWritingComments() const {
	return m_comments;
}

bool Logger::isWritingDebug() const {
	return m_debug;
}

bool Logger::useLogFile() const {
	return m_file;
}

bool Logger::timeStamp() const {
	return m_timestamp;
}

void Logger::setComments(bool value) {
	m_comments = value;
}

void Logger::setDebug(bool value) {
	m_debug = value;
}

void Logger::setLogFile(const std::string &fileName) {
	if(fileName.empty()) {
		this->m_file = false;
	} else {
		this->m_file = true;
		this->m_logFileName = fileName;
	}
}

void Logger::setTimeStamp(bool value) {
	m_timestamp = value;
}

void Logger::writeComment(const std::string &value) const {
	if(isWritingComments()) {
		write(value);
	}
}

void Logger::writeDebug(const std::string &str) const {
	if(isWritingDebug()) {
		write(str);
	}
}

void Logger::comment(const std::string &str)
{
	getInstance()->writeComment(str);
}

void Logger::debug(const std::string &str)
{
	getInstance()->writeDebug(str);
}

std::string Logger::getTimeStamp() const {
	auto now = boost::posix_time::second_clock::local_time();
	
	auto f = boost::format("[%s %s %s, %s:%s:%s]") % now.date().day()
	        % now.date().month() % now.date().year() %
	        now.time_of_day().hours() % now.time_of_day().minutes()
	        % now.time_of_day().seconds();
	
	return boost::lexical_cast<std::string>(f);
}

void Logger::write(const std::string &str) const {
	std::string strCopy;
	if(timeStamp()) {
		strCopy = getTimeStamp() + " " + str;
	} else {
		strCopy = str;
	}
	std::cout<<strCopy<<std::endl;
	if(m_file) {
                std::ofstream ofstr(m_logFileName.c_str());
		ofstr<<strCopy<<std::endl;
	}	
}

Logger* Logger::getInstance() {
	boost::call_once(m_once_flag, AxiomLib::Logger::reinit);
	return m_self.get();
}

void AxiomLib::Logger::reinit()
{
	Logger::m_self.reset(new Logger());
}

