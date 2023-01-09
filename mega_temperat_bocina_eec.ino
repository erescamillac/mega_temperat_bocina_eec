#include <Wire.h>
#include "Arduino.h"
#include "SoftwareSerial.h"
#include "DFRobotDFPlayerMini.h"
#include <Adafruit_MLX90614.h>

//--INI: includes Conectividad WIFI
//Include the needed library, we will use softer serial communication with the ESP8266
#include <SoftwareSerial.h>
#include <avr/power.h>

//LCD config
#include <Wire.h> 
// #include <LiquidCrystal_I2C.h>
// LiquidCrystal_I2C lcd(0x27,20,4);  //sometimes the LCD adress is not 0x3f. Change to 0x27 if it dosn't work.
//++FIN: includes Conectividad WIFI

//Pulsadores
/*
const int Temp_Alta = 2;  // pulsador 1
const int Temp_Baja = 3;  // pulsador 2
*/


const int sensorPin = 9;  // Sensor de Obtaculos

const int led_Temp_Alta = 4;  // led Temperatura Alta
const int led_Temp_Baja = 5;  // led Tempertatura Baja
const int Led_Indicador_Sensor = 8;  // led Indicador del Sensor

int value = 0;

//--INI: variables WIFI
//Define the used
#define ESP8266_RX 10  //Connect the TX pin from the ESP to this RX pin of the Arduino
#define ESP8266_TX 11  //Connect the TX pin from the Arduino to the RX pin of ESP

/*
int LED1 = 2;
int LED2 = 3;
int LED3 = 4;
int LED4 = 5;
int LED5 = 6;
*/


int Potentiometer_1 = A0;
int Potentiometer_2 = A1;
int Potentiometer_3 = A2;
int Potentiometer_4 = A3;

// int switch1 = 7;
// int switch2 = 8;
// int switch3 = 9;

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////Variables you must change according to your values/////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//Add your data: SSID + KEY + host + location + id + password
//////////////////////////////////////////////
// const char SSID_ESP[] = "MiFibra-4132";         //Give EXACT name of your WIFI
// const char SSID_KEY[] = "vzP5anY5";             //Add the password of that WIFI connection

// const char SSID_ESP[] = "Totalplay-2EA8";         //Give EXACT name of your WIFI
// const char SSID_KEY[] = "2EA8659EXKHY9c85";             //Add the password of that WIFI connection

// const char SSID_ESP[] = "Nokia C30";         //Give EXACT name of your WIFI
// const char SSID_KEY[] = "i3px97vzdbkyjua";             //Add the password of that WIFI connection

const char SSID_ESP[] = "angel";         //Give EXACT name of your WIFI
const char SSID_KEY[] = "yupiyupi";             //Add the password of that WIFI connection

// const char* host = "noobix.000webhostapp.com";  //Add the host without "www" Example: electronoobs.com
const char* host = "conceptos-web-2010067-eec.000webhostapp.com";  //Add the host without "www" Example: electronoobs.com
String NOOBIX_id = "99999";                     //This is the ID you have on your database, I've used 99999 becuase there is a maximum of 5 characters
String NOOBIX_password = "12345";               //Add the password from the database, also maximum 5 characters and only numerical values
String location_url = "/TX.php?id=";            //location of your PHP file on the server. In this case the TX.php is directly on the first folder of the server
                                                //If you have the files in a different folder, add thas as well, Example: "/ESP/TX.php?id="     Where the folder is ESP 
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

//Used variables in the code
String url = "";
String URL_withPacket = "";    
unsigned long multiplier[] = {1,10,100,1000,10000,100000,1000000,10000000,100000000,1000000000};
//MODES for the ESP
const char CWMODE = '1';//CWMODE 1=STATION, 2=APMODE, 3=BOTH
const char CIPMUX = '1';//CWMODE 0=Single Connection, 1=Multiple Connections


//Define the used functions later in the code, thanks to Kevin Darrah, YT channel:  https://www.youtube.com/user/kdarrah1234
boolean setup_ESP();
boolean read_until_ESP(const char keyword1[], int key_size, int timeout_val, byte mode);
void timeout_start();
boolean timeout_check(int timeout_ms);
void serial_dump_ESP();
boolean connect_ESP();
void connect_webhost();
unsigned long timeout_start_val;
char scratch_data_from_ESP[20];//first byte is the length of bytes
char payload[200];
byte payload_size=0, counter=0;
char ip_address[16];

