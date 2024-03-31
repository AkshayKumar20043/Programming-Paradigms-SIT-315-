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

mutex mutex1;

// This is the initialization of threads for producers and consumers
int p_num_threads = 2;
int c_num_threads = 2;

int hour_ind = 48; // Value of 48 hour 

int ccount = 0; // Producer counter
int con_count = 0; // Consumer counter
int m = 0; // Number of rows

condition_variable producer_cv, consumer_cv; // These are condition variables for producers and consumers

// String variables and vectors are initialized to store values from the data file
string ind, t_stamp, tr_light_id, no_of_cars;
vector<int> in;
vector<int> tr_light;
vector<int> no_cars;
vector<string> tstamp;

// Struct for traffic data row
struct TrafficSignal
{
    int ind;
    std::string t_stamp;
    int tr_id;
    int num_cars;
};

// Array of TrafficSignal structures initialized to hold the totals of each of the 4 traffic lights
TrafficSignal tlSorter[4] = { {0, "", 1, 0}, {0, "", 2, 0}, {0, "", 3, 0}, {0, "", 4, 0} };

// Queue to store traffic light data
queue<TrafficSignal> tr_sig_queue;
TrafficSignal sig;

// Function to sort traffic light data
bool sortMethod(TrafficSignal first, TrafficSignal second)
{
    if (first.num_cars > second.num_cars)
        return true;
    return false;
}

void* produce(void* args)
{
    while (ccount < m)
    {
        unique_lock<mutex> lk(mutex1); // Locking until producer finishes processing 

        if (ccount < m) // If count is less than the number of rows in the dataset 
        {
            tr_sig_queue.push(TrafficSignal{in[ccount], tstamp[ccount], tr_light[ccount], no_cars[ccount]}); // Push into queue
            consumer_cv.notify_all(); // Notifying consumer threads
            ccount++;
        }
        else
        {
            producer_cv.wait(lk, []{ return ccount < m; }); // If count is greater than the number of rows in the dataset, wait
        }

        lk.unlock(); // Unlock after processing
        sleep(rand()%3);
    }
    
    return nullptr;
}

void* consume(void* args)
{
    while(con_count < m)
    {
        unique_lock<mutex> lk(mutex1); // Lock until processing

        if (!tr_sig_queue.empty()) 
        {
            sig = tr_sig_queue.front(); // Get the front element of the queue

            // Add the number of cars to the respective traffic light ID
            if (sig.tr_id == 1)
            {
                tlSorter[0].num_cars += sig.num_cars; 
            }
            if (sig.tr_id == 2)
            {
                tlSorter[1].num_cars += sig.num_cars;
            }
            if (sig.tr_id == 3)
            {
                tlSorter[2].num_cars += sig.num_cars;
            }
            if (sig.tr_id == 4)
            {
                tlSorter[3].num_cars += sig.num_cars;
            }

            tr_sig_queue.pop(); // Pop the data
            producer_cv.notify_all(); // Notify producer
            con_count++;
        }
        else
        { 
            consumer_cv.wait(lk, []{ return !tr_sig_queue.empty(); }); // If the queue is empty, wait until the producer produces
        }

        // Check if an hour has passed, checking every 48th row
        if (con_count % hour_ind == 0)
        {
            sort(tlSorter, tlSorter + 4, sortMethod); // Sorting data
            printf("Traffic lights sorted according to most busy| Time: %s \n", sig.t_stamp.c_str());
            cout << "Traf Lgt" << "\t" << "Number of Cars" << endl;
            cout << tlSorter[0].tr_id << "\t\t\t" << "\t" << tlSorter[0].num_cars << endl;
            cout << tlSorter[1].tr_id << "\t\t\t" << "\t" << tlSorter[1].num_cars << endl;
            cout << tlSorter[2].tr_id << "\t\t\t" << "\t" << tlSorter[2].num_cars << endl;
            cout << tlSorter[3].tr_id << "\t\t\t" << "\t" << tlSorter[3].num_cars << endl;
        }
        
        lk.unlock();
        sleep(rand()%3);
    }
    
    return nullptr;
}

// Function to get data from file
void getTrafficData()
{ 
    ifstream infile;
    string file;
    cout << "Using file.txt ....";
    file = "data.txt";

    infile.open(file);

    if (infile.is_open())                                
    {
        std::string line;
        getline(infile, line);

        while (!infile.eof()) 
        {
            getline(infile, ind, ',');
            in.push_back(stoi(ind));
            getline(infile, t_stamp, ',');
            tstamp.push_back(t_stamp);
            getline(infile, tr_light_id, ',');
            tr_light.push_back(stoi(tr_light_id));
            getline(infile, no_of_cars, '\n');
            no_cars.push_back(stoi(no_of_cars));
            
            m += 1; 
        }
        infile.close();
    }
    else printf("Could not open file, try again.");
}

int main() 
{
    getTrafficData();
    
    pthread_t producers[p_num_threads];
    pthread_t consumers[c_num_threads];
	
    for(long i=0; i<p_num_threads; i++) pthread_create(&producers[i], NULL, produce, (void*) i); 
    for(long i=0; i<c_num_threads; i++) pthread_create(&consumers[i], NULL, consume, (void*) i);

    for(long i=0; i<p_num_threads; i++) pthread_join(producers[i], NULL);
    for(long i=0; i<c_num_threads; i++) pthread_join(consumers[i], NULL);
}
