#include "cpu_info.h"
#include <fstream>
#include <iostream>
#include <string>
#include <sstream>
#include <cstring>

using namespace std;


vector<CpuInfo> get_cpu_info() {

    ifstream cpu("proc/stat");
    if (cpu.fail()) {
        cout << "Couldn't find the file\n";
        return vector<CpuInfo>();    
    }
    else {
        vector<CpuInfo> allCpus;

        string line;
        string cpuName;
        int value;
        char compare[4] = "cpu";
        
        stringstream cpuStream;
        
        for (int i = 0; i < 9; i++) {
            //includes top cpu line
            getline(cpu, line);
            /*
            get first word
            if (first word deosn't contain cpu)
                break
            create cpu
            go to next word
            get int, save int
            */
            cpuStream << line;
            cpuStream >> cpuName;
            //char *cpuWord = new char [4];
            //strcpy(cpuWord, cpuName.c_str());

/*
            if (cpuWord != compare) {
                break;
            }
*/

            //TODO:
            //many values come back incorrectly as 0,
            //find why this is happening and all tests
            //should pass. We may need to use pointers.
            //We should finish this file before continuing
            //to processes.

            struct CpuInfo cpuInfo;

            cpuStream >> value;
            cpuInfo.user_time = value;

            cpuStream >> value;
            cpuInfo.nice_time = value;

            cpuStream >> value;
            cpuInfo.system_time = value;

            cpuStream >> value;
            cpuInfo.idle_time = value;

            cpuStream >> value;
            cpuInfo.io_wait_time = value;            

            cpuStream >> value;
            cpuInfo.irq_time = value;

            cpuStream >> value;
            cpuInfo.softirq_time = value;

            cpuStream >> value;
            cpuInfo.steal_time = value;

            cpuStream >> value;
            cpuInfo.guest_time = value;

            cpuStream >> value;
            cpuInfo.guest_nice_time = value;

            allCpus.push_back(cpuInfo);

        }


        return allCpus;
    }
    
}

/**
 * Overloaded '-' operator for subtracting one CpuInfo from another.
 */
CpuInfo operator -(const CpuInfo& lhs, const CpuInfo& rhs) {
  // TODO: implement me
  return CpuInfo();
}