/////////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////////
//Variable to SEND to the DATABASE
bool sent_bool_1 = 0;
bool sent_bool_2 = 0;
bool sent_bool_3 = 0;
float  sent_nr_1 = 0.0;
float temperaturaObjEnCentigrados = 0.0;
int  sent_nr_2 = 0;
int  sent_nr_3 = 0;
int  sent_nr_4 = 0;
int  sent_nr_5 = 0;

//Variable RECEIVED from the DATABASE
bool received_bool_1 = 0;
bool received_bool_2 = 0;
bool received_bool_3 = 0;
bool received_bool_4 = 0;
bool received_bool_5 = 0;
int  received_nr_1 = 0;
int  received_nr_2 = 0;
int  received_nr_3 = 0;
int  received_nr_4 = 0;
int  received_nr_5 = 0;
String received_text = "";


/////////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////////

//Store received chars in this variables
char t1_from_ESP[5];  //For time from web
char d1_from_ESP[2];  //For received_bool_2
char d2_from_ESP[2];  //For received_bool_2
char d3_from_ESP[2];  //For received_bool_3
char d4_from_ESP[2];  //For received_bool_4
char d5_from_ESP[2];  //For received_bool_5
char d9_from_ESP[6];  //For received_nr_1
char d10_from_ESP[6]; //For received_nr_2
char d11_from_ESP[6]; //For received_nr_3
char d12_from_ESP[6]; //For received_nr_4
char d13_from_ESP[6]; //For received_nr_5
char d14_from_ESP[300]; //For received_text

//DEFINE KEYWORDS HERE
const char keyword_OK[] = "OK";
const char keyword_Ready[] = "Ready";
const char keyword_no_change[] = "no change";
const char keyword_blank[] = "#&";
const char keyword_ip[] = "192.";
const char keyword_rn[] = "\r\n";
const char keyword_quote[] = "\"";
const char keyword_carrot[] = ">";
const char keyword_sendok[] = "SEND OK";
const char keyword_linkdisc[] = "Unlink";

const char keyword_t1[] = "t1";
const char keyword_b1[] = "b1";
const char keyword_b2[] = "b2";
const char keyword_b3[] = "b3";
const char keyword_b4[] = "b4";
const char keyword_b5[] = "b5";
const char keyword_n1[] = "n1";
const char keyword_n2[] = "n2";
const char keyword_n3[] = "n3";
const char keyword_n4[] = "n4";
const char keyword_n5[] = "n5";
const char keyword_n6[] = "n6";
const char keyword_doublehash[] = "##";

SoftwareSerial ESP8266(ESP8266_RX, ESP8266_TX);// rx tx

//++FIN: variables WIFI

Adafruit_MLX90614 mlx = Adafruit_MLX90614();

// Comentado :: Original mySoftwareSerial(10, 11); :: MP3MiniPlayer module
// SoftwareSerial mySoftwareSerial(10, 11);  // RX, TX
// DFRobotDFPlayerMini myDFPlayer;

void printDetail(uint8_t type, int value);

