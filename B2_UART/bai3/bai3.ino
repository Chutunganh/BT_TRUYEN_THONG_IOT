#include <Wire.h>
#include <RTClib.h>
#include <LiquidCrystal_I2C.h>

LiquidCrystal_I2C lcd(0x27, 16, 2);
RTC_DS1307 rtc;

// Định nghĩa các chân
#define BUZZER_PIN 6
#define MODE_PIN   2
#define UP_PIN     3
#define DOWN_PIN   4
#define SET_PIN    5

// Biến lưu trữ báo thức
byte alarmHour = 6;
byte alarmMinute = 0;
bool isAlarmOn = false;
bool isAlarmRinging = false;
bool alarmTriggered = false;

// Biến cho máy trạng thái
enum Mode {
  DISPLAY_TIME,
  SET_ALARM_HOUR,
  SET_ALARM_MINUTE
};
Mode currentMode = DISPLAY_TIME;
Mode lastMode = (Mode)-1;

// Biến cho xử lý nút nhấn
bool lastModeState = HIGH;
bool lastUpState = HIGH;
bool lastDownState = HIGH;
bool lastSetState = HIGH;

unsigned long ignoreButtonsUntil = 0;
unsigned long lastUpPress = 0;
unsigned long lastDownPress = 0;
unsigned long lastModePress = 0;
unsigned long lastSetPress = 0;
#define DEBOUNCE_DELAY 50  // Thời gian chống dội ngắn (ms)

// Biến hiệu ứng nhấp nháy (chỉ dùng cho "Dậy đi!")
bool blinkState = false;
unsigned long lastBlinkTime = 0;

void setup() {
  Serial.begin(9600);
  
  lcd.init();
  lcd.backlight();
  lcd.setCursor(0, 0);

  if (!rtc.begin()) {
    lcd.print("Khong tim thay RTC!");
    Serial.println("Khong tim thay RTC!");
    while (1);
  }

  if (!rtc.isrunning()) {
    lcd.clear();
    lcd.print("Dang chinh dong ho RTC...");
    Serial.println("Dang chinh dong ho RTC...");
    rtc.adjust(DateTime(F(__DATE__), F(__TIME__)));
    delay(2000);
  }

  pinMode(BUZZER_PIN, OUTPUT);
  digitalWrite(BUZZER_PIN, LOW);

  pinMode(MODE_PIN, INPUT_PULLUP);
  pinMode(UP_PIN, INPUT_PULLUP);
  pinMode(DOWN_PIN, INPUT_PULLUP);
  pinMode(SET_PIN, INPUT_PULLUP);

  lcd.clear();
  Serial.println("He thong san sang!");
}

void loop() {
  handleButtons();
  checkAlarm();
  updateDisplay();
  delay(20); // Giảm xuống 20ms để phản hồi nhanh hơn
}

void handleButtons() {
  if (millis() < ignoreButtonsUntil) return;

  bool modeState = (digitalRead(MODE_PIN) == LOW);
  bool upState = (digitalRead(UP_PIN) == LOW);
  bool downState = (digitalRead(DOWN_PIN) == LOW);
  bool setState = (digitalRead(SET_PIN) == LOW);

  // Tắt báo thức đang kêu (BẤT KỲ NÚT NÀO)
  if (isAlarmRinging && (modeState || upState || downState || setState)) {
    Serial.println("Bao thuc da duoc tat");
    isAlarmRinging = false;
    alarmTriggered = false;
    digitalWrite(BUZZER_PIN, LOW);
    ignoreButtonsUntil = millis() + 500;
    return;
  }

  // Nút MODE
  if (modeState && !lastModeState && (millis() - lastModePress > DEBOUNCE_DELAY)) {
    Serial.println("Nút MODE nhấn");
    if (currentMode == DISPLAY_TIME) {
      currentMode = SET_ALARM_HOUR;
      Serial.println("Chuyen sang chinh gio bao thuc");
    }
    lastModePress = millis();
  }

  // Nút UP
  if (upState && !lastUpState && (millis() - lastUpPress > DEBOUNCE_DELAY)) {
    if (currentMode == SET_ALARM_HOUR) {
      alarmHour = (alarmHour + 1) % 24;
      Serial.print("Gio bao thuc: "); Serial.println(alarmHour);
      updateDisplay();
    } else if (currentMode == SET_ALARM_MINUTE) {
      alarmMinute = (alarmMinute + 1) % 60;
      Serial.print("Phut bao thuc: "); Serial.println(alarmMinute);
      updateDisplay();
    } else if (currentMode == DISPLAY_TIME) {
      isAlarmOn = !isAlarmOn;
      Serial.print("Bao thuc: ");
      Serial.println(isAlarmOn ? "BAT" : "TAT");
      updateDisplay();
    }
    lastUpPress = millis();
  }

  // Nút DOWN
  if (downState && !lastDownState && (millis() - lastDownPress > DEBOUNCE_DELAY)) {
    if (currentMode == SET_ALARM_HOUR) {
      alarmHour = (alarmHour == 0) ? 23 : alarmHour - 1;
      Serial.print("Gio bao thuc: "); Serial.println(alarmHour);
      updateDisplay();
    } else if (currentMode == SET_ALARM_MINUTE) {
      alarmMinute = (alarmMinute == 0) ? 59 : alarmMinute - 1;
      Serial.print("Phut bao thuc: "); Serial.println(alarmMinute);
      updateDisplay();
    }
    lastDownPress = millis();
  }

  // Nút SET
  if (setState && !lastSetState && (millis() - lastSetPress > DEBOUNCE_DELAY)) {
    Serial.println("Nút SET nhấn");
    if (currentMode == SET_ALARM_HOUR) {
      currentMode = SET_ALARM_MINUTE;
      Serial.println("Chuyen sang chinh phut bao thuc");
    } else if (currentMode == SET_ALARM_MINUTE) {
      currentMode = DISPLAY_TIME;
      isAlarmOn = true;
      Serial.println("Luu va bat bao thuc!");
      Serial.print("Bao thuc dat vao: ");
      Serial.print(alarmHour);
      Serial.print(":");
      Serial.println(alarmMinute);
    }
    lastSetPress = millis();
  }

  lastModeState = modeState;
  lastUpState = upState;
  lastDownState = downState;
  lastSetState = setState;
}

