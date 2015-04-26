/**
Set floor lamps
*/
void set_elev_floor_lamp(void); 

/**
Run once!
*/
void initialize(void);

/**
Listen to the IO panel
*/
void listen_to_io_panels(void);

/**
Stop the motor and open-close the doors this also resets the command array
*/

void stop_and_open(void);

/**
Turn off light at designated floor
*/
void turn_off_lights_at_floor (int i, int dir);

/**
Move the elevator and it will stop at any floor that has a stop flag -1 down 1 up 0 stop.
*/
void move_elevator(int dir);

/**
Execute orders from the array
*/
void execute_orders(void);

/**
returns 1 if door is open 0 if door is closed
*/
int is_door_open(double open_time);

void send_location(int stop);

void send_button(int floor, int dir);