/*
 *
 * Author: 208994285 Yehuda Schwartz
 * and 318960168 Avital Gololobov
 */
#ifndef HYBRIDANOMALYDETECTOR_H_
#define HYBRIDANOMALYDETECTOR_H_
#include "SimpleAnomalyDetector.h"
#include "minCircle.h"

class HybridAnomalyDetector : public SimpleAnomalyDetector {
 public:
  HybridAnomalyDetector();
  virtual ~HybridAnomalyDetector();

  virtual void checkCorrelation(const TimeSeries &ts, int col1, int col2, float correlation);
  virtual correlatedFeatures initializeCorrelatedFeatures(int i, int j, const TimeSeries &ts, float correlation);
  virtual void is_anomaly(vector<AnomalyReport> &reports,
                          int i,
                          correlatedFeatures &correlatedFeatures,
                          Point &checkPoint) const;
};

#endif /* HYBRIDANOMALYDETECTOR_H_ */
