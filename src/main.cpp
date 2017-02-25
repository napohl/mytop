#include <cstdlib>
#include <ncurses.h>
#include <unistd.h>
#include <getopt.h>
#include <iostream>
#include <map>
#include <algorithm>
#include <string.h>

#include "info/system_info.h"
#include "info/load_average_info.h"
#include "info/memory_info.h"
#include "info/cpu_info.h"


using namespace std;

const int TOTAL_PID_SHOWN = 10;

const struct option options[] = 
{
    {"delay", required_argument, NULL, 'd'},
    {"sort-key", required_argument, NULL, 's'},
    {"help", no_argument, NULL, 'h'},
    {0,0,0,0}
};

enum ColumnSort {PID, MEM, CPU, TIME};


/**
 * Gets a character from the user, waiting for however many milliseconds that
 * were passed to timeout() below. If the letter entered is q, this method will
 * exit the program.
 */
void exit_if_user_presses_q() {
    char c = getch();

    if (c == 'q') {
        endwin();
        exit(EXIT_SUCCESS);
    }
}

/**
 * These functions will be used for sort to sort all processes by the desired specification
 */
bool pidSort(const ProcessInfo& a, const ProcessInfo& b) {
    return a.pid < b.pid;
}

bool cpuSort(const ProcessInfo& a, const ProcessInfo& b) {
    return a.cpu_percent > b.cpu_percent;
}

bool memSort(const ProcessInfo& a, const ProcessInfo& b) {
    return a.rss > b.rss;
}

bool timeSort(const ProcessInfo& a, const ProcessInfo& b) {
    return (a.utime + a.stime) > (b.utime + b.stime);
}

void calcCpuUtilization(vector<ProcessInfo>& processes, const map<int, ProcessInfo>& previous, unsigned long long period) {
    //use map of old processes to find the pid, and calculate cpu_percent looking at current and old processes
    for (size_t i = 0; i < processes.size(); i++) {
        //if we can't find the pid, set percent to 0
        if (previous.find(processes[i].pid) == previous.end()) {
            processes[i].cpu_percent = 0;
        }
        else {
            ProcessInfo prev = previous.find(processes[i].pid)->second;
            unsigned long long delta = (processes[i].utime + processes[i].stime) - (prev.utime + prev.stime);
            processes[i].cpu_percent = 100.0 * delta / period;
        }
    }
}

 int sortProcesses(vector<ProcessInfo>&processes, ColumnSort col) {
    switch (col) {
        case PID:
            //sort is a member of the algorithm header
            sort(processes.begin(), processes.end(), pidSort);
            break;
        case MEM:
            sort(processes.begin(), processes.end(), memSort);
            break;
        case CPU:
            sort(processes.begin(), processes.end(), cpuSort);
            break;
        case TIME:
            sort(processes.begin(), processes.end(), timeSort);
            break;
        default:
            //there was an error reading the flag
            exit(EXIT_FAILURE);
    }

    return 0;
}

//displays the uptime of the system
void displayUptime (double uptime) {
    int seconds = (int)uptime % 60;
    int minutes = ((int)uptime / 60) % 60;
    int hours = ((int)uptime / 3600) % 24;
    int days = (int)uptime / 3600 / 24;
    printw("up %2d day(s), %d:%d:%d \t", days, hours, minutes, seconds);
}

//displays the load average of the system
void displayLoadAvg(LoadAverageInfo loadAvg) {
    printw("load average: %.2f, %.2f, %.2f \n", loadAvg.one_min, loadAvg.five_mins, loadAvg.fifteen_mins);
}

//displays information on all cpus
void displayCpuInfo(vector<CpuInfo> current, vector<CpuInfo> previous) {

    //CpuInfo change;

    double userTime = 0;
    double kernelTime = 0;
    double idleTime = 0;
    double totalUser = 0;
    double totalKernel = 0;
    double totalIdle = 0;

    for (size_t i = 0; i < current.size(); ++i) {
        double totalTime = (double)current[i].total_time() - (double)previous[i].total_time();
        
        //the three values need to be calculated using the new and previous time values to get
        //an up to date look at cpu usage
        userTime = (double)current[i].user_time - (double)previous[i].user_time;
        userTime = userTime / totalTime;
        userTime = userTime * 100;

        kernelTime = (double)current[i].total_system_time() - (double)previous[i].total_system_time();
        kernelTime = kernelTime / totalTime;
        kernelTime = kernelTime * 100;

        idleTime = (double)current[i].total_idle_time() - (double)previous[i].total_idle_time();
        idleTime = idleTime / totalTime;
        idleTime = idleTime * 100;

        totalUser += userTime;
        totalKernel += kernelTime;
        totalIdle += idleTime;

        printw("%%CPU (%d)\t User Mode: %.2f, Kernel Mode: %.2f, Idling: %.2f\n", i, userTime, kernelTime, idleTime);
    }
    totalUser = totalUser / current.size();
    totalKernel = totalKernel / current.size();
    totalIdle = totalIdle / current.size();

    printw("%%CPU (all)\t User Mode: %.2f, Kernel Mode: %.2f, Idling %.2f \n", totalUser, totalKernel, totalIdle);
}

