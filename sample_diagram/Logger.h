#pragma once
#include<string>
#include<iostream>
#include<fstream>

// print on cmd
class Logger{
private:
	std::string message;

private:
	Logger();
	Logger(const Logger& obj) = delete;
	Logger& operator=(const Logger& obj) = delete;

public:
	static Logger& getInstanceLogger();
	void setMessage(std::string message);
	void printMessage();
	void printMessageOnFile();
};

