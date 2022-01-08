/*
 * Author: 208994285 Yehuda Schwartz
 * and 318960168 Avital Gololobov
 */
#include "minCircle.h"
#include <algorithm>
#include <cmath>
#include <vector>

// return the distance between two points
float dist(const Point &a, const Point &b) {
    return float(sqrt(pow(a.x - b.x, 2) + pow(a.y - b.y, 2)));
}

// check whether a point is inside the circle
bool is_inside(const Circle &c, const Point &p) {
    return dist(c.center, p) <= c.radius;
}

/**
 * given 3 points create a triangle.
 * the middle of the circle is the intersection of the bisectors of the triangle.
 * @param p1 point
 * @param p2 point
 * @param p3 point
 * @return circle
 */
Circle circleFromPoints(const Point &p1, const Point &p2, const Point &p3) {
    // length of triangle edges
    float p1p2 = dist(p1, p2);
    float p1p3 = dist(p1, p3);
    float p2p3 = dist(p2, p3);
    // call edges a,b,c
    float a = p1p2;
    float b = p1p3;
    float c = p2p3;
    // calculate middle point of each edge
    Point midOfa = Point((p1.x + p2.x) / (float) 2.0, (p1.y + p2.y) / (float) 2.0);
    Point midOfb = Point((p1.x + p3.x) / (float) 2.0, (p1.y + p3.y) / (float) 2.0);
    Point midOfc = Point((p3.x + p3.x) / (float) 2.0, (p2.y + p3.y) / (float) 2.0);
    // calculate slope of every edge
    float slopeOfa = (p1.y - p2.y) / (p1.x - p2.x);
    float slopeOfb = (p1.y - p3.y) / (p1.x - p3.x);
    float slopeOfc = (p2.y - p3.y) / (p2.x - p3.x);
    // Calculate the slope of the line perpendicular to each edge
    float inverseSlopeOfa = (float) -1.0 / slopeOfa;
    float inverseSlopeOfb = (float) -1.0 / slopeOfb;
    float inverseSlopeOfc = (float) -1.0 / slopeOfc;
    // calculate the bisector line of every edge
    Line bisectorOfa = Line(inverseSlopeOfa, (midOfa.y) - (inverseSlopeOfa * midOfa.x));
    Line bisectorOfb = Line(inverseSlopeOfb, (midOfb.y) - (inverseSlopeOfb * midOfb.x));
    Line bisectorOfc = Line(inverseSlopeOfc, (midOfc.y) - (inverseSlopeOfc * midOfc.x));
    // calculate the point of intersection between 2 bisectors and that the middle of the circle
    float xValueOfBisectorOfaIntersectBisectorOfb = (bisectorOfa.b - bisectorOfb.b) / (bisectorOfb.a - bisectorOfa.a);
    Point bisectorOfaIntersectBisectorOfb =
        Point(xValueOfBisectorOfaIntersectBisectorOfb, bisectorOfa.f(xValueOfBisectorOfaIntersectBisectorOfb));
    // the radius is the distance from the middle to arbitrary point on the circle
    float r = dist(p3, bisectorOfaIntersectBisectorOfb);
    Circle circle = Circle(bisectorOfaIntersectBisectorOfb, r);
    return circle;
}
/**
 * mid point is mid of p1 and p2
 * radius is half the length between p1 and p2
 * @param p1 point
 * @param p2 point
 * @return circle
 */
Circle circleFromPoints(const Point &p1, const Point &p2) {
    Point middle = {(p1.x + p2.x) / (float) 2.0, (p1.y + p2.y) / (float) 2.0};
    return {middle, dist(p1, p2) / (float) 2.0};
}
/**
 * check if all points are inside the circle
 * @param c circle
 * @param points vector of points
 * @return
 */
bool is_valid_circle(const Circle &c, const vector<Point *> &points) {
    // if one points isn't inside the circle return not valid circle
    for (const Point *p: points) {
        if (!is_inside(c, *p)) {
            return false;
        }
    }
    return true;
}
/**
 * assume size of p is less equal to 3.
 * find the minimum enclosing circle when 3 of less points given.
 * @param points
 * @return circle
 */
Circle min_circle_trivial(vector<Point *> &points) {
    // if no points return trivial circle
    if (points.empty()) {
        return {{0, 0}, 0};
    }
        // single point is 0 radius circle
    else if (points.size() == 1) {
        return {*points[0], 0};
    }
        // for 2 points we have function
    else if (points.size() == 2) {
        return circleFromPoints(*points[0], *points[1]);
    }
    // iterate through all the combination of 3 points to find 2 points to make valid circle
    for (int i = 0; i < 3; i++) {
        for (int j = i + 1; j < 3; j++) {
            Circle c = circleFromPoints(*points[i], *points[j]);
            if (is_valid_circle(c, points)) {
                return c;
            }
        }
    }
    return circleFromPoints(*points[0], *points[1], *points[2]);
}
/**
 *
 * @param points points inside the circle
 * @param pointsR points on the circle boundary, start empty
 * @param n size of points
 * @return minimum enclosing circle.
 */
Circle findMinCircleHelper(vector<Point *> &points, vector<Point *> pointsR, size_t n) {
    // base case when all points processed or |pointsR| = 3
    if (n == 0 || pointsR.size() == 3) {
        return min_circle_trivial(pointsR);
    }
    // pick a random point randomly
    int idx = rand() % n;
    Point *p = points[idx];
    /*
     * put the picked point at the end of points
     * since it is more efficient than
     * deleting from the middle of the vector*/
    swap(points[idx], points[n - 1]);
    // get the MEC circle d from the remaining points without p
    Circle d = findMinCircleHelper(points, pointsR, n - 1);
    /*
     * assume the recursion returned valid circle for n-1 points.
     * if d contains p, return d.*/
    if (is_inside(d, *p)) {
        return d;
    }
    // otherwise, p must be on the boundary of the MEC
    pointsR.push_back(p);
    // return the MEC for points - {p} and pointsR U {p}
    return findMinCircleHelper(points, pointsR, n - 1);
}
/**
 * function convert pointer array to vector,
 * shuffle the values inside to reduce run time and send it to recursive function to calculate MEC
 * @param points array of points
 * @param size size of array
 * @return minimum enclosing circle
 */
Circle findMinCircle(Point **points, size_t size) {
    vector<Point *> p(points, points + size);
    random_shuffle(p.begin(), p.end());
    return findMinCircleHelper(p, {}, size);
}


