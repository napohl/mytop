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
    // TODO: implement me
    return 0.0;
}


SystemInfo get_system_info() {
    
    /*
    pretty sure this is going to require that all other
    files are complete. Uptime may be in its own file,
    but I believe that all the other values require knowledge
    within all the processes.
    */
    
    /*
    struct SystemInfo sysInfo;

    sysInfo.num_processes = 0;
    sysInfo.num_threads = 0;
    sysInfo.num_user_threads = 0;
    sysInfo.num_kernel_threads = 0;
    sysInfo.num_running = 0;
    sysInfo.uptime = 0;

    LoadAverageInfo loadAverage = get_load_average();
    MemoryInfo memInfo = get_memory_info();
    vector<CpuInfo> cpuInfo = get_cpu_info();
    vector<ProcessInfo> processInfo = get_all_processes("proc");

    sysInfo.load_average = loadAverage;
    sysInfo.memory_info = memInfo;
    sysInfo.cpus = cpuInfo;
    sysInfo.processes = processInfo;
    */
    return SystemInfo();

    
}
