/*
 *
 * Author: 208994285 Yehuda Schwartz
 * and 318960168 Avital Gololobov
 */
#include "SimpleAnomalyDetector.h"

SimpleAnomalyDetector::SimpleAnomalyDetector() {}

SimpleAnomalyDetector::~SimpleAnomalyDetector() {}
/**
 * loop through the table and checks which features are the most correlated
 * @param ts time series of normal input
 */
void SimpleAnomalyDetector::learnNormal(const TimeSeries &ts) {
    for (int i = 0; i < ts.getTable().size(); ++i) {
        float correlation = 0;
        int featureIndex = -1;
        for (int j = i + 1; j < ts.getTable().size(); ++j) {
            vector<float> v1 = ts.getCol(i);
            vector<float> v2 = ts.getCol(j);
            float p = abs(pearson(v1.data(), v2.data(), (int) v1.size()));
            if (p > correlation) {
                correlation = p;
                featureIndex = j;
            }
        }
        // if found a correlation, add it to cf vector of correlated Features
        checkCorrelation(ts, i, featureIndex, correlation);
    }
}
/**
 * if correlation is high enough, add correlated features struct to correlated features vector
 * @param ts time series
 * @param col1 column 1
 * @param col2 column 2
 * @param correlation correlation between columns 1 and 2
 */
void SimpleAnomalyDetector::checkCorrelation(const TimeSeries &ts, int col1, int col2, float correlation) {
    if (col2 != -1 && correlation >= 0.9) {
        cf.push_back(initializeCorrelatedFeatures(col1, col2, ts, correlation));
    }
}
/**
* initialize correlated features with regression line
* @param i index of feature 1
* @param j index of feature 2
* @param ts time series
* @param correlation correlation between features
* @return correlatedFeatures struct
*/
correlatedFeatures SimpleAnomalyDetector::initializeCorrelatedFeatures(int i,
                                                                       int j,
                                                                       const TimeSeries &ts,
                                                                       float correlation) {
    std::vector<Point *> points;
    points.reserve(ts.getTable().at(i).size());
    for (int k = 0; k < ts.getTable().at(i).size(); ++k) {
        points.push_back(new Point(ts.getCol(i).at(k), ts.getCol(j).at(k)));
    }
    Line line = linear_reg(points.data(), (int) ts.getCol(i).size());
    //checks the point with the greatest distance from the line
    float maxDistance = 0;
    for (auto &point: points) {
        if (maxDistance < dev(*point, line)) {
            maxDistance = dev(*point, line);
        }
    }
    for (auto p: points) {
        delete p;
    }
    points.clear();
    //initialize struct members
    correlatedFeatures newCf =
        {.feature1 = ts.getColNames().at(i), .feature2 = ts.getColNames().at(j), .corrlation = correlation, .lin_reg = line, .minCircle= {
            {0, 0}, 0}, .threshold = (float) 1.1 * maxDistance};
    return newCf;
}
/**
 * detect anomalies on test input
 * @param ts time series of test input
 * @return anomaly report vector
 */
vector<AnomalyReport> SimpleAnomalyDetector::detect(const TimeSeries &ts) {
    vector<AnomalyReport> reports;
    // iterating through all correlated features
    for (correlatedFeatures &correlatedFeatures: cf) {
        // iterating through rows in the table
        for (int i = 0; i < ts.getTable().at(0).size(); ++i) {
            // creating a point from the data in the row in the correlated features columns
            float x = ts.getValByColName(correlatedFeatures.feature1, i);
            float y = ts.getValByColName(correlatedFeatures.feature2, i);
            Point checkPoint = Point(x, y);
            // checks if there is any anomalies
            is_anomaly(reports, i, correlatedFeatures, checkPoint);
        }
    }
    return reports;
}
/**
 * check if point is an anomaly, if true report anomaly
 * @param reports vector of reports
 * @param i time step
 * @param correlatedFeatures struct
 * @param checkPoint point to check
 */
void SimpleAnomalyDetector::is_anomaly(vector<AnomalyReport> &reports,
                                       int i,
                                       correlatedFeatures &correlatedFeatures,
                                       Point &checkPoint) const {
    if (correlatedFeatures.threshold < dev(checkPoint, correlatedFeatures.lin_reg)) {
        string description = correlatedFeatures.feature1 + "-" + correlatedFeatures.feature2;
        AnomalyReport newReport = AnomalyReport(description, i + 1);
        // adds the report to reports vector
        reports.push_back(newReport);
    }
}


