#include "elev.h"
#include "client.h"
#include <stdio.h>
#include <unistd.h>
#include <time.h>

int commands[3][N_FLOORS];
int move_dir = 0;
time_t door_timer;
void initialize_io_pannel(void){
    int i, j;
    for (i=0; i<3; i++){
        for (j=0; j<N_FLOORS; j++){
            commands[i][j]=0;
        }
    }
}

int is_door_open(double open_time){
    int i = elev_get_floor_sensor_signal();
    time_t now;
    time(&now);
    double diff = difftime(now,door_timer);
    if (diff < open_time){
        elev_set_door_open_lamp(1);
        turn_off_lights_at_floor(i);
        return 1;
    }
    else{
        if(elev_get_obstruction_signal()==1){
            return 1;
        }
        elev_set_door_open_lamp(0);
        return 0;
    }
}

void move_elevator(int dir){
    int i = elev_get_floor_sensor_signal();
    if (i != -1) {
        if (dir > 0 && i < N_FLOORS-1) {
            elev_set_motor_direction(DIRN_UP);
            return;
        }
        if (dir == 0) {
            elev_set_motor_direction(DIRN_STOP);
            return;
        }
        if (dir < 0 && i > 0) {
            elev_set_motor_direction(DIRN_DOWN);
            return;
        }
    }
    if (i == -1) {
        if (dir > 0) {
            elev_set_motor_direction(DIRN_UP);
            return;
        }
        if (dir == 0) {
            elev_set_motor_direction(DIRN_STOP);
            return;
        }
        if (dir < 0) {
            elev_set_motor_direction(DIRN_DOWN);
            return;
        }
    }
}

int should_stop(int floor){
    if (commands[0][floor] == 1 || (commands[1][floor] == 1 && move_dir >= 0) || (commands[2][floor] == 1 && move_dir <= 0)){
        printf("stop %d\n", floor+1);
        return 1;
    }else{
        //printf("no stop\n");
        return 0;
    }
}

int could_stop(int floor){
    if (commands[0][floor] == 1 || commands[1][floor] == 1 || commands[2][floor] == 1){
        return 1;
    }else{
        return 0;
    }
}

void stop_if_done(int loc){
    int do_cont = 0;
    if (move_dir > 0){
        int i;
        for(i=loc+1;i<N_FLOORS;i++){
            if (could_stop(i)==1){
                do_cont = 1;
            }
        }
    }
    if (move_dir < 0){
        int i;
        for(i=0;i<loc;i++){
            if (could_stop(i)==1){
                printf("please continue down\n");
                do_cont = 1;
            }
        }
    }
    if (do_cont == 0){
        if (move_dir != 0)
            printf("nothing in this direction\n");
        move_dir = 0;
    }
}

void execute_orders(void){
    int loc = elev_get_floor_sensor_signal();
    if (loc != -1){
        if(should_stop(loc) == 1){
            stop_and_open();
            stop_if_done(loc);
        }else if(move_dir==0){
            int i,j;
            for (i=0; i<3;i++){
                for(j=0;j<N_FLOORS;j++){
                    if (commands[i][j] == 1){
                        if (loc<j){
                            printf("going up\n");
                            move_dir = 1;
                            move_elevator(1);
                            return;
                        }else{
                            printf("going down\n");
                            move_dir = -1;
                            move_elevator(-1);
                            return;
                        }
                    }
                }
            }

        }else{
            stop_if_done(loc);
            printf("direction %d\n", move_dir);
            if (loc == 0 && move_dir == -1){
                printf("at the bottom\n");
                move_dir = 0;
            }
            if (loc == N_FLOORS-1 && move_dir == 1){
                printf("at the top\n");
                move_dir = 0;
            }

            move_elevator(move_dir);
        }
    }
}

void stop_and_open(void){
    int i = elev_get_floor_sensor_signal();
    elev_set_motor_direction(DIRN_STOP);
    set_elev_floor_lamp();
    time(&door_timer);
    if (move_dir == 0) {
        commands[0][i] = 0;
        commands[1][i] = 0;
        commands[2][i] = 0;
    }
    if (move_dir > 0) {
        commands[0][i] = 0;
        commands[1][i] = 0;
    }
    if (move_dir < 0) {
        commands[0][i] = 0;
        commands[2][i] = 0;
    }

    return;
}

void turn_off_lights_at_floor(int i){
    if (i >= 0 && i <= N_FLOORS-1) {
        if (i == 0){
            elev_set_button_lamp(BUTTON_CALL_UP, i, 0);
        }
        if (i == N_FLOORS-1){
            elev_set_button_lamp(BUTTON_CALL_DOWN, i, 0);
        }
        if(i < N_FLOORS-1 && move_dir >= 0){ 
            elev_set_button_lamp(BUTTON_CALL_UP, i, 0);
        }
        if(i > 0 && move_dir <= 0){
            elev_set_button_lamp(BUTTON_CALL_DOWN, i, 0);
        }
        elev_set_button_lamp(BUTTON_COMMAND, i, 0);
    }
}


void set_elev_floor_lamp(void){
    int i = elev_get_floor_sensor_signal();
    if (i != -1) {
        elev_set_floor_indicator(i);
    }
}


void listen_to_io_panels(void){
    int i;
    for (i = 0; i < N_FLOORS; i++){
        if(elev_get_button_signal(BUTTON_COMMAND, i) == 1){
            elev_set_button_lamp(BUTTON_COMMAND, i, 1);
            commands[0][i]=1;
            //printf("Button COMMAND floor ",i+1," Pushed\n");
        }
        if(i < N_FLOORS-1){
            if(elev_get_button_signal(BUTTON_CALL_UP, i) == 1){
                elev_set_button_lamp(BUTTON_CALL_UP, i, 1);
                commands[1][i]=1;
                //printf("Button UP floor ",i+1," Pushed\n");
            }
        }
        if(i > 0){
            if(elev_get_button_signal(BUTTON_CALL_DOWN, i) == 1){
                elev_set_button_lamp(BUTTON_CALL_DOWN, i, 1);
                commands[2][i]=1;
                //printf("Button Down floor ",i+1," Pushed\n");
            }
        }
    }
}

void send_elevator_call_signal(void){
    return;
}
