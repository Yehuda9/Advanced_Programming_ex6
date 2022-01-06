/*
 *
 * Author: 208994285 Yehuda Schwartz
 * and 318960168 Avital Gololobov
 */
#ifndef SIMPLEANOMALYDETECTOR_H_
#define SIMPLEANOMALYDETECTOR_H_

#include "anomaly_detection_util.h"
#include "AnomalyDetector.h"
#include "minCircle.h"
#include <vector>
#include <algorithm>
#include <cstring>
#include <cmath>

struct correlatedFeatures {
  string feature1, feature2;  // names of the correlated features
  float corrlation;
  Line lin_reg;
  Circle minCircle;
  float threshold;
};

class SimpleAnomalyDetector : public TimeSeriesAnomalyDetector {
 protected:
  vector<correlatedFeatures> cf;
 public:
  SimpleAnomalyDetector();
  virtual ~SimpleAnomalyDetector();

  virtual void learnNormal(const TimeSeries &ts);
  virtual correlatedFeatures initializeCorrelatedFeatures(int i, int j, const TimeSeries &ts, float corrlation);
  virtual vector<AnomalyReport> detect(const TimeSeries &ts);
  virtual void is_anomaly(vector<AnomalyReport> &reports,
                          int i,
                          correlatedFeatures &correlatedFeatures,
                          Point &checkPoint) const;
  vector<correlatedFeatures> getNormalModel() {
      return cf;
  }

  virtual void checkCorrelation(const TimeSeries &ts, int col1, int col2, float correlation);
};

#endif /* SIMPLEANOMALYDETECTOR_H_ */
