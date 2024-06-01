#include "cpu_scheduling_simulator.h"

int main() {
    // 시드값 변경
    srand((unsigned int)time(NULL));

    while (1) {
        int select;

        printf("\n****************** Main menu ******************\n\n");
        printf("1. Create processes\n");
        printf("2. Print process info\n");
        printf("3. Process Scheduling\n");
        printf("4. Evaluate processes by schedule\n");
        printf("\n");

        printf("Enter the menu you want to conduct.\n");
        printf("If you haven't created processes yet, you should make them first.\n");
        printf("Menu : ");
        scanf("%d", &select);
        
        switch (select) {
            case 1:
                create_process();
                break;
            
            case 2:
                if (the_number_of_process == 0) {
                    printf("\nYou should create processes first!\n");
                    break;
                }

                print_process_info();
                break;

            case 3:
                if (the_number_of_process == 0) {
                    printf("\nYou should create processes first!\n");
                    break;
                }

                schedule_main();
                break;

            case 4:
                if (the_number_of_process == 0) {
                    printf("\nYou should create processes first!\n");
                    break;
                }

                evaluation();
                break;

            default:
                printf("\nYou have entered a wrong number. Please try again.\n");
                break;
        }
    }
}

void create_process() {
    printf("\n********** Create Processes **********\n");

    // 만약 프로세스가 이미 생성되어 있는 상황이라면 초기화
    if (the_number_of_process > 0) {
        int select;

        do {
            printf("The processes have been already created. You should remove them first!\n");
            printf("If you want to do, input 0. If you don't, input 1. : ");
            scanf("%d", &select);

            // 프로세스 초기화를 진행
            if (select == 0) {
                remove_process();
                break;
            }
            // 메인 메뉴로 돌아감
            else if (select == 1) {
                return;
            }
            // 잘못된 입력
            else {
                printf("Please input a right number.\n");
            }
        } while (1);
    }

    // 프로세스의 pid를 자동으로 생성할 것인지, 수동으로 할당할 것인지 모드 설정
    int pid_input_type; // pid 자동/수동 할당 (0 : 자동, 1 : 수동)
    do {
        printf("Please select the process creation mode. (0 : Automatic / 1 : Manual) : ");
        scanf("%d", &pid_input_type);

        if (pid_input_type != 0 && pid_input_type != 1) {
            printf("Please input a right number.\n");
        }
    } while (pid_input_type != 0 && pid_input_type != 1);
    
    // 몇 개의 프로세스를 생성할 것인지 설정
    printf("\n");
    do {
        printf("Please input the number of process you want to create. : ");
        scanf("%d", &the_number_of_process);

        // 프로세스의 갯수가 0 이하이거나, 지정해둔 MAXIMUM 값보다 클 때 오류 발생
        if (the_number_of_process > MAX_PROCESSES) {
            printf("Error : The number is not supposed to be more than %d.\n", MAX_PROCESSES);
        }
        else if (the_number_of_process <= 0) {
            printf("Error : The number is supposed to be positive integer.\n");
        }
    } while (the_number_of_process <= 0 || the_number_of_process > MAX_PROCESSES);
    
    int is_occupied[10001] = { 0, }; // pid값(1 ~ 10000)이 중복되지 않도록 보장하는 변수 (0이면 비어있음, 1이면 occupied)

    // pid를 자동으로 생성하여 할당
    if (pid_input_type == 0) {
        for (int i = 0; i < the_number_of_process; i++) {
            int random_num = 0;

            // 비어있는 pid를 찾을 때까지 반복 (중복 방지하기 위해)
            do {
                random_num = get_random_int(1, 10000); // random(1 ~ 10000)한 pid 값 지정 but 중복은 X
            } while (is_occupied[random_num] == 1);

            // 찾은 pid 값을 i번째 프로세스에 할당 후 occupied 처리
            list_of_processes[i].pid = random_num;
            is_occupied[random_num] = 1;
        }
    }
    // 수동으로 pid 할당
    else if (pid_input_type == 1) {
        int input_num;

        // 수동으로 i번째 process의 pid값 입력 (1 ~ 10000까지)
        printf("\nPlease input a pid of each process.\n");
        printf("The input number must be a positive integer(1 ~ 10000).\n");
        for (int i = 0; i < the_number_of_process; i++) {
            do {
                printf("the pid of the process %d : ", i);
                scanf("%d", &input_num);

                // 1 ~ 10000 범위이면서 중복되지 않은 integer input만 허용
                if (input_num < 1 || input_num > 10000) {
                    printf("Error : The input number must be positive integer(1 ~ 10000).\n");
                }
                else if (is_occupied[input_num] != 0) {
                    printf("Error : The input number is duplicate.\n");
                }
            } while (input_num < 1 || input_num > 10000 || is_occupied[input_num] != 0);

            // 찾은 pid 값을 i번째 프로세스에 할당 후 occupied 처리
            list_of_processes[i].pid = input_num;
            is_occupied[input_num] = 1;
        }
    }

    // pid를 제외한 나머지 Process 구조체 멤버에 값 할당
    for (int i = 0; i < the_number_of_process; i++) {
        list_of_processes[i].arrival_time = get_random_int(0, MAX_ARRIVAL_TIME);
        list_of_processes[i].cpu_burst = get_random_int(5, MAX_CPU_BURST);
        list_of_processes[i].io_time = get_random_int(1, list_of_processes[i].cpu_burst - 1);
        list_of_processes[i].io_burst = get_random_int(1, MAX_IO_BURST);
        list_of_processes[i].priority = get_random_int(1, MAX_PRIORITY);
        list_of_processes[i].original_cpu_burst = list_of_processes[i].cpu_burst;
        list_of_processes[i].original_io_burst = list_of_processes[i].io_burst;
        list_of_processes[i].waiting_time = 0;
    }

    printf("\nAll processes are generated!\n");
}