void checkAlarm() {
  if (isAlarmRinging) {
    if (millis() % 1000 < 200) digitalWrite(BUZZER_PIN, HIGH);
    else digitalWrite(BUZZER_PIN, LOW);
    return;
  }

  if (isAlarmOn && currentMode == DISPLAY_TIME) {
    DateTime now = rtc.now();
    
    if (now.hour() == alarmHour && now.minute() == alarmMinute && now.second() < 5 && !alarmTriggered) {
      Serial.println("BAO THUC KICH HOAT!");
      isAlarmRinging = true;
      alarmTriggered = true;
    }
    
    if (now.minute() != alarmMinute) {
      alarmTriggered = false;
    }
  }
}

void updateDisplay() {
  if (millis() - lastBlinkTime > 500) {
    blinkState = !blinkState;
    lastBlinkTime = millis();
  }

  if (currentMode != lastMode) {
    lcd.clear();
    lastMode = currentMode;
  }

  DateTime now = rtc.now();

  switch (currentMode) {
    case DISPLAY_TIME:
      lcd.setCursor(0, 0);
      lcd.print("Gio: ");
      if (now.hour() < 10) lcd.print('0');
      lcd.print(now.hour());
      lcd.print(':');
      if (now.minute() < 10) lcd.print('0');
      lcd.print(now.minute());
      lcd.print(':');
      if (now.second() < 10) lcd.print('0');
      lcd.print(now.second());

      lcd.setCursor(0, 1);
      lcd.print("Bao thuc: ");
      if (alarmHour < 10) lcd.print('0');
      lcd.print(alarmHour);
      lcd.print(':');
      if (alarmMinute < 10) lcd.print('0');
      lcd.print(alarmMinute);
      lcd.print(isAlarmOn ? " BAT " : " TAT ");

      if (isAlarmRinging && blinkState) {
        lcd.setCursor(0, 1);
        lcd.print("DAY DI BAN OI!");
      }
      break;

    case SET_ALARM_HOUR:
      lcd.setCursor(0, 0);
      lcd.print("Chinh gio");
      lcd.setCursor(0, 1);
      lcd.print(">");
      if (alarmHour < 10) lcd.print('0');
      lcd.print(alarmHour);
      lcd.print(":");
      if (alarmMinute < 10) lcd.print('0');
      lcd.print(alarmMinute);
      lcd.print(" SET>Phut ");
      break;

    case SET_ALARM_MINUTE:
      lcd.setCursor(0, 0);
      lcd.print("Chinh phut");
      lcd.setCursor(0, 1);
      if (alarmHour < 10) lcd.print('0');
      lcd.print(alarmHour);
      lcd.print(":>");
      if (alarmMinute < 10) lcd.print('0');
      lcd.print(alarmMinute);
      lcd.print(" SET>Luu");
      break;
  }
}
