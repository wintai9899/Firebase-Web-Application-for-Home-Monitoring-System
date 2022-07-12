#include <Arduino.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <Wire.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_BME680.h>
#include <WiFi.h>
#include <time.h>
#include <Firebase_ESP_Client.h>
#include "ESP32_MailClient.h"


#include "soc/soc.h"
#include "soc/rtc_cntl_reg.h"

// Helper functions for database communication
#include "addons/TokenHelper.h"
#include "addons/RTDBHelper.h"

// Wifi Credentials 
#define WIFI_SSID "SOTON-IoT"
#define WIFI_PASSWORD "Lkxrl1nZLJDW"


// Firebase project API Key
#define API_KEY "AIzaSyA0YwHlykfG1Y--17AEk085EPD2VgepFnA"
// Authourized User Email and Password 
#define USER_EMAIL "user@hotmail.com"
#define USER_PASSWORD "password1"
// Firebase Database URL
#define DATABASE_URL "https://esp32-monitoring-system-default-rtdb.europe-west1.firebasedatabase.app/"

// Email credentials 
#define emailSenderAccount    "motiondetectiontest1@gmail.com"
#define emailSenderPassword   "lvoxsulkiigxxzua"
#define smtpServer            "smtp.gmail.com"
#define smtpServerPort        465
#define emailSubject          "Unusual Activities Detected by ESP32"
#define emailRecipient        "wentai9899@gmail.com"

// OLED Display
#define SCREEN_WIDTH 128  
#define SCREEN_HEIGHT 64 
#define OLED_RST -1

// I2C ports
#define I2Cdisplay_SDA 21
#define I2Cdisplay_SCL 22

// BME680
#define I2C_SDA 21
#define I2C_SCL 22
// LDR (Light Dependent Resistor)
#define ldr  33          

// Pushbutton
#define buttonPin  18    

#define buzzer 2
#define mq2Gas 34

//RCWL-0516 Motion Detector 
#define Radar_Sensor 27
#define motionLED  26

#define VIN 5 //input power
#define R 1000//ldr resistance

// Define BME680 and OLED display object
TwoWire I2CBME = TwoWire(0);
TwoWire I2Cdisplay = TwoWire(1);
Adafruit_BME680 bme;
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &I2Cdisplay, OLED_RST);

// Define Firebase objects
FirebaseData fbData;
FirebaseAuth fbAuth;
FirebaseConfig config;
FirebaseJson json;



// Define SMTPDATA object
SMTPData smtp;

String bmeOutput; 

float temperature;
float humidity;
float pressure;
int lux;
int ldrReading;

// Variable to save USER UID
String uid;

// Database main path (to be updated in setup with the user UID)
String databasePath;
// Database child nodes
String tempPath = "/temperature";
String humPath = "/humidity";
String presPath = "/pressure";
String lightDataPath = "luminosity";
String smokeDataPath = "/smoke";
String timePath = "/timestamp";

// Parent Node (to be updated in every loop)
String parentPath;

int timestamp;


// NTP Server Details for getting current data and time
const char* ntpServer = "pool.ntp.org";
const long  gmtOffset_sec = 0;
const int   daylightOffset_sec = 3600;


int buttonState;              
int prevButtonState = LOW;    

unsigned long prevDebounceTime = 0;  
unsigned long debounceDelay = 50;    

// Timer variables 
// Send data every three minutes
unsigned long sendDataPrevMillis = 0;
unsigned long databaseTimerDelay = 180000;
boolean startTimer = false;
//unsigned long time_now=0;


// Screens
int OLEDScreenNumber = 0;
int OLEDScreenNumber_Max = 5;

unsigned long lastTimer = 0;
unsigned long OLEDtimerDelay = 15000;

hw_timer_t *timer = NULL;
void IRAM_ATTR rebootModule(){
    ets_printf("ESP32 going to reboot\n");
    esp_restart();
}

// Gets current epoch time
unsigned long getTime() {
  time_t currentTime;
  struct tm currentTimeInfo;
  if (!getLocalTime(&currentTimeInfo)) {
    Serial.println("Failed to obtain current time");
    return(0);
  }
  time(&currentTime);
  return currentTime;
}

void displayMarker(int displayNumber) {
  // The markers are 10 pixels apart
  int xCoordinates[6] = {35, 45, 55, 65, 75, 85};
  for (int i =0; i<6; i++) {
    if (i == displayNumber) {
      display.fillCircle(xCoordinates[i], 60, 2, WHITE);
    }
    else {
      display.drawCircle(xCoordinates[i], 60, 2, WHITE);
    }
  }
}

