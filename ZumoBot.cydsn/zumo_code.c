/* ========================================
 *
 * Copyright YOUR COMPANY, THE YEAR
 * All Rights Reserved
 * UNPUBLISHED, LICENSED SOFTWARE.
 *
 * CONFIDENTIAL AND PROPRIETARY INFORMATION
 * WHICH IS THE PROPERTY OF your company.
 *
 * ========================================
*/

/* [] END OF FILE */
#include <project.h>
#include <stdio.h>
#include <stdlib.h>
#include "FreeRTOS.h"
#include "task.h"
#include "Motor.h"
#include "Ultra.h"
#include "Nunchuk.h"
#include "Reflectance.h"
#include "Gyro.h"
#include "Accel_magnet.h"
#include "LSM303D.h"
#include "IR.h"
#include "Beep.h"
#include "mqtt_sender.h"
#include <time.h>
#include <sys/time.h>
#include "serial1.h"
#include <unistd.h>

#if 0
//Line Following code
void zmain(void)
{
    printf("\nStart\n");
   
    IR_Start();    // enable IR receive controller 
    struct sensors_ dig;
    int lines =0; // for counting number of line 
    bool loop = true, startline= true ; // loop in main while loop for breaking 
    int turn= 0;  // Which side it turn last time 
    TickType_t start, end, tot_time = 0; // Variable for sating, ending , total time 
    
    motor_start();  // enable motor controller 
    IR_flush();     // clear IR receive buffer
    
    reflectance_start(); // Start reflectance sensor controller
    reflectance_set_threshold(9000, 9000, 11000, 11000, 9000, 9000); // set center sensor threshold to 11000 and others to 9000

    while (true){
        lines =0;
        while(SW1_Read() == 1) { // Wait for user button press
            vTaskDelay(5);
        }
        print_mqtt("Zumo038/Ready ", " Line");  // Print Mqtt at ready
        while(startline){  // moving foward to starting line
            reflectance_digital(&dig); 
            motor_turn(50,50,0);           // motor forward
            if(dig.l3 == 1 && dig.l2 == 1 && dig.l1 == 1 && dig.r1 == 1 && dig.r2 == 1 && dig.r3 == 1){
                motor_forward(0,0);        // Stop motors
                startline = false;         // break while lopp
            }
        }
        
        IR_wait();      // wait for IR command
        start = xTaskGetTickCount(); // Satrt Tick Time count 
        print_mqtt("Zumo038/Start", "%d", start); // Print Mqtt at start
        
        
        while(loop)  // Start line follwing action
        {   
            reflectance_digital(&dig); 
            // Counting the lines when its all black
            if(dig.l3 == 1 && dig.r3 == 1 && dig.r3 == 1 && dig.l2 == 1 && dig.r2 == 1 && dig.l1 == 1 && dig.r1 == 1){ 
                lines++;
                reflectance_digital(&dig); 
                while(dig.l3 == 1 && dig.r3 == 1 && dig.r3 == 1 && dig.l2 == 1 && dig.r2 == 1 && dig.l1 == 1 && dig.r1 == 1){
                        motor_turn(50,50,0);           // motor forward
                        reflectance_digital(&dig); 
                    }
                // Stop at line number 3
                if(lines == 3){
                    motor_forward(0,0);        // Stop motors
                    loop = false;         // break while lopp
                    motor_stop();
                    end = xTaskGetTickCount(); // End Tick Time count 
                    tot_time = end - start;
                    print_mqtt("Zumo038/stop", "  %d ", end);
                    print_mqtt("Zumo038/time", "  %d ", tot_time);
                }
                
            }
            // if exiting the line see last cordinate 
            else if(dig.l3 == 0 && dig.r3 == 0 && dig.r3 == 0 && dig.l2 == 0 && dig.r2 == 0 && dig.l1 == 0 && dig.r1 == 0){
                end = xTaskGetTickCount(); // End Tick Time count 
                print_mqtt("Zumo038/miss", "  %d ", end);
                reflectance_digital(&dig); 
                while(turn == 1 && dig.r2 == 0){ // check last cordinate and turn 
                    reflectance_digital(&dig); 
                    motor_turn(0,150,0);       //  Hard turn right
                }// motor forward
                while(turn == 2 && dig.l1 == 0){ // check last cordinate and turn 
                    reflectance_digital(&dig); 
                    motor_turn(150,0,0);       // Hard turn left     
                }// motor forward
                end = xTaskGetTickCount(); // End Tick Time count 
                print_mqtt("Zumo038/line", "  %d ", end);
                        
            }
           
            else if((dig.l3 == 1 && dig.l2 == 1) || dig.l3 == 1 || (dig.l2 == 1 && dig.l1 == 0)){  
                reflectance_digital(&dig); 
                motor_turn(30,255,0);       //  Hard turn right
                turn = 1;
            }
            else if((dig.r3 == 1 && dig.r2 == 1) || dig.r3 == 1 || (dig.r1 == 0 && dig.r2 == 1)){
                reflectance_digital(&dig); 
                motor_turn(255,30,0);       // Hard turn left
                turn = 2;
            }
            else if(dig.r1 == 1 || dig.l1 == 1){
                reflectance_digital(&dig); 
                motor_turn(255,250,0);       // turn left 
                turn = 0;
            }
        }    
    }
}
    
   
#endif