void remove_process() {
    for (int i = 0; i < the_number_of_process; i++) {
        list_of_processes[i].pid = 0;
        list_of_processes[i].arrival_time = 0;
        list_of_processes[i].cpu_burst = 0;
        list_of_processes[i].io_time = 0;
        list_of_processes[i].io_burst = 0;
        list_of_processes[i].priority = 0;
        list_of_processes[i].original_cpu_burst = 0;
        list_of_processes[i].original_io_burst = 0;
        list_of_processes[i].waiting_time = 0;
    }
    the_number_of_process = 0;

    printf("\nAll processes are removed!\n");
}

void print_process_info() {
    printf("\n********** Print the information of each process **********\n");
    int print_type;

    do {
        printf("Please select the process printing mode. (0 : All processes / 1 : A specific process) : ");
        scanf("%d", &print_type);

        if (print_type != 0 && print_type != 1) {
            printf("Please input a right number.\n");
        }
    } while (print_type != 0 && print_type != 1);

    // 모든 프로세스의 정보 출력
    if (print_type == 0) {
        for (int i = 0; i < the_number_of_process; i++) {
            printf("\nProcess %d\n", i);
            printf("PID\t\t\t%d\n", list_of_processes[i].pid);
            printf("Arrival time\t\t%d\n", list_of_processes[i].arrival_time);
            printf("CPU burst\t\t%d\n", list_of_processes[i].original_cpu_burst);
            printf("IO execution time\t%d\n", list_of_processes[i].io_time);
            printf("IO burst\t\t%d\n", list_of_processes[i].original_io_burst);
            printf("Priority\t\t%d\n\n", list_of_processes[i].priority);
        }
    }
    // 특정한 하나의 프로세스 정보 출력
    else if (print_type == 1) {
        int i;
        do {
            printf("Which number of process are you looking for? (From 0 to %d): ", the_number_of_process - 1);
            scanf("%d", &i);

            if (i < 0 || i >= the_number_of_process) {
                printf("Please input a right number.\n");
            }
        } while (i < 0 || i >= the_number_of_process);

        printf("\nProcess %d\n", i);
        printf("PID\t\t\t%d\n", list_of_processes[i].pid);
        printf("Arrival time\t\t%d\n", list_of_processes[i].arrival_time);
        printf("CPU burst\t\t%d\n", list_of_processes[i].original_cpu_burst);
        printf("IO execution time\t%d\n", list_of_processes[i].io_time);
        printf("IO burst\t\t%d\n", list_of_processes[i].original_io_burst);
        printf("Priority\t\t%d\n\n", list_of_processes[i].priority);
    }
}

