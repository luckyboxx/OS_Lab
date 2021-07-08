#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <errno.h>
#include <time.h>
#include <sys/time.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <assert.h>
#include <pthread.h>
#include <math.h>
#define arrival_time 6 // number of data
#define run_time 2
#define size 6


void FIFO();
void RR();
void MLFQ();
void Lottery();
int int_pow(int base, int exp);

int main(int argc, char *argv[]){

    int choice = 0;
	
    
    while (1) {
        printf("이 프로그램은 5개의 데이터 셋을 사용합니다.\n");
        printf("데이터 셋의 갯수는 소스에서 수정이 가능합니다.\n");
        printf("1. FIFO\n");
        printf("2. RR\n");
        printf("3. MLFQ\n");
        printf("4. Lottery\n");
        printf("5. Exit\n");
        printf (">>>");
        scanf("%1d",&choice);

        while(1) {
            if (choice < 6) break;
            else {
                    printf("번호를 다시 입력하세요.\n\n\n");
                    printf("1. FIFO\n");
                    printf("2. RR\n");
                    printf("3. MLFQ\n");
                    printf("4. Lottery\n");
                    printf("5. Exit\n");
                    printf (">>>");
                    scanf("%1d",&choice);
            }
        }
        switch(choice) {
            case 1:
                FIFO();
                break;
            case 2:
                RR();
                break;
            case 3:
                MLFQ();
                break;
            case 4:
                Lottery();
                break;
            case 5:
                return 0;
        }
    }
}

int int_pow(int base, int exp)
{
    int result = 1;
    while (exp)
    {
        if (exp & 1)
           result *= base;
        exp /= 2;
        base *= base;
    }
    return result;
}

void FIFO()
{
	int i,data,j,k,l;
	int total_time = 0; // all time
	int process = 0; // now process start
	int deadline = 0; // last process exit space
	int allstop = 0; // all scheduling exit
	int throughput[arrival_time][100] = {}; // throughput data
	int **arr;

	arr = (int **) calloc (sizeof (int*), arrival_time);

	for(i = 0; i < arrival_time; i++) {
		arr[i] = (int*) calloc ( sizeof(int), run_time);
	}

	for (i = 1; i < arrival_time; i++) {
		printf("Enter the 'Arrival-Time' and 'Service(Run) time': ");
		for(j = 0; j < run_time; j++) {
			scanf("%d", &arr[i][j]);
		}
	}
	
	for (i = 1; i < arrival_time; i++) {
		total_time += arr[i][1] + arr[i][0];
	}

	// all turnaroud time
	for (i = 0; i < total_time; i++) {
		process = 0;
		for (data = 1; data < arrival_time; data++) {
			if (i >= arr[data][0] && arr[data][1] >= 1) {
				if (process == 0) { 
					process = data;
					break;
				}
			}
		}

		if (process > 0) {
			arr[process][1]--;
			throughput[process][i] = 1;
		}
		// all scheduling exit checking
		if (arr[arrival_time - 1][1] < 1) {
			deadline = ++i;
			break;
		}
	}
	
   	printf("        ");
    for (l = 0; l < deadline; l++)  {
            printf("%2d  ",l);
        }
        printf("\n    ");
        
        for (l = 0; l < deadline; l++)  {
            printf("------");
        }
    printf("\n");

    for (i = 1; i < arrival_time; i++) {
        printf(" [%c] >> ",i+64);
        for (k = 0; k < deadline; k++)  {
            if (throughput[i][k] == 1) {
                printf("■  ");
            }
            else
                printf("□  ");
        }
        printf("\n");
    }
}

