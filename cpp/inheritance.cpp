#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <memory>

using namespace std;

// An abstract (base) class to model a logging interface with two APIs.
class LoggerInterface {
  public:
    virtual ~LoggerInterface() = 0;
    virtual void Log(string const& mesg) = 0;
    virtual vector<string> Messages() const = 0;
};

LoggerInterface::~LoggerInterface() {}


// InMemoryLogger saves the log messages in memory.
class InMemoryLogger : public LoggerInterface {
  public:
    InMemoryLogger() = default;
    virtual ~InMemoryLogger() = default;
    virtual void Log(string const& mesg);
    virtual vector<string> Messages() const;
  private:
    vector<string> messages;
};

void InMemoryLogger::Log(string const& mesg) {
  messages.push_back(mesg);
}

vector<string> InMemoryLogger::Messages() const {
  return messages;
}


// LocalLogger saves the log messages in a file.
class LocalLogger : public LoggerInterface {
  public:
    LocalLogger(char const * const file);
    virtual ~LocalLogger();
    virtual void Log(string const& mesg);
    virtual vector<string> Messages() const;
  private:
    char const * const filename{nullptr};
    fstream outfile;
};

LocalLogger::LocalLogger(char const * const file) :
  filename{file} {
  outfile.open(filename, fstream::out);
  if (!outfile.is_open()) {
    cerr << "Failed to open file: " << filename << '\n';
    exit(1);
  }
}

void LocalLogger::Log(string const& mesg) {
  outfile << mesg << '\n';
  outfile.flush();
}

vector<string> LocalLogger::Messages() const {
  vector<string> messages;
  ifstream infile(filename);
  for (string line; getline(infile, line); ) {
    messages.push_back(line);
  }
  return messages;
}

LocalLogger::~LocalLogger() {
  outfile.close();
}


// Helper function to emit vector<string>.
ostream& operator<<(ostream& os, vector<string> const& vs) {
  for (auto const& s : vs) {
    os << s << '\n';
  }
  return os;
}


int main(int argc, char * argv[1]) {
  char const * const default_filename = "/tmp/outfile_cpp.txt";
  char const * const filename = (argc > 1) ? argv[1] : default_filename;

  // A sequential collection of interfaces via vector of unique pointers.
  vector<unique_ptr<LoggerInterface>> loggers;
  loggers.emplace_back(make_unique<InMemoryLogger>());
  loggers.emplace_back(make_unique<LocalLogger>(filename));

  vector<string> const testMessages = {
    "Hello, World!",
    "abracadabra",
    "Sayonara!"
  };
  for (auto const& mesg : testMessages) {
    for (auto& logger : loggers) {
      logger->Log(mesg);
    }
  }

  for (auto& logger : loggers) {
    auto const observedMessages = logger->Messages();
    if (observedMessages != testMessages) {
      cerr << "expected: " << testMessages
           << "; but observed: " << observedMessages << '\n';
      exit(1);
    }
  }
}
