// Heres a much cheaper and better alternative than a 300$ firelite panel. just dont use it for actual protection since i am not liable for injuries from this code
// VERSION HISTORY and possible future UPDATES
// COPYRIGHT (C)(TM)(R) 2021 ULISESTTS www.github.com/ulisestts
// 7/17/21 First Release, had alot of issues .
// 7/21/21 Removed some of the garbage looping and bugs with the leds acting "weird"
// 7/29/21 Added rtc DS3231 support and removed trashy variables and statements and replaced with "clean" digitalread arguments. Possibly the last update for july
// 8/02/21 Panel now has animations on reset and first power up and also flashes lights.
// 8/07/21 Finally added 1.2 second verication
// 8/11/21 Cleaned Code and added future variables that will be used for trouble and relarm eta next month panel is almost done and ready to soon rot in the garage
// 8/28/21 Added realarm and display scroll when both zones are active
// 9/16/21 Backlight now has a toggle combo for bedrooms turns on with events or a complete power cycle for safety
// 9/22/21 Fixed issues with zone naming you can now edit zone names with editing a single line 
// FOR ARDUINO MEGA ONLY PLZ
#include<Wire.h>
#include<LiquidCrystal_I2C.h>
LiquidCrystal_I2C lcd(0x27, 20, 4);
#include <DS3231.h>
#include <EEPROM.h>
const int  ledPin = 13;
const int  nac1 = 32; //NAC 1 PIN (HORNS) MOSFET TRIGGER
const int  nac2 = 34; //NAC 2 PIN (STROBES) MOSSFET TRIGGER (ALWAYS ON IN ALARM MODE)
const int  alarmled = 24; // ALARM LED
const int  silenceled = 22; // SILENCE LED
const int  zone1 = 28; // ZONE 1 OPTOISOLATOR
const int  zone2 = 30 ; // ZONE 2 OPTOISOLATOR
const int  alarmsil = 50; // ALARM SILENCE BUTTON
const int  ACK = 49; // FUTURE USE "ACK" BUTTON
const int  sysrst = 51; // PIN A0 IS SYSTEM RESET DOES NOT ACTUALLY HARDWARE RESET THOUGH AT THIS TIME
const int  piezo = 26; // FOR THE PIEZO BUZZER FOR FUTURE USE *26
const int  relay=48;
int blcombotrigger=HIGH;
int  generalalarm1 = 0;// realarm
int  generalalarm2 = 0;// realarm
int  generalalarm = 0;
int  notacked = 0;
int  dontshowtime = 0;
int  inmenu = 1;
int  lcdbacklight=1;
int  silenced=0;
int  silencedmsg=0;
long zone1verificationtime;
int  scrolltime2 = 8000;
int  scrolltime1 = 4000;
int  scrolltime3 = 12000;
unsigned long timenow = 0;
unsigned long previoustime2=0;
unsigned long previoustime1=0;
unsigned long previoustime3=0;
int  zone1value = LOW;
int  oldzone1value = LOW;
long zone2verificationtime;
int  zone2value = LOW;
int  oldzone2value = LOW;
char Str1[] = "SMOKE DET"; // zone 1 device type
char Str2[] = "SD355 L1M02"; // zone 1 name
char Str3[] = "PULL STATION"; // zone 2 device type
char Str4[] = "FIRE-LITE BG-12"; // zone 2 device name 
const char compile_date[] = __DATE__ ;
const char compile_time[] = __TIME__ ;
byte customChar[] = {
  B11111,
  B11111,
  B11111,
  B11111,
  B11111,
  B11111,
  B11111,
  B11111
};
DS3231 clock;
RTCDateTime dt;


