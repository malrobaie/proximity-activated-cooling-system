#include <Adafruit_Sensor.h>
#include <DHT.h>
#include <DHT_U.h>
#include <Wire.h>
#include <LiquidCrystal_I2C.h>

// -------------------- Pin Definitions --------------------
#define DHTPIN 2
#define DHTTYPE DHT11

#define MOTOR_PIN 3      // PWM pin to TIP120 base resistor
#define GREEN_LED 6
#define RED_LED 7

#define TRIG_PIN 9
#define ECHO_PIN 10

// -------------------- Objects --------------------
DHT dht(DHTPIN, DHTTYPE);
LiquidCrystal_I2C lcd(0x27, 16, 2);   // Change to 0x3F if needed

// -------------------- Thresholds --------------------
const float TEMP_ON_LOW  = 26.0;   // fan low speed threshold
const float TEMP_ON_MED  = 29.0;   // medium speed threshold
const float TEMP_ON_HIGH = 32.0;   // high speed threshold

const float TEMP_OFF = 25.0;       // hysteresis threshold to turn off

const int DISTANCE_THRESHOLD = 80; // cm, person considered "nearby"

// -------------------- Variables --------------------
float temperatureC = 0.0;
long distanceCM = 999;
bool personNearby = false;
bool fanOn = false;
int fanPWM = 0;

// -------------------- Function Prototypes --------------------
long readDistanceCM();
void updateFanLogic();
void updateOutputs();
void updateLCD();

void setup() {
  pinMode(MOTOR_PIN, OUTPUT);
  pinMode(GREEN_LED, OUTPUT);
  pinMode(RED_LED, OUTPUT);

  pinMode(TRIG_PIN, OUTPUT);
  pinMode(ECHO_PIN, INPUT);

  digitalWrite(MOTOR_PIN, LOW);
  digitalWrite(GREEN_LED, LOW);
  digitalWrite(RED_LED, LOW);

  Serial.begin(9600);
  dht.begin();

  lcd.init();
  lcd.backlight();
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Smart Cooling");
  lcd.setCursor(0, 1);
  lcd.print("System Start");
  delay(2000);
  lcd.clear();
}

void loop() {
  temperatureC = dht.readTemperature();
  distanceCM = readDistanceCM();

  if (isnan(temperatureC)) {
    Serial.println("DHT11 read failed.");
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("DHT Error");
    lcd.setCursor(0, 1);
    lcd.print("Check sensor");
    analogWrite(MOTOR_PIN, 0);
    digitalWrite(GREEN_LED, LOW);
    digitalWrite(RED_LED, HIGH);
    delay(1000);
    return;
  }

  personNearby = (distanceCM > 0 && distanceCM <= DISTANCE_THRESHOLD);

  updateFanLogic();
  updateOutputs();
  updateLCD();

  Serial.print("Temp: ");
  Serial.print(temperatureC);
  Serial.print(" C, Distance: ");
  Serial.print(distanceCM);
  Serial.print(" cm, Nearby: ");
  Serial.print(personNearby ? "YES" : "NO");
  Serial.print(", Fan PWM: ");
  Serial.println(fanPWM);

  delay(500);
}

long readDistanceCM() {
  digitalWrite(TRIG_PIN, LOW);
  delayMicroseconds(3);

  digitalWrite(TRIG_PIN, HIGH);
  delayMicroseconds(10);
  digitalWrite(TRIG_PIN, LOW);

  long duration = pulseIn(ECHO_PIN, HIGH, 30000); // timeout ~30 ms

  if (duration == 0) {
    return 999; // no echo
  }

  long distance = duration * 0.0343 / 2.0;
  return distance;
}

void updateFanLogic() {
  // Default off
  fanPWM = 0;

  if (personNearby) {
    if (temperatureC >= TEMP_ON_HIGH) {
      fanPWM = 255;   // high
      fanOn = true;
    }
    else if (temperatureC >= TEMP_ON_MED) {
      fanPWM = 180;   // medium
      fanOn = true;
    }
    else if (temperatureC >= TEMP_ON_LOW) {
      fanPWM = 120;   // low
      fanOn = true;
    }
    else if (temperatureC <= TEMP_OFF) {
      fanPWM = 0;
      fanOn = false;
    }
    // Between TEMP_OFF and TEMP_ON_LOW, keep off for simplicity
    else {
      fanPWM = 0;
      fanOn = false;
    }
  } else {
    fanPWM = 0;
    fanOn = false;
  }
}

void updateOutputs() {
  analogWrite(MOTOR_PIN, fanPWM);

  if (fanOn) {
    digitalWrite(GREEN_LED, HIGH);
    digitalWrite(RED_LED, LOW);
  } else {
    digitalWrite(GREEN_LED, LOW);
    digitalWrite(RED_LED, HIGH);
  }
}

void updateLCD() {
  lcd.clear();

  lcd.setCursor(0, 0);
  lcd.print("T:");
  lcd.print(temperatureC, 1);
  lcd.print((char)223); // degree symbol
  lcd.print("C D:");
  lcd.print(distanceCM);
  lcd.print(" ");

  lcd.setCursor(0, 1);
  if (fanOn) {
    lcd.print("Fan ON ");
    if (fanPWM >= 255) {
      lcd.print("HIGH");
    } else if (fanPWM >= 180) {
      lcd.print("MED ");
    } else {
      lcd.print("LOW ");
    }
  } else {
    if (personNearby) {
      lcd.print("Standby: Cool");
    } else {
      lcd.print("No Person Near ");
    }
  }
}