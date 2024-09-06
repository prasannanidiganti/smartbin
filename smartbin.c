#define BLYNK_TEMPLATE_ID "TMPL3UeCdZwdC"
#define BLYNK_TEMPLATE_NAME "SMART DUST BIN"
#define BLYNK_AUTH_TOKEN "iliJShAC7nHo-3PI6pohYRpRypK_FMP6"

#define BLYNK_PRINT Serial

#include <WiFi.h>
#include <WiFiClient.h>
#include <BlynkSimpleEsp32.h>
#include <ESP32Servo.h> // Include the ESP32 Servo library

char auth[] = BLYNK_AUTH_TOKEN;
char ssid[] = "iQO1";
char pass[] = "ranjith19";

BlynkTimer timer;
Servo servo;  // Use ESP32_Servo instead of Servo

#define echoPin 32
#define trigPin 33
#define irPin 34 // Define IR sensor pin

long duration;
int distance;
int binLevel = 0;

void SMESensor()
{
    int ir = digitalRead(irPin);
    Serial.print("IR Sensor State: ");
    Serial.println(ir);
    Serial.print("Bin Level: ");
    Serial.println(binLevel);

    if (ir == LOW)
    {    
        if (binLevel < 85) // Check if bin level is below 85%
        {
            Serial.println("IR LOW, rotating servo to 90 degrees...");
            servo.write(90);
            for (int i = 0; i < 50; i++)
            {
                Blynk.virtualWrite(V2, 90);
                ultrasonic();
                delay(100);
            }
            servo.write(0);
            Serial.println("IR LOW, rotating servo to 0 degrees...");
            Blynk.virtualWrite(V2, 0);
        }
        else
        {
            Serial.println("Bin level is above 85%, logging event...");
            Blynk.virtualWrite(V2, 0); // Ensure V2 is set to 0 when the bin level is above 85%
            Blynk.logEvent("bin_almost_full", "The bin is almost full!");
        }
    }
    if (ir == HIGH)
    {
        ultrasonic();
        delay(200);
    }
}

void ultrasonic()
{
    digitalWrite(trigPin, LOW);
    delayMicroseconds(2);
    digitalWrite(trigPin, HIGH);
    delayMicroseconds(10);
    digitalWrite(trigPin, LOW);
    duration = pulseIn(echoPin, HIGH);
    distance = duration * 0.034 / 2; // Formula to calculate the distance for ultrasonic sensor
    binLevel = map(distance, 27, 0, 0, 100);
    Blynk.virtualWrite(V0, distance);
    Blynk.virtualWrite(V1, binLevel);

    Serial.print("Distance: ");
    Serial.println(distance);
    Serial.print("Bin Level: ");
    Serial.println(binLevel);
}

void checkWiFi()
{
    if (WiFi.status() != WL_CONNECTED)
    {
        Serial.println("WiFi not connected. Reconnecting...");
        WiFi.disconnect();
        WiFi.begin(ssid, pass);
        while (WiFi.status() != WL_CONNECTED)
        {
            delay(500);
            Serial.print(".");
        }
        Serial.println("Reconnected to WiFi.");
    }
}

void setup()
{
    Serial.begin(9600);
    servo.attach(13);
    pinMode(irPin, INPUT);
    pinMode(trigPin, OUTPUT);
    pinMode(echoPin, INPUT);
    Blynk.begin(auth, ssid, pass);
    delay(2000);
    timer.setInterval(1000L, SMESensor);
    timer.setInterval(10000L, checkWiFi); // Check WiFi connection every 10 seconds
}

void loop()
{
    Blynk.run();
    timer.run();
}