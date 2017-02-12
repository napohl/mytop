#include "load_average_info.h"
#include <fstream>
#include <iostream>
#include <sstream>

using namespace std;


LoadAverageInfo get_load_average() {

    /**
     * Since all files should look and behave the same,
     * we can hardcode how we progress through the file to grab 
     * all necessary values.
     */
    ifstream load("proc/loadavg");
    if (load.fail()) {
        cout << "Couldn't find the file\n";
        return LoadAverageInfo();
    }
    else {
        struct LoadAverageInfo loadInfo;
        string line;
        double loadValue;

        stringstream loadStream;

        getline(load, line);

        loadStream << line;

        loadStream >> loadValue;
        loadInfo.one_min = loadValue;

        loadStream >> loadValue;
        loadInfo.five_mins = loadValue;

        loadStream >> loadValue;
        loadInfo.fifteen_mins = loadValue;
        
        load.close();

        return loadInfo;
    }
    
}
