//teensy LC
#include <NSegmentDisplay.h>
#include <RTClib.h>

RTC_PCF8523 rtc;
//common anode display with 9 segments
const int NUM_SEG_PINS = 9;
const int NUM_DIGIT_PINS = 4;
const int segments[] = {8, 7, 6, 5, 4, 3, 2, 1, 0};
const int digits[] = {9, 10, 11, 12};
NSegmentDisplay disp(true, NUM_SEG_PINS, segments, NUM_DIGIT_PINS, digits);
//const int unusedPins[] = {13,14,15,16,17,18,19,20,21,22,23,24,25};

char daysOfTheWeek[7][12] = {"Sunday", "Monday", "Tuesday", "Wednesday", "Thursday", "Friday", "Saturday"};
int hr = 0;
//colon blink timing
int onColon = 0;
const int delayTime = 1000;
double lastChange = 0;
//scrollign message timing
const int waitTime = 1000;
double lastTime = 0;
// for hourly animation
double animate = 0;
const int holdAnimation = 10000;

void setup() {
  Serial.begin(9600);
  if (! rtc.begin()) {
    Serial.println("Couldn't find RTC");
    Serial.flush();
    abort();
  }

  if (! rtc.initialized() || rtc.lostPower()) {
    Serial.println("RTC is NOT initialized, let's set the time!");
    // When time needs to be set on a new device, or after a power loss, the
    // following line sets the RTC to the date & time this sketch was compiled
    rtc.adjust(DateTime(F(__DATE__), F(__TIME__)));
    // This line sets the RTC with an explicit date & time, for example to set
    // January 21, 2014 at 3am you would call:
    // rtc.adjust(DateTime(2014, 1, 21, 3, 0, 0));
    //
    // Note: allow 2 seconds after inserting battery or applying external power
    // without battery before calling adjust(). This gives the PCF8523's
    // crystal oscillator time to stabilize. If you call adjust() very quickly
    // after the RTC is powered, lostPower() may still return true.
  }

  // When time needs to be re-set on a previously configured device, the
  // following line sets the RTC to the date & time this sketch was compiled
//     rtc.adjust(DateTime(F(__DATE__), F(__TIME__)));
  // This line sets the RTC with an explicit date & time, for example to set
  // January 21, 2014 at 3am you would call:
  // rtc.adjust(DateTime(2014, 1, 21, 3, 0, 0));

  // When the RTC was stopped and stays connected to the battery, it has
  // to be restarted by clearing the STOP bit. Let's do this to ensure
  // the RTC is running.
  rtc.start();

  // The PCF8523 can be calibrated for:
  //        - Aging adjustment
  //        - Temperature compensation
  //        - Accuracy tuning
  // The offset mode to use, once every two hours or once every minute.
  // The offset Offset value from -64 to +63. See the Application Note for calculation of offset values.
  // https://www.nxp.com/docs/en/application-note/AN11247.pdf
  // The deviation in parts per million can be calculated over a period of observation. Both the drift (which can be negative)
  // and the observation period must be in seconds. For accuracy the variation should be observed over about 1 week.
  // Note: any previous calibration should cancelled prior to any new observation period.
  // Example - RTC gaining 43 seconds in 1 week
  float drift = 43; // seconds plus or minus over oservation period - set to 0 to cancel previous calibration.
  float period_sec = (7 * 86400);  // total obsevation period in seconds (86400 = seconds in 1 day:  7 days = (7 * 86400) seconds )
  float deviation_ppm = (drift / period_sec * 1000000); //  deviation in parts per million (μs)
  float drift_unit = 4.34; // use with offset mode PCF8523_TwoHours
  // float drift_unit = 4.069; //For corrections every min the drift_unit is 4.069 ppm (use with offset mode PCF8523_OneMinute)
  int offset = round(deviation_ppm / drift_unit);
  // rtc.calibrate(PCF8523_TwoHours, offset); // Un-comment to perform calibration once drift (seconds) and observation period (seconds) are correct
  // rtc.calibrate(PCF8523_TwoHours, 0); // Un-comment to cancel previous calibration

  //Serial.print("Offset is "); Serial.println(offset); // Print to control offset
  //  for(int i = 0; i<13; i++) {
  //    pinMode(unusedPins[i], OUTPUT);
  //  }
  //  for(int i = 0; i<13; i++) {
  //    digitalWrite(unusedPins[i], LOW);
  //  }
}

