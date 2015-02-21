/*   www.nalug.net
 *   www.linuxdaynapoli.org
 *   www.gbr1technologies.altervista.org
 *
 *	 Open Data Day Napoli 2015
 *
 *   Giovanni di Dio Bruno ;)
 *
 *
 */



#include "mbed.h"
#include "EthernetInterface.h"
#include "TMP102.h"
#include <string>
#include <cstring>
#include "TextLCD.h"
 
 
//lcd pinout 
TextLCD lcd(p15, p16, p21, p22, p23, p24); // rs, e, d4-d7




//your latitude, longitude and altitude
float LAT= 40.86;
float LONG=14.17;
float ALT=100;



//cheap but powerfull temperature sensor
TMP102 temperature(p9,p10, 0x90); 

DigitalOut l1(LED1); //ip OK
DigitalOut l2(LED2); //temp read
DigitalOut l3(LED3); //openweathermap connction
DigitalOut l4(LED4); //working

float temp=0;        //temperature
char command[1024];  //string for http post
char s[1024];
char wk[5];

string id = "***************";  //username:password  with base64

TCPSocketConnection sock;


//thread to send temperature to openweathermap.org  -> every 20s
void send_temp(void const *args) {
    while (true) {
        l3=1;  //active led 3
        sock.connect("openweathermap.org",80);    //open socket connection
        printf("socket connesso\r\n");
        int len=0;
        len=sprintf(s,"temp=%f&lat=%2f&long=%f&alt=%f",temp,LAT,LONG,ALT);   //create data to send to server and his length
        printf("%s",s);
        sprintf(wk,"%d",len);    //convert int to char[]
        //complete command to do http post to openweathermap.org server
        string http_cmd = "POST /data/post HTTP/1.1\r\nAuthorization: Basic "+id+"\r\nHost: openweathermap.org\r\nAccept: */*\r\nContent-Length: "+string(wk)+"\r\nContent-Type: application/x-www-form-urlencoded\r\n\r\n"+string(s);
        int n = sprintf(command,http_cmd.c_str());  //string to char[] and its length
        printf("%s",command);
        printf("\r\n%d\r\n",n);
        //send
        sock.send_all(command, n);
        //receive response from openweeathermap server 
        sock.receive(command,1024);
        l3=0;
        printf("\r\n\r\n%s\r\n\r\n",command);
        //close socket
        sock.close();
        //thread every 20s
        Thread::wait(20000);
        
    }
}


//thread for updating screen
void update_screen(void const *args) {
    while (true) {
        l2=1;      //update temperature
        temp=temperature.read();
        printf("temp: %f\r\n",temp);
        wait(0.1); 
        l2=0;
        //lcd print
        lcd.cls();
        lcd.printf("%3.3f C\nNalug Weather",temp);
        //thread every 5s
        Thread::wait(5000);
    }
}



int main() {
    //init leds
    l1=0;
    l2=0;
    l3=0;
    l4=0;
    
    //getting ip and ethernet connection
    l1=1;
    lcd.printf("Open Data Day 15Nalug Weather");
    EthernetInterface eth;
    eth.init(); //Use DHCP
    eth.connect();
    printf("IP Address is %s\r\n", eth.getIPAddress());
    l1=0;
    l4=1;
    
    //starting threads
    Thread thread(send_temp);
    Thread thread2(update_screen);
    
    while(1){
    }
    
    //close ethernet
    eth.disconnect();
}