//displays info about all processes in use
void displayProcessInfo(unsigned num_processes, unsigned num_running, unsigned num_user_threads, unsigned num_kernel_threads) {
    printw("Tasks: %d total, %d running, %d user threads, %d kernel threads\n", 
        num_processes, num_running, num_user_threads, num_kernel_threads);
}

//displays information on memory
void displayMemInfo(unsigned long long total_memory, unsigned long long free_memory) {
    printw("kiB Mem:\t %d total, %d used, %d free\n\n", total_memory, (total_memory - free_memory), free_memory);
}

//displays the table of all processes
void displayProcessTable(vector<ProcessInfo> current, vector<ProcessInfo> previous, vector<CpuInfo> currentCpus, vector <CpuInfo> previousCpus, ColumnSort col) {
    //display table header
    attron(A_STANDOUT);
    printw("%-7s %-12s %-3s %-7s %-15s %-24s\n", "PID", "RES", "S", "%CPU", "TIME", "COMMAND");
    standend();

    //todo: sort pids given flag, and calculate cpu_utilization


    //create a map to calculate cpu_utilization
    map<int, ProcessInfo> prevProcesses;
    for (size_t i = 0; i < previous.size(); i++) {
        prevProcesses.insert(pair<int,ProcessInfo>(previous[i].pid, previous[i]));
    }
    unsigned long long period = currentCpus[1].total_time() - previousCpus[1].total_time();
    calcCpuUtilization(current, prevProcesses, period);
    sortProcesses(current, col);


    for (size_t i = 0; i < TOTAL_PID_SHOWN && i < current.size(); i++) {
        int pid = current[i].pid;
        //resident needs the pagesize to compile correctly
        double resident = current[i].rss * sysconf(_SC_PAGESIZE);
        char state = current[i].state;
        double cpuPercent = current[i].cpu_percent;
        double uptime = current[i].utime + current[i].stime;
        int seconds = (int)uptime % 60;
        int minutes = ((int)uptime / 60) % 60;
        int hours = ((int)uptime / 3600) % 24;
        int days = (int)uptime / 3600 / 24;
        string command = current[i].command_line;

        printw("%-7d %-12.0f %-3c %-4.2f \t %d:%d:%d:%d \t %-25s\n", pid, resident, state, cpuPercent, days, hours, minutes, seconds, command.c_str());

    }
}

void sort(ColumnSort& col, string arg) {
    if (arg == "PID") {
        col = PID;
    }
    else if (arg == "MEM") {
        col = MEM;
    }
    else if (arg == "CPU") {
        col = CPU;
    }
    else if (arg == "TIME") {
        col = TIME;
    }
    else {
        cout << "invalid argument for COLUMN: " << arg << "\nTry -h or --help for valid inputs\n";
        exit(EXIT_FAILURE);
    }
}

int handleFlags(int argc, char **argv, int& delay, ColumnSort& col) {
    int flag, index;
    for (int i = 0; i < argc; i++) {
        index = 0;
        flag = getopt_long(argc, argv, "hd:s:", options, &index);

        if (flag == -1) return 1;

        switch (flag) {
            case 'h':
                cout << "Valid flags:\n-h, --help for help\n-s COLUMN, --sort-key COLUMN to sort the PID table by column (PID, CPU, MEM, or TIME)\n-d DELAY, --delay DELAY to change refresh delay by 10ths of a second\n";
                exit(0);
                break;
            case 'd':
                delay = strtol(optarg, NULL, 10);
                if (delay == 0) {
                    cout << "invalid value for DELAY\nTry -h or --help for valid inputs\n";
                    exit(EXIT_FAILURE);
                }
                break;
            case 's':
                sort(col, optarg);
                break;
            default:
                cout << "invalid option" << flag << "\nTry -h or --help for valid inputs\n";
                exit (EXIT_FAILURE);

        }
    }
    return 0;
}

/**
 * Entry point for the program.
 */
int main(int argc, char **argv) {

    //default delay is 1 second, can be changed by measures of tenths of second
    int delay = 10;
    //default to sort by PID
    ColumnSort col = PID;

    if (argc != 0) {
        handleFlags(argc, argv, delay, col);
    }

    //This will be used to maintian info on the previous system snapshot, so that
    //we can measure differences between screen refreshes
    SystemInfo old = get_system_info();

    //TODO: add in handle flags

    // ncurses initialization
    initscr();

    // Don't show a cursor.
    curs_set(FALSE);

    // Set getch to return after 1000 milliseconds; this allows the program to
    // immediately respond to user input while not blocking indefinitely.
    timeout(delay * 100);;

    while (true) {
        SystemInfo info = get_system_info();

        wclear(stdscr);

        displayUptime(info.uptime);
        displayLoadAvg(info.load_average);
        displayCpuInfo(info.cpus, old.cpus);
        displayProcessInfo(info.num_processes, info.num_running, info.num_user_threads, info.num_kernel_threads);
        displayMemInfo(info.memory_info.total_memory, info.memory_info.free_memory);
        displayProcessTable(info.processes, old.processes, info.cpus, old.cpus, col);
        // Redraw the screen.
        refresh();

        //current system snapshot becomes the old one
        old = info;

        // End the loop and exit if Q is pressed
        exit_if_user_presses_q();
    }

    // ncurses teardown
    endwin();

    return EXIT_SUCCESS;
}