void setup() {
  /*if (EEPROM.read(0)==0){
    lcd.noBacklight();
  }else{
    lcd.backlight();
  }*/
  pinMode(zone1,INPUT_PULLUP);
  pinMode(zone2,INPUT_PULLUP);
  pinMode(alarmsil,INPUT_PULLUP);
  pinMode(sysrst,INPUT_PULLUP);
  pinMode(ACK,INPUT_PULLUP);
  pinMode(silenceled,OUTPUT);
  pinMode(alarmled,OUTPUT);
  pinMode(13,OUTPUT);
  pinMode(piezo,OUTPUT);
  pinMode(nac1,OUTPUT);
  pinMode(nac2,OUTPUT);
  pinMode(relay,OUTPUT);
  digitalWrite(relay,HIGH);
  clock.begin();
  lcd.backlight();
  digitalWrite(silenceled, HIGH);
  digitalWrite(alarmled, HIGH);
  digitalWrite(piezo, HIGH);
  delay(600);
  digitalWrite(silenceled, LOW);
  digitalWrite(alarmled, LOW);
  digitalWrite(piezo, LOW);
  lcd.begin();
  lcd.createChar(0, customChar);
  lcd.setCursor(0, 0);
  lcd.print("SOFTWARE VERSION:");
  lcd.setCursor(0, 1);
  lcd.print("");
  lcd.setCursor(0, 2);
  lcd.print(compile_date);
  lcd.setCursor(0, 3);
  lcd.print(compile_time);
  delay(5000);
  lcd.clear();
  // The following lines can be uncommented to set the date and time
  // Manual (YYYY,MM,DD,HH,MM,SS
  //clock.setDateTime(2021,8,4,19,29,30);
  sysnormal();
}


void loop() {
  checksystemresettbtn();
  checksilencbtn();
  refreshtime();
  checkackbtn();
  buttoncombonations();
  checkstateonzone1();
  checkstateonzone2();
  scrolldisplay();
  alarmrelay();
  delay(20); // prevents the panel from being "too fast" gives it that genuine firelite feel
}

void alarmrelay(){
  if (generalalarm==1){
    digitalWrite(relay,LOW);
  }
    timenow = millis();
}

void refreshtime() {
  if (dontshowtime == 0) {
    dt = clock.getDateTime();
    lcd.setCursor(1, 3);
    //lcd.print(clock.dateFormat("h:iA D m/d/y", dt));
    //lcd.print(clock.dateFormat("h:i:sA  M-d-y", dt));
    lcd.print(clock.dateFormat("h:iA mdy", dt));
  }
}



void checkstateonzone1() {
  if (generalalarm1==0){
  zone1value = digitalRead(zone1);
  if ( zone1value == LOW )
  {
    if (zone1value != oldzone1value)
    {
      zone1verificationtime = millis() + 1200;

    }
    else {
      if ( millis() >= zone1verificationtime )
      {
        alarm1();
        generalalarm1 = 1;
        generalalarm = 1;
        zone1verificationtime = millis() + 1200;

      }

    }

  }
  oldzone1value = zone1value;
}
}

void checkstateonzone2() {
  if (generalalarm2==0);{
  zone2value = digitalRead(zone2);
  if ( zone2value == LOW )
  {
    if (zone2value != oldzone2value)
    {
      zone2verificationtime = millis() + 1200;

    }
    else {
      if ( millis() >= zone2verificationtime )
      {
        alarm2();
        generalalarm2 = 1;
        generalalarm = 1;
        zone2verificationtime = millis() + 1200;

      }

    }

  }
  oldzone2value = zone2value;
}
}


void sysnormal() {
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("4830 W 120TH AVE"); // top custom banner
  lcd.setCursor(0, 1);
  lcd.print("       "); // bottom text banner
  lcd.setCursor(1, 2);
  lcd.print("SYSTEM NORMAL");
}


void checksystemresettbtn() {
  if ( digitalRead(sysrst) == LOW && digitalRead(alarmsil) == HIGH)
  {
    systemreset();
  }
}


