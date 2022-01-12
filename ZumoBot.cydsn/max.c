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
// Name and age
int CallFunctionThatDoesSomething();
void zmain(void)
{
    printf("\nStarts\n");
    TickType_t start;
    TickType_t end;
    int age,total_time;
    start = xTaskGetTickCount();
    age = CallFunctionThatDoesSomething();
    end = xTaskGetTickCount();
    total_time = (end -start)/1000;
  
    
   
       // BatteryLed_Write(!SW1_Read());
           if(total_time<3){
        if(age<21){
            printf("Super fast dude!\n");
           }
        else if (age >= 22 && age <=50 )
        {
             printf("Be quick or be dead\n");
        }
        else
        {
             printf("Still going strong\n");
        }
       }
    else if (total_time >= 3 && total_time <=5 )
    {
         if(age<21){
            printf("So mediocre.\n");
           }
        else if (age >= 22 && age <=50 )
        {
             printf("You\'re so average.\n");
        }
        else
        {
             printf("You are doing ok for your age.\n");
        }
    }
    else
    {
         if(age<21){
            printf("My granny is faster than you!\n");
           }
        else if (age >= 22 && age <=50 )
        {
             printf("Have you been smoking something illegal?\n");
        }
        else
        {
             printf("Do they still allow you to drive?\n");
        }
    }
  
    printf("Execution time: %d\n", total_time);
    printf("\nEnd\n");
 }

int CallFunctionThatDoesSomething() {
    int age;
    
    printf("Enter your age: ");
    scanf("%d", &age);
    return age;
}
#endif





#if 0
//ultrasonic sensor//
void zmain(void)
{
    
    vTaskDelay(3000);
    Ultra_Start(); 
    // Ultra Sonic Start function
    motor_start();              // enable motor controller 
    
    while(true) {
        
       int d = Ultra_GetDistance();
        // Print the detected distance (centimeters)
       if(d<10 && d!=0){
            Beep(300, 150);  
            printf("distance = %d\r\n", d);
            motor_backward(100,500);    // moving backward
            motor_turn(100,5,500);     // turn left
        }
        motor_forward(50,50);         // stop motors
        
        
       // motor_forward(0,0);         // stop motors
       // motor_stop();               // disable motor controller
        vTaskDelay(0);
    }
}   
#endif

#if 1
//IR receiverm - how to wait for IR remote commands
void zmain(void)
{
    printf("\nStart\n");
   
    IR_Start();
    struct sensors_ dig;
    int lines =0;
    bool loop = true, startline= true;
    
    motor_start();  // enable motor controller 
    IR_flush();     // clear IR receive buffer
    
    reflectance_start();
    reflectance_set_threshold(9000, 9000, 11000, 11000, 9000, 9000); // set center sensor threshold to 11000 and others to 9000

    while (true){
       
        lines =0;
        while(SW1_Read() == 1) {
            vTaskDelay(5);
        }
        
        while(startline){
            reflectance_digital(&dig); 
            motor_turn(50,50,0);           // motor forward
            if(dig.l3 == 1 && dig.l2 == 1 && dig.l1 == 1 && dig.r1 == 1 && dig.r2 == 1 && dig.r3 == 1){
                motor_forward(0,0);        // Stop motors
                startline = false;         // break while lopp
            }
        }
        while(SW1_Read() == 1) {
            vTaskDelay(5);
        }
        /* IR_wait();  // wait for IR command
        led = !led;
        BatteryLed_Write(led);   
        */

        while(loop)
        {   
            reflectance_digital(&dig); 
            /*if(dig.l3 == 1 && dig.r3 == 1){ 
                lines++;
                if(lines ==2){
                    motor_forward(0,0);        // Stop motors
                    loop = false;         // break while lopp
                }
                else{
                    startline = true;
                    while(startline){
                        reflectance_digital(&dig); 
                        motor_turn(50,50,0);           // motor forward
                        if(dig.l3 != 1 && dig.r3 != 1){
                            startline = false;         // break while lopp
                        }
                    }
                    motor_turn(50,50,0);       // motor forward
                    printf("%d\n",lines);
                    reflectance_digital(&dig); 
                }
                
            }
            else{*/
                while(dig.l3 == 1 && dig.r3 == 1){
                       
                        
                        if(dig.l3 != 0 && dig.r3 != 0){
                            motor_turn(50,50,0);           // motor forward
                        }
                        reflectance_digital(&dig); 
                }
                
                if((dig.l2 == 1 && dig.l1 == 1) || dig.l2 == 1|| dig.l1 == 1){
                    reflectance_digital(&dig); 
                    motor_turn(100,99,0);       // Run
                }
                else if((dig.l3 == 1 && dig.l2 == 1) || dig.l3 == 1){
                    reflectance_digital(&dig); 
                    motor_turn(0,255,0);       //  Hard turn right
                }
                else if((dig.r3 == 1 && dig.r2 == 1) || dig.r3 == 1){
                    reflectance_digital(&dig); 
                    motor_turn(255,0,0);       // Hard turn left
                }
               
                else if((dig.l2 == 1 && dig.l1 == 1) || dig.l2 == 1){
                    reflectance_digital(&dig); 
                    motor_turn(20,255,0);       //  turn right
                }
                else if((dig.r1 == 1 && dig.r2 == 1) || dig.r2 == 1){
                    reflectance_digital(&dig); 
                    motor_turn(255,20,0);       // turn left 
                }
                
              
                 else if((dig.l3 == 1 && dig.l1 == 1 && dig.r1 == 1)){
                    reflectance_digital(&dig); 
                    motor_turn(25,200,0);       //  Hard turn left
                } 
                else if((dig.r3 == 1 && dig.r1 == 1 && dig.l1 == 1)){
                    reflectance_digital(&dig); 
                    motor_turn(200,25,0);       //  Hard turn right
                } 
                
                else if(dig.r3 == 0 && dig.l1 == 0 && dig.r1 == 0 && dig.l3 == 0 ){
                    reflectance_digital(&dig); 
                    motor_forward(0,0);        // Stop motors
                }
            
        }    
     
    }
}
    
   
#endif