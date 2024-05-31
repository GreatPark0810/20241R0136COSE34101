#include "cpu_scheduling_simulator.h"

int main() {
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
                random_num = (rand() % 10000) + 1; // random(1 ~ 10000)한 pid 값 지정 but 중복은 X
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
        list_of_processes[i].io_time = get_random_int(1, list_of_processes[i].cpu_burst);
        list_of_processes[i].io_burst = get_random_int(5, MAX_IO_BURST);
        list_of_processes[i].priority = get_random_int(1, MAX_PRIORITY);
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
            printf("CPU burst\t\t%d\n", list_of_processes[i].cpu_burst);
            printf("IO execution time\t%d\n", list_of_processes[i].io_time);
            printf("IO burst\t\t%d\n", list_of_processes[i].io_burst);
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
        printf("CPU burst\t\t%d\n", list_of_processes[i].cpu_burst);
        printf("IO execution time\t%d\n", list_of_processes[i].io_time);
        printf("IO burst\t\t%d\n", list_of_processes[i].io_burst);
        printf("Priority\t\t%d\n\n", list_of_processes[i].priority);
    }
}

void config() {

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
                schedule_FCFS();
                return;
            
            case 2:
                schedule_nonpreemptive_SJF();
                return;

            case 3:
                schedule_preemptive_SJF();
                return;

            case 4:
                schedule_nonpreemptive_priority();
                return;

            case 5:
                schedule_preemptive_priority();
                return;

            case 6:
                schedule_RR();
                return;

            default:
                printf("\nYou have entered a wrong number. Please try again.\n");
                break;
        }
    }
}