void RR()
{
	int i,data,j,k,l;
	int total_time = 0; // all time
	int process = 0; // now process start
	int deadline = 0; // last process exit space
	int allstop = 0; // all scheduling exit
	int throughput[arrival_time][100] = {}; // throughput data
	int **arr;
    int hunger[arrival_time] = {0}; // 기아상태 측정용
    int check[arrival_time] = {0}; // 프로세스가 타임슬라이스를 다 사용했는지 체크
    int timeslice = 1; 

	arr = (int **) calloc (sizeof (int*), arrival_time);

	for(i = 0; i < arrival_time; i++) {
		arr[i] = (int*) calloc ( sizeof(int), run_time);
	}

	for (i = 1; i < arrival_time; i++) {
		printf("Enter the 'Arrival-Time' and 'Service(Run) time': ");
		for(j = 0; j < run_time; j++) {
			scanf("%d", &arr[i][j]);
		}
	}
	

    for (i = 1; i < arrival_time; i++) {
        total_time += arr[i][1] + arr[i][0];
    }

    for (i = 0; i < total_time; i++) {
        if (check[process] % timeslice == 0 || arr[process][1] < 1) { // 프로세스가 타임슬라이스 만큼 실행되었거나 수행이 완료되면 
            check[process] = 0; // 타임슬라이스를 초기화
            hunger[process] = 0; // 스케쥴링이 되어 기아상태가 사라짐
            for (k = 1; k < arrival_time; k++) {
                if (i >= arr[k][0] && k != process) // 현재 진행 시간에 작업이 도착했는지 판단하고 k 값이 직전에 스케쥴링되었는지 확인
                    hunger[k]++; // 스케쥴링이 직전에 되지 않았다면 기아상태를 증가시킴
            }
        }
        allstop = 0; // 초기화를 해 주어야 매 루프마다 모든 프로세스의 스케쥴링이 끝났는지 체크 가능
        process = 0; // 실행할 프로세스를 고르기 전에 초기화

        for (j = 1; j < arrival_time; j++) {
            if (i >=arr[j][0] && arr[j][1] >= 1) { // 현재 진행 시간에 프로세스가 도착했는지 판단하고 스케쥴링 할 프로세스의 처리시간이 남았는지 확인
                if (process == 0) { // 실행할 프로세스를 아직 고르지 못했다면
                    process = j; // 프로세스를 고름
                }
                else if (hunger[process] < hunger[j]) { // 프로세스를 이미 골랐음에도 지금 스케쥴링할 다른 프로세스가 존재한다면 배고픔 정도를 비교
                    process = j; // 더 배고프다면 스케쥴링할 프로세스를 교체
                }
            }

            if (arr[j][1] < 1) // 모든 프로세스 스케쥴링이 끝났는지 체크
                allstop++;
        }

        if (process > 0) { // 레디 큐에 처리할 프로세스가 존재한다면
            arr[process][1]--; // 프로세스의 실행시간을 1 줄이고
            throughput[process][i] = 1; // 아웃풋 데이터에 입력
            check[process]++;
        }

        if (allstop == arrival_time-1) { // 스케쥴링이 종료되었다면
            deadline = ++i; // deadline이 현재시간을 기억하고 루프 종료
            break;
        }
    }

    printf("RR 의 현재 Time quantom은 1 입니다. 소스에서 수정이 가능합니다.\n");
    printf("        ");
    for (l = 0; l < deadline; l++)  {
            printf("%2d  ",l);
        }
        printf("\n    ");
        
        for (l = 0; l < deadline; l++)  {
            printf("------");
        }
    printf("\n");

    for (i = 1; i < arrival_time; i++) {
        printf(" [%c] >> ",i+64);
        for (k = 0; k < deadline; k++)  {
            if (throughput[i][k] == 1) {
                printf("■  ");
            }
            else
                printf("□  ");
        }
        printf("\n");
    }
}



