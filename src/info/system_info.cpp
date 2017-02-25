#include "system_info.h"
#include <cstdio>
#include <fstream>
#include <iostream>
#include <sstream>
#include <dirent.h>
#include <string.h>
#include <stdlib.h>

using namespace std;


double get_uptime() {
    // uptime is found under the file uptime
    ifstream uptime(PROC_ROOT "/uptime"); 
    double returnVal = 0.0;
    
    uptime >> returnVal;
       
    uptime.close();
    return returnVal;
}


SystemInfo get_system_info() {
    
    struct SystemInfo sysInfo;
    vector<ProcessInfo> processes = get_all_processes(PROC_ROOT);
  
    sysInfo.num_user_threads = 0;
    sysInfo.num_kernel_threads = 0;
    sysInfo.num_running = 0;
    sysInfo.uptime = 0;

    for (int i = 0; i < processes.size(); i++) {
        if (processes[i].state == 'R') {
            sysInfo.num_running++;
        }
        if (processes[i].is_kernel_thread()) {
            sysInfo.num_kernel_threads++;
        }
        for (int j = 0; j < processes[i].threads.size(); j++) {
            if (processes[i].threads[j].is_user_thread()) {
                sysInfo.num_user_threads++;
            }
        }
    }


    sysInfo.num_processes = processes.size();
    sysInfo.num_threads = sysInfo.num_kernel_threads + sysInfo.num_user_threads;

    sysInfo.uptime = get_uptime();

    sysInfo.load_average = get_load_average();
    sysInfo.memory_info = get_memory_info();
    sysInfo.cpus = get_cpu_info();
    sysInfo.processes = processes;
    
    return sysInfo;   
}