void setup() {
  // mySoftwareSerial.begin(9600);

  // Serial.begin(115200);
  Serial.begin( 9600 );
  
  Serial.println();
  Serial.println(F("DFRobot DFPlayer Mini Demo"));
  // Serial.println(F("Initializing DFPlayer ... (May take 3~5 seconds)"));
  Serial.println(F("Initializing DFPlayer ... (May take 3 - 5 seconds)"));
  
  /*
  if (!myDFPlayer.begin(mySoftwareSerial)) {  //Use softwareSerial to communicate with mp3.
    Serial.println(F("Unable to begin:"));
    Serial.println(F("1.Please recheck the connection!"));
    Serial.println(F("2.Please insert the SD card!"));
    while (true);
  }
  */
  
  Serial.println(F("DFPlayer Mini online --FAKE--."));
  
  // myDFPlayer.setTimeOut(500);  //Set serial communictaion time out 500ms
  
  //----Set volume----
  /*
  myDFPlayer.volume(30);    //Set volume value (0~30).
  myDFPlayer.volumeUp();    //Volume Up
  myDFPlayer.volumeDown();  //Volume Down
  */
  
  //----Set different EQ----
  // myDFPlayer.EQ(DFPLAYER_EQ_NORMAL);
  
  //----Set device we use SD as default----
  // myDFPlayer.outputDevice(DFPLAYER_DEVICE_SD);
  
  // pinMode(Temp_Alta, INPUT_PULLUP);
  // pinMode(Temp_Baja, INPUT_PULLUP);
  
  pinMode(led_Temp_Alta, OUTPUT);
  pinMode(led_Temp_Baja, OUTPUT);

  pinMode(sensorPin, INPUT);  //definir pin como entrada
  pinMode(Led_Indicador_Sensor, OUTPUT);

  //--INI: pinModes para conn. WIFI
  //Pin Modes for ESP TX/RX
  pinMode(ESP8266_RX, INPUT);
  pinMode(ESP8266_TX, OUTPUT);
  
  /*
  pinMode(LED1, OUTPUT);
  pinMode(LED2, OUTPUT);
  pinMode(LED3, OUTPUT);
  pinMode(LED4, OUTPUT);
  pinMode(LED5, OUTPUT);
  */
  
  pinMode(Potentiometer_1, INPUT);
  pinMode(Potentiometer_2, INPUT);
  pinMode(Potentiometer_3, INPUT);
  pinMode(Potentiometer_4, INPUT);

  // pinMode(switch1, INPUT);
  // pinMode(switch2, INPUT);
  // pinMode(switch3, INPUT);
  //++FIN: pinModes para conn. WIFI

  mlx.begin();

  //--INI :: ESP config WIFI
  ESP8266.begin(9600);//default baudrate for ESP
  ESP8266.listen();//not needed unless using other software serial instances
  // Serial.begin(9600); //for status and debug
  
  delay(2000);//delay before kicking things off
  Serial.println( "##--INI :: Configurando ESP()..." );
  setup_ESP();//go setup the ESP 
  Serial.println( "##++FIN :: Configuracion ESP()+++" );
  //++FIN :: ESP config WIFI
} //--fin: setup()

void loop() {

  value = digitalRead(sensorPin);  //lectura digital de pin

  if (value == HIGH) {
    Serial.println("Detectado obstaculo");
    digitalWrite(Led_Indicador_Sensor, HIGH);
    
  } else {
    digitalWrite(Led_Indicador_Sensor, LOW);
    delay(100);

    if (mlx.readObjectTempC() <= 40.00) {
      Serial.print("----Temperatura Baja");
      
      Serial.print("ºC\tObjeto = ");
      temperaturaObjEnCentigrados = mlx.readObjectTempC();
      // Serial.print(mlx.readObjectTempC());
      Serial.print( temperaturaObjEnCentigrados );
      Serial.println("ºC");
      //-- INI: vars a Enviar WIFI
      sent_nr_1 = temperaturaObjEnCentigrados;

      send_to_server_1(); 
      //++ FIN: vars a Enviar WIFI
      //---------------------------------
      // myDFPlayer.play(2);

      digitalWrite(led_Temp_Baja, 1);
      digitalWrite(led_Temp_Alta, 0);
      delay(3000);
      
      digitalWrite(led_Temp_Baja, 0);
      
    } else if (mlx.readObjectTempC() > 40.00){
      Serial.print("+++++Temperatura Alta");
      
      Serial.print("ºC\tObjeto = ");
      temperaturaObjEnCentigrados = mlx.readObjectTempC();
      // Serial.print(mlx.readObjectTempC());
      Serial.print( temperaturaObjEnCentigrados );
      Serial.println("ºC");

      //-- INI: vars a Enviar WIFI
      sent_nr_1 = temperaturaObjEnCentigrados;
      send_to_server_1(); 
      //++ FIN: vars a Enviar WIFI

      //---------------------------------
      // myDFPlayer.play(1);

      digitalWrite(led_Temp_Alta, 1);
      digitalWrite(led_Temp_Baja, 0);
      delay(3000);
      
      digitalWrite(led_Temp_Alta, 0);
    }
  }
  // delay(1000);
  delay(1500);
}