void MLFQ()
{
    int i,data,j,k,l;
	int total_time = 0; // all time
	int process = 0; // now process start
	int deadline = 0; // last process exit space
	int allstop = 0; // all scheduling exit
	int throughput[arrival_time][100] = {}; // throughput data
	int **arr;
	int hunger[arrival_time] = {0}; // 기아상태 측정용
    int check[arrival_time] = {0}; // 프로세스가 타임슬라이스를 다 사용했는지 체크
    int timeslice = 1; 
	int pr[arrival_time] = { 0 }; // 처리된 회수를 저장
	int chktime = 0; // 현재 프로세스가 타임슬라이스이내에서 실행되고있는지
	int emptyq = 0; // 현재 큐에 단 하나의 프로세스만 존재하는지
	arr = (int **) calloc (sizeof (int*), arrival_time);

	for(i = 0; i < arrival_time; i++) {
		arr[i] = (int*) calloc ( sizeof(int), run_time);
	}

	for (i = 1; i < arrival_time; i++) {
		printf("Enter the 'Arrival-Time' and 'Service(Run) time': ");
		for(j = 0; j < run_time; j++) {
			scanf("%d", &arr[i][j]);
		}
	}
   
    for (i = 1; i < arrival_time; i++) {
        total_time += arr[i][1] + arr[i][0];
    }

	for (i = 0; i < total_time; i++) {

		if (chktime == 0) { // cpu에 프로세스가 올라가 있는지 확인합니다.
			for (k = 1; k < arrival_time; k++) {
				if (i >= arr[k][0] && k != process) {// 현재 진행 시간에 프로세스가 도착했는지 판단하고 k 값이 직전에 스케쥴링되었는지 확인
					hunger[k]++; // 스케쥴링이 직전에 되지 않았다면 배고픔을 증가시킴
					check[k] = 0; // 스케쥴링이 이뤄지지 않아서 타임퀀텀 초기화
				}
			}
		}

		if (chktime == 0) // cpu에 선점되었는지 확인
			process = 0; // 실행할 프로세스를 고르기 전에 초기화
		emptyq = 0; // 현재 스케쥴링의 대상인 프로세스 수
        allstop = 0; // 초기화를 해 주어야 매 루프마다 모든 프로세스의 스케쥴링이 끝났는지 체크 가능

		for (j = 1; j < arrival_time; j++) {
			if (i+1 >= arr[j][0] && arr[j][1] > 0) { emptyq++; } // 현재 스케쥴링의 대상이 될 수 있는 프로세스 조사
            
			if (i == arr[j][0] && arr[j][1] >= 1) { // 새롭게 들어온 프로세스라면
				hunger[j] = 9999; // 가장 높은 우선순위 할당
				if (process == 0) { // 현재 스케쥴링할 대상을 정하지 않은 상태라면
					process = j; // 후보에 넣음
				}
			} // 새로 들어온 프로세스는 가장 우선순위 큐에 들어감 

			if (i >= arr[j][0] && arr[j][1] >= 1) { // 현재 진행 시간에 프로세스가 도착했는지 판단하고 스케쥴링 할 프로세스의 처리시간이 남았는지 확인
				if (process == 0 && chktime == 0) { // 실행할 프로세스를 아직 고르지 못했고 선점된 프로세스가 없다면
					process = j; // 프로세스를 고름
				}
				else if (pr[process] > pr[j] && hunger[j] != 0) { // 실행될 프로세스가 새로 발견한 프로세스보다 더 낮은 우선순위의 큐에 있는지 확인하고 배고픔을 체크 (배고픔을 체크 안할경우 더 낮은 상태의 큐가 무조건 먼저 돌아감) 
					if (chktime == 0) { // 선점되지 않았다면
						process = j;
					} // 현재 process에 들어간 프로세스가 타임퀀텀만큼 충분히 돌았는가 판단
				}
			}

            if (arr[j][1] < 1)  // 모든 프로세스 스케쥴링이 끝났는지 체크
                allstop++; 
		}

		if (process > 0) { // 스케쥴링 후보에 오른 프로세스를 스케쥴링함
			arr[process][1]--; // 프로세스의 실행시간을 1 줄이고
			throughput[process][i] = 1; // 아웃풋 데이터에 입력
			check[process]++; // 프로세스가 타임퀀텀 사용
			chktime = 1; // 현재 프로세스가 cpu를 사용 중임
			hunger[process] = 0; // 스케쥴링이 되어 배고픔이 사라짐
			if (check[process] == (int)int_pow(timeslice, pr[process]) || arr[process][1] < 1) { // 프로세스가 타임퀀텀만큼 다 돌았으면 우선순위 낮은 큐로 들어감
				check[process] = 0; // 주어진 시간만큼 스케쥴링을 다 했기 때문에 초기화
				chktime = 0; // 프로세스가 cpu에서 내려감
				if (emptyq > 1) // 단 하나의 프로세서만 스케쥴링의 대상이 아니라면
				pr[process]++; // 프로세스의 우선순위를 낮춤
			}
		}

        if (allstop == arrival_time-1) { // 스케쥴링이 종료되었다면
            deadline = ++i; // deadline이 현재시간을 기억하고 루프 종료
            break;
        }
	}

    printf("arrival_timeLFQ 의 현재 Time quantom은 1 입니다. 소스에서 수정이 가능합니다.\n");
 	printf("        ");
    for (k = 0; k < deadline; k++)  {
            printf("%2d  ",k);
        }
    printf("\n    ");
        
    for (k = 0; k < deadline; k++)  {
            printf("------");
        }
    printf("\n");

    for (i = 1; i < arrival_time; i++) {
        printf(" [%c] >> ",i+64);
        for (k = 0; k < deadline; k++)  {
            if (throughput[i][k] == 1) {
                printf("■  ");
            }
            else
                printf("□  ");
        }
        printf("\n");
    }
}

