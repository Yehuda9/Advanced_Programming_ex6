

#ifndef COMMANDS_H_
#define COMMANDS_H_

#include<iostream>
#include <string.h>
#include <algorithm>
#include <fstream>
#include <utility>
#include <vector>
#include <map>
#include <iomanip>
#include <sys/socket.h>
#include "HybridAnomalyDetector.h"

using namespace std;

class DefaultIO {
 public:
  virtual string read() = 0;
  virtual void write(string text) = 0;
  virtual void write(float f) = 0;
  virtual void read(float *f) = 0;
  virtual ~DefaultIO() {}

  // you may add additional methods here
};

class StdIO : public DefaultIO {

 public:
  StdIO() : DefaultIO() {

  }
  virtual string read() {
      string s;
      cin >> s;
      return s;
  }
  virtual void write(string text) {
      cout << text;
  }

  virtual void write(float f) {
      cout << f;
  }

  virtual void read(float *f) {
      cin >> *f;
  }
  ~StdIO() {}
};
class SocketIO : public DefaultIO {
  int fd;
  SocketIO(int fd) noexcept(false) {
      this->fd = fd;
  }
  string read() override {
      char *buf;
      char *currChar = buf;
      while (*currChar++ != '\n') {
          recv(fd, currChar, 1, 0);
      }
      return buf;
  }
  void read(float *f) override {
      char buf[4];
      recv(fd, buf, 4, 0);
      char *t = (char *) f;
      for (int i = 0; i < 4; ++i) {
          t[i] = buf[i];
      }
  }
  void write(string text) override {
      send(fd, text.data(), text.size(), 0);
  }
  void write(float f) override {
      string text = to_string(f);
      send(fd, text.data(), text.size(), 0);
  }
};
// you may add here helper classes


// you may edit this class
class Command {
 protected:
  DefaultIO *dio;
 public:
  string description;
  Command(DefaultIO *d, const string &s) : dio(d), description() {
      description = s;
  }
  virtual void execute() = 0;
  virtual ~Command() {}
};

// option 1:
class Upload : public Command {
 private:
  TimeSeries *tsTest;
  TimeSeries *tsTrain;
  int *numOfLines;
 public:
  explicit Upload(DefaultIO *dio, const string &s, TimeSeries *tsTrain, TimeSeries *tsTest, int *numOfLines) : Command(
      dio,
      s) {
      this->tsTrain = tsTrain;
      this->tsTest = tsTest;
      this->numOfLines = numOfLines;
  }

  void readFile(const string &fileName) {
      fstream out;
      out.open(fileName, ios_base::out);
      string line;
      line = this->dio->read();
      while (line != "done") {
          out << line << endl;
          line = this->dio->read();
      }
      out.close();
  }

  virtual void execute() {
      dio->write("Please upload your local train CSV file.\n");
      readFile("anomalyTrain.csv");
      this->tsTrain->initialize("anomalyTrain.csv");
      dio->write("Upload complete.\n");
      dio->write("Please upload your local test CSV file.\n");
      readFile("anomalyTest.csv");
      this->tsTest->initialize("anomalyTest.csv");
      *numOfLines = (int) this->tsTest->getTable().at(0).size();
      dio->write("Upload complete.\n");
      close();
  }
  virtual void close() {}
  virtual ~Upload() {

  }
  //Yehuda

};

// option 2:
class ChangeSettings : public Command {
 private:
  float *correlationThreshold;
  virtual bool isValidCorrelationThreshold(float c) {
      return c <= 1 && c >= 0;
  }
 public:
  explicit ChangeSettings(DefaultIO *dio, const string &s, float *ct) : Command(dio, s) {
      correlationThreshold = ct;
  }
  virtual void execute() {
      dio->write("The current correlation threshold is ");
      dio->write(*correlationThreshold);
      dio->write("\n");
      float t;
      float *v = &t;
      dio->write("Type a new threshold\n");
      dio->read(v);
      while (!isValidCorrelationThreshold(*v)) {
          dio->write("please choose a value between 0 and 1.\n");
          dio->read(v);
      }
      *this->correlationThreshold = *v;
  }

  //Yehuda

};