void schedule_main() {
    while (1) {
        int select;

        printf("\n********** Processes Scheduling **********\n");
        printf("1. FCFS\n");
        printf("2. Non-preemptive SJF\n");
        printf("3. Preemptive SJF\n");
        printf("4. Non-preemptive priority\n");
        printf("5. Preemptive priority\n");
        printf("6. RR\n");
        printf("\n");

        printf("Enter the scheduling method you want to conduct.\n");
        printf("Input : ");
        scanf("%d", &select);
        
        switch (select) {
            case 1:
                schedule_FCFS(record, &record_size);
                restore_process_value();
                return;
            
            case 2:
                schedule_nonpreemptive_SJF(record, &record_size);
                restore_process_value();
                return;

            case 3:
                schedule_preemptive_SJF(record, &record_size);
                restore_process_value();
                return;

            case 4:
                schedule_nonpreemptive_priority(record, &record_size);
                restore_process_value();
                return;

            case 5:
                schedule_preemptive_priority(record, &record_size);
                restore_process_value();
                return;

            case 6:
                schedule_RR(record, &record_size);
                restore_process_value();
                return;

            default:
                printf("\nYou have entered a wrong number. Please try again.\n");
                break;
        }

    }
}

void schedule_FCFS(Record *record, int *record_size) {
    printf("\n******************** FCFS ********************\n");
    schedule(FCFS, 0, record, record_size);
    print_gantt_chart(record, record_size);
}

void schedule_nonpreemptive_SJF(Record *record, int *record_size) {
    printf("\n*************** Non-preemptive SJF ***************\n");
    schedule(SJF, 0, record, record_size);
    print_gantt_chart(record, record_size);
}

void schedule_preemptive_SJF(Record *record, int *record_size) {
    printf("\n*************** Non-preemptive SJF ***************\n");
    schedule(SJF, 1, record, record_size);
    print_gantt_chart(record, record_size);
}

void schedule_nonpreemptive_priority(Record *record, int *record_size) {
    printf("\n********** Non-preemptive Priority **********\n");
    schedule(PRIORITY, 0, record, record_size);
    print_gantt_chart(record, record_size);
}

void schedule_preemptive_priority(Record *record, int *record_size) {
    printf("\n*************** Non-preemptive SJF ***************\n");
    schedule(PRIORITY, 1, record, record_size);
    print_gantt_chart(record, record_size);
}

void schedule_RR(Record *record, int *record_size) {
    if (time_quantum == 0) {
        printf("Before you start this scheduling algorithm, you need to input a time quantum(2~5).\n");
        do {
            printf("Please input the time quantum : ");
            scanf("%d", &time_quantum);

            if (time_quantum < 2 || time_quantum > 5) {
                printf("you need to input that from 2 to 5!\n");
            }
        } while (time_quantum < 2 || time_quantum > 5);
    }
    
    printf("\n******************** Round Robin ********************\n");
    schedule(RR, 0, record, record_size);
    print_gantt_chart(record, record_size);
}

