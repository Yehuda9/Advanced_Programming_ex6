/*
 * Author: 208994285 Yehuda Schwartz
 * and 318960168 Avital Gololobov
 */
#ifndef COMMANDS_H_
#define COMMANDS_H_

#include<iostream>
#include <cstring>
#include <algorithm>
#include <fstream>
#include <utility>
#include <vector>
#include <map>
#include <iomanip>
#include "HybridAnomalyDetector.h"

using namespace std;

class DefaultIO {
 public:
  virtual string read() = 0;
  virtual void write(string text) = 0;
  virtual void write(float f) = 0;
  virtual void read(float *f) = 0;
  virtual ~DefaultIO() {}
};
/**
 * implementation of DefaultIO interface
 * standard IO using cin and cout to read and write
 */
class StdIO : public DefaultIO {

 public:
  StdIO() : DefaultIO() {}
  /**
   * @return string line from cin
   */
  virtual string read() {
      string s;
      cin >> s;
      return s;
  }
  /**
   * write text to cout
   * @param text to write to cout
   */
  virtual void write(string text) {
      cout << text;
  }
  /**
   * write float to cout
   * @param f float number to write
   */
  virtual void write(float f) {
      cout << f;
  }
  /**
   * read from float address to cin
   * @param f address of float to read
   */
  virtual void read(float *f) {
      cin >> *f;
  }
  ~StdIO() {}
};
/**
 * Command defines an interface for commands.
 * each command has a defaultIO and description to communicate and execute() method to run the command.
 */
class Command {
 protected:
  DefaultIO &dio;
 public:
  string description;
  Command(DefaultIO &d, const string &s) : dio(d), description() {
      description = s;
  }
  virtual void execute() = 0;
  virtual ~Command() {}
};
/**
 * Upload command has also test and train timeSeries.
 */
class Upload : public Command {
 private:
  TimeSeries &tsTest;
  TimeSeries &tsTrain;
  int *numOfLines;
 public:
  /**
   * constructor to initialize Upload
   * @param dio defaultIO
   * @param s string description to pass to super
   * @param tsTrain address of timeSeries
   * @param tsTest address of timeSeries
   * @param numOfLines address to int for number of lines in the files
   */
  explicit Upload(DefaultIO &dio, const string &s, TimeSeries &tsTrain, TimeSeries &tsTest, int *numOfLines) : Command(
      dio,
      s), tsTest(tsTest), tsTrain(tsTrain) {
      this->numOfLines = numOfLines;
  }
  /**
   * given file name, this method read from client until "done" and write in to <fileName>
   * @param fileName to write
   */
  void readFile(const string &fileName) {
      //open new file
      fstream out;
      out.open(fileName, ios_base::out);
      string line;
      //read until "done" and write to file
      line = this->dio.read();
      while (line != "done") {
          out << line << endl;
          line = this->dio.read();
      }
      out.close();
  }
  /**
   * handling receiving 2 csv file.
   */
  virtual void execute() {
      dio.write("Please upload your local train CSV file.\n");
      readFile("anomalyTrain.csv");
      this->tsTrain.initialize("anomalyTrain.csv");
      dio.write("Upload complete.\n");
      dio.write("Please upload your local test CSV file.\n");
      readFile("anomalyTest.csv");
      this->tsTest.initialize("anomalyTest.csv");
      *numOfLines = (int) this->tsTest.getTable().at(0).size();
      dio.write("Upload complete.\n");
  }
  virtual ~Upload() {}

};
/**
 * ChangeSettings holds pointer to correlationThreshold.
 */
class ChangeSettings : public Command {
 private:
  float *correlationThreshold;
  virtual bool isValidCorrelationThreshold(float c) {
      return c <= 1 && c >= 0;
  }
 public:
  /**
   * constructor to ChangeSettings initialize the pointer to correlationThreshold.
   * @param dio
   * @param s
   * @param ct
   */
  explicit ChangeSettings(DefaultIO &dio, const string &s, float *ct) : Command(dio, s) {
      correlationThreshold = ct;
  }
  /**
   * receiving new correlationThreshold.
   */
  virtual void execute() {
      dio.write("The current correlation threshold is ");
      dio.write(*correlationThreshold);
      dio.write("\n");
      float t;
      float *v = &t;
      dio.write("Type a new threshold\n");
      dio.read(v);
      //read new correlation until valid
      while (!isValidCorrelationThreshold(*v)) {
          dio.write("please choose a value between 0 and 1.\n");
          dio.read(v);
      }
      //update the value inside the address of correlationThreshold.
      *this->correlationThreshold = *v;
  }
};
/**
 * AnomalyDetection has pointers to correlationThreshold, reports vector and 2 TimeSeries
 */