#if 0
// Zumo code
void zmain(void)
{
    struct accData_ data;
    struct sensors_ dig;
    LSM303D_Start(); // enable acce controller 
    Ultra_Start(); // enable Ultra sencor controller 
    bool startline= true;  // Variable for breaking while loop that moving to starting line 
    TickType_t start, end, time = 0; // Variable for sating, ending , total time 
    
    reflectance_start();
    reflectance_set_threshold(11000, 11000, 13000, 13000, 11000, 11000); // set center sensor threshold to 11000 and others to 9000
     
    motor_start();  // enable motor controller 
    IR_flush();     // clear IR receive buffer
    IR_Start();     // enable IR receive controller
    
    // Wait for unser button to start the zumo  
    while (SW1_Read() == 1) {
        vTaskDelay(5);
    }
    
    // Find Starting line
    while(startline){
        reflectance_digital(&dig); 
        motor_turn(50,50,0);           // moving forward
        if(dig.l3 == 1 && dig.l2 == 1 && dig.l1 == 1 && dig.r1 == 1 && dig.r2 == 1 && dig.r3 == 1){
            motor_forward(0,0);        // Stop motors
            startline = false;         // break while lopp
        }
    }
  
    print_mqtt("Zumo038/Ready ", "Zumo"); // Print Mqtt at ready
    IR_wait();
    
    // Satrt action 
    motor_forward(100, 200);
   
    start = xTaskGetTickCount(); // Satrt Tick Time count 
    print_mqtt("Zumo038/start", "%d", start);  // Print Mqtt at start
    
    // Start loop 
    while (true) {
        motor_forward(100, 0); // moving forward
        LSM303D_Read_Acc(&data); // recive accelation meter reading 
     
        reflectance_digital(&dig);   // recive reflectance reading 
       
        if (Ultra_GetDistance() < 10) {
            reflectance_digital(&dig);
            start = xTaskGetTickCount(); // Run Tick Time count
            while (Ultra_GetDistance() < 10 && dig.l1 == 0 && dig.l2 == 0 && dig.l3 == 0 && dig.r1 == 0 && dig.r2 == 0 && dig.r3 == 0){
                end = xTaskGetTickCount(); // Run Tick Time count
                if((start-end) <= 50){  // set speed for certain amount of time 
                    motor_forward(255,0);
                }
                else{
                    motor_forward(50,0); 
                    if((start-end) >= 100){
                        start = xTaskGetTickCount(); // Run Tick Time count
                    }
                }
                reflectance_digital(&dig); // recive reflectance reading 
            }
        } 
        // When robot finds the black line 180 turn
        if (dig.l1 == 1 || dig.l2 == 1 || dig.l3 == 1||dig.r1 == 1 || dig.r2 == 1 || dig.r3 == 1) {
            
            motor_backward(100, 400); // moving backward 
            motor_forward(0,0); // stop moving  
            motor_turn_right(200); // turn 180 degree 
            vTaskDelay(500);
            motor_forward(100,0);  // moving forward  
            
            
        }
        
        // When robot get hits by other robot
        if((data.accX > 12000 || data.accY > 12000 || data.accX < -12000 || data.accY < -12000) && dig.l1 == 0 && dig.l2 == 0 && dig.l3 == 0 && dig.r1 == 0 && dig.r2 == 0 && dig.r3 == 0) {
            end = xTaskGetTickCount(); // End Tick Time count

            motor_forward(0, 0);
            bool decision = (rand() > .5);  // get random true or false value
            if(decision == true){ // if random value is true then 
                motor_backward(100, 400);
                motor_turn_right(200);
                vTaskDelay(250);
            }
            else  // if random value is false then 
            {
                motor_backward(100, 400);
                motor_turn_left(200);
                vTaskDelay(250);
            }
            
            motor_forward(0,0);
            motor_forward(100,0); 
            print_mqtt("Zumo038/hit", "  %d ", end);  // When its get hits by other robot print time
            double hit_x = 0;
            if(data.accY >0 && data.accX >0){ //  x,y plus
                hit_x = atan(data.accX/data.accY) * (180/M_PI);
            }
            if(data.accY <0 && data.accX >0){ //  x plus ,y minus
                hit_x = (atan(data.accX/data.accY) * (180/M_PI))+180;
            }
            if(data.accY <0 && data.accX <0){ //  x minus ,y minus
                hit_x = (atan(data.accX/data.accY) * (180/M_PI))+180;
            }
            if(data.accY >0 && data.accX <0){ //  x minus ,y plus
                hit_x = (atan(data.accX/data.accY) * (180/M_PI))+360;
            }
            
            if(hit_x != 0 && hit_x != 90 && hit_x <360){ 
                print_mqtt("Zumo038/angle ", "  %.0f ", hit_x);
            }
            // When robot hits 
            LSM303D_Read_Acc(&data);
        }
        reflectance_digital(&dig);
        
        // Wait for user button stop the function 
        if (SW1_Read() == 0) {
            motor_stop();
            end = xTaskGetTickCount(); // End Tick Time count 
            time = end - start;
            print_mqtt("Zumo038/stop", "  %d ", end); // Print mqtt stop time 
            print_mqtt("Zumo038/time", "  %d ", time); // Print mqtt total time 
            while (SW1_Read() == 0)  vTaskDelay(5); // while delay until user button press againg
        }
        
    }
}
   
