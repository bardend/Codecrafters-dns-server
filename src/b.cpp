#include <bits/stdc++.h>
using namespace std;
// AbstractProduct
class Logger {
public:
    virtual void log(const string& message) = 0;
};

// ConcreteProductA
class FileLogger : public Logger {
public:
    void log(const string& message) {
        cout << "FileLogger" << endl;
        // Code to log message to a file
    }
};

// ConcreteProductB
class ConsoleLogger : public Logger {
public:
    void log(const string& message) {
        cout << "ConsoleLogger" << endl;
        // Code to log message to the console
    }
};

// AbstractFactory
class LoggerFactory {
public:
    virtual Logger* createLogger() = 0;
};

// ProductFactoryA
class FileLoggerFactory : public LoggerFactory {
public:
    Logger* createLogger() {
        return new FileLogger();
    }
};

// ProductFactoryB
class ConsoleLoggerFactory : public LoggerFactory {
public:
    Logger* createLogger() {
        return new ConsoleLogger();
    }
};

int main() {
    LoggerFactory* factory = new FileLoggerFactory();
    Logger* logger = factory->createLogger();
    logger->log("This is a file log");

    delete logger;
    delete factory;

    factory = new ConsoleLoggerFactory();
    logger = factory->createLogger();
    logger->log("This is a console log");

    delete logger;
    delete factory;
    return 0;
}

