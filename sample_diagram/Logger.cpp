#include "Logger.h"
#include <chrono>
#include <format>

Logger::Logger(){}

Logger& Logger::getInstanceLogger() {
	static Logger instanceLogger;
	return instanceLogger;
}

void Logger::setMessage(string message) {
	this->message = message;
}

void Logger::printMessage() {
	cout << message << endl;
}

void Logger::printMessageOnFile() {
	ofstream fout("log.in", std::ios::app);
	// luam data curenta a sistemului
	auto now = std::chrono::system_clock::now();
	// o formatam
	std::string time_str = std::format("{:%Y-%m-%d %H:%M:%S}", now);
	// afisare propriu-zisa in fisierul de log
	fout << "Mesaj: " << message << " | Generat in data de: " << time_str << endl;

	fout.close();
}