void Lottery()
{
   
    int ticket_sum = 0; //all ticket
    int scpr[size] = {}; // ticket space
    int pick = -1; // pick the ticket
    int i,j,k,l;
    int total_time = 0; // all time
    int process = 0; // now process
    int deadline = 0; // exit space that last process
    int allstop = 0; // all process scheduling exit
    int throughput[arrival_time][100] = {}; // throughput data
    int **arr;

    arr = (int **) calloc (sizeof (int*), arrival_time);

	int *ticket = malloc(sizeof(int) * size);
    for (int i = 1; i < size; i++) {
	printf("pick the ticket: ");
        ticket[i] = i;
	scanf("%d", &ticket[i]);
    }
    
    for(i = 0; i < arrival_time; i++) {
    	arr[i] = (int*) calloc ( sizeof(int), run_time);
    }

    for (i = 1; i < arrival_time; i++) {
    	printf("Enter the 'Arrival-Time' and 'Service(Run) time': ");
    	for(j = 0; j < run_time; j++) {
    		scanf("%d", &arr[i][j]);
    	}
    }

    srand((unsigned int)time(NULL));

    printf("This is the ticket list that give each process\n");
    for (i = 1; i < arrival_time; i++) {
        ticket_sum += ticket[i];
        printf("%c :%d ",i+64,ticket[i]);
    }
    printf("\n");
    for (i = 1; i < arrival_time; i++) {
        scpr[i] = scpr[i-1] + ((float)ticket[i] / ticket_sum + 0.005) * 100;
    }
    scpr[arrival_time-1] += 1;

    for (i = 1; i < arrival_time; i++) {
        total_time += arr[i][1] + arr[i][0];
    }


    for (i = 0; i < total_time; i++) {

        allstop = 0;

        for (j = 1; j < arrival_time; j++) {

            if (arr[j][1] < 1)
                allstop++;

            if (pick == -1) {
                pick = rand() % scpr[size-1];
            }
            if (pick >= scpr[j-1] && pick < scpr[j]) { 
                if (i >= arr[j][0] && arr[j][1] >= 1)
                {
                    process = j;
                    break;
                }
                if (i < arr[j][0] || arr[j][1] < 1) {
                    if (process == 0) {
                        pick = -1;
                        i--;
                        break;
                    }
                }
            }
        }

        if (process > 0) {
            arr[process][1]--;
            throughput[process][i] = 1;
            process = 0;
            pick = -1;
        }

        if (allstop == arrival_time-1) {
            deadline = ++i; 
            break;
        }
    }
    
   	printf("        ");
    for (l = 0; l < deadline; l++)  {
            printf("%2d  ",l);
    }
    printf("\n    ");
        
    for (l = 0; l < deadline; l++)  {
            printf("------");
        }
    printf("\n");

    for (i = 1; i < arrival_time; i++) {
        printf(" [%c] >> ",i+64);
        for (k = 0; k < deadline; k++)  {
            if (throughput[i][k] == 1) {
                printf("■  ");
            }
            else
                printf("□  ");
        }
        printf("\n");
    }
}