#endif

#if 1
//Maze Code 
void turn_right();  // for sharp 90 degree turn for right side 
void turn_right_180(); // for sharp 180 degree turn for right side 
void turn_left(); // for sharp 90 degree turn for left side 
void turn_left_180(); // for sharp 180 degree turn for left side 

void zmain(void)
{
   
    
    
    struct sensors_ dig;
    Ultra_Start();  // enable Ultra sencor controller 
    motor_start(); // enable motor controller
    IR_Start();  // enable IR controller
    reflectance_start();  // enable reflectance controller 
    bool startline= true; // Variable for breaking while loop that moving to starting line 
    int left = 0 , right = 0; //  Variable for decide which side the robot is  
    TickType_t start, end, time = 0; // Variable for sating, ending , total time 
    int lines=-1, cordi=0;  //  Variable for counting lines and finding crdinate of robot 
    
    reflectance_set_threshold(9000, 9000, 11000, 11000, 9000, 9000); // set center sensor threshold to 11000 and others to 9000
    lines =0;
    while(SW1_Read() == 1) {
        vTaskDelay(5);
    }
    print_mqtt("Zumo038/Ready", " Maze");
    
    while(startline){ // Moving to Starting line
        reflectance_digital(&dig);  // recive reflectance reading 
        motor_turn(50,50,0);           // motor forward
        if(dig.l3 == 1 && dig.l2 == 1 && dig.l1 == 1 && dig.r1 == 1 && dig.r2 == 1 && dig.r3 == 1){ //in sensor sees black
            motor_forward(0,0);        // set speed to zero to stop motors
            startline = false;         // break while lopp
        }
    }
        
    IR_wait();  // wait for IR command
    start =xTaskGetTickCount();  // Start tick time for total run
    print_mqtt("Zumo038/Start", " %d",start); // Print mqtt start
    
    while(true)
    {   
        reflectance_digital(&dig);
        if(dig.l3 == 1 && dig.r3 == 1 && dig.r2 == 1 && dig.l2 == 1){    // if reading is black  
            
            if(left == false && right == false){ // if robot pointing north (not pointing left or right)
                lines++;  //count number of lines  
                if(lines >=0){
                    print_mqtt("Zumo038/position ", " %d %d",cordi,lines); // Print mqtt 
                }
            }
            reflectance_digital(&dig); 
            while(dig.l3 == 1 && dig.r3 == 1 && dig.r3 == 1 && dig.l2 == 1 && dig.r2 == 1 && dig.l1 == 1 && dig.r1 == 1){
                motor_turn(50,48,0);           // motor forward
                reflectance_digital(&dig); 
            }
            
            if(Ultra_GetDistance() < 20 && left == false && right == false){  // if object les than 20 and direction is looking final line point 
                bool decision = (rand() > .5); // rondom true or false  
                if(decision == true){
                    vTaskDelay(250);
                    turn_left();  // turn left side 90
                    right = true;
                    if(Ultra_GetDistance() < 20){ // when it turn left side and sees object infront of it (less than 20)
                         if(left == false && right == true){
                            turn_left_180(); // then turn 180 
                            left = true;
                            right = false;
                        }
                    }   
                }
                else{
                    vTaskDelay(250);
                    turn_right(); // turn left side 90
                    left = true;
                    if(Ultra_GetDistance() < 20){// when it turn right side and sees object infront of it (less than 20)
                         if(left == true && right == false){
                            turn_right_180(); // then turn 180 
                            left = false;
                            right = true;
                        }
                    }
                }
            }
            else{
                if(left == true){ // when robot point left direction <--
                    vTaskDelay(250);
                    turn_left(); // then turn left direction ^
                    left = false;
                    cordi--;
                    if (Ultra_GetDistance() < 20) {// when it turn left side and sees object infront of it (less than 20)
                        left = true; // then left direction is true ^
                        turn_right();  // then turn right direction -->
                    }
                }
                else if(right == true){  // when robot point right direction -->
                    
                    vTaskDelay(250);
                    turn_right(); // then turn right direction ^
                    right = false;
                    cordi++;
                    if (Ultra_GetDistance() < 20) {// when it turn right side and sees object infront of it (less than 20)
                        right = true;
                        turn_left(); // then turn left direction -->
                    }
                }
                
            }
        }
        else{
            reflectance_digital(&dig);
            
            if(dig.l3 == 1 && dig.l2 == 1 && dig.r3 == 0 && cordi == -3){
                if(left == false && right == false){
                    lines++;  //count number of lines  
                    if(lines >=0){
                        print_mqtt("Zumo038/position ", " %d %d",cordi,lines); // Print mqtt 
                    }
                }
                vTaskDelay(250);
                turn_right();
                left = true;
            }
            if(dig.r3 == 1 && dig.r2 == 1 && dig.l3 == 0 && cordi == 3 ){
                if(left == false && right == false){
                    lines++;  //count number of lines  
                    if(lines >=0){
                        print_mqtt("Zumo038/position ", " %d %d",cordi,lines); // Print mqtt 
                    }
                }
                vTaskDelay(250);
                turn_right();
                right = true;
            }
            
            if((dig.l3 == 1 && dig.l2 == 1) || (dig.l3 == 1 && dig.l2 == 0) || (dig.l2 == 1 && dig.l1 == 0)){
                motor_turn(0,200,0);       //  Hard turn right
                reflectance_digital(&dig);
            }
            else if((dig.r3 == 1 && dig.r2 == 1) || (dig.r3 == 1  && dig.r2 == 0) || (dig.r1 == 0 && dig.r2 == 1)){
                motor_turn(200,0,0);       // Hard turn left
                reflectance_digital(&dig); 
            }
            else if((dig.r2 == 0 && dig.l2 == 0) || (dig.r1 == 1 && dig.l1 == 1)  || (dig.r1 == 1 && dig.l1 == 0)  || (dig.r1 == 0 && dig.l1 == 1)){
                motor_turn(75,73,0);           // moving forward
                reflectance_digital(&dig); 
            }
        }
        
        if(lines >= 15){
 
            motor_turn(75,73,0);           // moving forward
            vTaskDelay(600);  // when its in final line move forward a bit
            motor_stop();
            end = xTaskGetTickCount();
            print_mqtt("Zumo038/Stop  ", " %d",end ); // print mqtt stop time 
            time = end  - start ; // counting total time 
            print_mqtt("Zumo038/time  ", " %d",time ); // print mqtt total time 
            
            /* Folowing code I added after competition*/
            while (SW1_Read() == 0)  vTaskDelay(5); // while delay until user button press againg 
        }
        
        
    }
}


