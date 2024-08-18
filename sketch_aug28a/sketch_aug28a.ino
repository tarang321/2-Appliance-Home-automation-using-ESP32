
#include <BluetoothSerial.h>//nw edit
#define DEBUG_SW 1
BluetoothSerial SerialBT;//nw
#include <WiFi.h>
#include <WiFiClient.h>
#include <FirebaseESP32.h>

// Firebase Credentials
#define FIREBASE_HOST "light-pcb-default-rtdb.firebaseio.com"
#define FIREBASE_AUTH "1Dj76DHXGFH0FHUA8Y1quHzxhpnOoI47eaKZ2ffl"
String ssid_pass;//nw edit

// WiFi Credentials
#define WIFI_SSID "🎈"
#define WIFI_PASSWORD "1223334444"

String ssid; //nw edit
String pass; //nw edit
// Function Declaration
void  with_internet();
void  without_internet();


// Indicator light
#define L1  13
#define L2  14

#define LED_BT 12//nw edit
#define SW  5//nw edit

// Pins of Switches
#define S5 34 //s1
#define S6 35 //s2

// Pins of Relay (Appliances Control)
#define R5 27
#define A0 25



// Pins of Relay (Fan Speed Control)


//Define FirebaseESP32 data object
FirebaseData firebaseData;
FirebaseJson json;

// Necessary Variables

bool flag = false;//nw edit

int switch_ON_Flag1_previous_I = 0;

void setup()
{
  // put your setup code here, to run once:

  pinMode(S5, INPUT_PULLUP);
  pinMode(S6, INPUT_PULLUP);

  pinMode(R5, OUTPUT);pinMode(L1, OUTPUT);pinMode(L2, OUTPUT);


  Serial.begin(115200);
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);

  Firebase.begin(FIREBASE_HOST, FIREBASE_AUTH);
  Firebase.reconnectWiFi(true);

}


void loop()
{
  /*************************************/
digitalWrite(LED_BT, LOW);
  int input = digitalRead(SW);
  Serial.print("input: ");
  Serial.println(input);
  if(input == 0)
  {
    delay(50);
    flag = true;
    digitalWrite(LED_BT, HIGH);
  }
  
  while(flag == true)
  { 
    
    while(SerialBT.available())
    {
      ssid_pass = SerialBT.readString();
      //Serial.write(SerialBT.read());
      Serial.println(ssid_pass);
  
      for (int i = 0; i < ssid_pass.length(); i++) 
      {
        if (ssid_pass.substring(i, i+1) == ",") 
        {
          ssid = ssid_pass.substring(0, i);
          pass = ssid_pass.substring(i+1);
          Serial.print("SSID = ");Serial.println(ssid);
          Serial.print("Password = ");Serial.println(pass);
          delay(2000);
          flag = false;

          int n1 = ssid.length();
    char char_array1[n1 + 1];
    strcpy(char_array1, ssid.c_str());
  
    int n2 = pass.length();
    char char_array2[n2 + 1];
    strcpy(char_array2, pass.c_str());
  

    WiFi.begin(char_array1, char_array2);                 // write wifi name & password           
  
    Serial.print("Connecting");
    while (WiFi.status() != WL_CONNECTED)
    {
      delay(500);
      Serial.print(".");
    }
    Serial.println();
    Serial.print("Connected, IP address: ");
    Serial.println(WiFi.localIP());
    


          break;
        }
      }  
    }    
  }
 /**********************************************/
  if (WiFi.status() != WL_CONNECTED)
  {
    if (DEBUG_SW) Serial.println("Not Connected");
    digitalWrite(L1, HIGH);
    digitalWrite(L2, LOW);
    without_internet();
  }

  else
  {
    if (DEBUG_SW) Serial.println(" Connected");
    Data_from_firebase();
    digitalWrite(L2, HIGH);
    digitalWrite(L1, LOW);
    with_internet();
  }
}





void Data_from_firebase()
{
  if (Firebase.getString(firebaseData, "/Appliances/appliance1"))
  {
    if (DEBUG_SW) Serial.print("Relay1 - ");
    if (DEBUG_SW) Serial.println(firebaseData.stringData());

    if (firebaseData.stringData() == "1")
    {
      digitalWrite(R5, HIGH);
      

    }
    else
    {
      digitalWrite(R5, LOW);
    }
  }
  

 }
 
  
  
  
  void with_internet()
  {

    // FOR FAN


    // FOR SWITCH
    //Switch 1
    if (digitalRead(S5) == LOW)
    {
      if (switch_ON_Flag1_previous_I == 0 )
      {
        digitalWrite(R5, HIGH);
        if (DEBUG_SW) Serial.println("Relay1- ON");
        String Value1 = "1";
        json.set("/appliance1", Value1);
        Firebase.updateNode(firebaseData, "/Appliances", json);
        switch_ON_Flag1_previous_I = 1;
      }
      if (DEBUG_SW) Serial.println("Switch1 -ON");

    }
    if (digitalRead(S5) == HIGH )
    {
      if (switch_ON_Flag1_previous_I == 1)
      {
        digitalWrite(R5, LOW);
        if (DEBUG_SW) Serial.println("Relay1 OFF");
        String Value1 = "0";
        json.set("/appliance1", Value1);
        Firebase.updateNode(firebaseData, "/Appliances", json);
        switch_ON_Flag1_previous_I = 0;
      }
      if (DEBUG_SW)Serial.println("Switch1 OFF");
    }
  
  //Switch 2
  
  }
  

  
  void without_internet()
  {
    // FOR FAN
    // FOR SWITCH
    digitalWrite(R5, !digitalRead(S5));
    
  }
