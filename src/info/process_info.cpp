#include "process_info.h"
#include <cstdio>
#include <fstream>
#include <iostream>
#include <sstream>
#include <string>
#include <dirent.h>
#include <string.h>
#include <stdlib.h>
#include <stdint.h>

using namespace std;

/**
 * Returns a Process instance representing the process with the given process
 * ID (pid). The data for the process should be loaded from the directory formed
 * by joining the given basedir with the process ID (basedir/pid).
 */
ProcessInfo get_process(int pid, const char* basedir) {
    /**
     * So far, found three files we need. Still missing data
     * on cpu_percent and threads.
     *
     * Should probably start by creating 3 file io, checking to see if all exist
     * if one doesn't return ProcessInfo(). If they all pass create the construct
     */

    char pidString [10];
    snprintf(pidString, sizeof(pidString), "%d", pid);
    
    char memString[50];
    strcpy(memString, basedir);
    strcat(memString, "/");
    strcat(memString, pidString);
    strcat(memString, "/statm");

    char cpuSetString[50];
    strcpy(cpuSetString, basedir);
    strcat(cpuSetString, "/");
    strcat(cpuSetString, pidString);
    strcat(cpuSetString, "/cpuset");

    char statString[50];
    strcpy(statString, basedir);
    strcat(statString, "/");
    strcat(statString, pidString);
    strcat(statString, "/cpuset");

    ifstream memStat(statString);
    ifstream cpuSet(cpuSetString);
    ifstream stat(statString);
    
    if (memStat.fail() || cpuSet.fail() || stat.fail()) {
        cout << "Couldn't find the file\n";
        return ProcessInfo();
    }
    
    else {

        //turn on to check for segfault
        //return ProcessInfo();

        stringstream processStream;
        string line;
        int value;
        struct ProcessInfo process;

        //Start with statm file
        //this file is one line
        getline(memStat, line);
        processStream << line;

        processStream >> value;
        process.size = value;

        processStream >> value;
        process.resident = value;

        processStream >> value;
        process.share = value;

        processStream >> value;
        process.trs = value;

        processStream >> value;
        process.lrs = value;

        processStream >> value;
        process.drs = value;

        processStream >> value;
        process.dt = value;
        
        //end statm
        

        //comes from input
        process.pid = pid;
        

        //is a string
        //is the whole file pid/cpuset
        getline(cpuSet, line);
        process.command_line = line;


        //The rest of the values are in stat
        //this file is one line
        
        getline(stat, line);
        processStream << line;

        processStream >> value;
        process.tgid = value;

        string commValue;
        processStream >> commValue;
        //is a char array
        //whole file pid/comm
        strcpy(process.comm, commValue.c_str());

        uint8_t stateVal;
        //string stateVal;
        processStream >> stateVal;

//TODO: fix
        //is a char
        //process.state = 'R';
        //process.state = stateVal.at(0); //string version
        //process.state = 82;
        process.state = stateVal; //uint8_t version

        processStream >> value;
        process.ppid = value;
        processStream >> value;
        process.pgrp = value;
        processStream >> value;
        process.session = value;
        processStream >> value;
        process.tty_nr = value;
        processStream >> value;
        process.tpgid = value;
        processStream >> value;
        process.flags = value;
        processStream >> value;
        process.minflt = value;
        processStream >> value;
        process.cminflt = value;
        processStream >> value;
        process.majflt = value;
        processStream >> value;
        process.cmajflt = value;
        processStream >> value;
        process.utime = value;
        processStream >> value;
        process.stime = value;
        processStream >> value;
        process.cutime = value;
        processStream >> value;
        process.cstime = value;
        processStream >> value;
        process.priority = value;
        processStream >> value;
        process.nice = value;
        processStream >> value;
        process.num_threads = value;
        processStream >> value;
        process.itrealvalue = value;
        processStream >> value;
        process.starttime = value;
        processStream >> value;
        process.vsize = value;
        processStream >> value;
        process.rss = value;
        processStream >> value;
        process.rsslim = value;
        processStream >> value;
        process.startcode = value;
        processStream >> value;
        process.endcode = value;
        processStream >> value;
        process.startstack = value;
        processStream >> value;
        process.kstkesp = value;
        processStream >> value;
        process.kstkeip = value;
        processStream >> value;
        process.signal = value;
        processStream >> value;
        process.blocked = value;
        processStream >> value;
        process.sigignore = value;
        processStream >> value;
        process.sigcatch = value;
        processStream >> value;
        process.wchan = value;
        processStream >> value;
        process.nswap = value;
        processStream >> value;
        process.cnswap = value;
        processStream >> value;
        process.exit_signal = value;
        processStream >> value;
        process.processor = value;
        processStream >> value;
        process.rt_priority = value;
        processStream >> value;
        process.policy = value;
        processStream >> value;
        process.delayacct_blkio_ticks = value;
        processStream >> value;
        process.guest_time = value;
        processStream >> value;
        process.cguest_time = value;
        //end of pid/stat file

        //threads is a vector of processInfos
        //not sure why it is processInfo
        vector<ProcessInfo> processThreads;
        processThreads.push_back(ProcessInfo());
        process.threads = processThreads;

        /**
         * The percentage of a single CPU used by this process over some period. You
         * will need to calculate this value using a delta between the process at two
         * different points in time.
         */
        process.cpu_percent = 0;


    }
        
    
}

/**
 * Returns a vector consisting of one ProcessInfo entry for each process
 * currently present on the system (if passed the root of the /proc file
 * system),  or one entry for each thread in a single process (if passed a
 * /proc/<pid>/task basedir).
 */
vector<ProcessInfo> get_all_processes(const char* basedir) {
    // TODO: implement me
    return vector<ProcessInfo>();
}