int sensorToLux(int rawData){
  // Conversion rule
  float Vout = float(rawData) * (VIN / float(1023));// Conversion analog to voltage
  float RLDR = (R * (VIN - Vout))/Vout; // Conversion voltage to resistance
  int lux =500/(RLDR/1000); // Conversion resitance to lumen
  return lux;
}

//SCREEN NUMBER 1: DATE AND TIME
void timeScreen(){
  struct tm currentTimeInfo;
  if(!getLocalTime(&currentTimeInfo)){
    Serial.println("Failed to obtain time");
  }
  Serial.println(&currentTimeInfo, "%A, %B %d %Y %H:%M:%S");

  // GET DATE
  
  char dayWeek[10];
  strftime(dayWeek, sizeof(dayWeek), "%a", &currentTimeInfo);
  //Get day of month
  char dayMonth[4];
  strftime(dayMonth, sizeof(dayMonth), "%d", &currentTimeInfo);
  //Get abbreviated month name
  char nameofMonth[5];
  strftime(nameofMonth, sizeof(nameofMonth), "%b", &currentTimeInfo);
  //Get year
  char currentYear[6];
  strftime(currentYear, sizeof(currentYear), "%Y", &currentTimeInfo);
  
  //Get hour (24 hour format)
  char hour[4];
  strftime(hour, sizeof(hour), "%H", &currentTimeInfo);
  //Get minute
  char minute[4];
  strftime(minute, sizeof(minute), "%M", &currentTimeInfo);

  //Display Date and Time on OLED display
  display.clearDisplay();
  display.setTextColor(WHITE);
  display.setTextSize(3);
  //Sets the coordinates of the starting text
  display.setCursor(19,5);
  //prints the current hour
  display.print(hour);
  display.print(":");
  //prints the current minute
  display.print(minute);
  display.setTextSize(1);
  display.setCursor(16,40);
  //
  display.print(dayWeek);
  display.print(", ");
  display.print(dayMonth);
  display.print(" ");
  display.print(nameofMonth);
  display.print(" ");
  display.print(currentYear);
  displayMarker(OLEDScreenNumber);
  display.display();
}

// SCREEN NUMBER 2: TEMPERATURE
void temperatureScreen(){
  display.clearDisplay();
  display.setTextSize(2);
  //Sets the coordinates of the starting text
  display.setCursor(35, 3);
  float temperature = bme.readTemperature();
  display.print(temperature);
  display.cp437(true);
  display.setTextSize(1);
  display.print(" ");
  display.write(167);
  display.print("C");
  display.setTextSize(2);
  display.setCursor(2, 34);
  display.print("Room Temp.");
  display.setTextSize(2);
  displayMarker(OLEDScreenNumber);
  display.display();
  

}

// SCREEN NUMBER 3: HUMIDITY
void humidityScreen(){
  display.clearDisplay();
  display.setTextSize(2);
  //Sets the coordinates of the starting text
  display.setCursor(35, 5);
  float humidity = bme.readHumidity();
  display.print(humidity);
  display.print(" %");
  display.setCursor(20, 34);
  display.print("Humidity");
  display.setTextSize(2);
  displayMarker(OLEDScreenNumber);
  display.display();
  
  
}

// SCREEN NUMBER 4: PRESSURE
void pressureScreen(){
  display.clearDisplay();
  display.setTextSize(2);
  //Sets the coordinates of the starting text
  display.setCursor(20, 5);
  display.print(bme.readPressure()/100.0F);
  display.setTextSize(1);
  display.print(" hpa");
  display.setTextSize(2);
  display.setCursor(20, 34);
  display.print("Pressure");
  display.setTextSize(2);
  displayMarker(OLEDScreenNumber);
  display.display();
 
}

// SCREEN NUMBER 5: LUMINOSITY
void lightScreen(){
  display.clearDisplay();
  display.setTextSize(2);
  //Sets the coordinates of the starting text
  display.setCursor(34, 5);
  int ldrReading = analogRead(ldr);
  int lux = sensorToLux(ldrReading);
 // int ldrReading = map(analogRead(ldr), 0, 4095, 100, 0);
  display.print(lux);
  display.print(" lux");
  display.setCursor(5, 34);
  display.print("Luminosity");
  display.setTextSize(2);
  displayMarker(OLEDScreenNumber);
  display.display();
  
 
}

