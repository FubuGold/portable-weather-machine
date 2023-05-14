// Please run
// This is stupid, but Im lazy. So what do you expect ?
// This code looks like shit, but Im lazy. So what do you expect ?
#include <Arduino.h>
#include <Wire.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_BMP280.h>
#include <DHT_U.h>
#include <LiquidCrystal_I2C.h>

/*Define some variables*/

const int round_digits = 1;

LiquidCrystal_I2C screen1(0x27, 16, 2); // Binary: 100111
LiquidCrystal_I2C screen2(0x23, 16, 2); // Binary: 100011
LiquidCrystal_I2C screen3(0x25, 16, 2); // Binary: 100101
Adafruit_BMP280 bmp;                    // The address is maybe 0x77 or 0x76, just test both of them ¯\_( ͡° ͜ʖ ͡°)_/¯
DHT dht(2, DHT21);                      // I have no idea what this is

byte degree[8] = {
    0b00000,
    0b00111,
    0b00101,
    0b00111,
    0b00000,
    0b00000,
    0b00000,
    0b00000};

/*Main code*/

// Length of a number as a string
int num_length(double number)
{
    int temp = (int)number;
    int length = (temp == 0) or (temp < 0); // the length should be set to 1 first if the number is zero
                                            // because the loop below will be terminated immediate.
                                            // if the number is negative, then the length is included the "-" character
    while (temp != 0)
    {
        temp /= 10;
        length++;
    }

    return length + round_digits * (round_digits > 0) + (round_digits > 0); // return the length of the number plus the decimal part and
                                                                            // decimal point if there are digits beyond
}

void setup()
{
    Serial.begin(9600);

    screen1.init();
    screen2.init();
    screen3.init();

    screen1.backlight();
    screen2.backlight();
    screen3.backlight();

    bmp.begin(0x76);

    dht.begin(); // I have no idea

    screen1.createChar(0, degree);

    screen1.setCursor(2, 0);
    screen2.setCursor(6, 0);
    screen3.setCursor(6, 0);

    screen1.print("Temperature");
    screen2.print("Pressure");
    screen1.print("Humidity");
}

void loop()
{
    double temperature = bmp.readTemperature();
    double pressure = bmp.readPressure();
    double humidity = dht.readHumidity();

    screen1.setCursor(13 - num_length(temperature), 1); //This mess below should align the text to the right of the screen
    screen1.print(temperature, round_digits);
    screen1.write(0); screen1.print("C"); // °C
    Serial.print(temperature, round_digits);

    screen2.setCursor(13 - num_length(pressure), 1);
    screen2.print(pressure, round_digits);
    screen2.print("Pa");

    if (!isnan(humidity))
    {
        screen3.setCursor(13 - num_length(humidity), 1);
        screen3.print(humidity, round_digits);
        screen3.print("%");
        Serial.print(humidity, round_digits);
    }
}