// option 3:
class AnomalyDetection : public Command {
 private:
  float correlationThreshold;
  vector<AnomalyReport> *reports;
  TimeSeries *tsTest;
  TimeSeries *tsTrain;
 public:
  explicit AnomalyDetection(DefaultIO *dio,
                            const string &s,
                            vector<AnomalyReport> *r,
                            TimeSeries *tsTest,
                            TimeSeries *tsTrain,
                            float correlationThreshold) : Command(dio, s)/*,reports(r)*/{
      this->reports = r;
      this->tsTest = tsTest;
      this->tsTrain = tsTrain;
      this->correlationThreshold = correlationThreshold;
  };

  void execute() override {
      HybridAnomalyDetector ad;
      ad.learnNormal(*this->tsTrain);
      //reports(ad.detect(*this->tsTest));
      *this->reports = move(ad.detect(*this->tsTest));
      dio->write("anomaly detection complete.\n");
  }
};

// option 4:
class GetResults : public Command {
  vector<AnomalyReport> *results;

 public:
  GetResults(DefaultIO *dio, const string &s, vector<AnomalyReport> *reports) : Command(dio, s) {
      this->results = reports;
  }
  void execute() override {
      for (const AnomalyReport &r: *results) {
          dio->write(to_string(r.timeStep) + "\t" + r.description + "\n");
      }
      dio->write("Done.\n");
  }

};

// option 5:
class Analyze : public Command {
  vector<tuple<long, long>> anomaliesSequenceVec;
  vector<AnomalyReport> *ar;
  int *numOfLines;
 public:
  Analyze(DefaultIO *dio, vector<AnomalyReport> *reports, int *n, const string &s) : Command(dio, s) {
      this->ar = reports;
      numOfLines = n;
      //this->anomaliesSequenceVec = anomaliesSequence();
  }

  vector<tuple<long, long>> checkDetectionRanges() {
      vector<tuple<long, long>> detectionRanges;
      string oldDescription;
      long oldTimeStep = 0;
      long start = 0;
      long end = 0;
      int numOfAnomalyReports = (int) ar->size();
      int i = 1;
      for (const AnomalyReport &anomalyReport: *ar) {
          string newDescription = anomalyReport.description;
          long newTimeStep = anomalyReport.timeStep;
          if (newDescription == oldDescription && oldTimeStep == newTimeStep - 1) {
              end = newTimeStep;
              if (i == numOfAnomalyReports) {
                  detectionRanges.emplace_back(start, end);
              }
          }
          else {
              if (end > start) {
                  detectionRanges.emplace_back(start, end);
              }
              start = newTimeStep;
              end = newTimeStep;
          }
          oldDescription = newDescription;
          oldTimeStep = newTimeStep;
          i++;
      }
      return detectionRanges;
  }

  void execute() override {
      dio->write("Please upload your local anomalies file.\n");
      string line, f, t;
      line = dio->read();
      dio->write("Upload complete.\n");
      vector<tuple<long, long>> clientAnomalies{};
      int N = *numOfLines;
      int P = 0;
      while (line != "done") {
          stringstream stringStream(line);
          getline(stringStream, f, ',');
          getline(stringStream, t, ',');
          clientAnomalies.emplace_back((long) stof(f), (long) stof(t));
          line = dio->read();
          N -= ((int) stof(t) - (int) stof(f) + 1);
          P++;
      }

      vector<tuple<long, long>> detectionRanges = checkDetectionRanges();

      int FP = 0, TP = 0;
      for (tuple<long, long> detectionTuple: detectionRanges) {
          bool isFP = true;
          long dStart = get<0>(detectionTuple);
          long dEnd = get<1>(detectionTuple);
          for (tuple<long, long> clientTuple: clientAnomalies) {
              long cStart = get<0>(clientTuple);
              long cEnd = get<1>(clientTuple);
              if (cEnd >= dStart && dEnd >= cStart) {
                  TP++;
                  isFP = false;
                  break;
              }
          }
          if (isFP) {
              FP++;
          }
      }
      float TPRate = ((int) (1000.0 * TP / P)) / 1000.0f;
      float FPRate = ((int) (1000.0 * FP / N)) / 1000.0f;
      dio->write("True Positive Rate: ");
      dio->write(TPRate);
      dio->write("\nFalse Positive Rate: ");
      dio->write(FPRate);
      dio->write("\n");

      //      for (const AnomalyReport &anomalyReport: *ar) {
      //          bool c = true;
      //          for (tuple<long, long> tuple: clientAnomalies) {
      //              c = c && (anomalyReport.timeStep < get<0>(tuple) || anomalyReport.timeStep > get<1>(tuple));
      //              i += 1;
      //              if (!c) {
      //                  tp += 1;
      //                  break;
      //              }
      //          }
      //          if (c) {
      //              fp += i;
      //              i = 0;
      //          }
      //      }

      //      dio->write("Upload complete.\n");
      //      std::stringstream stream;
      //      stream << std::fixed << std::setprecision(3) << (float)TP / (float) clientAnomalies.size();
      //      string s = stream.str();
      //      string s1 = "True Positive Rate: " + s + '\n';
      //      dio->write(s1);
      //      std::stringstream stream1;
      //      stream1 << std::fixed << std::setprecision(3) << (float)FP / (float)N;
      //      s = stream1.str();
      //      s1 = "False Positive Rate: " + s + '\n';
      //      dio->write(s1);
  }

