/*
 *
 * Author: 208994285 Yehuda Schwartz
 * and 318960168 Avital Gololobov
 */
#ifndef ANOMALYDETECTOR_H_
#define ANOMALYDETECTOR_H_

#include <string>
#include <vector>
#include "timeseries.h"
#include <cmath>
using namespace std;

class AnomalyReport {
 public:
  const string description;
  const long timeStep;
  AnomalyReport(string description, long timeStep) : description(description), timeStep(timeStep) {}
};

class TimeSeriesAnomalyDetector {
 public:
  virtual void learnNormal(const TimeSeries &ts) = 0;
  virtual vector<AnomalyReport> detect(const TimeSeries &ts) = 0;
  virtual ~TimeSeriesAnomalyDetector() {}
};

#endif /* ANOMALYDETECTOR_H_ */
