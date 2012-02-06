#ifndef QDEBUGSTREAM_H
#define QDEBUGSTREAM_H

#include <iostream>
#include <streambuf>
#include <string>

#include "qtextedit.h"
#include <QMutex>
#include <QMutexLocker>
#include <QScrollBar>
#include <QDateTime>

class LoggerServer : public QObject {
	Q_OBJECT
public:
	LoggerServer(QTextEdit *textEdit, bool autoScroll = true) : m_textEdit(textEdit), m_autoScroll(autoScroll)
	{
		//connect(this, SIGNAL(signalAppend(QString)), m_textEdit, SLOT(append(QString)), Qt::QueuedConnection);
	}

public slots:
	void append(QString str) {
		QString strWithTimestamp = QString("[%1] %2")
								   .arg(QDateTime::currentDateTime().toString("dd.MM.yyyy hh:mm:ss"), str);
		m_textEdit->append(strWithTimestamp);
		if(m_autoScroll) {
			QScrollBar *sb = m_textEdit->verticalScrollBar();
			if(sb) {
				sb->setValue(sb->maximum());
			}
			QTextCursor c = m_textEdit->textCursor();
			c.movePosition(QTextCursor::End);
			m_textEdit->setTextCursor(c);
		}

	}

	void setAutoScroll(bool value) {
		m_autoScroll = value;
	}

private:
	QTextEdit * m_textEdit;
	bool m_autoScroll;
};

class LoggerClient : public QObject {
	Q_OBJECT
public:
	LoggerClient(LoggerServer *server) {
		connect(this, SIGNAL(appendSignal(QString)), server, SLOT(append(QString)), Qt::QueuedConnection);
		connect(this, SIGNAL(setAutoScrollSignal(bool)), server, SLOT(setAutoScroll(bool)), Qt::QueuedConnection);
	}
public slots:

	void setAutoScroll(bool value) {
		emit setAutoScrollSignal(value);
	}

	void append(const QString& value) {
		emit appendSignal(value);
	}

signals:
	void appendSignal(QString str);
	void setAutoScrollSignal(bool value);
};

class QDebugStream : public std::basic_streambuf<char>
{
public:
	QDebugStream(std::ostream &stream, QTextEdit* text_edit, bool autoscroll = true) : m_stream(stream),
	m_loggerServer(text_edit)
    {
		m_old_buf = stream.rdbuf();
		stream.rdbuf(this);
    }
    ~QDebugStream()
    {
		// output anything that is left
		if (!m_string.empty()) {
			performAppend(m_string.c_str());
		}

		m_stream.rdbuf(m_old_buf);
    }

	void performAppend(const QString &str) {
		LoggerClient client(&m_loggerServer);
		client.append(m_string.c_str());
	}

	LoggerServer &loggerServer() { return m_loggerServer; }

protected:
    virtual int_type overflow(int_type v)
    {
		// Запираем данный блок mutex-ом
		//QMutexLocker locker(&mutex);

		if (v == '\n')
		{
			performAppend(m_string.c_str());
			m_string.erase(m_string.begin(), m_string.end());
		}
		else
			m_string += v;

		return v;
    }

    virtual std::streamsize xsputn(const char *p, std::streamsize n)
    {
		// Запираем данный блок mutex-ом
		//QMutexLocker locker(&mutex);

		m_string.append(p, p + n);

		int pos = 0;
		while (pos != std::string::npos)
		{
			pos = m_string.find('\n');
			if (pos != std::string::npos)
			{
				std::string tmp(m_string.begin(), m_string.begin() + pos);
				performAppend(tmp.c_str());
				m_string.erase(m_string.begin(), m_string.begin() + pos + 1);
			}
		}

		return n;
    }

private:
    std::ostream &m_stream;
    std::streambuf *m_old_buf;
    std::string m_string;

    QMutex mutex;

	LoggerServer m_loggerServer;
};



#endif // QDEBUGSTREAM_H
