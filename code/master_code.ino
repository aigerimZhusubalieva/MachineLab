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

#define MOTOR_ENA_PIN 12  // Motor enable pin
#define MOTOR_IN1_PIN 10  // Motor input pin 1
#define MOTOR_IN2_PIN 11  // Motor input pin 2

bool buttonPreviouslyHigh = false; // Flag to track previous state of the button
bool action = false; // Flag to track if motor is running
bool buttonState = true;
int speed = 50;

Adafruit_VS1053_FilePlayer musicPlayer =
  Adafruit_VS1053_FilePlayer(SHIELD_RESET, SHIELD_CS, SHIELD_DCS, DREQ, CARDCS);

Adafruit_NeoPixel strip(NUMPIXELS, NEOPIN, NEO_GRB + NEO_KHZ800);
Servo servoMotor1;  // Create a servo object for motor 1
Servo servoMotor2;  // Create a servo object for motor 2

void setup() {
  Serial.begin(9600);
  pinMode(BUTTON_PIN, INPUT_PULLUP);
  pinMode(13, OUTPUT);

  initTreeMotor();
  initServoMotors();
  initNeoPixels();
}

void loop() {
  if (buttonState && buttonPreviouslyHigh && !action) {
    // Button state has changed from high to low, start all actions
    Serial.println("Button pressed!");
    startAllActions();
    buttonPreviouslyHigh = false;
    action = true;
  }
  if (digitalRead(BUTTON_PIN) == HIGH && !buttonPreviouslyHigh) {
    // Button has been released, reset the flag
    buttonPreviouslyHigh = true;
  }
  if (action && !musicPlayer.stopped()) {
    moveServoMotors();
    moveTreeMotor();
    // if(!neopixelsDone){runNeoPixelAnimation();}
  } else if (action && musicPlayer.stopped()) {
    turnOffNeoPixels();
    stopMotor();
    digitalWrite(13, HIGH);
    action = false;
    // Serial.println("All actions completed!");
  }
}

void initServoMotors() {
  servoMotor1.write(0);  // Set initial position of motor 1 to 0 degrees
  servoMotor2.write(50); // Set initial position of motor 2 to 180 degrees

  servoMotor1.attach(SERVO_PIN_1);  // Attach motor 1 to pin A2
  servoMotor2.attach(SERVO_PIN_2);  // Attach motor 2 to pin A3

  Serial.println("Bunnies: Initial positions done!");
}

void initTreeMotor(){
  pinMode(MOTOR_ENA_PIN, OUTPUT);
  pinMode(MOTOR_IN1_PIN, OUTPUT);
  pinMode(MOTOR_IN2_PIN, OUTPUT);

  digitalWrite(MOTOR_IN1_PIN, LOW); // Set motor direction
  digitalWrite(MOTOR_IN2_PIN, LOW);

  Serial.println("Tree initialized!");
}

void startAllActions() {
  Serial.println("Starting all actions...");
  initMusicPlayer();
  playMusic("/candy001.mp3");
  runNeoPixelAnimation();
  moveTreeMotor();
  moveServoMotors();
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
  musicPlayer.setVolume(1, 1);

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
    delay(50);    // Adjust delay as needed
  }
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

void moveTreeMotor() {
  analogWrite(MOTOR_ENA_PIN, speed); // Set motor speed
  digitalWrite(MOTOR_IN1_PIN, LOW); // Set motor direction
  digitalWrite(MOTOR_IN2_PIN, HIGH);
  delay(100);
}

void stopMotor() {
  analogWrite(MOTOR_ENA_PIN, 0); // Turn off motor
  Serial.println("Motor Stopped");
}
 
