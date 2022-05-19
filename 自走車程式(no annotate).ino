#include<ESP8266WiFi.h>
#include<ESP8266WebServer.h>
#include<Ultrasonic.h>
const char *ssid="max2124",*pas="ji394xji6ufu3";
ESP8266WebServer esp8266_server(80);
int D0=16, D1=5, D2=4, D3=0, D4=2, D5=14, D6=12, D7=13, D8=15, D9=3, D10=1;
int RF=D8, RB=D7, LF=D4, LB=D3;
int RL=D1,RR=D2;
#define detm 500
#define dis 50
Ultrasonic ultrasonic(D5, D6);
bool bF=true,bB=true,Cbb=false,Cbf=false,Cstop=false;
bool ult=true;
const int forward[4]={HIGH,LOW,HIGH,LOW},back[4]={LOW,HIGH,LOW,HIGH},rightf[4]={LOW,LOW,HIGH,LOW},rightb[4]={LOW,LOW,LOW,HIGH},leftf[4]={HIGH,LOW,LOW,LOW},leftb[4]={LOW,HIGH,LOW,LOW},STOP[4]={LOW,LOW,LOW,LOW};
const int pin[4]={RF,RB,LF,LB};
void Root()
{
  String hda;
  hda="<!doctype html><html lang='zh-TW'><head><meta charset='UTF-8'><meta name='viewport' content='width=device-width; initial-scale=1.0;'></head>"; 
  hda+="<body><p>當前狀態：";
  int now[4];
  for(int q=0;q<4;q++)
  now[q]=digitalRead(pin[q]);
  if(now==forward) hda+="前進<br>";
  else if(now==back) hda+="後退<br>"; 
  else if(now==rightf) hda+="前進右轉<br>";
  else if(now==rightb) hda+="後退右轉<br>";
  else if(now==leftf) hda+="前進左轉<br>";
  else if(now==leftb) hda+="後退左轉<br>";
  else hda+="停止<br>";
  hda+="超音波避障：";
  if(ult) hda+="開啟<br>";
  else hda=+"關閉<br>";
  hda+="</p></body>";
  hda+="<form action='Forward' method='post'>"; 
  hda+="<input type='submit' value='前進'>";
  hda+="</form>"; 
  hda+="<form action='Back' method='post'>";
  hda+="<input type='submit' value='後退'>";
  hda+="</form>";
  hda+="<form action='LeftF' method='post'>";
  hda+="<input type='submit' value='向前左轉'>";
  hda+="</form>";
  hda+="<form action='LeftB' method='post'>";
  hda+="<input type='submit' value='向後左轉'>";
  hda+="</form>";
  hda+="<form action='RightF' method='post'>";
  hda+="<input type='submit' value='向前右轉'>";
  hda+="</form>";
  hda+="<form action='RightB' method='post'>";
  hda+="<input type='submit' value='向後右轉'>";
  hda+="</form>";
  hda+="<form action='Stop' method='post'>";
  hda+="<input type='submit' value='停止'>";
  hda+="</form>";
  hda+="<form action='startult' method='post'>";
  hda+="<input type='submit' value='開啟避障'>";
  hda+="</form>";
  hda+="<form action='stopult' method='post'>";
  hda+="<input type='submit' value='關閉避障'>";
  hda+="</form>";
  hda+="</html>"; 
  esp8266_server.send(200,"text/html",hda);
}
void NotFound()
{
  esp8266_server.send(404,"text/plain","404 : Not found");
}
void situation(const int sta[])
{
  for(int q=0;q<4;q++)
  digitalWrite(pin[q],sta[q]);  
}
void Forward()
{
  situation(forward);
  delay(detm);
  esp8266_server.send(200,"text/html","<head><script>history.back()</script></head>");   
} 
void Back()
{
  situation(back);
  delay(detm);
  esp8266_server.send(200,"text/html","<head><script>history.back()</script></head>"); 
}
void RightF()
{
  situation(rightf);
  delay(detm);
  esp8266_server.send(200,"text/html","<head><script>history.back()</script></head>");   
}
void LeftF()
{
  situation(leftf);
  delay(detm);
  esp8266_server.send(200,"text/html","<head><script>history.back()</script></head>");  
}
void RightB()
{
  situation(rightb);
  delay(detm);
  esp8266_server.send(200,"text/html","<head><script>history.back()</script></head>"); 
}
void LeftB()
{
  situation(leftb);
  delay(detm);
  esp8266_server.send(200,"text/html","<head><script>history.back()</script></head>"); 
} 
void Stop()
{
  situation(STOP);
  delay(detm);
  esp8266_server.send(200,"text/html","<head><script>history.back()</script></head>"); 
}
void startult()
{
  ult=true;
  esp8266_server.send(200,"text/html","<head><script>history.back()</script></head>");   //回上一頁
}
void stopult()
{
  ult=false;
  esp8266_server.send(200,"text/html","<head><script>history.back()</script></head>");   //回上一頁
}
void setup() 
{
  Serial.begin(9600);
  WiFi.disconnect(1);
  WiFi.mode(WIFI_AP);
  Serial.print("開始連接");
  WiFi.softAP(ssid,pas);
  Serial.println(ssid);
  Serial.print("IP address: "); 
  Serial.println(WiFi.softAPIP());
  pinMode(RR,INPUT);
  pinMode(RL,INPUT);
  pinMode(RF,OUTPUT);
  pinMode(RB,OUTPUT);
  pinMode(LF,OUTPUT);
  pinMode(LB,OUTPUT);
  pinMode(D9,OUTPUT);
  pinMode(D10,OUTPUT);
  analogWrite(D9,512);
  analogWrite(D10,512);
  esp8266_server.begin();
  esp8266_server.on("/",Root);
  esp8266_server.on("/Forward", HTTP_POST, Forward);  
  esp8266_server.on("/Back", HTTP_POST, Back);  
  esp8266_server.on("/RightF", HTTP_POST, RightF);  
  esp8266_server.on("/LeftF", HTTP_POST, LeftF);
  esp8266_server.on("/RightB", HTTP_POST, RightB);
  esp8266_server.on("/LeftB", HTTP_POST, LeftB);
  esp8266_server.on("/Stop", HTTP_POST, Stop);
  digitalWrite(RF,LOW);
  digitalWrite(RB,LOW);
  digitalWrite(LF,LOW);
  digitalWrite(LB,LOW);
}
void loop() 
{
  if(ult)
    {
       if(ultrasonic.distanceRead() <= dis)
       {
          Stop();
          while(ultrasonic.distanceRead() <= dis) LeftB();
       } 
    }
  bool stateRL=digitalRead(RL),stateRR=digitalRead(RR); 
  if(stateRL and stateRR){
      Stop();
      while(stateRL and stateRR) Back();
      for(int q=0;q<8;q++) LeftF();
    }
  else if(!stateRR && stateRL){ 
    Stop();
    while(stateRL and stateRR) Back();
    for(int q=0;q<3;q++) LeftF();
  }
  else if(stateRR && !stateRL){  
    Stop();
    while(stateRL and stateRR) Back();
    for(int q=0;q<3;q++) RightF();
  }
  if(bF) Forward();
  else if(bB) Back();
  esp8266_server.handleClient();
}
