#include <MFRC522.h>
#include<SPI.h>
#include"HX711.h"
int count = 0;
#include <LiquidCrystal.h>
#include <Servo.h>
int i=1;
const int buzzer=8;
int IR_1 = 4;
int IR_2 = 5;
Servo myservo1;
#define servopin 6
const int rs = 7, en = A1, d4 = A2,d5 = A3, d6 = A4, d7 = A5;
LiquidCrystal lcd(rs, en, d4, d5, d6, d7);
#define SS_PIN 10
#define RST_PIN 9 
#define DOUT  3
#define CLK  2
#define mois  A0
int new_weight=0,wt=0,reward=0;
HX711 scale;
float calibration_factor = 696;
float wgt;
MFRC522 mfrc522(SS_PIN, RST_PIN);
char input[12];
int moisture=0;
boolean flag1 = false,flag2 = false,flag3 = false;
void setup()
{
 Serial.begin(9600);
 scale.begin(DOUT,CLK);
 myservo1.attach(6);
 pinMode(IR_1,INPUT);
 pinMode(IR_2,INPUT);
 pinMode(buzzer,OUTPUT);
 lcd.begin(16,2);
 lcd.setCursor(0, 0);
 lcd.print(" Smart Dustbin");
 Serial.println("Initialized");
 SPI.begin();    
 mfrc522.PCD_Init();  
 Serial.println("Put your card to the reader...");
 Serial.println();
 Serial.println ("SIM900A Ready");
 delay(100);
 scale.set_scale(696);
 scale.tare();  
 delay(2000);
 lcd.clear();
}
void loop()
{
 weight();
 rfid_call();
 level_detect();
}
//for RF id////////////////////////////////////////////////
void rfid_call()
{
 if ( ! mfrc522.PICC_IsNewCardPresent())
  {
   return;
  }
  if ( ! mfrc522.PICC_ReadCardSerial())
  {
   return;
  }
  //Show UID on serial monitor////////////////////////////
  Serial.print("UID tag :");
  String content= "";
  byte letter;
  for (byte i = 0; i < mfrc522.uid.size; i++)
  {
   Serial.print(mfrc522.uid.uidByte[i] < 0x10 ? " 0" : " ");
   Serial.print(mfrc522.uid.uidByte[i], HEX);
   content.concat(String(mfrc522.uid.uidByte[i] < 0x10 ? " 0" : " "));
   content.concat(String(mfrc522.uid.uidByte[i], HEX));    
  }
  Serial.println();
  Serial.print("Message : ");
  content.toUpperCase();
 if(content.substring(1) == "76 99 53 29") // Ambikesh
 {
  sendmsg1();
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Thank you Ambikesh ");
  delay(2000);
  segregate();
  delay(2000);
  lcd.setCursor(0, 0);
  lcd.print("reward");
  lcd.setCursor(11, 0);
  lcd.print(reward);
  Serial.println("Thank for using the dustbin");
  Serial.println("reward add in your account:- ");
  Serial.println(reward);              
  delay(500);
 }
 else if(content.substring(1) == "9A 94 FF 80") // user
 {
  sendmsg2();
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Thankyou user");
  delay(2000);
  ////
  segregate();
  delay(2000);
  weight_new();
  delay(2000);
  sendmsg2();
  delay(2000);
   /////////////////////////////////
  lcd.setCursor(0, 0);
  lcd.print("reward");
  lcd.setCursor(11, 0);
  lcd.print(reward);
  Serial.println("Thank for using the dustbin");
  Serial.println("reward add in your account:- ");
  Serial.println(reward);              
  delay(500);
 }
 else  
 {
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("INVALID   CARD");
  delay(400);
 }
 delay(1000);
 lcd.clear();
}
//for the load cell/////////////////////////////////////////
void weight()
{
 lcd.clear();
 wgt=scale.get_units();
 wt=(wgt*1000)/8;
 moisture=analogRead(mois);
 Serial.println(moisture);
 Serial.print("Weight: ");
 Serial.print(wgt);
 Serial.println("gms");
 lcd.setCursor(0, 0);
 lcd.print("Weight");
 lcd.setCursor(11, 0);
 lcd.print(wt);
 lcd.setCursor(0, 1);
 lcd.print("moisture");
 lcd.setCursor(12, 1);
 lcd.print(moisture);
 
 delay(200);
}
void weight_new()
{
  lcd.clear();
wgt=scale.get_units();
 new_weight=(wgt*1000)/8;
 
 Serial.print("Weight: ");
 Serial.print(wt);
 Serial.println("gms");
 
 lcd.setCursor(0, 0);
 lcd.print("Weight");
 lcd.setCursor(11, 0);
 lcd.print(new_weight);
 delay(100);
 wgt=scale.get_units();
 new_weight=(wgt*1000)/8;
 Serial.print("Weight: ");
 Serial.print(wt);
 Serial.println("gms");
 lcd.setCursor(0, 0);
 lcd.print("Weight");
 lcd.setCursor(11, 0);
 lcd.print(new_weight);
 lcd.setCursor(0, 1);
 lcd.print(" old Weight");
 lcd.setCursor(11, 1);
 lcd.print(wt);
 int nw=new_weight;
 int w=wt;
 reward =(nw-w);
}

//for the servo motor//////////////////////////////////////////
void segregate()
{
 moisture=analogRead(mois);
 delay(1000);
 Serial.println(moisture);
 delay(100);
 if(moisture>800)
 {
   myservo1.write(90);
   delay(1000);
   myservo1.write(145);
 }
 else
 {
  myservo1.write(90);
  delay(1000);
  myservo1.write(35);
 }
}
//for the IR sensor and buzzer////////////////////////////////////
 void level_detect()
 {
  int StatusSensor1= digitalRead(IR_1);
  int StatusSensor2= digitalRead(IR_2);
  if(StatusSensor1||StatusSensor2 == 1)
  {
    digitalWrite(buzzer,HIGH);
    sendmsg3();
  }
  else
  {
    digitalWrite(buzzer,LOW);
  }
 }
 //for gsm module/////////////////////////////////////////////////
 void sendmsg1()
{
  if(i==1)
   {
     Serial.println();
     Serial.println("AT+CMGF=1");
     delay(3000);
     Serial.println();
     Serial.print("AT+CMGS=");   
     Serial.print("\"+91xxxxxxxxxx\"");
     Serial.println();
     delay(3000);
     Serial.print("Thankyou!for saving the NATURE,your reward is added to your account");
     delay(2000);
     Serial.println();
     Serial.write(26);    
     i++;       
      }  
}          
void sendmsg2()
{
  if(i==1)
   {
     Serial.println();
     Serial.println("AT+CMGF=1");
     delay(3000);
     Serial.println();
     Serial.print("AT+CMGS="); 
     Serial.print("\"+91xxxxxxxxxx\"");
     Serial.println();
     delay(3000);
     Serial.print("Thankyou!for saving the NATURE,your reward is added to your account");
     delay(2000);
     Serial.println();
     Serial.write(26);    
     i++;      
      }  
}    
void sendmsg3()
{
  if(i==1)
   {
     Serial.println();
     Serial.println("AT+CMGF=1"); 
     delay(3000);
     Serial.println();
     Serial.print("AT+CMGS=");     
     Serial.print("\"+91xxxxxxxxxx\"");
     Serial.println();
     delay(3000);
     Serial.print("WARNING!! dustbinwill fill soon!");
     delay(2000);
     Serial.println();
     Serial.write(26);    
     i++;    
      }  
}               
