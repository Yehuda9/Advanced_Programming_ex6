/*
 * Author: 208994285 Yehuda Schwartz
 * and 318960168 Avital Gololobov
 */
#include "HybridAnomalyDetector.h"
#include "minCircle.h"
HybridAnomalyDetector::HybridAnomalyDetector(float *p) : SimpleAnomalyDetector(p) {
}
HybridAnomalyDetector::~HybridAnomalyDetector() {}
/**
 * check the correlation and pass to the right method.
 * @param ts time series
 * @param col1 column 1
 * @param col2 column 2
 * @param correlation correlation between columns 1 and 2
 */
void HybridAnomalyDetector::checkCorrelation(const TimeSeries &ts, int col1, int col2, float correlation) {
    // if correlation is high, use SimpleAnomalyDetector
    if (col2 != -1 && correlation >= *this->correlationThreshold) {
        cf.push_back(SimpleAnomalyDetector::initializeCorrelatedFeatures(col1, col2, ts, correlation));
        return;
    }
    // if correlation isn't so high use HybridAnomalyDetector
    if (col2 != -1 && correlation >= 0.5) {
        cf.push_back(initializeCorrelatedFeatures(col1, col2, ts, correlation));
    }
}
/**
 * initialize correlated features struct with MEC.
 * @param i index of feature 1
 * @param j index of feature 2
 * @param ts time series
 * @param correlation correlation between features
 * @return correlatedFeatures struct
 */
correlatedFeatures HybridAnomalyDetector::initializeCorrelatedFeatures(int i,
                                                                       int j,
                                                                       const TimeSeries &ts,
                                                                       float correlation) {
    // create vector of points from values of the features
    std::vector<Point *> points;
    points.reserve(ts.getTable().at(i).size());
    for (int k = 0; k < ts.getTable().at(i).size(); ++k) {
        points.push_back(new Point(ts.getCol(i).at(k), ts.getCol(j).at(k)));
    }
    // find min circle of points
    Circle circle = findMinCircle(points.data(), points.size());
    circle.radius *= 1.1;
    for (auto p: points) {
        delete p;
    }
    points.clear();
    // initialize struct members, with circle.
    correlatedFeatures newCf =
        {.feature1 = ts.getColNames().at(i), .feature2 = ts.getColNames().at(j), .corrlation = correlation, .lin_reg = {
            0, 0}, .minCircle= circle, .threshold = circle.radius};
    return newCf;
}
/**
 * check if point is an anomaly, if true report anomaly
 * @param reports vector of reports
 * @param i time step
 * @param correlatedFeatures struct
 * @param checkPoint point to check
 */
void HybridAnomalyDetector::is_anomaly(vector<AnomalyReport> &reports,
                                       int i,
                                       correlatedFeatures &correlatedFeatures,
                                       Point &checkPoint) const {
    // if the correlation described by linear line, use SimpleAnomalyDetector methods
    if (correlatedFeatures.minCircle.radius == 0) {
        SimpleAnomalyDetector::is_anomaly(reports, i, correlatedFeatures, checkPoint);
        return;
    }
    // if the correlation described by circle, use HybridAnomalyDetector methods
    if (!is_inside(correlatedFeatures.minCircle, checkPoint)) {
        string description = correlatedFeatures.feature1 + "-" + correlatedFeatures.feature2;
        AnomalyReport newReport = AnomalyReport(description, i + 1);
        // adds the report to reports vector
        reports.push_back(newReport);
    }
}

