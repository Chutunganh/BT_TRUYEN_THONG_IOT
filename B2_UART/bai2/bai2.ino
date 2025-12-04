
#define BUTTON 2
#define LED 3
long boardrate = 9600;
unsigned long curTime = 0;
const int holdTime = 3000;
int chedo = 0;
bool isPress = 0;
bool settingState = 0;

// --- Thêm biến để nháy LED liên tục ---
unsigned long lastBlink = 0;
bool ledState = false;

void setup() {
  pinMode(BUTTON, INPUT_PULLUP);
  pinMode(LED, OUTPUT);
  digitalWrite(LED, 1);
  Serial.begin(boardrate);
  Serial.println("BR: " + String(boardrate));
}

void loop() {
  int btnState = digitalRead(BUTTON);

  if (btnState == LOW)
  {
    if (!isPress)
    {
      isPress = 1;
      curTime = millis();
    }
    if (millis() - curTime >= holdTime)
    {
      if (settingState == 0)
      {
        settingState = 1;
        Serial.println("|| Start ||");
        curTime = millis();
        for (int i = 0; i < 4; i++)
        {
          digitalWrite(LED, 0);
          delay(50);
          digitalWrite(LED, 1);
          delay(50);
        }
      }
      else
      {
        settingState = 0;
        Serial.println("|| ESC ||");
        curTime = millis();
      }
      isPress = 0;
    }
  }
  else
  {
    if (isPress == 1)
    {
      if (settingState)
      {
        chedo += 1;
        if (chedo > 1)
          chedo = 0;
        switch (chedo)
        {
          case 0:
          {
            boardrate = 9600;
            Serial.println("Dang chuyen sang " + String(boardrate));
            digitalWrite(LED, 0);
            delay(300);
            digitalWrite(LED, 1);
            delay(500);
            Serial.println("Da chuyen sang " + String(boardrate));
            break;
          }
          case 1:
          {
            boardrate = 115200;
            Serial.println("Dang chuyen sang " + String(boardrate));
            for (int i = 0; i < 2; i++)
            {
              digitalWrite(LED, 0);
              delay(150);
              digitalWrite(LED, 1);
              delay(150);
            }
            Serial.println("Da chuyen sang " + String(boardrate));
            break;
          }
        }
      }
    }
    isPress = 0;
  }

  // --- Thêm phần nháy LED liên tục theo boardrate ---
  unsigned long blinkDelay = (boardrate == 9600) ? 600 : 200; 
  if (millis() - lastBlink >= blinkDelay) {
    ledState = !ledState;
    digitalWrite(LED, ledState);
    lastBlink = millis();
  }

  delay(10); 
}