class AnomalyDetection : public Command {
 private:
  float *correlationThreshold;
  vector<AnomalyReport> &reports;
  TimeSeries &tsTest;
  TimeSeries &tsTrain;
 public:
  /**
   * constructor for AnomalyDetection initialize all its members to pointers from outside
   * @param dio reference to DefaultIO
   * @param s description string
   * @param r reference to report vector
   * @param tsTest reference to TimeSeries
   * @param tsTrain reference to TimeSeries
   * @param correlationThreshold pointer to correlationThreshold
   */
  explicit AnomalyDetection(DefaultIO &dio,
                            const string &s,
                            vector<AnomalyReport> &r,
                            TimeSeries &tsTest,
                            TimeSeries &tsTrain,
                            float *correlationThreshold) : Command(dio, s), reports(r), tsTest(tsTest),
                                                           tsTrain(tsTrain) {
      this->correlationThreshold = correlationThreshold;
  };
  /**
   * execute method create new HybridAnomalyDetector passing to its constructor the address of correlationThreshold.
   * invoke learnNormal method on train TimeSeries and get the report vector from detect method on test TimeSeries.
   */
  void execute() override {
      HybridAnomalyDetector ad(correlationThreshold);
      ad.learnNormal(this->tsTrain);
      this->reports = ad.detect(this->tsTest);
      dio.write("anomaly detection complete.\n");
  }
};
/**
 * GetResults hold reference to report anomalies vector
 */
class GetResults : public Command {
  vector<AnomalyReport> &results;

 public:
  /**
   * GetResults constructor
   * @param dio defaultIO
   * @param s string description
   * @param reports anomalies vector
   */
  GetResults(DefaultIO &dio, const string &s, vector<AnomalyReport> &reports) : Command(dio, s), results(reports) {}
  /**
   * write to defaultIO the anomalies vector
   */
  void execute() override {
      for (const AnomalyReport &r: results) {
          dio.write(to_string(r.timeStep) + "\t" + r.description + "\n");
      }
      dio.write("Done.\n");
  }

};

class Analyze : public Command {
  vector<AnomalyReport> &ar;
  // num of lines in csv file
  int *numOfLines;
 public:
  // constructor:
  Analyze(DefaultIO &dio, vector<AnomalyReport> &reports, int *n, const string &s) : Command(dio, s), ar(reports) {
      numOfLines = n;
  }

  // checks the ranges of anomalies that was detected in detect function
  vector<tuple<long, long>> checkDetectionRanges() {
      vector<tuple<long, long>> detectionRanges;
      string oldDescription;
      long oldTimeStep = 0;
      long start = 0;
      long end = 0;
      int numOfAnomalyReports = (int) ar.size();
      int i = 1;
      // iterates through the anomalies and checks time ranges with the same description
      for (const AnomalyReport &anomalyReport: ar) {
          string newDescription = anomalyReport.description;
          long newTimeStep = anomalyReport.timeStep;
          if (newDescription == oldDescription && oldTimeStep == newTimeStep - 1) {
              end = newTimeStep;
              // condition to insert the last anomaly
              if (i == numOfAnomalyReports) {
                  detectionRanges.emplace_back(start, end);
              }
          }
          else {
              // checks the range is bigger then 1
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
      // returns vector with tuples - start of range and end of range
      return detectionRanges;
  }

  void execute() override {
      dio.write("Please upload your local anomalies file.\n");
      string line, f, t;
      line = dio.read();
      dio.write("Upload complete.\n");
      vector<tuple<long, long>> clientAnomalies{};
      int N = *numOfLines;
      int P = 0;
      while (line != "done") {
          stringstream stringStream(line);
          getline(stringStream, f, ',');
          getline(stringStream, t, ',');
          clientAnomalies.emplace_back((long) stof(f), (long) stof(t));
          line = dio.read();
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
      dio.write("True Positive Rate: ");
      dio.write(TPRate);
      dio.write("\nFalse Positive Rate: ");
      dio.write(FPRate);
      dio.write("\n");
  }
};
/**
 * Exit holds pointer to bool variable that determine whether to loop in the CLI should stop.
 */
class Exit : public Command {
  bool *running;
 public:
  Exit(DefaultIO &dio, const string &s, bool *b) : Command(dio, s), running(b) {}
  /**
   * if user wants to exit, just turn off the running plug
   */
  void execute() override {
      *running = false;
  }
};

class CLIData {
 private:
  DefaultIO &dio;
  vector<AnomalyReport> reports{};
  vector<Command *> commands{6};
  TimeSeries *tsTest = new TimeSeries();
  TimeSeries *tsTrain = new TimeSeries();
  float *correlationThreshold = new float;
  int *numOfLines = new int;
 public:

  CLIData(DefaultIO &d, bool *r) : dio(d) {
      *correlationThreshold = 0.9;
      commands[0] = new Upload(d, "1.upload a time series csv file\n", *tsTrain, *tsTest, numOfLines);
      commands[1] = new ChangeSettings(d, "2.algorithm settings\n", correlationThreshold);
      commands[2] = new AnomalyDetection(d, "3.detect anomalies\n", reports, *tsTest, *tsTrain, correlationThreshold);
      commands[3] = new GetResults(d, "4.display results\n", reports);
      commands[4] = new Analyze(dio, reports, numOfLines, "5.upload anomalies and analyze results\n");
      commands[5] = new Exit(d, "6.exit\n", r);
  }
  void executeCommand(int i) {
      commands[i - 1]->execute();
  }
  void printMenu() {
      dio.write("Welcome to the Anomaly Detection Server.\n"
                "Please choose an option:\n");
      for (Command *command: commands) {
          dio.write(command->description);
      }
  }
  ~CLIData() {
      for (int i = 0; i < 6; ++i) {
          delete commands[i];
      }
      delete tsTest;
      delete tsTrain;
      delete numOfLines;
      delete correlationThreshold;
  }
};

#endif /* COMMANDS_H_ */

