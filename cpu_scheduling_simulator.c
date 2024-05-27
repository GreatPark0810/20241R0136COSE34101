#include "cpu_scheduling_simulator.h"

int main() {
    while (1) {
        int select;

        printf("\n************************************************\n");
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

                evaluation_main();
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
        list_of_processes[i].arrival_time = (rand() % MAX_ARRIVAL_TIME) + 1;
        list_of_processes[i].cpu_burst = (rand() % MAX_CPU_BURST) + 1;
        list_of_processes[i].io_burst = (rand() % MAX_IO_BURST) + 1;
        list_of_processes[i].priority = (rand() % MAX_PRIORITY) + 1;
    }

    printf("\nAll processes are generated!\n");
}

void remove_process() {
    for (int i = 0; i < the_number_of_process; i++) {
        list_of_processes[i].pid = 0;
        list_of_processes[i].arrival_time = 0;
        list_of_processes[i].cpu_burst = 0;
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
            printf("PID\t\t%d\n", list_of_processes[i].pid);
            printf("Arrival time\t%d\n", list_of_processes[i].arrival_time);
            printf("CPU burst\t%d\n", list_of_processes[i].cpu_burst);
            printf("IO burst\t%d\n", list_of_processes[i].io_burst);
            printf("Priority\t%d\n\n", list_of_processes[i].priority);
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
        printf("PID\t\t%d\n", list_of_processes[i].pid);
        printf("Arrival time\t%d\n", list_of_processes[i].arrival_time);
        printf("CPU burst\t%d\n", list_of_processes[i].cpu_burst);
        printf("IO burst\t%d\n", list_of_processes[i].io_burst);
        printf("Priority\t%d\n\n", list_of_processes[i].priority);
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

void schedule_FCFS() {
    // ready_queue와 waiting_queue initialize
    init(&ready_queue);
    init(&waiting_queue);

    // 프로세스 배열에 list_of_processes 복사 후 arrival_time 기준으로 오름차순 정렬
    Process copied_list[the_number_of_process];
    memcpy(copied_list, list_of_processes, the_number_of_process * sizeof(Process));
    qsort(copied_list, the_number_of_process, sizeof(Process), arrival_compare);

    // 스케줄링을 기록하기 위한 Record 배열 선언 및 초기화
    Record schedule_record[100];
    for (int i = 0; i < 100; i++) {
        schedule_record[i].process = NULL;
        schedule_record[i].burst_type = NONE;
        schedule_record[i].start_time = -1;
    }

    int process_idx = 0;
    int record_idx = 0;
    int time = 0;
    while (process_idx < the_number_of_process) {
        // 아직 idx번째 프로세스가 도착하지 않았다면 arrival_time까지 IDLE
        if (time < copied_list[process_idx].arrival_time) {
            schedule_record[record_idx].burst_type = IDLE;
            schedule_record[record_idx].start_time = time;
            
            record_idx++;
            time = copied_list[process_idx].arrival_time;
        }
        // 딱 맞춰 도착했다면 프로세스 진행
        else if (time >= copied_list[process_idx].arrival_time) {
            schedule_record[record_idx].process = &(copied_list[process_idx]);
            schedule_record[record_idx].burst_type = CPU_BURST;
            schedule_record[record_idx].start_time = time;
            
            time += copied_list[process_idx].cpu_burst; // cpu_burst만큼 time 진행
            process_idx++;
            record_idx++;
        }
        // // 도착했는데 다른 프로세스가 진행중이라면 대기하다가 진행
        // else {
        //     schedule_record[record_idx].process = &(copied_list[process_idx]);
        //     schedule_record[record_idx].burst_type = CPU_BURST;
        //     schedule_record[record_idx].start_time = time;

        //     time += copied_list[process_idx].cpu_burst; // cpu_burst만큼 time 진행
        //     process_idx++;
        //     record_idx++;
        // }
    }

    for (int i = 0; i < record_idx; i++) {
        if (schedule_record[i].burst_type == CPU_BURST) {
            printf("PID %d : start time %d\n", (schedule_record[i].process)->pid, schedule_record[i].start_time);
        }
        else if (schedule_record[i].burst_type == IDLE) {
            printf("IDLE : start time %d\n", schedule_record[i].start_time);
        }
    }
}

int arrival_compare(const void *p1, const void *p2) {
    Process *a = (Process *)p1;
    Process *b = (Process *)p2;
    return (a->arrival_time > b->arrival_time) ? 1 : 0;
}

void schedule_nonpreemptive_SJF() {

}

void schedule_preemptive_SJF() {

}

void schedule_nonpreemptive_priority() {

}

void schedule_preemptive_priority() {

}

void schedule_RR() {

}

void evaluation_main() {

}

void evaluation_FCFS() {

}

void evaluation_nonpreemptive_SJF() {

}

void evaluation_preemptive_SJF() {

}

void evaluation_nonpreemptive_priority() {

}

void evaluation_preemptive_priority() {

}

void print_gantt_chart() {

}

void evaluation_RR() {

}