#include <stdio.h>
#include <vector>
#include <stdlib.h>

#define MAX_TQ 2147483647
#define MAX_WIDTH 70
#define FINISH -1
using namespace std;

class Process{
private:
    int id;
    int burstTime;
    int priority;
    int lastExecutedTime;
	int tq;
public:
    void load(FILE *in, int t){
        fscanf(in, "%d %d %d\n", &id, &burstTime, &priority);
        lastExecutedTime = 0;
		tq = t;
    }
    int getCPU(){
        printf(" %d", id);
        burstTime--;
		tq--;

        if (burstTime == 0) return FINISH;
    }
    int getBurstTime(){
        return burstTime;
    }
    int getPriority(){
        return priority;
    }
};

int sjfComparer(const void *x, const void *y){
    if (((Process*)x)->getBurstTime() > ((Process*)y)->getBurstTime()) return 1;
    else if (((Process*)x)->getBurstTime() < ((Process*)y)->getBurstTime()) return -1;
    else return 0;
}
int priorityComparer(const void *x, const void *y){
    if (((Process*)x)->getPriority() > ((Process*)y)->getPriority()) return 1;
    else if (((Process*)x)->getPriority() < ((Process*)y)->getPriority()) return -1;
    else return 0;
}

void printUsage(){
    printf("Usage : ./filename <data filename> <scheduling policy>\n");
    printf("scheduling policy:\n");
    printf("-f   FCFS Scheduling\n");
    printf("-s   SJF Scheduling\n");
    printf("-p   Priority Scheduling\n");
    printf("-r <number>  Round-Robin Scheduling\n");
    exit(-1);
}

void runScheduler(int cntProc,vector<Process>* scheduler){
    int i;
}

int main(int argc, char* argv[]){
    vector<Process> scheduler;
    int cntProc;
    int tq;
    FILE *in;
    int i;
    if (argc != 3)
        printUsage();

	tq = MAX_TQ;
    switch(argv[2][1]){
    case 'r':
        tq = atoi(argv[3]);
    case 'f':
        break;
    case 's':
        qsort(&scheduler, cntProc, sizeof(Process), sjfComparer);
        break;
    case 'p':
        qsort(&scheduler, cntProc, sizeof(Process), priorityComparer);
        break;
    default:
        printUsage();
    }


    in = fopen(argv[1], "r");
    if (in == NULL){
        printf("unable to Load File %s\n", argv[1]);
        return -1;
    }

    fscanf(in, "%d", &cntProc);
    scheduler.clear();
    
    for (i = 0; i < cntProc; i++)
       scheduler[i].load(in, tq);
    fclose(in);

    runScheduler(cntProc, &scheduler);

    return 0;
}
