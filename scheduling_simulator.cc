#include <stdio.h>
#include <vector>
#include <stdlib.h>

#define MAX_TQ 2147483647
#define MAX_WIDTH 70
#define FINISH 0
#define NOTEXIST 1
#define SWITCH 2
#define CONTINUE 3
using namespace std;

class Process{
private:
    int id;
    int burstTime;
    int priority;
    int lastExecutedTime;
	int waitingTime;
	int tq;
public:
    void load(FILE *in, int t){
        fscanf(in, "%d %d %d\n", &id, &burstTime, &priority);
        lastExecutedTime = 0;
		tq = t;
		waitingTime = 0;
    }
    int useCPU(int curTime){
		if (burstTime == 0) return NOTEXIST;
        printf("%2d ", id);
        burstTime--;
		tq--;
		waitingTime += curTime - lastExecutedTime - 1;
		lastExecutedTime = curTime;

        if (burstTime == 0) return FINISH;
		if (tq == 0) return SWITCH;
		return CONTINUE;
    }
	void setTQ(int t){
		tq = t;
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
void printGantt(int i){
	int max = MAX_WIDTH + i;
	printf("%2d ", ++i);
	for (; i < max; i++){
		if (i % 5 == 0) printf("%2d",i);
		else printf("  ");
	}
	printf("\n");
}

void runScheduler(int cntProc,int tq,vector<Process>* scheduler){
	int ret;
    int i = 0;
	int finishCnt = 0;
	int curTime = 0;

	printf("Gantt Chart");
	printGantt(curTime);

	while(finishCnt == cntProc){
		curTime++;
		if (curTime % MAX_WIDTH == 0){
			printf("\n");
			printGantt(curTime-1);
		}
		ret = (*scheduler)[i].useCPU(curTime);
		if (ret != CONTINUE){
			if (ret == FINISH)
				finishCnt++;
			else if(ret == SWITCH)
				(*scheduler)[i].setTQ(tq);

			i = (i + 1)%cntProc;
		}
	}
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

    runScheduler(cntProc, tq, &scheduler);

    return 0;
}