void systemreset() {
  zone1value = HIGH;
  oldzone1value = HIGH;
  zone2value = HIGH;
  oldzone2value = HIGH;
  generalalarm1 = 0;
  generalalarm2 = 0;
  generalalarm = 0;
  notacked = 0;
  silenced =0;
  silencedmsg=0;
  digitalWrite(alarmled, HIGH);
  digitalWrite(silenceled, HIGH);
  digitalWrite(piezo, HIGH);
  lcd.clear();
  lcd.write(0);
  lcd.write(0);
  lcd.write(0);
  lcd.write(0);
  lcd.write(0);
  lcd.write(0);
  lcd.write(0);
  lcd.write(0);
  lcd.write(0);
  lcd.write(0);
  lcd.write(0);
  lcd.write(0);
  lcd.write(0);
  lcd.write(0);
  lcd.write(0);
  lcd.write(0);
  lcd.write(0);
  lcd.write(0);
  lcd.write(0);
  lcd.write(0);
  lcd.write(0);
  lcd.write(0);
  lcd.write(0);
  lcd.write(0);
  lcd.write(0);
  lcd.write(0);
  lcd.write(0);
  lcd.write(0);
  lcd.write(0);
  lcd.write(0);
  lcd.write(0);
  lcd.write(0);
  lcd.write(0);
  lcd.write(0);
  lcd.write(0);
  lcd.write(0);
  lcd.write(0);
  lcd.write(0);
  lcd.write(0);
  lcd.write(0);
  lcd.write(0);
  lcd.write(0);
  lcd.write(0);
  lcd.write(0);
  lcd.write(0);
  lcd.write(0);
  lcd.write(0);
  lcd.write(0);
  lcd.write(0);
  lcd.write(0);
  lcd.write(0);
  lcd.write(0);
  lcd.write(0);
  lcd.write(0);
  lcd.write(0);
  lcd.write(0);
  lcd.write(0);
  lcd.write(0);
  lcd.write(0);
  lcd.write(0);
  lcd.write(0);
  lcd.write(0);
  lcd.write(0);
  lcd.write(0);
  lcd.write(0);
  lcd.write(0);
  lcd.write(0);
  lcd.write(0);
  lcd.write(0);
  lcd.write(0);
  lcd.write(0);
  lcd.write(0);
  lcd.write(0);
  lcd.write(0);
  lcd.write(0);
  lcd.write(0);
  lcd.write(0);
  lcd.write(0);
  lcd.write(0);
  lcd.write(0);
  delay(600);
  lcd.clear();
  digitalWrite(piezo, LOW);
  digitalWrite(nac1, LOW);
  digitalWrite(nac2, LOW);
  lcd.setCursor(2, 0);
  lcd.print("  SYSTEM RESET");
  lcd.setCursor(1, 1);
  lcd.print("2-ZONE UD; 3AMP PS");
  lcd.setCursor(2, 2);
  lcd.print("VERSION 5.3 B2");
  lcd.setCursor(4, 3);
  lcd.print(compile_date);
  delay(2000);
  digitalWrite(alarmled, LOW);
  digitalWrite(silenceled, LOW);
  digitalWrite(piezo, LOW);
  digitalWrite(relay,HIGH);
  delay(1500);
  lcd.setCursor(0,3);
  lcd.print("  CHECKING MEMORY  ");
  delay(3000);
  sysnormal();
}


void alarm1() {
  if (generalalarm1==0){
     if (generalalarm==0){
      lcd.clear();
    }
    lcd.backlight();
    lcd.setCursor(0, 0);
    lcd.print("ALARM:");
    lcd.setCursor(7, 0);
    lcd.print(Str1);
    lcd.setCursor(1, 1);
    lcd.print(Str2);                                    // ZONE NAME HERE
    lcd.setCursor(2, 2);
    lcd.print("ZONE 1");
    digitalWrite(nac2, HIGH);
    digitalWrite(nac1, HIGH);
    digitalWrite(alarmled, HIGH);
    digitalWrite(piezo, HIGH);
    digitalWrite(silenceled, LOW);
    generalalarm = 1;
    notacked = 1;
    silencedmsg =0;
    silenced=0;
  }
}