void turn_right(){
// enable right 90 degree turn
    struct sensors_ dig;
    Ultra_Start();  // enable motor controller
    motor_start();  // enable motor controller
    reflectance_start();
    reflectance_set_threshold(9000, 9000, 11000, 11000, 9000, 9000); // set center sensor threshold to 11000 and others to 9000
    
    reflectance_digital(&dig);
    while(dig.r1 == 1)          // left 45 turn
    {
        motor_turn_right(200);
        reflectance_digital(&dig);
    }
    motor_forward(0,0);         // set speed to zero to stop motors
    while(dig.r1 == 0)          // left 45 turn
    {
        motor_turn_right(200);
        reflectance_digital(&dig);
    }
    motor_forward(0,0);             // set speed to zero to stop motors
}

void turn_right_180(){
// enable right 180 degree turn   
    struct sensors_ dig;
    Ultra_Start();  // enable motor controller
    motor_start();  // enable motor controller
    reflectance_start();
    reflectance_set_threshold(9000, 9000, 11000, 11000, 9000, 9000); // set center sensor threshold to 11000 and others to 9000
    
    reflectance_digital(&dig);
    while(dig.r1 == 1)              // left 45 turn
    {
        motor_turn_right(200);
        reflectance_digital(&dig);
    }
    motor_forward(0,0);        // Stop motors
    reflectance_digital(&dig);
    while(dig.r1 == 0)              // left 45 turn
    {
        motor_turn_right(200);
        reflectance_digital(&dig);
    }
    while(dig.r1 == 1)              // left 45 turn
    {
        motor_turn_right(200);
        reflectance_digital(&dig);
    }
    motor_forward(0,0);        // Stop motors
    reflectance_digital(&dig);
    while(dig.r1 == 0)          // left 45 turn
    {
        motor_turn_right(200);
        reflectance_digital(&dig);
    }
    motor_forward(0,0);        // set speed to zero to stop motors
}

