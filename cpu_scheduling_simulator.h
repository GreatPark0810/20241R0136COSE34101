#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_PROCESSES 10
#define MAX_ARRIVAL_TIME 100
#define MAX_CPU_BURST 10
#define MAX_IO_BURST 10
#define MAX_PRIORITY 100

#define NONE 0
#define IDLE 1
#define CPU_BURST 2
#define IO_BURST 3

////////////////////////////////////////////////////////////////////////////////

typedef struct {
    int pid; // 1 ~ RAND_MAX + 1
    int arrival_time; // 1 ~ MAX_ARRIVAL_TIME
    int cpu_burst; // 1 ~ MAX_CPU_BURST
    int io_burst; // 1 ~ MAX_IO_BURST
    int priority; // 1 ~ MAX_PRIORITY (the smaller number, the higher priority)
} Process; // 프로세스의 정보를 담는 구조체

////////////////////////////////////////////////////////////////////////////////

typedef struct {
    Process *process;
    int start_time;
    int burst_type; // 0 : None, 1 : CPU, 2 : I/O, 3 : Idle
} Record; // 스케줄링에서 각 프로세스가 언제부터 언제까지 burst되었는지 기록하는 구조체

////////////////////////////////////////////////////////////////////////////////

typedef struct {
    int front; // 첫 번째 프로세스의 바로 한 칸 앞
    int rear; // 마지막 프로세스
    Process *queue[MAX_PROCESSES]; // 프로세스를 가리키는 포인터를 저장
} Queue; // 프로세스 스케줄링에서 사용되는 원형 큐

// 원형 큐의 초기화
void init(Queue *q) {
    q->front = q->rear = 0;

    for (int i = 0; i < MAX_PROCESSES; i++) {
        (q->queue)[i] = NULL;
    }
}

int is_empty(Queue *q) { // front와 rear가 같으면 비어있음
    return (q->front == q->rear);
}

int is_full(Queue *q) { // rear의 한칸 앞이 front면 포화
    return ((q->rear + 1) % MAX_PROCESSES == q->front);
}

void push(Queue *q, Process *p) {
    if (is_full(q)) {
        printf("The queue is full!\n");
        return;
    }
    
    q->rear = (q->rear + 1) % MAX_PROCESSES;
    q->queue[q->rear] = p;
}

Process *pop(Queue *q) {
    if (is_empty(q)) {
        printf("The queue is empty!\n");
        return NULL;
    }
    // front는 target 프로세스의 한칸 앞이므로 한칸 이동 후 지운다.
    q->front = (q->front + 1) % MAX_PROCESSES;
    // target 프로세스 저장하고 있는 칸 깨끗이 비운 후 target을 return한다.
    Process *target = q->queue[q->front];
    q->queue[q->front] = NULL;
    return target;
}

////////////////////////////////////////////////////////////////////////////////

Process list_of_processes[MAX_PROCESSES]; // create_process()를 통해 생성된 프로세스들 저장
int the_number_of_process = 0; // 생성된 프로세스의 개수

Queue ready_queue; // 스케줄링에서 사용되는 ready_queue (원형 큐로 동작)
Queue waiting_queue; // 스케줄링에서 사용되는 waiting_queue (원형 큐로 동작)

/**
 * @brief 
 * 프로세스를 the_number_of_process만큼 create한다.
 * PID는 사용자가 직접 키보드로 입력하고 싶은 경우와, 자동으로 할당되는 경우를 나눈다.
 */
void create_process();

/**
 * @brief 
 * 프로세스가 이미 생성되어 있다면 모든 프로세스를 지운다. (멤버를 0으로 초기화)
 */
void remove_process();

/**
 * @brief 
 * 생성된 프로세스의 정보를 출력한다.
 * 모든 프로세스의 정보를 출력할 수도 있고, 특정한 프로세스만의 정보를 출력할 수도 있다.
 */
void print_process_info();

void config();

void schedule_main();

/**
 * @brief 
 * FCFS 스케줄링을 수행하고, 간트 차트를 출력한다.
 */
void schedule_FCFS();

/**
 * @brief 
 * FCFS 스케줄링에서 qsort 함수를 사용할 때 사용되는 compare 함수
 */
int arrival_compare(const void *, const void *);

void schedule_nonpreemptive_SJF();

void schedule_preemptive_SJF();

void schedule_nonpreemptive_priority();

void schedule_preemptive_priority();

void schedule_RR();

void print_gantt_chart();

void evaluation_main();

////////////////////////////////////////////////////////////////////////////////