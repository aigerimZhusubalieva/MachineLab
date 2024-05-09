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

const int numSongs = 5; // Number of songs available
const char* songNames[numSongs] = {"candy1.mp3", "candy2.mp3", "candy3.mp3", "candy4.mp3", "candy5.mp3"};
int randomIndex = 0;

unsigned long lastUpdate = 0;
unsigned long animationInterval = 50; // Interval between lighting up each pixel (in milliseconds)
int currentPixel = 0;

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
    randomIndex = random(0, numSongs);
    startAllActions(randomIndex);
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
    // change the way neopixels run in loop - use/dont use delay depending on whether this is one time or continuous animation
    if(randomIndex==0):
      runNeoPixelAnimation0();
    elif(randomIndex==1):
      runNeoPixelAnimation1();
    elif(randomIndex==2):
      runNeoPixelAnimation2();
    elif(randomIndex==3):
      runNeoPixelAnimation3();
    elif(randomIndex==4):
      runNeoPixelAnimation4();
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

void startAllActions(randomIndex) {
  Serial.println("Starting all actions...");
  initMusicPlayer();
  playRandomSong(randomIndex);
  // moveTreeMotor();
  // moveServoMotors();
}

void playRandomSong(randomIndex) {
  // Generate a random index
  // Play the corresponding song
  Serial.print(F("Playing track "));
  Serial.println(randomIndex + 1);
  // musicPlayer.startPlayingFile("/candy001.mp3");
  musicPlayer.startPlayingFile(songNames[randomIndex]);
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

// void playMusic(const char* filename) {
//   Serial.print("Playing ");
//   Serial.println(filename);
//   musicPlayer.startPlayingFile(filename);
// }

// candy shop
void runNeoPixelAnimation0() {
  unsigned long currentTime = millis();

  if (currentTime - lastUpdate >= animationInterval) {
    lastUpdate = currentTime;

    // Set the color of the current pixel
    strip.setPixelColor(currentPixel, strip.Color(64, 0, 64)); // Purple color
    strip.show(); // Update strip with new color

    // Move to the next pixel
    currentPixel++;
    if (currentPixel >= NUMPIXELS) {
      currentPixel = 0; // Reset to the beginning once reached the end
    }
  }
}
// watermelon sugar high
void runNeoPixelAnimation1() {
  for(int i = 0; i < 12; i++) {
    strip.setPixelColor(i, strip.Color(0, 255, 0)); // Green color for first 12 pixels
  }
  for(int i = 12; i < 24; i++) {
    strip.setPixelColor(i, strip.Color(255, 255, 255)); // White color for next 12 pixels
  }
  for(int i = 24; i < 35; i++) {
    strip.setPixelColor(i, strip.Color(255, 0, 0)); // Red color for last 11 pixels
  }
  strip.show(); // Update strip with new colors
}

// disney intro
void runNeoPixelAnimation2() {
  unsigned long currentTime = millis();

  if (currentTime - lastUpdate >= animationInterval) {
    lastUpdate = currentTime;

    // Calculate the progress (0 to 1) of the animation
    float progress = float(currentStep) / float(NUMPIXELS);

    // Calculate the color gradient from white to blue based on the progress
    int red = map(progress, 0, 1, 255, 0); // Gradient from 255 (white) to 0 (blue)
    int green = map(progress, 0, 1, 255, 0); // Gradient from 255 (white) to 0 (blue)
    int blue = 255; // Blue component stays constant (255)

    // Set the color of all pixels
    for (int i = 0; i < NUMPIXELS; i++) {
      strip.setPixelColor(i, strip.Color(red, green, blue));
    }
    strip.show(); // Update strip with new colors

    // Move to the next step
    currentStep++;
    if (currentStep >= NUMPIXELS) {
      currentStep = 0; // Reset to the beginning once reached the end
    }
  }
}

// small world
void runNeoPixelAnimation3() {
  for(int i = 0; i < 12; i++) {
    strip.setPixelColor(i, strip.Color(144, 238, 144)); // Green color for first 12 pixels
  }
  for(int i = 12; i < 24; i++) {
    strip.setPixelColor(i, strip.Color(114, 93, 232)); // White color for next 12 pixels
  }
  for(int i = 24; i < 35; i++) {
    strip.setPixelColor(i, strip.Color(252, 219, 3)); // Red color for last 11 pixels
  }
  strip.show(); // Update strip with new colors
}

// hozier youre too sweet for me
void runNeoPixelAnimation4() {
  for(int i = 0; i < 12; i++) {
    strip.setPixelColor(i, strip.Color(255, 0, 0)); 
  }
  for(int i = 12; i < 24; i++) {
    strip.setPixelColor(i, strip.Color(255, 0, 0)); 
  }
  for(int i = 24; i < 35; i++) {
    strip.setPixelColor(i, strip.Color(255, 0, 0)); 
  }
  strip.show(); // Update strip with new colors
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
 
