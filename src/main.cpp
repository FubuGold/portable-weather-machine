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
const int TimeToCount = 1000; // Count every 1 second
const double RotorDiameter = 0.8;
const double pi = 3.14159;

// Define pin
const int PinDT = 5; // Set this to interrupt
const int PinBtn = 10;
const int num_of_state = 3;

volatile unsigned int pulse;
unsigned short screen_state = 0;

LiquidCrystal_I2C screen(0x27, 16, 2); // Binary: 100111
// LiquidCrystal_I2C screen2(0x23, 16, 2); // Binary: 100011
// LiquidCrystal_I2C screen3(0x25, 16, 2); // Binary: 100101
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

void count_pulse(){
  pulse++;
}

double get_wind_speed()
{
    pulse = 0;
    interrupts();
    delay(TimeToCount);
    noInterrupts();
    return pi/30 * pulse * RotorDiameter;
}

void change_state()
{
    screen.clear();
    screen_state = (screen_state + 1) % num_of_state;
}

void display_screen()
{
    // State:
    // 0 : temperature
    // 1 : hunidity
    // 2 : presure
    // 3 : wind speed
    switch (screen_state)
    {
    case 0:
    {
        double temperature = bmp.readTemperature();
        screen.setCursor(13 - num_length(temperature), 1); //This mess below should align the text to the right of the screen
        screen.print(temperature, round_digits);
        screen.write(0); screen.print("C"); // °C
        break;
    }
    
    case 1:
    {
        double pressure = bmp.readPressure();
        screen.setCursor(13 - num_length(pressure), 1);
        screen.print(pressure, round_digits);
        screen.print("Pa");
        break;
    }
    
    case 2:
    {
        double humidity = dht.readHumidity();
        if (!isnan(humidity))
        {
            screen.setCursor(13 - num_length(humidity), 1);
            screen.print(humidity, round_digits);
            screen.print("%");
        }
        break;
    }

    // case 3:
    // {
    //     double wind_speed = get_wind_speed();
    //     screen.setCursor(13 - num_length(wind_speed),1);
    //     screen.print(wind_speed, round_digits);
    //     screen.print("m/s");
    //     break;
    // }

    }

        
}

void setup()
{
    Serial.begin(9600);

    // Init rotory
    // pinMode(PinDT,INPUT);
    // attachInterrupt(0, count_pulse, RISING);

    // Init changing button
    pinMode(PinBtn, INPUT_PULLUP);

    screen.init();
    // screen2.init();
    // screen3.init();

    screen.backlight();
    // screen2.backlight();
    // screen3.backlight();

    bmp.begin(0x76);

    dht.begin(); // I have no idea

    screen.createChar(0, degree);

    screen.setCursor(2, 0);
    // screen2.setCursor(6, 0);
    // screen3.setCursor(6, 0);

    // screen1.print("Temperature");
    // screen2.print("Pressure");
    // screen1.print("Humidity");
}

void loop()
{
    int prevInput = 0;
    if (prevInput == 0 && digitalRead(PinBtn))
    {
        prevInput = 1;
        change_state();
        display_screen();
    }
    prevInput = digitalRead(PinBtn);
}