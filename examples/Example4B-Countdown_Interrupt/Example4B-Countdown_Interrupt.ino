/*
  Getting the alarm to fire an interrupt on the RV-8803 Real Time Clock
  By: Andy England
  SparkFun Electronics
  Date: 2/22/2017
  License: This code is public domain but you buy me a beer if you use this and we meet someday (Beerware license).

  Feel like supporting our work? Buy a board from SparkFun!
  https://www.sparkfun.com/products/14642

  This example shows how to set an alarm and make the RTC generate an interrupt when the clock time matches the alarm time
  The INT pin will be 3.3V. When the real time matches the alarm time the INT pin will go low.

  Hardware Connections:
    Attach the Qwiic Shield to your Arduino/Photon/ESP32 or other
    Plug the RTC into the shield (any port)
    Open the serial monitor at 115200 baud
*/

#include <SparkFun_RV8803.h>

RV8803 rtc;

long lastInterruptTime = 0;

void setup() {

  Wire.begin();

  Serial.begin(115200);
  Serial.println("Alarm from RTC Example");

  if (rtc.begin() == false) {
    Serial.println("Something went wrong, check wiring");
  }

  //To configure a periodic interrupt, we'll need to do some math to figure out how to set our registers. We are able to change how many clock ticks we want to count (0-4095), and also the length of each clock tick. (4 options: 4096 Hz, 64 Hz, 1 Hz, 1/60 Hz)
  //The time ranges covered by each freqeuncy setting are as follows
  //4096 Hz: 244.14 uS - .9998 Second;                            244.14 uS per LSB             COUNTDOWN_TIMER_FREQUENCY_4096_HZ
  //64 Hz: 15.625 mS - 63.984 Seconds;                            15.625 mS per LSB             COUNTDOWN_TIMER_FREQUENCY_64_HZ
  //1 Hz: 1 Second - 4095 Seconds (68 minutes 16 seconds);        1 Second per LSB              COUNTDOWN_TIMER_FREQUENCY_1_HZ
  //1/60 Hz: 1 Minute = 4095 Minutes (68 hours 16 minutes)        1 Minute per LSB              COUNTDOWN_TIMER_FREQUENCY_1/60_HZ
  //As an example, we'll configure a ~3.5 second interrupt. We'll choose 60 Hz as our frequency as we want something longer than 1 second, but still want enough granularity to fire an interrupt at 3.5 seconds, not 3 or 4.
  //Since the resolution for this setting is 15.625 mS per LSB, we'll convert 3.5 seconds to 3500 ms. We'll then simply divide the time we want by the resolution to get the number of clock ticks we need to wait to fire the interrupt
  //3500 / 15.625 = 224 Clock ticks
  rtc.setCountdownTimerFrequency(COUNTDOWN_TIMER_FREQUENCY_64_HZ);
  rtc.setCountdownTimerClockTicks(224);
  rtc.enableHardwareInterrupt(TIMER_INTERRUPT);
  //rtc.disableAllInterrupts(); //Uncomment this line if you've been playing with interrupts elsewhere.
  rtc.setCountdownTimerEnable(ENABLE); //This will start the timer on the last clock tick of the I2C transaction
  lastInterruptTime = millis(); //Change millis() to micros() if you end up using the 4096 Hz counter
}

void loop() {
  if(rtc.getInterruptFlag(FLAG_TIMER))
  {
    long timeSinceLastInterrupt = millis() - lastInterruptTime; //Change millis() to micros() if you end up using the 4096 Hz counter
    lastInterruptTime = millis(); //Change millis() to micros() if you end up using the 4096 Hz counter
    rtc.clearInterruptFlag(FLAG_TIMER);
    //rtc.clearAllInterruptFlags(); // This can also be used, but beware as it will clear the entire flag register
    Serial.print("Time between interrupts: ");
    Serial.print(timeSinceLastInterrupt); 
    Serial.println(" mS");
  }
}
