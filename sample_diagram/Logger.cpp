#include "Logger.h"
#include <chrono>
#include <format>

Logger::Logger() = default;

Logger& Logger::getInstanceLogger() {
	static Logger instanceLogger;
	return instanceLogger;
}

void Logger::setMessage(std::string message) {
	this->message = message;
}

void Logger::printMessage() {
	std::cout << message << std::endl;
}

void Logger::printMessageOnFile() {
	std::ofstream fout("log.in", std::ios::app);
	// luam data curenta a sistemului
	auto now = std::chrono::system_clock::now();
	// o formatam
	std::string time_str = std::format("{:%Y-%m-%d %H:%M:%S}", now);
	// afisare propriu-zisa in fisierul de log
	fout << "Mesaj: " << message << " | Generat in data de: " << time_str << std::endl;

	fout.close();
}