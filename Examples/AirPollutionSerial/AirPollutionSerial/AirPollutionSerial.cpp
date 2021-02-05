// AirPollutionSerial.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include <iostream>
#include "CsvTools.h"
#include "AirQualityIndex.h"
#include <vector>
#include <numeric>
#include <ctime>
#include <fstream> 
#include <cmath>
using namespace std;

int main()
{ 
    string fileName = "CO.csv";
    int fileLength = 0;
    aqi::Element element = aqi::Element::CO;

    printf("Reading from sensor %s...\n", fileName.c_str());
  

    

    vector<MeasurementInfo>* measurements = new vector<MeasurementInfo>();
    vector<MeasurementInfo>* normalMeasurements = new vector<MeasurementInfo>();
    csv::Read(fileName, measurements);

    float duration;
    clock_t start = clock();

    printf("Computing AQI...\n");

    int size = 0;
    double sum = 0;
    for (int i = 0; i < measurements->size(); i++) {
        if (static_cast<Status>(measurements->at(i).StatusCode) == Status::Normal) {
            sum += measurements->at(i).MeasurementValue;
            size++;
            normalMeasurements->push_back(measurements->at(i));
        }
    }

    double mean = sum / size;

    pair<double, aqi::Category> qualityIndex = aqi::ComputeAirQualityIndex(mean, element);
    if (qualityIndex.first == NULL) {
        printf("Eroare! AQI nu a putut fi calculat.");
    }
    else{
        bool isGoodAqi = aqi::CheckAirQualityIndex(qualityIndex.first, element, qualityIndex.second);

        if (isGoodAqi) {
            printf("AQI %s: %f is %s\n", aqi::ToString(element).c_str(), qualityIndex.first, aqi::ToString(qualityIndex.second).c_str());
        }
        else {
            printf("AQI %s: %f is NOT %s\n", aqi::ToString(element).c_str(), qualityIndex.first, aqi::ToString(qualityIndex.second).c_str());
        }        
    }    

    double variance = 0;
    for (int i = 0; i < normalMeasurements->size(); i++) {
        variance += pow(normalMeasurements->at(i).MeasurementValue - mean, 2);
    }

    variance = variance / normalMeasurements->size();
    double stdDeviation = sqrt(variance);

    printf("Std Deviation: %f\n", stdDeviation);

    duration = (clock() - start) / (float)CLOCKS_PER_SEC;
    printf("Computation time: %.2f seconds\n", duration);

    delete measurements;
    delete normalMeasurements;
}

// Run program: Ctrl + F5 or Debug > Start Without Debugging menu
// Debug program: F5 or Debug > Start Debugging menu

// Tips for Getting Started: 
//   1. Use the Solution Explorer window to add/manage files
//   2. Use the Team Explorer window to connect to source control
//   3. Use the Output window to see build output and other messages
//   4. Use the Error List window to view errors
//   5. Go to Project > Add New Item to create new code files, or Project > Add Existing Item to add existing code files to the project
//   6. In the future, to open this project again, go to File > Open > Project and select the .sln file