const uint8_t alpha[27][9] {
  {1, 1, 0, 0, 0, 1, 0, 1, 1}, //a
  {1, 1, 0, 0, 0, 0, 0, 1, 1}, //b
  {1, 1, 1, 0, 0, 1, 0, 1, 1}, //c
  {1, 0, 0, 0, 0, 1, 0, 1, 1}, //d
  {0, 1, 1, 0, 0, 0, 0, 1, 1}, //e
  {0, 1, 1, 1, 0, 0, 0, 1, 1}, //f
  {0, 0, 0, 0, 1, 0, 0, 1, 1}, //g
  {1, 1, 0, 1, 0, 0, 0, 1, 1}, //h
  {1, 1, 0, 1, 1, 1, 1, 1, 1}, //i
  {1, 0, 0, 0, 1, 1, 1, 1, 1}, //j
  {1, 0, 0, 1, 0, 0, 0, 1, 1}, //k
  {1, 0, 0, 1, 1, 1, 1, 1, 1}, //l
  {0, 1, 1, 0, 0, 0, 0, 1, 1}, //m
  {1, 1, 0, 1, 0, 1, 0, 1, 1}, //n
  {1, 1, 0, 0, 0, 1, 0, 1, 1}, //o
  {0, 0, 1, 1, 0, 0, 0, 1, 1}, //p
  {0, 0, 0, 1, 1, 0, 0, 1, 1}, //q
  {1, 1, 1, 1, 0, 1, 0, 1, 1}, //r
  {0, 1, 0, 0, 1, 0, 0, 1, 1}, //s
  {1, 1, 1, 0, 0, 0, 0, 1, 1}, //t
  {1, 1, 0, 0, 0, 1, 1, 1, 1}, //u
  {1, 1, 0, 0, 0, 1, 1, 1, 1}, //v
  {0, 0, 0, 0, 1, 1, 0, 1, 1}, //w
  {1, 0, 0, 1, 0, 0, 0, 1, 1}, //x
  {1, 0, 0, 0, 1, 0, 0, 1, 1}, //y
  {0, 1, 0, 0, 1, 0, 0, 1, 1}, //z
  {1, 1, 1, 1, 1, 1, 1, 1, 1} // space
};

void loop() {
  DateTime now = rtc.now();
//  Serial.print("hour: ");
//  Serial.print(now.hour());
//  Serial.print(" minute: ");
//  Serial.println(now.minute());

  if (now.minute() == 59 && now.second() == 59) {
    animate = millis();
  }
  if (now.minute() == 0 && millis() < animate + holdAnimation) {
    scrollingMessage(daysOfTheWeek[now.dayOfTheWeek()]);
  } else { 
    //convert to 12 hr
    if (now.hour() > 12) {
      hr = now.hour() - 12;
    } else if(now.hour() == 0) {
      hr = 12;
    } else {
      hr = now.hour();
    }
    disp.multiDigitNumber(hr, 2);
    colon();
    if (now.minute() < 10) disp.number(1, 0);
    disp.multiDigitNumber(now.minute());
  }
}

void colon() {
  if (onColon == 0) {
    disp.segment(1, 8, 2);
    disp.segment(2, 7, 2);
  }
  if (millis() > lastChange + delayTime) {
    onColon = !onColon;
    lastChange = millis();
  }
}

byte segment = 0;
void scrollingMessage(String text) {
  String newText = "    " + text;
  int len = newText.length();
  if (millis() > lastTime + waitTime / 4) {
    segment++;
    lastTime = millis();
  }
  if (segment > len) segment = 0;
  messageSegment(newText.substring(segment, segment + 4));
}
// input substring and display over 4 digits
void messageSegment(String mesg) {
  mesg.toLowerCase();
  letter(0, mesg[3]);
  letter(1, mesg[2]);
  letter(2, mesg[1]);
  letter(3, mesg[0]);
}

void letter(int whichDigit, char whichLetter) {
  for (int i = 0; i < sizeof(digits) / sizeof(digits[0]); i++) {
    if (i == whichDigit) {
      digitalWrite(digits[i], HIGH);
    } else {
      digitalWrite(digits[i], LOW);
    }
  }
  int tempLetter;
  switch (whichLetter) {
    case 'a':
      tempLetter = 0;
      break;
    case 'b':
      tempLetter = 1;
      break;
    case 'c':
      tempLetter = 2;
      break;
    case 'd':
      tempLetter = 3;
      break;
    case 'e':
      tempLetter = 4;
      break;
    case 'f':
      tempLetter = 5;
      break;
    case 'g':
      tempLetter = 6;
      break;
    case 'h':
      tempLetter = 7;
      break;
    case 'i':
      tempLetter = 8;
      break;
    case 'j':
      tempLetter = 9;
      break;
    case 'k':
      tempLetter = 10;
      break;
    case 'l':
      tempLetter = 11;
      break;
    case 'm':
      tempLetter = 12;
      break;
    case 'n':
      tempLetter = 13;
      break;
    case 'o':
      tempLetter = 14;
      break;
    case 'p':
      tempLetter = 15;
      break;
    case 'q':
      tempLetter = 16;
      break;
    case 'r':
      tempLetter = 17;
      break;
    case 's':
      tempLetter = 18;
      break;
    case 't':
      tempLetter = 19;
      break;
    case 'u':
      tempLetter = 20;
      break;
    case 'v':
      tempLetter = 21;
      break;
    case 'w':
      tempLetter = 22;
      break;
    case 'x':
      tempLetter = 23;
      break;
    case 'y':
      tempLetter = 24;
      break;
    case 'z':
      tempLetter = 25;
      break;
    case ' ':
      tempLetter = 26;
      break;
    default:
      tempLetter = 26;
      break;
  }
  for (int i = 0; i < sizeof(alpha[0]) / sizeof(alpha[0][0]); i++) {
    digitalWrite(segments[i], alpha[tempLetter][i]);
  }
  delay(3);
}