//SCREEN NUMBER 6: SMOKE
void smokeScreen(){
  display.clearDisplay();
  display.setTextSize(2);
  //Sets the coordinates of the starting text
  display.setCursor(15, 5);
  display.print("Smoke:");
  display.print(analogRead(mq2Gas));
  display.setTextSize(1);
  display.setCursor(0, 34);
  displayMarker(OLEDScreenNumber);
  display.display();
 
}

// updates the OLED screen
void updateScreen() {
  
  if (OLEDScreenNumber == 0){
    timeScreen();
  }
  else if (OLEDScreenNumber == 1) {
    temperatureScreen();
  }
  else if (OLEDScreenNumber == 2){
    humidityScreen();
  }
  else if (OLEDScreenNumber == 3){
    pressureScreen();
  }
  else if (OLEDScreenNumber == 4){
    lightScreen();
  }
  else{
    smokeScreen();
    }
}

// initialize OLED Display
void initOLED(){

  //I2C address determined by I2CScanner
  if(!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) {
    Serial.println(F("SSD1306 OLED allocation failed"));
    for(;;);
  }
  display.clearDisplay();
  display.setTextColor(WHITE);
  }

// Initialize BME680
void initBME(){
  // Initialize BME680
  //I2C address determined by I2CScanner
  bool status = bme.begin(0x77, &I2CBME);  
  if (!status) {
    Serial.println("Failed to detect BME680, please check your wire connections.");
    while (1);
  }
  }

void setBME680(){
  // Set up oversampling and filter initialization
  bme.setTemperatureOversampling(BME680_OS_8X);
  bme.setHumidityOversampling(BME680_OS_2X);
  bme.setPressureOversampling(BME680_OS_4X);
  bme.setIIRFilterSize(BME680_FILTER_SIZE_3);
  bme.setGasHeater(320, 150); // 320*C for 150 ms
}
void changeScreen(){
  int buttonReading = digitalRead(buttonPin);
  // Change screen when the pushbutton is pressed
  if (buttonReading != prevButtonState) {
    prevDebounceTime = millis();
  }

  // filter out noice by setting a time buffer
  if ((millis() - prevDebounceTime) > debounceDelay) {
    if (buttonReading != buttonState) {
      buttonState = buttonReading;
      if (buttonState == HIGH) {
        updateScreen();
        Serial.println("Current Screen Number:");
        Serial.println(OLEDScreenNumber);
        if(OLEDScreenNumber < OLEDScreenNumber_Max) {
          OLEDScreenNumber++;
        }
        else {
          OLEDScreenNumber = 0;
        }
        lastTimer = millis();
      }
    }
  }
  prevButtonState = buttonReading;
  
  // Change screen every 15 seconds 
  if ((millis() - lastTimer) > OLEDtimerDelay) {
    updateScreen();
    //Serial.println("Current Screen Number:");
    //Serial.println(OLEDScreenNumber);
    if(OLEDScreenNumber < OLEDScreenNumber_Max) {
      OLEDScreenNumber++;
    }
    else {
      OLEDScreenNumber = 0;
    }
    lastTimer = millis();
  }
 }
void sendEmail( String reason ) {
  
  Serial.println("Sending email...");
  //Setup SMTP Server Credentials
  smtp.setLogin(smtpServer, smtpServerPort, emailSenderAccount, emailSenderPassword);
  // Set the sender name and Email
  smtp.setSender("ESP32", emailSenderAccount);
  // Set Email priority or importance : Low, Normal, High
  smtp.setPriority("Normal");
  // Set the subject
  smtp.setSubject(emailSubject);
  // Set the email message in HTML format
  if (reason == "Intruder"){
    smtp.setMessage("<h2>Intruder Alert</h2>", true);
    }else {
       smtp.setMessage("<h2>Smoke Detected</h2>", true);
      }
      
 
  // Add recipients, can add more than one recipient
  smtp.addRecipient(emailRecipient);


  //smtp.setSendCallback(sendCallback);
  
  // Start sending Email, can be set callback function to track the status
  if (!MailClient.sendMail(smtp))
    Serial.println("Error sending Email, " + MailClient.smtpErrorReason());

  // Clear all data from Email object to free memory
  smtp.empty();
}

// Callback function to get the Email sending status
void sendCallback(SendStatus msg) {
  //Print the current status
  Serial.println(msg.info());
}

