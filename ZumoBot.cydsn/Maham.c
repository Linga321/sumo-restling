#if 0
//motor
void zmain(void)
{
    motor_start();              // enable motor controller
    motor_forward(0,0);         // set speed to zero to stop motors

    vTaskDelay(3000);
    
    
    motor_turn(200,199,1700);     // moving forward
    motor_turn(200,10,500);     // turn right 
    
    motor_turn(200,199,1400);  // moving forward
    motor_turn(200,5,500);     // turn right
    
    motor_turn(200,199,1400);  // moving forward
    motor_turn(200,5,500);     // turn right
    
    motor_turn(200,120,750);     // turn right
    motor_turn(200,120,750);     // turn right
    
    motor_turn(200,199,500);  // moving forward
    
    //motor_turn(198,200,500);     // turn
    //motor_turn(200,20,500);     // turn
    
   // motor_forward(100,2000);     // moving forward
   // motor_forward(100,2000);     // moving forward
    
    
    
   // motor_backward(20,2000);    // moving backward
    
    
     
    motor_forward(0,0);         // stop motors

    motor_stop();               // disable motor controller
    
    while(true)
    {
        vTaskDelay(100);
    }
}
#endif


#if 1
/* Example of how to use the Accelerometer!!!*/
void zmain(void)
{
    struct accData_ data;
    int count =0;
    vTaskDelay(3000);
    // Ultra Sonic Start function
    motor_start();              // enable motor controller 
    printf("Accelerometer test...\n");

    if(!LSM303D_Start()){
        printf("LSM303D failed to initialize!!! Program is Ending!!!\n");
        vTaskSuspend(NULL);
    }
    else {
        printf("Device Ok...\n");
    }
        
    while(true) {
       
        // Print the detected distance (centimeters)
       LSM303D_Read_Acc(&data);
       int num =0,num2 =0;
        printf("%8d %8d %8d\n",data.accX, data.accY, data.accZ);
        if(count==10){
           num2 = rand() % 5;
           count =0;
        }
       if(num2 == 1){
            motor_turn(200,0,50);     // turn right
            count++;
        }
        else if(num2 == 2){
            motor_turn(0,200,50);     // turn left
            count++;
        }
        else {
           motor_forward(100,20);         // run motors
           count++;
        }
        
        
       if(data.accX <= -3000){
            
            num = rand();
            if(num%2 ==0){
                motor_backward(100,200);    // moving backward
                motor_turn(150,0,500);     // turn left
            }
            else{
                motor_backward(100,200);    // moving backward
                motor_turn(0,150,500);     // turn left
            }
            Beep(300, 150);  
    }
       
       
       
    }
 }   
#endif

 #if 1
//IR receiver - how to wait for IR remote commands
void zmain(void)
{
    uint8_t button_;
    printf("\nStart\n");
    
    while(true){
        button_ = SW1_Read();
        if(button_==0){
            IR_Start();
            printf("\n\nIR test\n");
            struct sensors_ ref;
            struct sensors_ dig;
            bool led = false,loop = true, startline= true;
            int count =0;
            motor_start();              // enable motor controller 
            IR_flush(); // clear IR receive buffer
            printf("Buffer cleared\n");
            
            reflectance_start();
            reflectance_set_threshold(9000, 9000, 11000, 11000, 9000, 9000); // set center sensor threshold to 11000 and others to 9000
            vTaskDelay(200);
                while(startline){
                    // read raw sensor values
                    reflectance_read(&ref);
                    reflectance_digital(&dig); 
                    if(dig.l3 != 1 && dig.r3 != 1){
                        motor_turn(50,50,50);       // motor forward
                    }
                    else{
                        motor_forward(0,0);       // Stop motors
                        startline = false;
                    }
                }
            IR_wait();  // wait for IR command
            led = !led;
            BatteryLed_Write(led);   
            
            // Toggle led when IR signal is received
            while(loop)
            {   
                if(led){
                    // read raw sensor values
                    reflectance_read(&ref);
                    reflectance_digital(&dig); 
                    
                    if(dig.l3 == 1 && dig.r3 == 1 ){
                        motor_turn(50,50,50);       // motor forward
                        vTaskDelay(50);
                        count++;
                        printf("count %d \n",count);
                        if(count >= 7){
                            motor_forward(0,0);       // Stop motors
                            loop = false;
                        }
                    }
                    else{
                         motor_turn(50,50,50);       // motor forward
                    }
                }
                else {
                    printf("Led is OFF\n");
                    loop = false;
                }
               
            }    
        }
    }
 }   
#endif 

