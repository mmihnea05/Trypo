#pragma once
#include<string>
#include<iostream>
#include<fstream>
using namespace std;

// print on cmd
class Logger{
private:
	string message;

private:
	Logger();
	Logger(const Logger& obj) = delete;
	Logger& operator=(const Logger& obj) = delete;

public:
	static Logger& getInstanceLogger();
	void setMessage(string message);
	void printMessage();
	void printMessageOnFile();
};

