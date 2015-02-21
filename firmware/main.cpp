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
 
TextLCD lcd(p15, p16, p21, p22, p23, p24); // rs, e, d4-d7





float LAT= 40.86;
float LONG=14.17;
float ALT=100;



 
TMP102 temperature(p9,p10, 0x90); 
DigitalOut l1(LED1);
DigitalOut l2(LED2);
DigitalOut l3(LED3);
DigitalOut l4(LED4);

float temp=0;
char command[1024];
char s[1024];
char wk[5];

string id = "***************";  //username:password  with base64

TCPSocketConnection sock;

void send_temp(void const *args) {
    while (true) {
        l3=1;
        sock.connect("openweathermap.org",80);
        printf("socket connesso\r\n");
        int len=0;
        len=sprintf(s,"temp=%f&lat=%2f&long=%f&alt=%f",temp,LAT,LONG,ALT);
        printf("%s",s);
        sprintf(wk,"%d",len);
        string http_cmd = "POST /data/post HTTP/1.1\r\nAuthorization: Basic "+id+"\r\nHost: openweathermap.org\r\nAccept: */*\r\nContent-Length: "+string(wk)+"\r\nContent-Type: application/x-www-form-urlencoded\r\n\r\n"+string(s);
        int n = sprintf(command,http_cmd.c_str());
        printf("%s",command);
        printf("\r\n%d\r\n",n);
        sock.send_all(command, n);
        sock.receive(command,1024);
        l3=0;
        printf("\r\n\r\n%s\r\n\r\n",command);
        wait(19.9); 
        sock.close();
        Thread::wait(20000);
        
    }
}

void update_screen(void const *args) {
    while (true) {
        l2=1;
        temp=temperature.read();
        printf("temp: %f\r\n",temp);
        wait(0.1); 
        l2=0;
        lcd.cls();
        lcd.printf("%3.3f C\nNalug Weather",temp);
        Thread::wait(5000);
    }
}



int main() {
    l1=0;
    l2=0;
    l3=0;
    l4=0;
    l1=1;
    lcd.printf("Open Data Day 15Nalug Weather");
    EthernetInterface eth;
    eth.init(); //Use DHCP
    eth.connect();
    printf("IP Address is %s\r\n", eth.getIPAddress());
    l1=0;
    l4=1;
    
    Thread thread(send_temp);
    Thread thread2(update_screen);
    
    while(1){
    }
    eth.disconnect();
}