  // private:
  //  bool isIntersect(long f1, long t1, long f2, long t2) {
  //      return !(t2 < f1 || f2 > t1);
  //  }
  //  vector<tuple<long, long>> anomaliesSequence() {
  //      vector<bool> isDeleted(ar->size());
  //      //std::for_each(isDeleted.begin(), isDeleted.end(), [](bool b) { b = false; });
  //      vector<tuple<long, long>> result{};
  //      string d;
  //      long f, t = 0;
  //      while (find_if_not(isDeleted.begin(), isDeleted.end(), [](bool b) { return b; }) != isDeleted.end()) {
  //          bool j = true;
  //          for (int i = 0; i < ar->size(); ++i) {
  //              if (j) {
  //                  d = ar->at(i).description;
  //                  f = ar->at(i).timeStep;
  //                  t = f;
  //                  isDeleted.at(i) = true;
  //                  j = false;
  //              }
  //              else if (!isDeleted.at(i)) {
  //                  if (ar->at(i).timeStep == t + 1 && d == ar->at(i).description) {
  //                      t += 1;
  //                      isDeleted.at(i) = true;
  //                  }
  //                  else if (ar->at(i).timeStep > t + 1) {
  //                      i -= 1;
  //                      j = true;
  //                      result.emplace_back(f, t);
  //                  }
  //              }
  //          }
  //      }
  //      result.emplace_back(f, t);
  //      return result;
  //  }
};

// option 6:
class Exit : public Command {
  bool *running;
  void execute() override {
      *running = false;
  }
 public:
  Exit(DefaultIO *dio, const string &s, bool *b) : Command(dio, s), running(b) {}
};

// implement here your command classes

class CLIData {
 private:
  DefaultIO *dio;
  vector<AnomalyReport> reports{};
  vector<Command *> commands{6};
  TimeSeries *tsTest = new TimeSeries();
  TimeSeries *tsTrain = new TimeSeries();
  float *correlationThreshold = new float;
  bool *running;
  int *numOfLines = new int;
 public:
  CLIData(DefaultIO *d, bool *r, float ct) : dio(d) {
      running = r;
      *correlationThreshold = ct;
      //commands = new Command*[6];
      commands[0] = new Upload(d, "1.upload a time series csv file\n", tsTrain, tsTest, numOfLines);
      commands[1] = new ChangeSettings(d, "2.algorithm settings\n", correlationThreshold);
      commands[2] = new AnomalyDetection(d, "3.detect anomalies\n", &reports, tsTest, tsTrain, *correlationThreshold);
      commands[3] = new GetResults(d, "4.display results\n", &reports);
      commands[4] = new Analyze(dio, &reports, numOfLines, "5.upload anomalies and analyze results\n");
      commands[5] = new Exit(d, "6.exit\n", running);
  }
  void executeCommand(int i) {
      commands[i - 1]->execute();
  }
  void printMenu() {
      dio->write("Welcome to the Anomaly Detection Server.\n"
                 "Please choose an option:\n");
      /*dio->write((*commands)->description);
      dio->write(commands[1]->description);*/
      /*for (int i = 0; i < 6; ++i) {
          dio->write(commands[i]->description);
      }*/
      for (Command *command: commands) {
          dio->write(command->description);
      }
  }
  bool getRunning() {
      return *this->running;
  }
};

#endif /* COMMANDS_H_ */