void setup() {

  // Define serial communication baud rate
  Serial.begin(115200);
  
  // Initialize the pushbutton pin as an input
  pinMode(buttonPin, INPUT);
  pinMode(buzzer,OUTPUT);
  pinMode(motionLED, OUTPUT);
  digitalWrite(motionLED,LOW);
  
  I2CBME.begin(I2C_SDA, I2C_SCL, 100000);
  I2Cdisplay.begin(I2Cdisplay_SDA, I2Cdisplay_SCL, 100000); 

  initOLED();
  initBME();
  setBME680();

  // Init and get the time
  //configTime(gmtOffset_sec, daylightOffset_sec, ntpServer);
  configTime(0,0,ntpServer);

  // Connect to Wi-Fi
  WiFi.mode(WIFI_STA);
  Serial.print("Connecting to ");
  Serial.println(WIFI_SSID);
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  while (WiFi.status() != WL_CONNECTED) {
    Serial.print("...");
    delay(1000);
  }
  Serial.println("");
  Serial.println("WiFi connected.");
   // Print ESP32 Local IP Address
  Serial.print("IP Address: http://");
  Serial.println(WiFi.localIP());

  timer = timerBegin(0, 80, true); //timer 0, div 80Mhz 
  timerAttachInterrupt(timer, &rebootModule, true);
  timerAlarmWrite(timer, 20000000, false);
  timerAlarmEnable(timer); //enable interrupt

  
  // Configurate Firebase API key
  config.api_key = API_KEY;

  // Configurate user email and password for authentication
  fbAuth.user.email = USER_EMAIL;
  fbAuth.user.password = USER_PASSWORD;

  // Configure database url
  config.database_url = DATABASE_URL;

  Firebase.reconnectWiFi(true);
  fbData.setResponseSize(4096);

  // Assign the callback function for the long running token generation task */
  config.token_status_callback = tokenStatusCallback; //see addons/TokenHelper.h

  // Assign the maximum retry of token generation
  config.max_token_generation_retry = 5;

  // Initialize the library with the Firebase authentication and config
  Firebase.begin(&config, &fbAuth);

  // Getting the user UID might take a few seconds
  Serial.println("Getting User UID");
  while ((fbAuth.token.uid) == "") {
    Serial.print('.');
    delay(1000);
  }
  // Print user UID
  uid = fbAuth.token.uid.c_str();
  Serial.print("User UID: ");
  Serial.println(uid);

  // Update database path
  databasePath = "/UsersData/" + uid + "/readings";
  
  
}

void loop() {
  String emailReason;
  changeScreen();
  //printData();

   timerWrite(timer, 0); //reset timer (feed watchdog)
    long tme = millis();
    
    if(digitalRead(Radar_Sensor) == 1 && startTimer != true){
           startTimer = true;
           Serial.println("Motion detected");
           emailReason = "Intruder";
           sendEmail(emailReason);
           digitalWrite(motionLED, HIGH);
      }else if(digitalRead(Radar_Sensor) == 0){
        startTimer = false;
        digitalWrite(motionLED,LOW);
        
      }
 
  
    int mq2_analog = analogRead(mq2Gas);
//  Serial.print("Gas Sensor: ");
//  Serial.print(mq2_analog);
//  Serial.print("\t");

  if (mq2_analog > 1000){
    digitalWrite (buzzer, HIGH) ; 
    Serial.print("Smoke Detected!");
    //Serial.print("Buzzer Alerted");
    emailReason = "Smoke";
    sendEmail(emailReason);
    delay(1000);
    digitalWrite (buzzer, LOW) ;  
    }
    else {
    digitalWrite (buzzer, LOW);
  }
  

  // Send new readings to database
  if (Firebase.ready() && (millis() - sendDataPrevMillis > databaseTimerDelay || sendDataPrevMillis == 0)){
    sendDataPrevMillis = millis();

    //Get current timestamp
    timestamp = getTime();
    Serial.print ("time: ");
    Serial.println (timestamp);

    int lux = sensorToLux(analogRead(ldr));
    parentPath= databasePath + "/" + String(timestamp);
    json.set(tempPath.c_str(), String(bme.readTemperature()));
    json.set(humPath.c_str(), String(bme.readHumidity()));
    json.set(presPath.c_str(), String(bme.readPressure()/100.0F));
    json.set(lightDataPath.c_str(),String(lux));
    json.set(smokeDataPath.c_str(),String(analogRead(mq2Gas)));
    json.set(timePath, String(timestamp));
    Serial.printf("Set json... %s\n", Firebase.RTDB.setJSON(&fbData, parentPath.c_str(), &json) ? "ok" : fbData.errorReason().c_str());
  }

}
//  Serial.print("Smoke Value: ");
//  Serial.println(mq2_analog);
//  delay(5000);
