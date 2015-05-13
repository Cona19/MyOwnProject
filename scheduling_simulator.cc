#include <stdio.h>
#include <vector>
#include <stdlib.h>
/*
기본적인 상수입니다.
MAX_WIDTH는 한줄에 출력하는 Gantt Chart의 크기이고
DEFAULT_TQ는 Round Robin에서 Time Quantum이 지정되지 않은 경우 기본값을 나타냅니다.
*/
#define MAX_TQ 2147483647
#define MAX_WIDTH 30
#define DEFAULT_TQ 5

#define NOTFINISH 0
#define FINISH 1

using namespace std;

/*
사용법을 출력하는 함수입니다.
*/
void printUsage(){
    printf("Usage : ./filename <data filename> <scheduling policy>\n");
    printf("scheduling policy:\n");
    printf("-f   FCFS Scheduling\n");
    printf("-s   SJF Scheduling\n");
    printf("-p   Priority Scheduling\n");
    printf("-r <number;default:5>  Round-Robin Scheduling\n");
    exit(0);
}


/*
프로세스의 정보를 나타내는 구조체입니다.
PID, burstTime, priority, 최근실행시간, 총 대기시간을 가지고 있습니다.
*/
struct strc_process{
		int id;
		int burstTime;
		int priority;
		int lastExecutedTime;
		int waitingTime;
};


/*
프로세스의 행동을 나타내기 위한 클래스입니다.
*/
class Process{
private:
	strc_process proc;

public:
/*
파일을 입력받으면 한줄을 읽어 id와 burstTime, priority를 입력받는 함수입니다.
*/
    void load(FILE *in){
        fscanf(in, "%d %d %d\n", &proc.id, &proc.burstTime, &proc.priority);
        proc.lastExecutedTime = 0;
				proc.waitingTime = 0;
    }
/*
CPU를 잡았을 때 작업하기 위해 호출되는 함수입니다.
curTime은 현재 시스템 시간을 나타내고,
현재 PID출력
burstTime, waitingTime, lastExecutedTime값의 갱신이 이루어집니다.
마지막으로 burstTime을 확인해 모든 작업이 끝났다면 FINISH 아니면 NOTFINISH를 반환합니다. 
*/
    int useCPU(int curTime){
        printf("%2d", proc.id);
        proc.burstTime--;
				proc.waitingTime += curTime - proc.lastExecutedTime - 1;
				proc.lastExecutedTime = curTime;

        if (proc.burstTime == 0) return FINISH;
				return NOTFINISH;
    }
/*
아래 함수들은 값을 얻기 위한 getter함수입니다.
*/
    int getID(){
				return proc.id;
		}
    int getBurstTime(){
        return proc.burstTime;
    }
    int getPriority(){
				return proc.priority;
    }
    int getWaitingTime(){
				return proc.waitingTime;
		}
/*
결과 출력시 대기시간을 출력하는 함수입니다.
*/
    void printWaitingTime(){
				printf("Process %d waiting time = %d sec\n", proc.id, proc.waitingTime);
		}
};

/*
프로세스를 관리하는 스케쥴러를 나타내는 프로그램입니다.
멤버로는 Process의 개수, CPU를 필요로 하는 Process의 Queue,
작업을 끝마친 Process의 Queue, Round Robin시의 Time Quantum값으로 구성되어 있습니다.
*/
class Scheduler{
private:
		int cntProc;
		vector<Process> readyQueue;
		vector<Process> endQueue;
		int tq;

/*
Gannt차트를 출력하는 함수입니다.
Gantt차트의 길이가 모자른 경우 이 함수가 호출되어 새로운 줄을 만들게 됩니다.
*/
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
/*
생성자 함수로 값을 초기화해줍니다.
*/
		Scheduler(){
				readyQueue.clear();
				endQueue.clear();
				tq = 0;
				cntProc = 0;
		}
/*
출력시 id순으로 출력하기 위해 정렬하는 Comparer 함수입니다.
*/
		static int idComparer(const void *x, const void *y){
				if (((Process*)x)->getID() > ((Process*)y)->getID()) return 1;
				else if (((Process*)x)->getID() < ((Process*)y)->getID()) return -1;
				else return 0;
		}
/*
아래 2가지는 각각 scheduling policy에 따라 프로세스를 정렬하는데 사용하는 comparer함수입니다.
*/
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
/*
멤버 값을 변경하는 setter함수입니다.
*/
		void setCntProc(int cntProc){
				this->cntProc = cntProc;
		}
		void setTQ(int tq){
				this->tq = tq;
		}
/*
process를 처음 추가할 경우 작업을 수행하기 위해 readyQueue에 넣는 함수입니다.
*/
		void addProc(Process element){
				readyQueue.push_back(element);
		}
/*
프로세스를 입력받은 Scheduling Policy에 따라 정렬하는 작업을 수행하는 함수입니다.
s일 경우 burstTime이 작은 순으로, p일경우 priority가 작은 순으로 정렬하게 됩니다.
*/
		int sort(char ch){
				int (*comparer)(const void*, const void*) = NULL;
				if (ch != 'f' && ch != 's' && ch != 'p' && ch != 'r'){
						printUsage();
						exit(0);
				}
				if (ch =='s')
						comparer = sjfComparer;
				if (ch == 'p')
						comparer = priorityComparer;
				if (comparer)
						qsort(&readyQueue[0], cntProc, sizeof(readyQueue[0]), comparer);
			
				return 0;
		}
/*
Scheduling Simulation을 돌리는 함수입니다.
Gantt Chart를 출력한 후 시스템시간을 나타내는 curTime값을 꾸준히 증가시키면서 해당 시간에 하나의 프로세스를 선정해 CPU를 할당합니다.
결과가 Finish이면 readyQueue에서 값을 제거하고 endQueue에 프로세스를 추가합니다.
그리고 다음에 CPU를 할당할 프로세스를 결정하기 위해 i값을 수정하게 됩니다.
출력해야 하는 time값이 Gantt Chart를 벗어나는 경우 새로 Gantt Chart를 출력하게 됩니다.
*/
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
						case NOTFINISH:
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
/*
결과를 출력하는 함수입니다.
id별로 waiting time을 출력하고 마지막줄에 전체 평균 waiting time을 출력합니다.
*/
		void result(){
				int i;
				int totWait = 0;
				int length = endQueue.size();
				qsort(&endQueue[0], length, sizeof(endQueue[0]), idComparer);
				for (i = 0; i < length; i++){
						endQueue[i].printWaitingTime();
						totWait += endQueue[i].getWaitingTime();
				}
				printf("Average waiting time = %.2lf sec\n", (double)totWait / length);
		}
};

/*
메인함수입니다.
입력값을 확인해 사용법에 맞게 입력되었다면 파일에서 값을 읽어들여 scheduler에 process를 추가합니다.
이후 Round Robin의 경우 Time Quantum을 입력받거나 설정하고, 이외의 경우 최대값으로 넣어 Time Quantum에 의해 스케줄링이 되지 않도록 합니다.
그 다음 Scheduler를 실행하고 결과를 출력하게 됩니다.
*/
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
    
    if (argv[2][1] == 'r'){
				if (argc != 3)
						scheduler.setTQ(atoi(argv[3]));
				else
						scheduler.setTQ(DEFAULT_TQ);
    }
    else
				scheduler.sort(argv[2][1]);
    
    scheduler.run();
    printf("\n");
    scheduler.result();

    return 0;
}
