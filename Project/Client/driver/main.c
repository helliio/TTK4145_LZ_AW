#include "elev.h"
#include <stdio.h>

//move elevator up or down
void moveElev(int floor,int dir){
    //Do something
    return()
}

//Button pressed outside the elevator
void buttonPressedWall(){
    //Do something
    return;
}

//Button pressed inside the elevator
void buttonPressedElev(){
    //Do something
    return;
}

//Send signal to server
void sendSignal(){
    //Send this shit
    return;
}


int main() {
    // Initialize hardware
    if (!elev_init()) {
        printf("Unable to initialize elevator hardware!\n");
        return 1;
    }

    printf("Press STOP button to stop elevator and exit program.\n");

    elev_set_motor_direction(DIRN_UP);

    while (1) {
        // Change direction when we reach top/bottom floor
        if (elev_get_floor_sensor_signal() == N_FLOORS - 1) {
            elev_set_motor_direction(DIRN_DOWN);
        } else if (elev_get_floor_sensor_signal() == 0) {
            elev_set_motor_direction(DIRN_UP);
        }

        // Stop elevator and exit program if the stop button is pressed
        if (elev_get_stop_signal()) {
            elev_set_motor_direction(DIRN_STOP);
            break;
        }
    }

    return 0;
}