void turn_left(){
    // enable left 90 degree turn   
    struct sensors_ dig;
    Ultra_Start();  // enable motor controller
    motor_start();  // enable motor controller
    reflectance_start();
    reflectance_set_threshold(9000, 9000, 11000, 11000, 9000, 9000); // set center sensor threshold to 11000 and others to 9000
    
    reflectance_digital(&dig);
    while(dig.l1 == 1)              // left 45 turn
    {
        motor_turn_left(200);
        reflectance_digital(&dig);
    }
    motor_forward(0,0);             // set speed to zero to stop motors
    while(dig.l1 == 0)              // left 45 turn
    {
        motor_turn_left(200);
        reflectance_digital(&dig);
    }
    motor_forward(0,0);             // set speed to zero to stop motors
}

void turn_left_180(){
// enable left 180 degree turn    
    struct sensors_ dig;
    Ultra_Start();  // enable motor controller
    motor_start();  // enable motor controller
    reflectance_start();
    reflectance_set_threshold(9000, 9000, 11000, 11000, 9000, 9000); // set center sensor threshold to 11000 and others to 9000
    
    reflectance_digital(&dig);
    while(dig.l1 == 1)          // left 45 turn
    {
        motor_turn_left(200);
        reflectance_digital(&dig);
    }
    motor_forward(0,0);         // set speed to zero to stop motors
    reflectance_digital(&dig);
    while(dig.l1 == 0)          // left 45 turn
    {
        motor_turn_left(200);
        reflectance_digital(&dig);
    }
    while(dig.l1 == 1)          // left 45 turn
    {
        motor_turn_left(200);
        reflectance_digital(&dig);
    }
    motor_forward(0,0);         // Stop motors
    reflectance_digital(&dig);
    while(dig.l1 == 0)          // left 45 turn
    {
        motor_turn_left(200);
        reflectance_digital(&dig);
    }
    motor_forward(0,0);         // set speed to zero to stop motors
}
#endif
