#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_PROCESSES 10
#define MAX_ARRIVAL_TIME 50
#define MAX_CPU_BURST 10
#define MAX_IO_BURST 10
#define MAX_PRIORITY 100

#define FCFS 0
#define SJF 1
#define PRIORITY 2
#define RR 3
#define WAITING_QUEUE 4

#define NONE 0
#define IDLE 1
#define CPU_BURST 2
#define IO_BURST 3

int get_random_int(int min, int max) {
    return rand() % (max - min + 1) + min;
}


////////////////////////////////////////////////////////////////////////////////

typedef struct {
    int pid; // 1 ~ RAND_MAX + 1
    int arrival_time; // 0 ~ MAX_ARRIVAL_TIME
    int cpu_burst; // 5 ~ MAX_CPU_BURST
    int io_burst; // 5 ~ MAX_IO_BURST
    int io_time; // 1 ~ (cpu_burst-1) (중간에 IO operation이 언제 발생되는지에 대한 time)
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
    Process *process[MAX_PROCESSES]; // 프로세스의 데이터
    int size;
} PriorityQueue; // SJF에서 사용되는 우선순위 큐

void swap(Process **a, Process **b) {
    Process *temp = *a;
    *a = *b;
    *b = temp;
}

void heapify_up(PriorityQueue *pq, int idx, int condition) {
    if (idx == 0) {
        return;
    }
    int parent_idx = (idx-1) / 2;
    int is_swap_needed = 0;

    switch (condition) {
        case FCFS: // FCFS 스케줄링 -> arrival time 작은 순서 먼저
            is_swap_needed = pq->process[parent_idx]->arrival_time > pq->process[idx]->arrival_time;
            break;
        
        case SJF: // SJF 스케줄링 -> cpu_burst 작은 순서 먼저
            is_swap_needed = pq->process[parent_idx]->cpu_burst > pq->process[idx]->cpu_burst;
            break;

        case PRIORITY: // Priority 스케줄링 -> priority 작은 순서 먼저
            is_swap_needed = pq->process[parent_idx]->priority > pq->process[idx]->priority;
            break;

        case RR: // RR 스케줄링 -> FCFS와 동일하게 arrival_time 작은 순서 먼저 (어차피 quantum만큼 실행한다)
            is_swap_needed = pq->process[parent_idx]->arrival_time > pq->process[idx]->arrival_time;
            break;

        case WAITING_QUEUE: // Waiting queue에서는 io_burst가 작은 순서 먼저 (io_burst는 waiting_queue에서 실시간으로 줄어든다)
            is_swap_needed = pq->process[parent_idx]->io_burst > pq->process[idx]->io_burst;
            break;

        default:
            break;
    }

    if (is_swap_needed) {
        swap(&(pq->process[idx]), &(pq->process[parent_idx]));
        heapify_up(pq, parent_idx, condition);
    }
}

void heapify_down(PriorityQueue *pq, int idx, int condition) {
    int smallest = idx;
    int left = 2*idx + 1;
    int right = 2*idx + 2;

    if (left < pq->size) {
        int left_cond = 0;
        switch (condition) {
            case FCFS:
                left_cond = pq->process[left]->arrival_time < pq->process[smallest]->arrival_time;
                break;
            
            case SJF:
                left_cond = pq->process[left]->cpu_burst < pq->process[smallest]->cpu_burst;
                break;

            case PRIORITY:
                left_cond = pq->process[left]->priority < pq->process[smallest]->priority;
                break;

            case RR:
                left_cond = pq->process[left]->arrival_time < pq->process[smallest]->arrival_time;
                break;

            case WAITING_QUEUE:
                left_cond = pq->process[left]->io_burst < pq->process[smallest]->io_burst;
                break;
            
            default:
                break;
        }

        if (left_cond) {
            smallest = left;
        }
    }

    if (right < pq->size) {
        int right_cond = 0;
        switch (condition) {
            case FCFS:
                right_cond = pq->process[right]->arrival_time < pq->process[smallest]->arrival_time;
                break;
            
            case SJF:
                right_cond = pq->process[right]->cpu_burst < pq->process[smallest]->cpu_burst;
                break;

            case PRIORITY:
                right_cond = pq->process[right]->priority < pq->process[smallest]->priority;
                break;

            case RR:
                right_cond = pq->process[right]->arrival_time < pq->process[smallest]->arrival_time;
                break;

            case WAITING_QUEUE:
                right_cond = pq->process[right]->io_burst < pq->process[smallest]->io_burst;
                break;
            
            default:
                break;
        }

        if (right_cond) {
            smallest = right;
        }
    }

    if (smallest != idx) {
        swap(&pq->process[idx], &pq->process[smallest]);
        heapify_down(pq, smallest, condition);
    }
}

void pq_init(PriorityQueue* pq) {
    pq->size = 0;
    for (int i = 0; i < MAX_PROCESSES; i++) {
        pq->process[i] = NULL;
    }
}

void pq_push(PriorityQueue* pq, Process *p, int condition) {
    if (pq->size == MAX_PROCESSES) {
        return;
    }

    pq->process[pq->size] = p;
    heapify_up(pq, pq->size, condition);
    (pq->size)++;
}

Process *pq_pop(PriorityQueue *pq, int condition) {
    if (pq->size == 0) {
        return NULL;
    }

    Process *top = pq->process[0];
    pq->process[0] = pq->process[--(pq->size)];
    heapify_down(pq, 0, condition);
    return top;
}

int pq_is_empty(PriorityQueue *pq) {
    return pq->size == 0;
}

int pq_is_full(PriorityQueue *pq) {
    return pq->size == MAX_PROCESSES;
}

////////////////////////////////////////////////////////////////////////////////

Process list_of_processes[MAX_PROCESSES]; // create_process()를 통해 생성된 프로세스들 저장
int the_number_of_process = 0; // 생성된 프로세스의 개수

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

/**
 * @brief 
 * 스케줄링을 선택할 수 있는 메뉴를 출력하고, 스케줄링 방법을 선택할 수 있도록 한다.
 */
void schedule_main();

/**
 * @brief 
 * 비선점형 스케줄링 알고리즘
 * 파라미터를 통해 FCFS, SJF, Priority 스케줄링을 구별한다.
 */
void nonpreemptive_schedule(int);

/**
 * @brief 
 * FCFS 스케줄링을 수행하고, 간트 차트를 출력한다.
 */
void schedule_FCFS();

/**
 * @brief 
 * 비선점 SJF 스케줄링을 수행하고, 간트 차트를 출력한다.
 */
void schedule_nonpreemptive_SJF();

void schedule_preemptive_SJF();

void schedule_nonpreemptive_priority();

void schedule_preemptive_priority();

void schedule_RR();

/**
 * @brief 
 * 스케줄링에서 qsort 함수를 사용할 때 사용되는 compare 함수
 * arrival_time이 작을수록 배열 앞에 배치한다.
 */
int arrival_compare(const void *, const void *);

/**
 * @brief 
 * 완성된 스케줄링을 기반으로 간트 차트를 출력한다.
 */
void print_gantt_chart(Record *, int);

void evaluation();

////////////////////////////////////////////////////////////////////////////////