void schedule(int condition, int is_preemptive, Record *schedule_record, int *record_size) {
    PriorityQueue ready_queue, waiting_queue;
    // ready_queue와 waiting_queue initialize
    pq_init(&ready_queue);
    pq_init(&waiting_queue);

    // 프로세스 배열에 list_of_processes 배열의 각 포인터 할당 후 arrival_time 기준으로 오름차순 정렬
    Process *copied_list[the_number_of_process];
    for (int i = 0; i < the_number_of_process; i++) {
        copied_list[i] = &list_of_processes[i];
    }
    qsort(copied_list, the_number_of_process, sizeof(Process *), arrival_compare);

    // CPU 스케줄링을 기록하기 위한 Record 배열 초기화
    for (int i = 0; i < MAX_RECORD; i++) {
        schedule_record[i].process = NULL;
        schedule_record[i].burst_type = NONE;
        schedule_record[i].start_time = 0;
    }
    *record_size = 0;

    int time = 0;
    int process_idx = 0;
    
    int is_idle = 1;
    Process *current_processor = NULL;

    while (1) {
        // 디버깅용 printf 코드
        // usleep(10000);
        // printf("Time : %d\n", time);
        // printf("Ready queue : ");
        // for (int i = 0; i < ready_queue.size; i++) {
        //     printf("%d / ", ready_queue.process[i]->pid);
        // }
        // printf("\nWaiting queue : ");
        // for (int i = 0; i < waiting_queue.size; i++) {
        //     printf("%d / ", waiting_queue.process[i]->io_burst);
        // }
        // printf("\nprocess index : %d\n", process_idx);
        // printf("record_size : %d\n", *record_size);
        // printf("current processor is %s", current_processor == NULL ? "Null\n" : "exist\n");
        // if (current_processor != NULL) {
        //     printf("cpu_burst : %d\n", current_processor->cpu_burst);
        // }
        // printf("\n\n");

        // 종료 조건
        if (process_idx >= the_number_of_process && pq_is_empty(&ready_queue) &&
            pq_is_empty(&waiting_queue) && current_processor == NULL) {
            break;
        }

        // time == arrival_time이면 ready_queue에 프로세스를 condition 기준으로 push한다
        // 중복된 arrival_time으로 인한 버그를 방지하기 위해 while문 사용
        while (process_idx < the_number_of_process && time == copied_list[process_idx]->arrival_time) {
            pq_push(&ready_queue, copied_list[process_idx], condition);
            process_idx++;
        }

        // ready_queue의 처리 (ready_queue 내 대기하는 프로세스들의 waiting_time 증가)
        if (!pq_is_empty(&ready_queue)) {
            for (int i = 0; i < ready_queue.size; i++) {
                (ready_queue.process[i]->waiting_time)++;
            }
        }

        // waiting_queue의 처리 (io_burst 소진 및 끝난 프로세스 pop)
        if (!pq_is_empty(&waiting_queue)) {
            // 모든 waiting_queue 내의 프로세스의 io_burst 1씩 소진
            for (int i = 0; i < waiting_queue.size; i++) {
                (waiting_queue.process[i]->io_burst)--;
            }
            // 만약 waiting_queue의 맨 앞에 있는 프로세스의 io_burst가 0이 된다면 pop 후 ready_queue로 push
            // 중복으로 waiting_queue 내 여러 프로세스의 io_burst가 0이 될 수 있으므로 while문 사용
            while (waiting_queue.process[0]->io_burst == 0 && waiting_queue.size > 0) {
                Process *p = pq_pop(&waiting_queue, WAITING_QUEUE);
                pq_push(&ready_queue, p, condition);
            }
        }

        // 처음 시작 시
        if (time == 0) {
            // ready_queue가 비어있다면 IDLE로 시작
            if (pq_is_empty(&ready_queue)) {
                current_processor = NULL;

                schedule_record[*record_size].process = NULL;
                schedule_record[*record_size].burst_type = IDLE;
                schedule_record[*record_size].start_time = time;

                is_idle = 1;
                (*record_size)++;
            }
            // 차있다면 바로 ready_queue에서 pop하고 CPU_BURST로 시작
            else {
                Process *next = pq_pop(&ready_queue, condition);

                current_processor = next;
                schedule_record[*record_size].process = next;
                schedule_record[*record_size].burst_type = CPU_BURST;
                schedule_record[*record_size].start_time = time;

                is_idle = 0;
                (*record_size)++;
            }
        }
        // time > 0
        else {
            // ready_queue도 차있는데 idle 상태라면 바로 ready_queue에서 pop해서 실행한다
            if (is_idle && !pq_is_empty(&ready_queue)) {
                Process *next = pq_pop(&ready_queue, condition);

                current_processor = next;
                schedule_record[*record_size].process = next;
                schedule_record[*record_size].burst_type = CPU_BURST;
                schedule_record[*record_size].start_time = time;

                is_idle = 0;
                (*record_size)++;
            }
        }

        // 현재 프로세서가 CPU에 올라가 있을 때
        if (!is_idle) {
            // 현재 CPU에 있는 프로세스가 cpu_burst 완료했다면 (cpu_burst 모두 소진)
            if (current_processor->cpu_burst == 0) {
                // 다음으로 CPU에 들어갈 프로세스 ready_queue에서 pop
                Process *next = pq_pop(&ready_queue, condition);
                
                // 만약 ready_queue가 비어있다면 IDLE 상태로 들어간다
                if (next == NULL) {
                    current_processor = NULL;
                    schedule_record[*record_size].process = NULL;
                    schedule_record[*record_size].burst_type = IDLE;
                    schedule_record[*record_size].start_time = time;
                
                    is_idle = 1;
                }
                // ready_queue에서 다음 프로세스를 뽑아오는데 성공했다면 바로 다음 프로세스를 실행한다
                else {
                    current_processor = next;
                    schedule_record[*record_size].process = next;
                    schedule_record[*record_size].burst_type = CPU_BURST;
                    schedule_record[*record_size].start_time = time;

                    current_processor->cpu_burst--;
                }

                (*record_size)++;
            }
            // CPU burst 중 IO operation이 발생하였다면 waiting_queue에 push
            // 이미 IO operation 갔다온 프로세스들은 제외
            else if (time == schedule_record[(*record_size)-1].start_time + current_processor->io_time &&
                current_processor->io_burst > 0) {
                pq_push(&waiting_queue, current_processor, WAITING_QUEUE);
                // 다음으로 CPU에 들어갈 프로세스 ready_queue에서 pop
                Process *next = pq_pop(&ready_queue, condition);

                // 만약 ready_queue가 비어있다면 IDLE 상태로 들어간다
                if (next == NULL) {
                    current_processor = NULL;
                    schedule_record[*record_size].process = NULL;
                    schedule_record[*record_size].burst_type = IDLE;
                    schedule_record[*record_size].start_time = time;

                    is_idle = 1;
                }
                // ready_queue에서 다음 프로세스를 뽑아오는데 성공했다면 바로 다음 프로세스를 실행한다
                else {
                    current_processor = next;
                    schedule_record[*record_size].process = next;
                    schedule_record[*record_size].burst_type = CPU_BURST;
                    schedule_record[*record_size].start_time = time;

                    current_processor->cpu_burst--;
                }

                (*record_size)++;
            }
            // 선점형 알고리즘에만 해당
            else if (is_preemptive && // 현재 스케줄링 알고리즘이 선점형이고, 
                // ready_queue에서 현재 프로세스보다 condition 기준으로 우선적인 프로세스가 검출된다면

                // condition == SJF : 현재 프로세스의 원래 CPU burst > ready_queue 내 프로세스의 CPU burst
                ((condition == SJF && ready_queue.size > 0 &&
                current_processor->original_cpu_burst > ready_queue.process[0]->original_cpu_burst) ||

                // condition == PRIORITY : 현재 프로세스의 Priority > ready_queue 내 프로세스의 Priority
                (condition == PRIORITY && ready_queue.size > 0 &&
                current_processor->priority > ready_queue.process[0]->priority))) {
                
                // 즉시 현재 프로세스를 ready_queue로 넘기고 새로운 프로세스를 가져온다.
                Process *next = pq_pop(&ready_queue, condition);
                pq_push(&ready_queue, current_processor, condition);

                current_processor = next;
                schedule_record[*record_size].process = next;
                schedule_record[*record_size].burst_type = CPU_BURST;
                schedule_record[*record_size].start_time = time;

                (*record_size)++;
                current_processor->cpu_burst--;
            }
            // Round Robin 알고리즘에만 해당
            // 현재 CPU에 올라와 있는 프로세스가 time_quantum만큼 시간이 지났다면
            else if (condition == RR && !is_preemptive && 
                time - schedule_record[(*record_size)-1].start_time == time_quantum &&
                ready_queue.size > 0 && current_processor->cpu_burst > 0) {

                // 즉시 현재 프로세스를 ready_queue로 넘기고 새로운 프로세스를 가져온다.
                Process *next = pq_pop(&ready_queue, condition);
                pq_push(&ready_queue, current_processor, condition);

                current_processor = next;
                schedule_record[*record_size].process = next;
                schedule_record[*record_size].burst_type = CPU_BURST;
                schedule_record[*record_size].start_time = time;

                (*record_size)++;
                current_processor->cpu_burst--;
            } 
            // 모두 아니라면 그냥 cpu_burst 소진
            else {
                current_processor->cpu_burst--;
            }
        }
        time++;
    }
}

