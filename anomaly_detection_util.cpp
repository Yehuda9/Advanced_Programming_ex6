/*
 * anomaly_detection_util.cpp
 *
 * Author: 208994285 Yehuda Schwartz
 * and 318960168 Avital Gololobov
 */
#include <cmath>
#include "anomaly_detection_util.h"
#include <stdexcept>
using namespace std;
void isValidParameters(const float *x, int size) {
    if (size <= 0 || !x) {
        throw logic_error("error");
    }
}
void noneZero(float n) {
    if (n == 0) {
        throw logic_error("error");
    }
}
float avg(const float *x, int size) {
    isValidParameters(x, size);
    float a = 0;
    for (int i = 0; i < size; ++i) {
        a += x[i];
    }
    return a / (float) size;
}
float var(float *x, int size) {
    isValidParameters(x, size);
    float squareSum = 0;
    for (int i = 0; i < size; ++i) {
        squareSum += x[i] * x[i];
    }
    float a = avg(x, size);
    return (squareSum / (float) size) - (a * a);
}
float cov(float *x, float *y, int size) {
    isValidParameters(x, size);
    isValidParameters(y, size);
    float eX = avg(x, size);
    float eY = avg(y, size);
    float *subX = new float[size];
    float *subY = new float[size];
    for (int i = 0; i < size; ++i) {
        subX[i] = x[i] - eX;
        subY[i] = y[i] - eY;
    }
    float *mult = new float[size];
    for (int i = 0; i < size; ++i) {
        mult[i] = subX[i] * subY[i];
    }
    float result = avg(mult, size);
    delete[] subX;
    delete[] subY;
    delete[] mult;
    return result;
}
float pearson(float *x, float *y, int size) {
    isValidParameters(x, size);
    isValidParameters(y, size);
    float sdX = sqrtf(var(x, size));
    float sdY = sqrtf(var(y, size));
    noneZero(sdX);
    noneZero(sdY);
    return cov(x, y, size) / (sdX * sdY);
}
Line linear_reg(Point **points, int size) {
    if (!points || size <= 0) {
        throw logic_error("error");
    }
    float *x = new float[size];
    float *y = new float[size];
    for (int i = 0; i < size; ++i) {
        x[i] = points[i]->x;
        y[i] = points[i]->y;
    }
    float v = var(x, size);
    noneZero(v);
    float a = cov(x, y, size) / v;
    float b = avg(y, size) - a * avg(x, size);
    Line line = Line(a, b);
    delete[] x;
    delete[] y;
    return line;
}
float dev(Point p, Line l) {
    return abs(l.f(p.x) - p.y);
}

float dev(Point p, Point **points, int size) {
    return dev(p, linear_reg(points, size));
}

