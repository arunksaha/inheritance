package main

import (
  "bufio"
  "fmt"
  "log"
  "os"
  "reflect"
)

// A logging interface with two APIs.
type LoggerInterface interface {
  Log(string)
  Messages() []string
}

func Log(li LoggerInterface, mesg string) {
  li.Log(mesg)
}

func Messages(li LoggerInterface) []string {
  return li.Messages()
}


// InMemoryLogger saves the log messages in memory.
type InMemoryLogger struct {
  LoggerInterface
  messages []string
}

func (this *InMemoryLogger) Log(mesg string) {
  this.messages = append(this.messages, mesg)
}

func (this *InMemoryLogger) Messages() []string {
  return this.messages
}


// LocalLogger saves the log messages in a file.
type LocalLogger struct {
  LoggerInterface
  filename string
  file *os.File
}

func makeLocalLogger(filename string) *LocalLogger {
  file, err := os.OpenFile(filename, os.O_RDWR|os.O_CREATE, 0755)
  if err != nil {
    log.Fatal(err)
  }
  return &LocalLogger{filename: filename, file:file}
}

func (this *LocalLogger) Log(mesg string) {
  fmt.Fprintln(this.file, mesg)
}

func (this *LocalLogger) Messages() []string {
  file, err := os.Open(this.filename)
  if err != nil {
    log.Fatal(err)
  }
  defer file.Close()

  var messages []string
  scanner := bufio.NewScanner(file)
  for scanner.Scan() {
    messages = append(messages, scanner.Text())
  }
  if err := scanner.Err(); err != nil {
    log.Fatal(err)
  }
  return messages
}


func main() {
  filename := "/tmp/outfile_golang.txt";
  if len(os.Args) > 1 {
    filename = os.Args[1]
  }

  // A sequential collection of interfaces via slices.
  var loggers []LoggerInterface
  loggers = append(loggers, &InMemoryLogger{})
  loggers = append(loggers, makeLocalLogger(filename))

  var testMessages = []string{
    "Hello, World!",
    "abracadabra",
    "Sayonara!",
  }
  for _, mesg := range(testMessages) {
    for _, logger := range(loggers) {
      logger.Log(mesg)
    }
  }

  for _, logger := range(loggers) {
    observedMessages := logger.Messages()
    if !reflect.DeepEqual(observedMessages, testMessages) {
      log.Fatal("expected: ", testMessages,
        "; but observed: ", observedMessages, "\n")
    }
  }
}