void nonpreemptive_schedule(int condition) {
    PriorityQueue ready_queue, waiting_queue;
    // ready_queue와 waiting_queue initialize
    pq_init(&ready_queue);
    pq_init(&waiting_queue);

    // 프로세스 배열에 list_of_processes 복사 후 arrival_time 기준으로 오름차순 정렬
    Process copied_list[the_number_of_process];
    memcpy(copied_list, list_of_processes, the_number_of_process * sizeof(Process));
    qsort(copied_list, the_number_of_process, sizeof(Process), arrival_compare);

    // CPU 스케줄링과 IO 스케줄링을 기록하기 위한 Record 배열 선언 및 초기화
    Record schedule_record[1000];
    for (int i = 0; i < 1000; i++) {
        schedule_record[i].process = NULL;
        schedule_record[i].burst_type = NONE;
        schedule_record[i].start_time = -1;
    }

    int time = 0;
    int process_idx = 0;
    int record_idx = 0;
    
    int is_idle = 1;
    Process *current_processor = NULL;

    // CPU 스케줄링 시작
    while (1) {
        // 종료 조건
        if (process_idx >= the_number_of_process && pq_is_empty(&ready_queue) &&
            pq_is_empty(&waiting_queue) && current_processor == NULL) {
            break;
        }

        // time == arrival_time이면 ready_queue에 프로세스를 삽입한다
        // 중복된 arrival_time으로 인한 버그를 방지하기 위해 while문 사용
        while (process_idx < the_number_of_process && time == copied_list[process_idx].arrival_time) {
            pq_push(&ready_queue, &(copied_list[process_idx]), condition);
            process_idx++;
        }

        // ready_queue도 차있는데 idle 상태라면 바로 ready_queue에서 pop해서 실행한다
        if (is_idle && !pq_is_empty(&ready_queue)) {
            Process *next = pq_pop(&ready_queue, condition);

            current_processor = next;
            schedule_record[record_idx].process = next;
            schedule_record[record_idx].burst_type = CPU_BURST;
            schedule_record[record_idx].start_time = time;

            is_idle = 0;
            record_idx++;
        }

        // 만약 처음 시작에 ready_queue가 비어있다면 처음에는 IDLE로 시작한다
        if (time == 0 && pq_is_empty(&ready_queue)) {
            current_processor = NULL;

            schedule_record[record_idx].process = NULL;
            schedule_record[record_idx].burst_type = IDLE;
            schedule_record[record_idx].start_time = time;

            is_idle = 1;
            record_idx++;
        }

        // waiting_queue의 처리 (io_burst 소진 및 끝난 프로세스 pop)
        if (!pq_is_empty(&waiting_queue)) {
            // 모든 waiting_queue 내의 프로세스의 io_burst 소진
            for (int i = 0; i < waiting_queue.size; i++) {
                (waiting_queue.process[i]->io_burst)--;
            }
            // 만약 waiting_queue의 맨 앞에 있는 프로세스의 io_burst가 0이 된다면 pop 후 ready_queue로 push
            if (waiting_queue.process[0]->io_burst == 0) {
                Process *p = pq_pop(&waiting_queue, WAITING_QUEUE);
                pq_push(&ready_queue, p, condition);
            }
        }

        // 현재 프로세서가 CPU에 올라가 있을 때
        if (!is_idle) {
            // 현재 CPU에 있는 프로세스가 모든 burst 완료했다면 (cpu_burst와 io_burst 모두 소진)
            if (current_processor->cpu_burst <= 0 && current_processor->io_burst <= 0) {
                // 다음으로 CPU에 들어갈 프로세스 ready_queue에서 pop
                Process *next = pq_pop(&ready_queue, condition);
                
                // 만약 ready_queue가 비어있다면 IDLE 상태로 들어간다
                if (next == NULL) {
                    current_processor = NULL;
                    schedule_record[record_idx].process = NULL;
                    schedule_record[record_idx].burst_type = IDLE;
                    schedule_record[record_idx].start_time = time;
                
                    is_idle = 1;
                }
                // ready_queue에서 다음 프로세스를 뽑아오는데 성공했다면 바로 다음 프로세스를 실행한다
                else {
                    current_processor = next;
                    schedule_record[record_idx].process = next;
                    schedule_record[record_idx].burst_type = CPU_BURST;
                    schedule_record[record_idx].start_time = time;

                    current_processor->cpu_burst--;
                }

                record_idx++;
            }
            // CPU burst 중 IO operation이 발생하였다면 waiting_queue에 push
            // 이미 IO operation 갔다온 프로세스들은 제외
            else if (time == schedule_record[record_idx-1].start_time + current_processor->io_time &&
                current_processor->io_burst > 0) {
                pq_push(&waiting_queue, current_processor, WAITING_QUEUE);
                // 다음으로 CPU에 들어갈 프로세스 ready_queue에서 pop
                Process *next = pq_pop(&ready_queue, condition);

                // 만약 ready_queue가 비어있다면 IDLE 상태로 들어간다
                if (next == NULL) {
                    current_processor = NULL;
                    schedule_record[record_idx].process = NULL;
                    schedule_record[record_idx].burst_type = IDLE;
                    schedule_record[record_idx].start_time = time;

                    is_idle = 1;
                }
                // ready_queue에서 다음 프로세스를 뽑아오는데 성공했다면 바로 다음 프로세스를 실행한다
                else {
                    current_processor = next;
                    schedule_record[record_idx].process = next;
                    schedule_record[record_idx].burst_type = CPU_BURST;
                    schedule_record[record_idx].start_time = time;

                    current_processor->cpu_burst--;
                }

                record_idx++;
            }
            // 두 경우 모두 아니라면 그냥 cpu_burst 소진
            else {
                current_processor->cpu_burst--;
            }
        }
        time++;
    }

    print_gantt_chart(schedule_record, record_idx);
}

void schedule_FCFS() {
    printf("\n******************** FCFS ********************\n");
    nonpreemptive_schedule(FCFS);
}


void schedule_nonpreemptive_SJF() {
    printf("\n*************** Non-preemptive SJF ***************\n");
    nonpreemptive_schedule(SJF);
}

void schedule_preemptive_SJF() {

}

void schedule_nonpreemptive_priority() {
    printf("\n********** Non-preemptive Priority **********\n");
    nonpreemptive_schedule(PRIORITY);
}

void schedule_preemptive_priority() {

}

void schedule_RR() {

}

int arrival_compare(const void *p1, const void *p2) {
    Process *a = (Process *)p1;
    Process *b = (Process *)p2;
    return (a->arrival_time > b->arrival_time) ? 1 : 0;
}

void print_gantt_chart(Record *record, int record_size) {
    printf("\nGantt Chart\n\n");

    printf("+-------+------------+\n");
    printf("|  Time |   Process  |\n");
    printf("+-------+------------+\n");

    for (int i = 0; i < record_size; i++) {
        if (record[i].burst_type == NONE) {
            continue;
        }

        printf("| %5d | ", record[i].start_time);
        
        if (record[i].burst_type == IDLE) {
            printf("%10s |\n", "IDLE");
        } 
        else {
            printf("PID %6d |\n", record[i].process->pid);
        }
    }

    printf("+-------+------------+\n");
}

void evaluation() {

}