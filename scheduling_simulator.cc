#include <stdio.h>
#include <vector>
#include <stdlib.h>

#define MAX_TQ 2147483647
#define MAX_WIDTH 30
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
public:
    void load(FILE *in){
        fscanf(in, "%d %d %d\n", &id, &burstTime, &priority);
        lastExecutedTime = 1;
		waitingTime = 0;
    }
    int useCPU(int curTime){
		if (burstTime == 0) return NOTEXIST;
        printf("%2d", id);
        burstTime--;
		waitingTime += curTime - lastExecutedTime - 1;
		lastExecutedTime = curTime;

        if (burstTime == 0) return FINISH;
		return CONTINUE;
    }
    int getBurstTime(){
        return burstTime;
    }
    int getPriority(){
        return priority;
    }
    void printID(){
    	printf("%d\n", id);
	}
};

class Scheduler{
private:
	int cntProc;
	vector<Process> readyQueue;
	vector<Process> endQueue;
	int tq;
	
	void printGantt(int i){
		int max = MAX_WIDTH + i;
		i++;
		printf("%2d", i++);
		
		for (; i <= max; i++){
			if (i % 5 == 0) printf("%2d",i);
			else printf("  ");
		}
		printf("\n");
	}
public:
	Scheduler(){
		readyQueue.clear();
		endQueue.clear();
		tq = 0;
		cntProc = 0;
	}
	static int sjfComparer(const void *x, const void *y){
	    if (((Process*)x)->getBurstTime() > ((Process*)y)->getBurstTime()) return 1;
	    else if (((Process*)x)->getBurstTime() < ((Process*)y)->getBurstTime()) return -1;
	    else return 0;
	}
	static int priorityComparer(const void *x, const void *y){
	    if (((Process*)x)->getPriority() > ((Process*)y)->getPriority()) return 1;
	    else if (((Process*)x)->getPriority() < ((Process*)y)->getPriority()) return -1;
	    else return 0;
	}
	void setCntProc(int cntProc){
		this->cntProc = cntProc;
	}
	void addProc(Process element){
		readyQueue.push_back(element);
	}
	void setTQ(int tq){
		this->tq = tq;
	}
	int sort(char ch){
		int (*comparer)(const void*, const void*) = NULL;
		if (ch != 'f' && ch != 's' && ch != 'p' && ch != 'r')
			return 1;
		if (ch =='s')
			comparer = sjfComparer;
		if (ch == 'p')
			comparer = priorityComparer;
		if (comparer)
			qsort(&readyQueue, cntProc, sizeof(Process), comparer);
		
		for (int i = 0; i < cntProc; i++)
			readyQueue[i].printID();
			
		return 0;
	}
	void run(){
		int ret;
	    int i = 0;
		int curTime = 0;
		int curTQ = tq;
	
		printf("Gantt Chart\n");
		printGantt(curTime);
	
		while(cntProc){
			curTime++;
			curTQ--;
			ret = readyQueue[i].useCPU(curTime);
			switch(ret){
			case FINISH:
				cntProc--;
				curTQ = tq;
				endQueue.push_back(readyQueue[i]);
				readyQueue.erase(readyQueue.begin() + i);
				if (!cntProc)
					return;
				i = i % cntProc;
				break;
			case CONTINUE:
				if (!curTQ){
					curTQ = tq;
					i = (i + 1)%cntProc;
				}
				break;
			}
			if (curTime % MAX_WIDTH == 0){
				printf("\n");
				printGantt(curTime);
			}
		}
	}
};

void printUsage(){
    printf("Usage : ./filename <data filename> <scheduling policy>\n");
    printf("scheduling policy:\n");
    printf("-f   FCFS Scheduling\n");
    printf("-s   SJF Scheduling\n");
    printf("-p   Priority Scheduling\n");
    printf("-r <number>  Round-Robin Scheduling\n");
    exit(-1);
}

int main(int argc, char* argv[]){
	Scheduler scheduler;
    int cntProc;
    int tq;
    FILE *in;
    int i;
 
    if (argc != 3 && argc != 4)
        printUsage();

	scheduler.setTQ(MAX_TQ);
    in = fopen(argv[1], "r");
    if (in == NULL){
        printf("unable to Load File %s\n", argv[1]);
        return -1;
    }

    fscanf(in, "%d", &cntProc);
    scheduler.setCntProc(cntProc);
    Process tmp;
    for (i = 0; i < cntProc; i++){
    	tmp.load(in);
    	scheduler.addProc(tmp);
	}
    fclose(in);
    
    if (argv[2][1] == 'r')
    	scheduler.setTQ(atoi(argv[3]));
    else
    	scheduler.sort(argv[2][1]);
    
    scheduler.run();
    printf("\n");

    return 0;
}
