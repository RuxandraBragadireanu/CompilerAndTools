#include "CsvTools.h"
#include "MeasurementInfo.h"
#include <iostream>
#include <fstream>
#include <vector>
#include <sstream>
#include <stdio.h>
#include <locale.h>

using namespace std;
namespace csv {
    void Read(string fileName, vector<MeasurementInfo>* measurementsCO) {

        std::ifstream fin;
        fin.open(fileName, ios::in);

        vector<string> row;
        string line, word, temp;

        while (fin >> temp) {
            getline(fin, line);

            row.clear();
            stringstream ss(line);
            while (getline(ss, word, ',')) {

                row.push_back(word);
            }

            MeasurementInfo measurement;
            measurement.StatusCode = stoi(row[4]);
            measurement.MeasurementValue = stof(row[3]);

            measurementsCO->push_back(measurement);              

        }
    }
}
