#include<ESP8266WiFi.h>
#include<ESP8266WebServer.h>
#include<Ultrasonic.h>
const char *ssid="max2124",*pas="ji394xji6ufu3"; //ssid=WIFI名稱。pas=WIFI密碼。const=恆定，代表變數(ssid,pas)不可被更改
ESP8266WebServer esp8266_server(80); //ESP8266WebServer庫用於HTTP協議通訊。通過ESP8266WebServer庫，我們可以使用ESP8266開發板建立網絡服務器,從而允許其它網絡設備以及物聯網設備通過HTTP協議訪問並實現物聯網信息交流。 來源：http://www.taichi-maker.com/homepage/iot-development/iot-dev-reference/esp8266-c-plus-plus-reference/esp8266webserver/
int D0=16, D1=5, D2=4, D3=0, D4=2, D5=14, D6=12, D7=13, D8=15, D9=3, D10=1; //腳位定義
//int RF=D3, RB=D4, LF=D8, LB=D7; //RF=RightMotorForward，右馬達前進。RB=RightMotorBack，右馬達後退。LF=LeftMotorForward，左馬達前進。LB=LeftMotorBack，左馬達後退。
int RF=D8, RB=D7, LF=D4, LB=D3;
int RL=D1,RR=D2; // RL=右輪紅外線感測器，RL=左輪紅外線感測器
#define detm 500 //更改行動模式間的delay時間，單位為毫秒
#define dis 50 //超音波感測器感測的最短距離
Ultrasonic ultrasonic(D5, D6); // 超音波感測器的物件，其中D5的部分是控制端(trig_pin)，D6是接收端(echo_pin)
bool bF=true,bB=true,Cbb=false,Cbf=false,Cstop=false;
bool ult=true; //bB=
const int forward[4]={HIGH,LOW,HIGH,LOW},back[4]={LOW,HIGH,LOW,HIGH},rightf[4]={LOW,LOW,HIGH,LOW},rightb[4]={LOW,LOW,LOW,HIGH},leftf[4]={HIGH,LOW,LOW,LOW},leftb[4]={LOW,HIGH,LOW,LOW},STOP[4]={LOW,LOW,LOW,LOW};
const int pin[4]={RF,RB,LF,LB};
void Root()//設定html網站程式碼
{
  String hda; //hda變數儲存html程式
  hda="<!doctype html><html lang='zh-TW'><head><meta charset='UTF-8'><meta name='viewport' content='width=device-width; initial-scale=1.0;'></head>"; //設定使用者介面樣式

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
  
  hda+="<form action='Forward' method='post'>"; //form標籤是用來建立一個html表單。action: 用來指定一個位址 (URL)，這個位址是告訴瀏覽器 (browser) 當 user 按送出表單後，要將表格內的資料送去哪邊。method: 用來指定資料傳輸時用的 HTTP 協議，最常用的是 get 或 post：...post: 會將表單資料放在 HTTP 傳輸封包 body 中送出。post 通常用在表單資料量比較大、有夾帶檔案上傳 (file upload) 或隱私性考量的資料。
  hda+="<input type='submit' value='前進'>"; //input在form裡十分重要，可以建立各種表單控制元件(form control)。type表示表單元件類型，預設type="text"，type="subnit"為表單送出按鈕，會送出表單給伺服器至action。value可改變按鈕中的文字
  hda+="</form>"; //form的結束標籤。因input為空元素，故無結束標籤
  
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

//  hda+="<form action='Autogo' method='post'>";
//  hda+="<input type='submit' value='自動前進'>";
//  hda+="</form>";
//
//  hda+="<form action='Autoback' method='post'>";
//  hda+="<input type='submit' value='自動後退'>";
//  hda+="</form>";
//
//  hda+="<form action='Autostop' method='post'>";
//  hda+="<input type='submit' value='停止自動'>";
//  hda+="</form>";

  hda+="<form action='startult' method='post'>";
  hda+="<input type='submit' value='開啟避障'>";
  hda+="</form>";

  hda+="<form action='stopult' method='post'>";
  hda+="<input type='submit' value='關閉避障'>";
  hda+="</form>";
  
  hda+="</html>"; //html的結束標籤
  esp8266_server.send(200,"text/html",hda); 
/*
  ESP8266WebServer中的send函數，用作生成與發送http響應信息。也就是說，電腦瀏覽器所收到的網頁信息都是通過send函數生成並且發送的。
  send(HTTP狀態碼(responseCode),字符串參數(contentType),響應內容(responseContent))。HTTP狀態碼(HTTP Status Code)：用以表示網頁伺服器超文字傳輸協定的回應狀態的三位數代碼
  所有的HTTP狀態碼(responseCode)可分為五種：
  1. 1XX為訊息，代表請求已被接受，需要繼續處理。這類回應是臨時回應，只包含狀態行和某些可選的回應頭資訊，並以空行結束。
  2. 2XX為成功，代表請求已成功被伺服器接收、理解、並接受。ex.200=OK，代表請求已成功，請求所希望的回應頭或資料體將隨此回應返回。在POST請求中
     ，回應將包含描述或操作結果的實體。
  3. 3XX為重新導向，這類狀態碼代表需要客戶端採取進一步的操作才能完成請求。通常，這些狀態碼用來重新導向，後續的請求位址（重新導向目標）在本次
     回應的Location域中指明。
  4. 4XX為客戶端錯誤，這類的狀態碼代表了客戶端似乎發生了錯誤，妨礙了伺服器的處理。ex.404=Not Found，代表請求失敗，請求所希望得到的資源未被
     在伺服器上發現，但允許使用者的後續請求。
  5. 5XX為伺服器失誤，表示伺服器無法完成明顯有效的請求。這類狀態碼代表了伺服器在處理請求的過程中有錯誤或者異常狀態發生，也有可能是伺服器意識到
     以當前的軟硬體資源無法完成對請求的處理。
  字符串參數(contentType)用途為說明http響應的信息類型，常見的類型有六種：
  1. HTML模式：text/html
  2. 純文本模式：text/plain
  3. XML模式：text/xml
  4. gif圖片模式：image/gif
  5. jpg圖片模式：image/jpeg
  6. png圖片模式：image/png
  響應內容(responseContent)
*/
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
//  digitalWrite(RF,HIGH);
//  digitalWrite(RB,LOW);
//  digitalWrite(LF,HIGH);
//  digitalWrite(LB,LOW);
  situation(forward);
  delay(detm);
  esp8266_server.send(200,"text/html","<head><script>history.back()</script></head>");   //回上一頁
}
void Back()
{
//  digitalWrite(RF,LOW);
//  digitalWrite(RB,HIGH);
//  digitalWrite(LF,LOW);
//  digitalWrite(LB,HIGH);
  situation(back);
  delay(detm);
  esp8266_server.send(200,"text/html","<head><script>history.back()</script></head>");   //回上一頁
}
void RightF()
{
//  digitalWrite(RF,LOW);
//  digitalWrite(RB,LOW);
//  digitalWrite(LF,HIGH);
//  digitalWrite(LB,LOW);
  situation(rightf);
  delay(detm);
  esp8266_server.send(200,"text/html","<head><script>history.back()</script></head>");   //回上一頁
}
void LeftF()
{
//  digitalWrite(RF,HIGH);
//  digitalWrite(RB,LOW);
//  digitalWrite(LF,LOW);
//  digitalWrite(LB,LOW);
  situation(leftf);
  delay(detm);
  esp8266_server.send(200,"text/html","<head><script>history.back()</script></head>");   //回上一頁
}
void RightB()
{
//  digitalWrite(RF,LOW);
//  digitalWrite(RB,LOW);
//  digitalWrite(LF,LOW);
//  digitalWrite(LB,HIGH);
  situation(rightb);
  delay(detm);
  esp8266_server.send(200,"text/html","<head><script>history.back()</script></head>");   //回上一頁
}
void LeftB()
{
//  digitalWrite(RF,LOW);
//  digitalWrite(RB,HIGH);
//  digitalWrite(LF,LOW);
//  digitalWrite(LB,LOW);
  situation(leftb);
  delay(detm);
  esp8266_server.send(200,"text/html","<head><script>history.back()</script></head>");   //回上一頁
}
void Stop()
{
//  digitalWrite(RF,LOW);
//  digitalWrite(RB,LOW);
//  digitalWrite(LF,LOW);
//  digitalWrite(LB,LOW);
  situation(STOP);
  delay(detm);
  esp8266_server.send(200,"text/html","<head><script>history.back()</script></head>");   //回上一頁
}
//void Autogo()
//{
//  Cbf=true;
//  bB=false;
//  bF=true;
//  esp8266_server.send(200,"text/html","<head><script>history.back()</script></head>");   //回上一頁
//}
//void Autoback()
//{
//  Cbf=false;
//  Cbb=true;
//  bB=true;
//  bF=false;
//  esp8266_server.send(200,"text/html","<head><script>history.back()</script></head>");   //回上一頁
//}
//void Autostop() // stop auto
//{
//  Cstop=true;  
//  bB=false;
//  bF=false;
//  esp8266_server.send(200,"text/html","<head><script>history.back()</script></head>");   //回上一頁
//}
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
void setup() {
  Serial.begin(9600);
  //
  WiFi.disconnect(1);
  WiFi.mode(WIFI_AP);
  Serial.print("開始連接");
  //以上為2021/4/8新增，為增強WIFI連結穩定度
  WiFi.softAP(ssid,pas);
  Serial.println(ssid);
  Serial.print("IP address: "); //預設的IP為192.168.4.1
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
//  esp8266_server.on("/Autogo", HTTP_POST, Autogo);
//  esp8266_server.on("/Autoback", HTTP_POST, Autoback);
//  esp8266_server.on("/Autostop", HTTP_POST, Autostop);
  esp8266_server.on("/startult", HTTP_POST, startult);
  esp8266_server.on("/stopult", HTTP_POST, stopult);
  esp8266_server.onNotFound(NotFound);
  Serial.println("HTTP esp8266_server started"); //告知用戶esp8266網路服務功能已開啟
  
  digitalWrite(RF,LOW);
  digitalWrite(RB,LOW);
  digitalWrite(LF,LOW);
  digitalWrite(LB,LOW);
}
void loop() {
    esp8266_server.handleClient();// ESP8266WebServer函式庫中的handleClient為處理客戶端請求的函式，此函數主要作用是檢查有沒有客戶端設備通過網絡向ESP8266網絡服務器發送請求。每一次handleClient函數被調用時，ESP8266網絡服務器都會檢查一下是否有客戶端發送HTTP請求。因此建議將該函數放在loop函數中，從而確保它能經常被調用。假如loop函數里有類似delay一類的函數延遲程序運行，那麼就一定要注意了。如果handleClient函數長時間得不到調用，ESP8266網絡服務器會因為無法經常檢查HTTP客戶端請求而導致服務器響應變慢，嚴重的情況下，會導致服務器工作不穩定。 來源：http://www.taichi-maker.com/homepage/iot-development/iot-dev-reference/esp8266-c-plus-plus-reference/esp8266webserver/
    if(ult)
    {
       if(ultrasonic.distanceRead() <= dis)
       {
          Stop();
          while(ultrasonic.distanceRead() <= dis) LeftB();
       } 
    }
//    if(Cbb || Cbf || Cstop)
//    {
//      Stop();
//      bB=Cbb;
//      bF=Cbf;
//      Cbb=Cbf=Cstop=false;
//    }
    bool stateRL=digitalRead(RL),stateRR=digitalRead(RR); //黑線=1，其他=0
//    if(!stateRL && stateRR)
//      while(!stateRL) RightF();
//    else if(!stateRR && stateRL)
//      while(!stateRR) LeftF();
//    else if(!stateRR && !stateRL)
//      while(!stateRR || !stateRL) Back();
    if(stateRL and stateRR){  //前空
      Stop();
      while(stateRL and stateRR) Back();
      for(int q=0;q<8;q++) LeftF();
    }
    else if(!stateRR && stateRL){  //右空
      Stop();
      while(stateRL and stateRR) Back();
      for(int q=0;q<3;q++) LeftF();
    }
    else if(stateRR && !stateRL){  //左空
      Stop();
      while(stateRL and stateRR) Back();
      for(int q=0;q<3;q++) RightF();
    }
    if(bF) Forward();
    else if(bB) Back();
}
