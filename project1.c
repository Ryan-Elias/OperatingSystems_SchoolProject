#include <stdlib.h>
#include <stdio.h>

#define MAXP 32
#define TRUE -1
#define FALSE 0
#define CPU 100.00


typedef struct instruction instruction;
struct instruction {
    //given
    int pid;
    int ExecutionTime;
    int priority;

    //derived
    int waitTime; //TotalBurst - (Burst*Instances)
    int responseTime; // TotalBurst - Burst
    int turnaroundTime; //TotalBurst (includeing execution time)

    int instances;
};

int readFile(FILE *fp) {

    instruction *list = calloc(MAXP, sizeof(instruction)); //creates an array of the unique instructions. 
    int P; 
    int uniqueInst; //# of unique PIDs, also number of Voluntary Context Switches
    int N; // total instructions

    int x = 0; //total number of unique pids read in. not a very good varible name.

    fscanf(fp,"%d",&P);
    fscanf(fp,"%d",&uniqueInst);
    fscanf(fp,"%d",&N);


    int currpid;
    int currburst;
    int currpri;

    int isUnique;
    int totalBurstTime = 0;
    int prevpid;
    int totalContextSwitches = 1;

    for (int i = 0; i < N; ++i) {
        fscanf(fp,"%d",&currpid);
        fscanf(fp,"%d",&currburst);
        fscanf(fp,"%d",&currpri);

        isUnique = TRUE; //reset
        totalBurstTime += currburst; //totalBurstTime will update regardless. 

        if (x == 0) { //should only hit on the first instruction
            list[0].pid = currpid;
            list[0].ExecutionTime += currburst;
            list[0].priority = currpri;

            list[0].responseTime = 0;
            list[0].waitTime = 0;
            list[0].turnaroundTime = totalBurstTime;

            prevpid = currpid;
            x++;
        }
        else {
            if (currpid != prevpid) {
                totalContextSwitches++;
            }

            for (int i = 0; i < x; ++i) {

                if (list[i].pid == currpid) {
                    //update turnaround, waittime, and instances.
		    list[i].ExecutionTime += currburst;
                    list[i].turnaroundTime = totalBurstTime;
                    list[i].waitTime = totalBurstTime - list[i].ExecutionTime;

                    isUnique = FALSE;
                    prevpid = currpid;
                    break; //breaks the for-loop because it found the same process ID. The current ID is NOT unique. 
                }
            }

            if (isUnique == TRUE) { //if the pid is unique, adds the process to the array and increments x.
                list[x].pid = currpid;
                list[x].ExecutionTime += currburst;
                list[x].priority = currpri;

                list[x].instances = 1;
                list[x].responseTime = totalBurstTime - currburst;
                list[x].waitTime = totalBurstTime - currburst; //as this is a unique item at the time of entry, it will be the same as response time.
                list[x].turnaroundTime = totalBurstTime;
                x++;
                prevpid = currpid;
            }
        }
    }

    //Sample Output:

    //<VoluntaryContextSwitches>
    //<Non-VoluntaryContextSwitches>
    //<CPU Utilization>
    //<Average Throughput>
    //<Average Waiting Time>
    //<Average Response Time>


    printf("%d\n", uniqueInst); //Voluntary Context Switches
    printf("%d\n", totalContextSwitches - uniqueInst); //Non-Voluntary Context Switches
    printf("%.2f\n", CPU); // CPU Utilization
    printf("%.2f\n", (double) uniqueInst / totalBurstTime ); // Avg Throughput


    //calculate total turnaround times
    int totalTurnaround = 0;
    for (int i = 0; i < x; ++i) {
        totalTurnaround += list[i].turnaroundTime;
    }
    printf("%.2f\n", (double) totalTurnaround / uniqueInst); // Average Turnaround Time

    //calculate total wait times
    int totalWaitTime = 0;
    for (int i = 0; i < x; ++i) {
        totalWaitTime += list[i].waitTime;
    }
    printf("%.2f\n", (double) totalWaitTime / uniqueInst); // Average Waiting Time

    //calculate total response time
     int totalResponseTime = 0;
    for (int i = 0; i < x; ++i) {
        totalResponseTime += list[i].responseTime;
    }
    printf("%.2f\n", (double) totalResponseTime / uniqueInst); // Average Response Time









    return 0;
}

    //Voluntary Context Switch - The number of times the scheduler completes a process. (once for every unique process)
    //Non-Voluntary Context Switch - A pid from earlier is repeated. (total context switches - voluntary context switches)
    //CPU Utilization - range from 0 - 100 percent, but for this project is always at 100 percent. 
    //Throughput - Total Burst Time / number of unique pids
    //Turnaround Time - The time it takes from submission to task complete. (A processes burst time + total burst time)
    //Waiting Time - The time a process spends on the wait queue. If a process is "gapped" then the wait time will be TotalBurstTime - (burstT * instances); The waiting time does not include time spent executing
    //Response Time - The time it takes for a task to start running. A task starts running the first time its pid is mentioned


//Found method of printing to standard error and reading from file from:
//https://stackoverflow.com/questions/49992400/how-to-read-from-files-in-argv-or-stdin-if-none-are-given
int main(int argc, char **argv) {  

    FILE *filePointer;
    if (argc > 1) {
        filePointer = fopen(argv[1], "r");
        if (filePointer == NULL) {
            fprintf(stderr, "cannot open file"); //Could list file in command line argument but I'll leave it be for now.
            return 1; 
        }


        readFile(filePointer);

        fclose(filePointer); //closes file
    }

    else {
        //reads from stdin if there is not file found
        readFile(stdin);
    }



    return 0;
}
