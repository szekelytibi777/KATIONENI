#pragma once
#include <QFile>
#include <QTextStream >
class Log
{
public:
	static Log* instance();
	static void write(const QString& text);
	static void writeLine(const QString& text);
	Log(const QString& fileName = "KatitoNeni.log");
	~Log();
private:
	void write_(const QString& text);
	void writeLine_(const QString& text);
	static Log* instance_;
	QFile file_;
	QTextStream *stream_;
};

