cmake_minimum_required(VERSION 3.16.1)
project(assignment6)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_FLAGS "-pthread")

add_executable(assignment6
        anomaly_detection_util.cpp
        anomaly_detection_util.h
        AnomalyDetector.h
        CLI.cpp
        CLI.h
        commands.h
        HybridAnomalyDetector.cpp
        HybridAnomalyDetector.h
        MainTrain.cpp
        minCircle.cpp
        minCircle.h
        Server.cpp
        Server.h
        SimpleAnomalyDetector.cpp
        SimpleAnomalyDetector.h
        timeseries.cpp
        timeseries.h
        #[[main.cpp]])
