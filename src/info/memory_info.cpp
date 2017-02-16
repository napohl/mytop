#include "memory_info.h"
#include <fstream>
#include <iostream>
#include <sstream>

using namespace std;


MemoryInfo get_memory_info() {

    /**
     * Since all meminfo files should look and behave the same,
     * we can hardcode how we progress through the file to grab 
     * all necessary values.
     */
    ifstream memory(PROC_ROOT "/meminfo");
    if (memory.fail()) {
        cout << "Couldn't find the file\n";
        return MemoryInfo();
    }
    else {
        struct MemoryInfo memInfo;
        string line;
        string value;
        int memValue;

        stringstream memoryStream;

        //Total Memory
        getline(memory, line);

        //Get the second word, which has the value we want
        memoryStream << line;
        memoryStream >> value;
        memoryStream >> memValue;

        memInfo.total_memory = memValue;


        //Free Memory
        getline(memory, line);

        memoryStream << line;
        memoryStream >> value;
        memoryStream >> memValue;

        memInfo.free_memory = memValue;


        //Buffered Memory
        getline(memory, line);

        memoryStream << line;
        memoryStream >> value;
        memoryStream >> memValue;

        memInfo.buffers_memory = memValue;


        //Cached Memory
        getline(memory, line);

        memoryStream << line;
        memoryStream >> value;
        memoryStream >> memValue;

        memInfo.cached_memory = memValue;

               
        //Total Swap Space
        for (int i = 0; i < 10; i++) {
            getline(memory, line);
        }

        memoryStream << line;
        memoryStream >> value;
        memoryStream >> memValue;

        memInfo.total_swap = memValue;

                
        //Free Swap Space
        getline(memory, line);

        memoryStream << line;
        memoryStream >> value;
        memoryStream >> memValue;

        memInfo.free_swap = memValue;

        memory.close();

        return memInfo;
    }
}
