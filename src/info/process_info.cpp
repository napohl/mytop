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
#include <algorithm>

using namespace std;

/**
 * This function is used to locate all pids inside the basedirectory given to it.
 * Most uses should be for either locating threads within a process, or finding all
 * processes in proc.
 */
vector<int> getAllPids(const char* basedir) {
    vector<int> pids;
    // these are variables to store directory info
    DIR *dir;
    struct dirent *ent;

    // open the directory
    dir = opendir(basedir);

    if (!dir) {
        //cout << "Couldn't open directory\n";
        return pids;
    }

    while ((ent = readdir(dir))) {
        char* dirName = ent->d_name; 

        // only want to count directories if they have digits (these are pids)
        if (isdigit(dirName[0])) {
            // convert to int
            int pid = atoi(dirName);

            pids.push_back(pid);
        }
    }

    closedir(dir);

    return pids;

}


ProcessInfo get_process(int pid, const char* basedir) {

    char pidString [10];
    snprintf(pidString, sizeof(pidString), "%d", pid);
    
    char memString[50];
    strcpy(memString, basedir);
    strcat(memString, "/");
    strcat(memString, pidString);
    strcat(memString, "/statm");

    char commandString[50];
    strcpy(commandString, basedir);
    strcat(commandString, "/");
    strcat(commandString, pidString);
    strcat(commandString, "/cmdline");

    char statString[50];
    strcpy(statString, basedir);
    strcat(statString, "/");
    strcat(statString, pidString);
    strcat(statString, "/stat");

    ifstream memStat(memString);
    ifstream commandLine(commandString);
    ifstream stat(statString);
    
    if (memStat.fail() || commandLine.fail() || stat.fail()) {
        //cout << "Couldn't find the file\n";
        return ProcessInfo();
    }
    
    else {

        //turn on to check for segfault
        //return ProcessInfo();

        //cout << "Just started the process\n\n\n";

        stringstream memStatStream;
        string line;
        struct ProcessInfo process;

        //Start with statm file
        //this file is one line
        getline(memStat, line);
        memStatStream << line;

        memStatStream >> process.size;
        memStatStream >> process.resident;
        memStatStream >> process.share;
        memStatStream >> process.trs;
        memStatStream >> process.lrs;
        memStatStream >> process.drs;
        memStatStream >> process.dt;
        
        //end statm
        

        //comes from input
        process.pid = pid;

        //cout << "Finished statm, size = " << process.size << "\n\n\n";
        

        //is a string
        //is the whole file pid/cpuset
        getline(commandLine, process.command_line);

        //The rest of the values are in stat
        //this file is one line
        stringstream processStream;
        getline(stat, line);
        processStream << line;

        processStream >> process.tgid;

        //string commValue;
        processStream >> process.comm;
        //is a char array
        //whole file pid/comm
        //strcpy(process.comm, commValue.c_str());

        uint8_t stateVal;
        //string stateVal;
        processStream >> stateVal;

//TODO: fix
        //is a char
        //process.state = 'R';
        //process.state = stateVal.at(0); //string version
        //process.state = 82;
        process.state = stateVal; //uint8_t version

        processStream >> process.ppid;
        processStream >> process.pgrp;
        processStream >> process.session;
        processStream >> process.tty_nr;
        processStream >> process.tpgid;
        processStream >> process.flags;
        processStream >> process.minflt;
        processStream >> process.cminflt;
        processStream >> process.majflt;
        processStream >> process.cmajflt;
        processStream >> process.utime;
        processStream >> process.stime;
        processStream >> process.cutime;
        processStream >> process.cstime;
        processStream >> process.priority;
        processStream >> process.nice;
        processStream >> process.num_threads;
        processStream >> process.itrealvalue;
        processStream >> process.starttime;
        processStream >> process.vsize;
        processStream >> process.rss;
        processStream >> process.rsslim;
        processStream >> process.startcode;
        processStream >> process.endcode;
        processStream >> process.startstack;
        processStream >> process.kstkesp;
        processStream >> process.kstkeip;
        processStream >> process.signal;
        processStream >> process.blocked;
        processStream >> process.sigignore;
        processStream >> process.sigcatch;
        processStream >> process.wchan;
        processStream >> process.nswap;
        processStream >> process.cnswap;
        processStream >> process.exit_signal;
        processStream >> process.processor;
        processStream >> process.rt_priority;
        processStream >> process.policy;
        processStream >> process.delayacct_blkio_ticks;
        processStream >> process.guest_time;
        processStream >> process.cguest_time;
        //end of pid/stat file

        char taskString[50];
        strcpy(taskString, basedir);
        strcat(taskString, "/");
        strcat(taskString, pidString);
        strcat(taskString, "/task");

        //cout << "starting threads, task is in this state: " << process.state << "\n\n\n";
        process.threads = get_all_processes(taskString);

        //all thread group ids are equal to the process pid
        for (int i = 0; i < process.threads.size(); i++) {
            process.threads[i].tgid = process.pid; 
        }

        //need to clean the command_line up
        //if it is empty, we need to use comm without parenthesis
        //if it is not empty, we need to get rid of the null characters
        if (process.command_line == "") {
            string str(process.comm);

            //cout << "command_line before: " << str << endl;

            size_t i = 0;
            while (i < str.size()) {
                if (str[i] == '(' || str[i] == ')') str.erase(i,1);
                else ++i;
            }

            //cout << "command line after: " << str << endl;
    
            process.command_line = str;
        }
        else {
            // change weird null characters (\0) in the cmdline file to spaces.
            replace(process.command_line.begin(), process.command_line.end(), '\0', ' ');
            process.command_line = process.command_line.substr(0, process.command_line.size() - 1);
        }

        /**
         * The percentage of a single CPU used by this process over some period. You
         * will need to calculate this value using a delta between the process at two
         * different points in time.
         */
        process.cpu_percent = 0;

        return process;


    }
        
    
}

/**
 * Returns a vector consisting of one ProcessInfo entry for each process
 * currently present on the system (if passed the root of the /proc file
 * system),  or one entry for each thread in a single process (if passed a
 * /proc/<pid>/task basedir).
 */
vector<ProcessInfo> get_all_processes(const char* basedir) {
    
    vector<int> pids = getAllPids(basedir);

    vector<ProcessInfo> allProcesses;
    ProcessInfo process;
    
    for (int i = 0; i < pids.size(); i++) {
        //cout << "calling get process\n\n\n";
        process = get_process(pids[i], basedir);
        allProcesses.push_back(process);
    }

    return allProcesses;
}
