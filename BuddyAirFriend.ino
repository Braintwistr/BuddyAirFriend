#include "DHT.h" 
#include "CTBot.h"
#include <Wire.h>
#include <LiquidCrystal_I2C.h> 
 
#define ANALOGPIN A0
#define DHTPIN 2 
#define DHTTYPE DHT11 

String ssid = ""; // укажите название вашей точки-доступа     
String pass = ""; // укажите пароль от вашей точки-доступа
String token = ""; // укажите ваш API ключ 

CTBot myBot; // создаём обьект обозначающий телеграм-бота
DHT dht(DHTPIN, DHTTYPE);
LiquidCrystal_I2C lcd(0x27, 20, 4);

bool sleep_status=0;
float humidity=50;
float temp=21;
float analogValue=0;
float C02=0;
int id=0;

byte smiley_animation[8] = {
  B00000,
  B10001,
  B00000,
  B00000,
  B10001,
  B01110,
  B00000,
};
byte eye_animation1[8] = {
  B01110,
  B10001,
  B10101,
  B10101,
  B10101,
  B10001,
  B01110,
};
byte eye_animation2[8] = {
  B00000,
  B00000,
  B01110,
  B10101,
  B10101,
  B01110,
  B00000,
};
byte eye_animation3[8] = {
  B00000,
  B00000,
  B00000,
  B11111,
  B11111,
  B00000,
  B00000,
};
byte eye_animation5[8] = {
  B01110,
  B10001,
  B10101,
  B10101,
  B10101,
  B10001,
  B01110,
};
byte happy_animation1[8] = {
  B00000,//1
  B00000,//2
  B00100,//3
  B00010,//4
  B00001,//5
  B00000,//6
  B00000,//7
};
byte happy_animation2[8] = {
  B00000,
  B00000,
  B00000,
  B00000,
  B11111,
  B00000,
  B00000,
};
byte happy_animation3[8] = {
  B00000,
  B00000,
  B00100,
  B01000,
  B10000,
  B00000,
  B00000,
};

void setup() {
  Serial.begin(115200); // настраиваем скорость работы COM-порта
  Serial.println(" Включаем Телеграм-Бота ... ");

  myBot.wifiConnect(ssid, pass); // подключаемся к WiFi

  myBot.setTelegramToken(token); // запускаем телеграм-бота 

  if (myBot.testConnection()) 
  { // если бот успешно стартовал, то
    Serial.println(" Бот успешно запущен! ");
  } 
  else 
  { // иначе
    Serial.println(" Возникли проблемы :( ");
  }
  dht.begin(); // запускаем датчик
  lcd.init();
  lcd.backlight();
  
  lcd.createChar(0, eye_animation1);
  lcd.createChar(1, eye_animation2);
  lcd.createChar(2, eye_animation3);
  lcd.createChar(3, eye_animation5);
  lcd.createChar(4, happy_animation1);
  lcd.createChar(5, happy_animation2);
  lcd.createChar(6, happy_animation3);
  lcd.createChar(7, smiley_animation);
  lcd.setCursor(2,1);
  
  lcd.println("Hi,I am Buddy");
  lcd.setCursor(15,1);
  lcd.write(byte(7));
  lcd.setCursor(16,1);
  lcd.write(byte(7));
  lcd.setCursor(17,1);
  lcd.write(byte(7));
  delay(5000);
  lcd.clear();
  
  }

void loop() {
  TBMessage msg;
  if(sleep_status==0)
  {
  humidity = dht.readHumidity(); 
  temp = dht.readTemperature();
  analogValue = analogRead(ANALOGPIN);
  C02=analogValue*2.6188;
  }
  if(sleep_status==0)
  {
     for(int i=0;i<4;i++)
     {
      if(i!=3)
      {
       lcd.setCursor(8,1);
       lcd.write(byte(i));
       lcd.setCursor(10,1);
       lcd.write(byte(i));
       delay(500);
      }
      else
      {
       lcd.setCursor(8,1);
       lcd.write(byte(1));
       lcd.setCursor(10,1);
       lcd.write(byte(1));
       delay(500);
      }
     }
       lcd.setCursor(8,1);
       lcd.write(byte(0));
       lcd.setCursor(10,1);
       lcd.write(byte(0));
      if(temp>20&&temp<30&&humidity>40&&humidity<60&&C02<=1000)
      {
        lcd.setCursor(8,2);
        lcd.write(byte(4));
        lcd.write(byte(5));
        lcd.write(byte(6));
      }
      else
      {
        lcd.setCursor(8,2);
        lcd.write(byte(5));
        lcd.write(byte(5));
        lcd.write(byte(5));
      }
  }
  else
  {
      lcd.setCursor(8,1);
      lcd.write(byte(2));
      lcd.setCursor(10,1);
      lcd.write(byte(2));
      lcd.setCursor(8,2);
      lcd.write(byte(5));
      lcd.write(byte(5));
      lcd.write(byte(5));
  }
  if (myBot.getNewMessage(msg)) 
  { // если телеграм-бот получил новое сообщение, то
    if(msg.text.equalsIgnoreCase("How is air?"))
    {
      if(sleep_status==0){
        myBot.sendMessage(msg.sender.id, "Temperature:"+(String)temp);
        myBot.sendMessage(msg.sender.id, "Humidity:"+(String)humidity);
        myBot.sendMessage(msg.sender.id, "C02:"+(String)C02);
      }
      else
      {
        myBot.sendMessage(msg.sender.id, "Omg,I sleep.");
      }
    }
    if(msg.text.equalsIgnoreCase("Sleep"))
    {
      if(sleep_status==0)
      {
        sleep_status=1;
        myBot.sendMessage(msg.sender.id, "Ok");
      }
      else
      {
        myBot.sendMessage(msg.sender.id, "I'm already sleeping.");
      }
    }
    if(msg.text.equalsIgnoreCase("Wake up"))
    {
      if(sleep_status==0)
      {
         myBot.sendMessage(msg.sender.id, "I don't sleep ,bro.");
      }
      if(sleep_status==1)
      {
        sleep_status=0;
        myBot.sendMessage(msg.sender.id, "Ok");
      }
    } 
    if(msg.text.equalsIgnoreCase("Hey Buddy"))
    {
      id=msg.sender.id;
      myBot.sendMessage(msg.sender.id, "Ugu"); // отправить сообщение с текстом из полученного раннее сообщения\
    }
    if(msg.text.equalsIgnoreCase("Sleep"))
    {
      sleep_status==1;
      myBot.sendMessage(msg.sender.id, "Ok");
    }
    }
}
if(C02>1000||humidity>65||humidity<40||temp<18||temp>30)
    {
      myBot.sendMessage(id, "WARNING");
      if(C02>1000)
      {
        myBot.sendMessage(id, "C02 level is too high.");
      }
      if(humidity>65)
      {
        myBot.sendMessage(id, "Humidity level is too high.");
      }
      if(humidity<40)
      {
        myBot.sendMessage(id, "Humidity level is too low.");
      }
      if(temp>30)
      {
        myBot.sendMessage(id, "Temperature is too high.");
      }
      if(temp<18)
      {
        myBot.sendMessage(id, "Temperature is too low.");
      }
    }
  delay(1000); // делаем задержку в 1 секунда (она нужна что-бы бот успевал проверять сообщения)
}
