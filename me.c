/**********************************************************************
* Filename    : I2CLCD1602.c
* Description : Use the LCD display data
* Author      : www.freenove.com
* modification: 2020/07/23
**********************************************************************/
#include <stdlib.h>
#include <stdio.h>
#include <wiringPi.h>
#include <wiringPiI2C.h>
#include <pcf8574.h>
#include <lcd.h>
#include <time.h>
#include <sys/time.h>

int pcf8574_address = 0x27;        // PCF8574T:0x27, PCF8574AT:0x3F
#define BASE 64         // BASE any number above 64
//Define the output pins of the PCF8574, which are directly connected to the LCD1602 pin.
#define RS      BASE+0
#define RW      BASE+1
#define EN      BASE+2
#define LED     BASE+3
#define D4      BASE+4
#define D5      BASE+5
#define D6      BASE+6
#define D7      BASE+7

char nums[] = "0123456789ab";

int lcdhd;// used to handle LCD
void printDataTime(){//used to print system time 

    struct timeval tv;
    struct timezone tz;
    gettimeofday(&tv,&tz);
    
    time_t rawtime;
    struct tm *timeinfo;
    time(&rawtime);// get system time
    timeinfo = localtime(&rawtime);//convert to local time
    
    float daySecs = timeinfo->tm_hour * 3600 + timeinfo->tm_min * 60 + timeinfo->tm_sec + (tv.tv_usec / 1000000.);
    float dayFrac = daySecs / 3600. / 24.;
    int bigSecs = dayFrac * 12 * 12 * 12 * 12;
    
    // now set string
    char disp[] = "0000";
    disp[3] = nums[bigSecs % 12];
    bigSecs /= 12;
    disp[2] = nums[bigSecs % 12];
    bigSecs /= 12;
    disp[1] = nums[bigSecs % 12];
    bigSecs /= 12;
    disp[0] = nums[bigSecs % 12];
    
    
    lcdPosition(lcdhd,0,0);
    lcdPrintf(lcdhd, "Time: %s", disp);
    // lcdPosition(lcdhd,0,1);// set the LCD cursor position to (0,1) 
    // lcdPrintf(lcdhd,"%02d:%02d:%02d",timeinfo->tm_hour,timeinfo->tm_min,timeinfo->tm_sec); //Display system time on LCD
}
int detectI2C(int addr){
    int _fd = wiringPiI2CSetup (addr);   
    if (_fd < 0){		
        printf("Error address : 0x%x \n",addr);
        return 0 ;
    } 
    else{	
        if(wiringPiI2CWrite(_fd,0) < 0){
            printf("Not found device in address 0x%x \n",addr);
            return 0;
        }
        else{
            printf("Found device in address 0x%x \n",addr);
            return 1 ;
        }
    }
}
int main(void){
    int i;

    printf("Program is starting ...\n");

    wiringPiSetup();  
    if(detectI2C(0x27)){
        pcf8574_address = 0x27;
    }else if(detectI2C(0x3F)){
        pcf8574_address = 0x3F;
    }else{
        printf("No correct I2C address found, \n"
        "Please use command 'i2cdetect -y 1' to check the I2C address! \n"
        "Program Exit. \n");
        return -1;
    }
    pcf8574Setup(BASE,pcf8574_address);//initialize PCF8574
    for(i=0;i<8;i++){
        pinMode(BASE+i,OUTPUT);     //set PCF8574 port to output mode
    } 
    digitalWrite(LED,HIGH);     //turn on LCD backlight
    digitalWrite(RW,LOW);       //allow writing to LCD
	lcdhd = lcdInit(2,16,4,RS,EN,D4,D5,D6,D7,0,0,0,0);// initialize LCD and return “handle” used to handle LCD
    if(lcdhd == -1){
        printf("lcdInit failed !");
        return 1;
    }
    while(1){
        printDataTime();        // print system time
        delay(50);
    }
    return 0;
}

