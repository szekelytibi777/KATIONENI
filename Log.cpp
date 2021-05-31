#include "Log.h"

Log* Log::instance_ = 0;
Log* Log::instance()
{
	return instance_;
}

Log::Log(const QString& fileName)
	: file_(fileName)
	, stream_(0)
{
	Log::instance_ = this;
	if (file_.open(QIODevice::WriteOnly | QIODevice::Text)) {
		stream_ = new QTextStream(&file_);
	}
}

Log::~Log()
{
	if (stream_) {
		file_.close();
		delete stream_;	
	}
}

void Log::write(const QString& text)
{
	if (instance_) {
		Log::instance()->write_(text);
	}
}

void Log::writeLine(const QString& text)
{
	if (instance_) {
		Log::instance()->writeLine_(text);
	}
}

void Log::write_(const QString& text)
{
	if (stream_) {
		*stream_ << text;
	}
}

void Log::writeLine_(const QString& text)
{
	if (stream_) {
		*stream_ << text << "\n";
	}
}