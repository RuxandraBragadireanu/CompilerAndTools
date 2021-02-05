#include "mpi.h"
#include <iostream>
#include "CsvTools.h"
#include "AirQualityIndex.h"
#include <vector>
#include<numeric>
#include<ctime>
#include<map>
#include<cmath>
using namespace std;

int main(int argc, char *argv[])
{	
	int    nRank, nWorldSize;
	int    nSource, nDestination;
	int dataPerProcessor = 0;
	vector<MeasurementInfo>* globalMeasurements = new vector<MeasurementInfo>();
	vector<MeasurementInfo> localMeasurements;
	vector<MeasurementInfo>* normalMeasurements = new vector<MeasurementInfo>();
	vector<double> sums;
	vector<int> sizes;
	vector<double> variances;
	double aqiTimeSeconds = 0.0; 
	double mean = 0;

	aqi::Element element = aqi::Element::CO;
	
	MPI_Status *status = new MPI_Status();

	MPI_Init(&argc, &argv);
	MPI_Comm_size(MPI_COMM_WORLD, &nWorldSize);
	MPI_Comm_rank(MPI_COMM_WORLD, &nRank);	

	MPI_Datatype MPI_MEASUREMENTINFO;

	int count = 4;
	int blocklens[] = { 1, 1, 1, 1 };

	MPI_Aint indices[4];
	indices[0] = (MPI_Aint)offsetof(MeasurementInfo, StationCode);
	indices[1] = (MPI_Aint)offsetof(MeasurementInfo, ItemCode);
	indices[2] = (MPI_Aint)offsetof(MeasurementInfo, MeasurementValue);
	indices[3] = (MPI_Aint)offsetof(MeasurementInfo, StatusCode);	
	

	MPI_Datatype old_types[] = { MPI_INT, MPI_INT, MPI_FLOAT, MPI_INT };

	MPI_Type_create_struct(count, blocklens, indices, old_types, &MPI_MEASUREMENTINFO);
	MPI_Type_commit(&MPI_MEASUREMENTINFO);

	
	if (nRank == 0) {
		string fileName = "CO.csv";		

		printf("Reading from sensor %s...\n", fileName.c_str());

		//double duration;
		//clock_t start = clock();

		csv::Read(fileName, globalMeasurements);

		//duration = (clock() - start) / (double)CLOCKS_PER_SEC;
		printf("Computing AQI...\n");
		aqiTimeSeconds -= MPI_Wtime();

		dataPerProcessor = globalMeasurements->size() / nWorldSize;

		sums.resize(nWorldSize); 
		sizes.resize(nWorldSize); 
		variances.resize(nWorldSize);
	}

	MPI_Bcast(&dataPerProcessor, 1, MPI_INT, 0, MPI_COMM_WORLD);

	int* sendCount;
	int* displs;
	sendCount = new int[nWorldSize]; 
	displs = new int[nWorldSize];
	
	for (int i = 0; i < nWorldSize; i++) {
		sendCount[i] = dataPerProcessor;
		displs[i] = i*dataPerProcessor;
	}

	sendCount[0] += globalMeasurements->size() % nWorldSize;

	for (int i = 1; i < nWorldSize; i++) {
		displs[i] += globalMeasurements->size() % nWorldSize;
	}
	

	localMeasurements.resize(sendCount[nRank]);

	MPI_Scatterv(globalMeasurements->data(), sendCount, displs, MPI_MEASUREMENTINFO, localMeasurements.data(), sendCount[nRank], MPI_MEASUREMENTINFO, 0, MPI_COMM_WORLD);

	int size = 0;
	double sum = 0;
	for (int i = 0; i < localMeasurements.size(); i++) {
		if (static_cast<Status>(localMeasurements[i].StatusCode) == Status::Normal) {
			sum += localMeasurements[i].MeasurementValue;
			size++;
			normalMeasurements->push_back(localMeasurements[i]);
		}
	}

	MPI_Gather(&sum, 1, MPI_DOUBLE, sums.data(), 1, MPI_DOUBLE, 0, MPI_COMM_WORLD);
	MPI_Gather(&size, 1, MPI_INT, sizes.data(), 1, MPI_INT, 0, MPI_COMM_WORLD);

	if (nRank == 0) {
		for (int i = 1; i < sums.size(); i++) {
			sum += sums[i];
			size += sizes[i];
		}
		
		mean = sum / size;		

		pair<double, aqi::Category> qualityIndex = aqi::ComputeAirQualityIndex(mean, element);
		if (qualityIndex.first == NULL) {
			printf("Eroare! AQI nu a putut fi calculat.");
		}
		else {
			bool isGoodAqi = aqi::CheckAirQualityIndex(qualityIndex.first, element, qualityIndex.second);

			if (isGoodAqi) {
				printf("AQI %s: %f is %s\n", aqi::ToString(element).c_str(), qualityIndex.first, aqi::ToString(qualityIndex.second).c_str());
			}
			else {
				printf("AQI %s: %f is NOT %s\n", aqi::ToString(element).c_str(), qualityIndex.first, aqi::ToString(qualityIndex.second).c_str());
			}
		}
	}

	MPI_Bcast(&mean, 1, MPI_DOUBLE, 0, MPI_COMM_WORLD);
	double variance = 0;
	for (int i = 0; i < normalMeasurements->size(); i++) {
		variance += pow(normalMeasurements->at(i).MeasurementValue - mean, 2);
	}

	MPI_Gather(&variance, 1, MPI_DOUBLE , variances.data(), 1, MPI_DOUBLE, 0, MPI_COMM_WORLD);

	if (nRank == 0) {
		for (int i = 1; i < variances.size(); i++) {
			variance += variances[i];
		}
		
		variance = variance / size;
		double stdDeviation = sqrt(variance);
		printf("Std Deviation: %f\n", stdDeviation);

		aqiTimeSeconds += MPI_Wtime();
		printf("Computation time: %.2f seconds\n", aqiTimeSeconds);
	}

	MPI_Finalize();

	delete[] sendCount;
	delete[] displs; 
	delete globalMeasurements;
	delete normalMeasurements;

	return 0; 
	
}

