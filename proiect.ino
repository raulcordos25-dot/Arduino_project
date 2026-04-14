#include <LiquidCrystal_I2C.h>
#include <Keypad.h>
#include <Wire.h>
#include <Servo.h>
#include "DHT.h"
Servo servo;
const byte randuri = 4;
const byte coloane = 4;
byte piniRanduri[randuri] = { 26, 27, 28, 29 };
byte piniColoane[coloane] = { 30, 31, 32, 33 };
char taste[randuri][coloane] = {
  { '1', '2', '3', 'A' },
  { '4', '5', '6', 'B' },
  { '7', '8', '9', 'C' },
  { '*', '0', '#', 'D' }
};
Keypad tastatura = Keypad(makeKeymap(taste), piniRanduri, piniColoane, randuri, coloane);
LiquidCrystal_I2C lcd(0x27,16,2);
int parolla = 1234, valoare = 0, pozitie = 0;
const int pin_buzzer = 9, pin_servo = 52, pin_trig = 7, pin_echo = 8, pin_pir = 25, pin_temp=6;
DHT dht(pin_temp,DHT11);
void setup() {
  Serial.begin(9600);
  servo.attach(pin_servo);
  dht.begin();
  pinMode(pin_pir, INPUT);
  pinMode(pin_temp,INPUT);
  pinMode(pin_trig, OUTPUT);
  pinMode(pin_echo, INPUT);
  pinMode(pin_buzzer, OUTPUT);
  lcd.init();
}

void loop() {
  valoare = digitalRead(pin_pir);
  lcd.clear();
  if (valoare == HIGH) {
    digitalWrite(24, HIGH);
    Serial.println("Am inceput");
    lcd_stuff();
    if (verificare_parola())
     {
      deschide_bariera();
      inchidere_bariera();
    } 
    else {
      Serial.println("Parola incorecta!");//Nu se apeleaza niciodata
    }
  } 
  else {
    digitalWrite(24, LOW);
    Serial.println(valoare);
    lcd.noBacklight();
    Serial.println("Sunt la pin_led_low");
  }
}


void deschide_bariera()
{
  lcd.clear();
  lcd.setCursor(0, 0);
  Serial.println("Parola corecta!");
  lcd.print("Parola corecta!");
      for (int i = 1; i <= 75; i++) {
        servo.write(i);
        delay(15);
        if(i%25==0){
        analogWrite(pin_buzzer, 100);
        delay(100);
        analogWrite(pin_buzzer, 10);
        delay(100);
        }
    }
        analogWrite(pin_buzzer, 0);
}


void lcd_stuff()
{
    lcd.backlight();
    lcd.setCursor(0,0);
    lcd.print("Buna!");
    delay(2000);
    temp_si_umid();
    lcd.clear();
    lcd.setCursor(0,0);
}


bool verificare_parola() {
  int cont = 0, cheie_int = 0, ver_par = 0;
  lcd.clear();
  lcd.print("Introdu parola:");
  lcd.setCursor(0, 1);
  while (cont != 4) {
    char cheie = tastatura.getKey();
    if (cheie) {
      Serial.println(cheie);
      lcd.print("*");
      cont++;
      cheie_int = cheie - '0';
      ver_par = ver_par * 10 + cheie_int;
      analogWrite(pin_buzzer, 100);
      delay(100);
      analogWrite(pin_buzzer, 0);
    }
  }
  if (ver_par == parolla) return 1;
  else if (ver_par==19010) 
  {
  schimba_parola();
  return 0+verificare_parola();
  }
  else {
    lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Parola gresita!");
  lcd.setCursor(0, 1);
    Serial.println("Incercati din nou!");
    analogWrite(pin_buzzer, 50);
    delay(100);
    analogWrite(pin_buzzer, 20);
    delay(100);
    analogWrite(pin_buzzer, 0);
     delay(1500);
    lcd.clear();
    lcd.setCursor(0,0);
    lcd.print("Introdu parola:");
    lcd.setCursor(0, 1);
    return 0 + verificare_parola();
  }
}


void inchidere_bariera() {
  delay(1500);
  long int distanta = 0;
  bool inchidere = 0;
  digitalWrite(pin_trig, LOW);
  delayMicroseconds(2);
  while (distanta <= 10) {
    digitalWrite(pin_trig, HIGH);
    delayMicroseconds(10);
    digitalWrite(pin_trig, LOW);
    int durata = pulseIn(pin_echo, HIGH);
    distanta = durata * 0.033 / 2;
    Serial.print("Distanta: ");
    Serial.println(distanta);
    delay(100);
  }
  for (int i = 75; i >= 0; i--) {
    servo.write(i);
    if(i%25==0)
    {
        analogWrite(pin_buzzer, 100);
        delay(100);
        analogWrite(pin_buzzer, 10);
        delay(100);}
        analogWrite(pin_buzzer,0);
    delay(30);
  }
  Serial.println("Program terminat,ez,gg,wp");
}


void temp_si_umid()
{
  float temp=0.0;
  float umid=0.0;
  temp=dht.readTemperature();
  umid=dht.readHumidity();
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Temp. este: ");
  lcd.print(temp);
  lcd.setCursor(0,1);
  lcd.print("Umid. este: ");
  lcd.print(umid);
  delay(2000);
}


void schimba_parola()
{
  int cont=0,cheie_ver=0,par_nou_ver=0;
  Serial.println("Schimba parola:");
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Resetare parola");
  delay(2000);
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Parola noua:");
  lcd.setCursor(0, 1);
  while(cont!=4)
  {
    char cheie = tastatura.getKey();
    if(cheie)
    {
      lcd.print(cheie);
      Serial.print(cheie);
      cont++;
      cheie_ver=cheie-'0';
      par_nou_ver=par_nou_ver*10+cheie_ver;
    }
    parolla=par_nou_ver;
  }
  Serial.println(parolla);
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Parola schimbata cu succes!");
  Serial.println("Parola schimbata cu succes!");
  delay(2000);
}