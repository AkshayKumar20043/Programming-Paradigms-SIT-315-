#include <iostream>
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <thread>
#include <mutex>
#include <condition_variable>
#include <string.h>
#include <queue>
#include <fstream>
#include <sstream>
#include <iomanip>
#include <algorithm>

using namespace std;

mutex trafficMutex; // This is Mutex used for synchronization

int producer_num = 2; // These are all the number of producer and consumer threads
int consumer_num = 2;

int time_i = 48; // these are constants for tracking time 
int producer_count = 0;
int consumer_count = 0;
int total_rows = 0;

condition_variable producer_condition, consumer_condition; // This is used to signal between producer and consumer

string new_idx, timestamp, trafficLightIdStr, numCarsStr; // These atre used for for holding traffic signal information
vector<int> trafficIndices, trafficLightIds, numCars;
vector<string> trafficTimestamps;

struct TrafficData // I had made this struct to make traffic signal data row
{
    int index;
    std::string timestamp;
    int trafficLightId;
    int numCars;
};

TrafficData trafficLightTotals[4] = { {0, "", 1, 0}, {0, "", 2, 0}, {0, "", 3, 0}, {0, "", 4, 0} }; // I had initialzied this array to hold totals of each of the 4 traffic lights

queue<TrafficData> trafficDataQueue; // This is used to store traffic light data
TrafficData currentTrafficData;

bool TrafficData_comp(TrafficData first, TrafficData second) // This method is used to define sorting method for traffic signal data
{
    return first.numCars > second.numCars;
}

void* produce_data(void* args) // This method reads data then pushes it into the queue
{
    while (producer_count < total_rows)
    {
        unique_lock<mutex> lock(trafficMutex);

        if (producer_count < total_rows)
        {
            trafficDataQueue.push(TrafficData{trafficIndices[producer_count], trafficTimestamps[producer_count], trafficLightIds[producer_count], numCars[producer_count]});
            consumer_condition.notify_all();
            producer_count++;
        }
        else
        {
            producer_condition.wait(lock, []{ return producer_count < total_rows; });
        }

        lock.unlock();
        sleep(rand() % 3);
    }
}

void* consume_data(void* args) // This method processes data from the queue
{
    while (consumer_count < total_rows)
    {
        unique_lock<mutex> lock(trafficMutex);

        if (!trafficDataQueue.empty())
        {
            currentTrafficData = trafficDataQueue.front();

            if (currentTrafficData.trafficLightId == 1) // This adds the number of cars into the respective traffic light id
            {
                trafficLightTotals[0].numCars += currentTrafficData.numCars;
            }
            if (currentTrafficData.trafficLightId == 2)
            {
                trafficLightTotals[1].numCars += currentTrafficData.numCars;
            }
            if (currentTrafficData.trafficLightId == 3)
            {
                trafficLightTotals[2].numCars += currentTrafficData.numCars;
            }
            if (currentTrafficData.trafficLightId == 4)
            {
                trafficLightTotals[3].numCars += currentTrafficData.numCars;
            }
            else {}

            trafficDataQueue.pop();
            producer_condition.notify_all();
            consumer_count++;
        }
        else
        {
            consumer_condition.wait(lock, []{ return !trafficDataQueue.empty(); });
        }

        if (consumer_count % time_i == 0) // It is used for sorting data and printing
        {
            sort(trafficLightTotals, trafficLightTotals + 4, TrafficData_comp);
            printf("Traffic lights are sorted | Time: %s \n", currentTrafficData.timestamp.c_str());
            cout << "Traffic Light" << setw(20) << "The Cars are - " << endl;

            for (int i = 0; i < 4; ++i)
            {
                cout << setw(3) << trafficLightTotals[i].trafficLightId << setw(20) << trafficLightTotals[i].numCars << endl;
            }
        }

        lock.unlock();
        sleep(rand() % 3);
    }
}

void getTraffic_data() // This method is used to get traffic data from a file
{
    ifstream infile;

    string filename;
    cout << "Hello I am Akshay and welcomes you to Traffic Control" << endl;
    cout << "Enter the name of the file: ";
    cin >> filename;

    infile.open(filename);

    if (infile.is_open())
    {
        std::string line;
        getline(infile, line);

        while (!infile.eof()) // It reads the data from the file and populate vectors
        {
            getline(infile, new_idx, ',');
            trafficIndices.push_back(stoi(new_idx));
            getline(infile, timestamp, ',');
            trafficTimestamps.push_back(timestamp);
            getline(infile, trafficLightIdStr, ',');
            trafficLightIds.push_back(stoi(trafficLightIdStr));
            getline(infile, numCarsStr, '\n');
            numCars.push_back(stoi(numCarsStr));

            total_rows += 1;
        }
        infile.close();
    }
    else
    {
        printf("Error while openeing file");
    }
}

int main()
{
    getTraffic_data();

    pthread_t producerThreads[producer_num];
    pthread_t consumerThreads[consumer_num];

    for(long i=0; i<producer_num; i++) pthread_create(&producerThreads[i], NULL, produce_data, (void*) i); // It creates producer and consumer threads
    for(long i=0; i<consumer_num; i++) pthread_create(&consumerThreads[i], NULL, consume_data, (void*) i);

    for(long i=0; i<producer_num; i++) pthread_join(producerThreads[i], NULL);  // It join threads after completion
    for(long i=0; i<consumer_num; i++) pthread_join(consumerThreads[i], NULL);
}