#include <SPI.h>
#include <Adafruit_VS1053.h>
#include <SD.h>
#include <Adafruit_NeoPixel.h>
#include <Servo.h>

#define SHIELD_RESET -1  // VS1053 reset pin (unused)
#define SHIELD_CS 7      // VS1053 chip select pin (output)
#define SHIELD_DCS 6     // VS1053 Data/command select pin (output)
#define CARDCS 4         // Card chip select pin
#define DREQ 3           // VS1053 Data request pin (interrupt)

#define NEOPIN     9    // Pin connected to the NeoPixels
#define NUMPIXELS  35   // Number of NeoPixels in the strip

#define SERVO_PIN_1 A2
#define SERVO_PIN_2 A3
#define BUTTON_PIN  8

bool buttonPreviouslyHigh = true; // Flag to track previous state of the button
bool neopixelsDone = false;

Adafruit_VS1053_FilePlayer musicPlayer =
  Adafruit_VS1053_FilePlayer(SHIELD_RESET, SHIELD_CS, SHIELD_DCS, DREQ, CARDCS);

Adafruit_NeoPixel strip(NUMPIXELS, NEOPIN, NEO_GRB + NEO_KHZ800);
Servo servoMotor1;  // Create a servo object for motor 1
Servo servoMotor2;  // Create a servo object for motor 2

void setup() {
  Serial.begin(9600);
  pinMode(BUTTON_PIN, INPUT_PULLUP);
  pinMode(13, OUTPUT);

  // Initialize servo motors
  initServoMotors();
}


void loop() {
  bool buttonState = digitalRead(BUTTON_PIN);

  if (buttonState == HIGH && buttonPreviouslyHigh) {
    // Button state has changed from high to low, start all actions
    Serial.println("Button pressed!");
    startAllActions();

    // Set the flag to false to prevent starting actions again until the button is released
    buttonPreviouslyHigh = false;
  }

  // Run NeoPixel animation and move servo motors while music is playing
  if(!musicPlayer.stopped()) {
    moveServoMotors();
    if(!neopixelsDone){runNeoPixelAnimation();}
  }
  else{
    turnOffNeoPixels();
    digitalWrite(13, HIGH);
    Serial.println("All actions completed!");
  }

}

void initServoMotors() {
  
  servoMotor1.write(0);  // Set initial position of motor 1 to 0 degrees
  servoMotor2.write(50); // Set initial position of motor 2 to 180 degrees
  
  servoMotor1.attach(SERVO_PIN_1);  // Attach motor 1 to pin A2
  servoMotor2.attach(SERVO_PIN_2);  // Attach motor 2 to pin A3
  
  delay(1000);  // Wait for the servos to reach their positions
  Serial.println("Initial positions done!");
}

void startAllActions() {
  Serial.println("Starting all actions...");

  // Initialize the music player
  initMusicPlayer();
  initNeoPixels();
  playMusic("/candy001.mp3");  
}

void initMusicPlayer() {
  if (!musicPlayer.begin()) {
    Serial.println("Couldn't find VS1053, check the pins defined!");
    while (1);
  }
  Serial.println("VS1053 found");
  
  // Initialize the SD card
  if (!SD.begin(CARDCS)) {
    Serial.println("SD failed, or not present");
    while (1);
  }

  // Set volume for left, right channels
  musicPlayer.setVolume(20, 20);

  // Use DREQ interrupt
  musicPlayer.useInterrupt(VS1053_FILEPLAYER_PIN_INT);
}

void initNeoPixels() {
  strip.begin();
  strip.clear();
  strip.show();
}

void playMusic(const char* filename) {
  Serial.print("Playing ");
  Serial.println(filename);
  musicPlayer.startPlayingFile(filename);
}

void runNeoPixelAnimation() {
  for(int i = 0; i < NUMPIXELS; i++) {
    strip.setPixelColor(i, strip.Color(64, 0, 64)); // Purple color
    strip.show(); // Update strip with new color
    delay(75);    // Adjust delay as needed
  }
  neopixelsDone = true;
}

void turnOffNeoPixels() {
  // Set all NeoPixels to black (no light)
  for(int i = 0; i < NUMPIXELS; i++) {
    strip.setPixelColor(i, strip.Color(0, 0, 0)); // Black color (no light)
  }
  strip.show(); // Update strip with new colors
}

void moveServoMotors() {
  // Move motor 0 to 15 degrees
  servoMotor1.write(25);
  
  // Move motor 2 to 15 degrees (180 - 25)
  servoMotor2.write(50);
  
  delay(1200);  // Wait for the servos to reach their positions
  
  // Move motor 1 back to 0 degrees
  servoMotor1.write(0);
  
  // Move motor 2 back to 180 degrees
    // Move motor 2 back to 180 degrees
  servoMotor2.write(25);
  
  delay(1200);  // Wait for the servos to reach their positions
}
 
