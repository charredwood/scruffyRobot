#include <Arduino_LSM6DS3.h>
#include <SPI.h>
#include <Adafruit_VS1053.h>
#include <SD.h>

//VS1053 audio pins
#define VS1053_RESET 9  //reset pin (output)
#define VS1053_CS 10  //chip select (output)
#define VS1053_DCS 2  //Data/command select (output)
#define CARDCS 18  // Card chip select pin
#define DREQ 3  // VS1053 Data request

//object
Adafruit_VS1053_FilePlayer mp3Player = Adafruit_VS1053_FilePlayer(VS1053_RESET, VS1053_CS, VS1053_DCS, DREQ, CARDCS);

//rgb pins
int rPin = A1;
int gPin = A2;
int bPin = A3;

//ldr
int ldr = A7;
int lightVal = 0;

//motor
int mPin = 5;

//music file randomize
char angry[11][13] = {"angry001.mp3", "angry002.mp3", "angry003.mp3", "angry004.mp3", "angry005.mp3", "angry006.mp3", "angry007.mp3", "angry008.mp3", "angry009.mp3", "angry010.mp3", "angry011.mp3"};
char happy[8][13] = {"happy001.mp3", "happy002.mp3", "happy003.mp3", "happy004.mp3", "happy005.mp3", "happy006.mp3", "happy007.mp3", "happy008.mp3"};
char mono[1][13] = {"mono0001.mp3"};
char ambi[8][13] = {"ambig001.mp3", "ambig002.mp3", "ambig003.mp3", "ambig004.mp3", "ambig005.mp3", "ambig006.mp3", "ambig007.mp3", "ambig008.mp3"};

//petting counter
int petCount = 1;

void setup() {

  Serial.begin(9600);
  
  while (!Serial);
  if (!IMU.begin()) {
    Serial.println("Failed to initialize IMU!");
    while (1);
  }

  Serial.print("Accelerometer sample rate = ");
  Serial.print(IMU.accelerationSampleRate());
  Serial.println(" Hz");
  Serial.println();
  Serial.println("Acceleration in G's");
  Serial.println("X\tY\tZ");


  randomSeed(analogRead(0)); //provide random numbers

  //rgb && motor setup
  pinMode(rPin, OUTPUT);
  pinMode(gPin, OUTPUT);
  pinMode(bPin, OUTPUT);
  pinMode(mPin, OUTPUT);

  //Audio Module Chunk
    //volume for L, R //0~100, loudest~silent
    mp3Player.setVolume(0, 0);

    // use the VS1053 interrrupt pin so it can
    // let you know when it's ready for commands.
    mp3Player.useInterrupt(VS1053_FILEPLAYER_PIN_INT) ;

    //reset the VS1053 by taking reset low, then high:
    pinMode(VS1053_RESET, OUTPUT);
    digitalWrite(VS1053_RESET, LOW);
    delay(10);
    digitalWrite(VS1053_RESET, HIGH);

    //initialize the MP3 player:
    if (! mp3Player.begin()) {
      Serial.println("VS1053 not responding.");
      while (true); //stop
    }

    if (! SD.begin(CARDCS)) {
      Serial.println("SD failed, or not present.");
      while (true); //stop
    }
}

void loop() {

  float x, y, z;

  if (IMU.accelerationAvailable()) {
    IMU.readAcceleration(x, y, z);
  }
  
    Serial.print(x);
    Serial.print('\t');
    Serial.print(y);
    Serial.print('\t');
    Serial.println(z);
    delay(500);

  //read the ldr value and print in console
  lightVal = analogRead(ldr);
  Serial.println("LDR value: ");
  Serial.println(lightVal);

  //pet count check
  Serial.print("Scruffy is petted as many as: ");
  Serial.println(petCount);

  //led color changes && motor turns on when light too low
  if (lightVal < 20 && petCount % 10 != 0) {
 
  
   //when angry 
   rgbVal(0, 255, 255); // Red

  int angrySound = random(1, 11);

  //digitalWrite(mPin, HIGH);
  mp3Player.playFullFile(angry[angrySound]);
  Serial.println(angry[angrySound]);
  mp3Player.stopPlaying();
  
  petCount = petCount + 1;
  delay(3000);

  } else {
  rgbVal(0, 0, 0); // White
  //digitalWrite(mPin, LOW);
  }

  if (petCount % 10 == 0 && petCount != 250) {
    int ambiSound = random(1, 8);
    rgbVal(255, 0, 255); //green
    mp3Player.playFullFile(ambi[ambiSound]);
    Serial.println(ambi[ambiSound]);
    delay(1000);
    mp3Player.stopPlaying();
    petCount = petCount + 1;
    delay(5000);
    
  } else {
    
    rgbVal(0, 0, 0);
    if (petCount == 250) {
      rgbVal(0, 0, 255); //yellow
      Serial.println("Scruffy is reciting a monologue.");
      mp3Player.playFullFile("mono0001.mp3");
      delay(3000);
      mp3Player.stopPlaying();
      petCount = petCount + 1;
      delay(1000);
    }
  }

}

void rgbVal(int rVal, int gVal, int bVal)
 {
  analogWrite(rPin, rVal);
  analogWrite(gPin, gVal);
  analogWrite(bPin, bVal);
 }