void alarm2() {
  if (generalalarm2==0){
    if (generalalarm==0){
      lcd.clear();
    }
    lcd.backlight();
    lcd.setCursor(0, 0);
    lcd.print("ALARM:");
    lcd.setCursor(7, 0);
    lcd.print(Str3);                                     // DEVICE TYPE
    lcd.setCursor(1, 1);
    lcd.print(Str4);                                     // ZONE NAME HERE
    lcd.setCursor(2, 2);
    lcd.print("ZONE 2");
    digitalWrite(nac2, HIGH);
    digitalWrite(nac1, HIGH);
    digitalWrite(alarmled, HIGH);
    digitalWrite(piezo, HIGH);
    digitalWrite(silenceled, LOW);
    generalalarm = 1;
    notacked = 1;
    silencedmsg=0;
    silenced=0;
}
}

void checksilencbtn() {
  if (generalalarm == 1 && digitalRead(alarmsil) == LOW) {
    notacked=0;
    silencedmsg=1;
    silenced=1;
    lcd.setCursor(0, 0);
    lcd.print("Alarm:");
    digitalWrite(nac1, LOW);
    digitalWrite(piezo, LOW);
    digitalWrite(silenceled, HIGH);
  }
}


void checkackbtn() {
  if (notacked == 1 && digitalRead(ACK) == LOW) {
    digitalWrite(piezo, LOW); // or shut the piezo up sequence
    lcd.setCursor(0, 0);
    lcd.print("Alarm:");
  }
}


void buttoncombonations() {
  
  if (digitalRead(sysrst) == LOW && digitalRead(alarmsil) == LOW && (generalalarm == 0)&&(lcdbacklight==0)) {
     lcdbacklight=1;
   }
  if (digitalRead(sysrst) == HIGH || digitalRead(alarmsil) == HIGH){
   if (generalalarm==0&&lcdbacklight==1){
      lcdbacklight=0;
      if(blcombotrigger==LOW){
      //EEPROM.write(0,0);
      lcd.noBacklight();
      blcombotrigger=HIGH;
    }else{
      //EEPROM.write(0,1);
      lcd.backlight();
      blcombotrigger=LOW;
    }
}
}
}


 

void scrolldisplay(){
   /*if (timenow-previoustime3>=scrolltime3&&(silencedmsg==1)){
    previoustime3=timenow;
    lcd.setCursor(0,0);
    lcd.print("SILENCE IN SYSTEM    ");
    silencedmsg=0;
  }*/
  
    if ((generalalarm1==1)&&(generalalarm==1)&&(generalalarm2==1)&&timenow-previoustime1>=scrolltime1){
    previoustime1=timenow;
    lcd.setCursor(7, 0);
    lcd.print("               ");
    lcd.setCursor(7, 0);
    lcd.print(Str1             );
    lcd.setCursor(1, 1);
    lcd.print("               ");
    lcd.setCursor(1, 1);
    lcd.print(Str2             );                                     // ZONE NAME HERE
    lcd.setCursor(2, 2);
    lcd.print("ZONE 1");
   if ((generalalarm2==1)&&(generalalarm1==1)&&(generalalarm==1)&&timenow-previoustime2>=scrolltime2){
    previoustime2=timenow;
    lcd.setCursor(7, 0);
    lcd.print("               ");
    lcd.setCursor(7, 0);
    lcd.print(Str3             );                                     // DEVICE TYPE
    lcd.setCursor(1, 1);
    lcd.print("               ");
    lcd.setCursor(1, 1);
    lcd.print(Str4             );                                     // ZONE NAME HERE
    lcd.setCursor(2, 2);
    lcd.print("ZONE 2");
}
}
}