int arrival_compare(const void *p1, const void *p2) {
    Process *a = *(Process **)p1;
    Process *b = *(Process **)p2;
    return (a->arrival_time > b->arrival_time) ? 1 : 0;
}

void restore_process_value() {
    for (int i = 0; i < the_number_of_process; i++) {
        list_of_processes[i].cpu_burst = list_of_processes[i].original_cpu_burst;
        list_of_processes[i].io_burst = list_of_processes[i].original_io_burst;
        list_of_processes[i].waiting_time = 0;
    }
}

void print_gantt_chart(Record *schedule_record, int *record_size) {
    printf("\nGantt Chart\n\n");
    printf("+-------+------------+\n");
    printf("|  Time |   Process  |\n");
    printf("+-------+------------+\n");

    for (int i = 0; i < *record_size; i++) {
        if (schedule_record[i].burst_type == NONE) {
            continue;
        }

        printf("| %5d | ", schedule_record[i].start_time);
        
        if (schedule_record[i].process == NULL) {
            printf("%10s |\n", "IDLE");
        } 
        else {
            printf("PID %6d |\n", schedule_record[i].process->pid);
        }
    }

    printf("+-------+------------+\n");
}

void evaluation() {
    printf("Before you start this scheduling algorithm, you need to input a time quantum(2~5).\n");
    do {
        printf("Please input the time quantum : ");
        scanf("%d", &time_quantum);

        if (time_quantum < 2 || time_quantum > 5) {
            printf("you need to input that from 2 to 5!\n");
        }
    } while (time_quantum < 2 || time_quantum > 5);

    // 각각 FCFS, 비선점 SJF, 선점 SJF, 비선점 Priority, 선점 Priority, RR 스케줄링 기록 저장
    Record records[6][MAX_RECORD];
    int record_size[6] = { 0, };
    float avg_waiting_time[6] = { 0.0f, };
    float avg_turnaround_time[6] = { 0.0f, };

    // 스케줄링과 동시에 평균 waiting time 계산 (초기화 시)
    schedule_FCFS(records[0], &(record_size[0]));
    for (int i = 0; i < the_number_of_process; i++) {
        avg_waiting_time[0] += list_of_processes[i].waiting_time;
    }
    avg_waiting_time[0] /= the_number_of_process;
    restore_process_value();

    schedule_nonpreemptive_SJF(records[1], &(record_size[1]));
    for (int i = 0; i < the_number_of_process; i++) {
        avg_waiting_time[1] += list_of_processes[i].waiting_time;
    }
    avg_waiting_time[1] /= the_number_of_process;
    restore_process_value();

    schedule_preemptive_SJF(records[2], &(record_size[2]));
    for (int i = 0; i < the_number_of_process; i++) {
        avg_waiting_time[2] += list_of_processes[i].waiting_time;
    }
    avg_waiting_time[2] /= the_number_of_process;
    restore_process_value();

    schedule_nonpreemptive_priority(records[3], &(record_size[3]));
    for (int i = 0; i < the_number_of_process; i++) {
        avg_waiting_time[3] += list_of_processes[i].waiting_time;
    }
    avg_waiting_time[3] /= the_number_of_process;
    restore_process_value();

    schedule_preemptive_priority(records[4], &(record_size[4]));
    for (int i = 0; i < the_number_of_process; i++) {
        avg_waiting_time[4] += list_of_processes[i].waiting_time;
    }
    avg_waiting_time[4] /= the_number_of_process;
    restore_process_value();

    schedule_RR(records[5], &(record_size[5]));
    for (int i = 0; i < the_number_of_process; i++) {
        avg_waiting_time[5] += list_of_processes[i].waiting_time;
    }
    avg_waiting_time[5] /= the_number_of_process;
    restore_process_value();

    printf("\n************************* Time Analysis *************************\n\n");

    // 평균 Turnaround time 구하기 (완료 - Arrival)
    for (int i = 0; i < 6; i++) { // 스케줄링마다
        int total_turnaround_time = 0;
        for (int j = 0; j < the_number_of_process; j++) { // 각 프로세스마다
            int pid = list_of_processes[j].pid;
            int arrival_time = list_of_processes[j].arrival_time;
            int completion_time = 0;

            // completion_time 갱신
            for (int k = 0; k < record_size[i]-1; k++) { 
                if (records[i][k].process != NULL && pid == records[i][k].process->pid) {
                    completion_time = records[i][k+1].start_time;
                }
            }

            int turnaround_time = completion_time - arrival_time;
            total_turnaround_time += turnaround_time;
        }

        avg_turnaround_time[i] = (float)total_turnaround_time / the_number_of_process;
    }

    // turnaround_time과 waiting_time을 합한 total_time : 알고리즘 평가의 기준
    float total_time[6] = { 0.0f, };
    // 가장 작은 total_time을 가진 알고리즘이 가장 efficient한 알고리즘이다.
    int min_total_time_idx = 0;

    for (int i = 0; i < 6; i++) {
        total_time[i] = avg_turnaround_time[i] + avg_waiting_time[i];
        if (total_time[min_total_time_idx] > total_time[i]) {
            min_total_time_idx = i;
        }
    }

    printf("\t\t%-20s %-20s %-20s\n", "Waiting Time", "Turnaround Time", "Total Time");
    for (int i = 0; i < 6; i++) {
        switch (i) {
            case 0:
                printf("FCFS\t\t");
                break;
            case 1:
                printf("Nonpre-SJF\t");
                break;
            case 2:
                printf("Pre-SJF\t\t");
                break;
            case 3:
                printf("Nonpre-priority\t");
                break;
            case 4:
                printf("Pre-priority\t");
                break;
            case 5:
                printf("Round Robin\t");
                break;
        }
        printf("%-20.2f %-20.2f %-20.2f\n", avg_waiting_time[i], avg_turnaround_time[i], total_time[i]);
    }

    printf("\nTherefore, the most efficient algorithm in those processes is ");
    switch (min_total_time_idx) {
        case 0:
            printf("FCFS.");
            break;
        case 1:
            printf("Nonpreemptive SJF.\n");
            break;
        case 2:
            printf("Preemptive SJF.\n");
            break;
        case 3:
            printf("Nonpreemptive priority.\n");
            break;
        case 4:
            printf("Preemptive priority.\n");
            break;
        case 5:
            printf("Round Robin.\n");
            break;
    }
}