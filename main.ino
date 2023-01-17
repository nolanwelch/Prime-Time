#include <WiFi.h>
#include <time.h>

#define LED 5

#define PT_DEBUG

const char* ntpServer = "pool.ntp.org";
char* ssid = "22-23-UNC-PSK";
const char* password = "ReachHighAndDreamBig";

void(* resetFunc) (void) = 0;

// all times in GMT
#define RST_HR 8
#define RST_MIN 0
#define SECS_IN_MIN 60
#define SECS_IN_HR 3600
#define SECS_IN_DAY 86400
struct tm currTime;
int lastSec = -1;
bool isNextPrime = false;
bool didReset = true;

// https://arduino.stackexchange.com/a/85958
bool getLocalTime(struct tm *info)
{
    uint32_t start = millis();
    time_t now;
    while((millis()-start) <= 500) {
        time(&now);
        localtime_r(&now, info);
        if(info->tm_year > (2016 - 1900)){
            return true;
        }
        delay(10);
    }
    return false;
}

bool isPrime(unsigned long n) {
  if (n <= 3) {
    return n > 1;
  } else if (n%2 == 0 || n%3==0) {
    return false;
  }
  unsigned long limit = ceil(sqrt(n));
  for (int i = 5; i < limit; i += 6) {
    if (n%i == 0 || n%(i+2) == 0) {
      return false;
    }
  }
  return true;
}

void turnOnLED() {
  digitalWrite(LED, HIGH);
}

void turnOffLED() {
  digitalWrite(LED, LOW);
}

#ifdef PT_DEBUG
void setup() {
  pinMode(LED, OUTPUT);
  Serial.begin(9600);
  Serial.println("Beginning setup...");
  Serial.print("Joining WiFi network with SSID ");
  Serial.print(ssid);
  Serial.print(" and password ");
  Serial.print(password);
  Serial.print("...\n");
  WiFi.begin(ssid, password);
  Serial.print("Connected to WiFi network. Checking WiFi for successful Internet connection");
  while (WiFi.status() != WL_CONNECTED) {
    Serial.print(".");
    delay(500);
  }
  Serial.print("\n");
  Serial.println("Internet connection successful. Syncing RTC with NTP server...");
  configTime(0, 0, ntpServer);
  Serial.println("RTC synced with NTP server. Severing WiFi connection...");
  WiFi.disconnect();
  Serial.print("WiFi connection severed. Setup complete.");
}
#else
void setup() {
  pinMode(LED, OUTPUT);
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
  }
  configTime(0, 0, ntpServer);
  WiFi.disconnect();
}
#endif

void loop() {
  getLocalTime(&currTime);
  int currSec = currTime.tm_sec;
  
  if (currSec != lastSec) {
    lastSec = currSec;
    if (isNextPrime) {
      turnOnLED();
    } else {
      turnOffLED();
    }
    
    unsigned long nextSecsIntoYear = 1 + currTime.tm_sec + (currTime.tm_min*SECS_IN_MIN) + (currTime.tm_hour*SECS_IN_HR) + (currTime.tm_yday*SECS_IN_DAY);
    isNextPrime = isPrime(nextSecsIntoYear);
    
    #ifdef PT_DEBUG
    Serial.print("Next secs: ");
    Serial.print(nextSecsIntoYear);
    if (isNextPrime) {
      Serial.print(" [PRIME]");
    }
    Serial.print("\n");
    #endif

    if (!didReset && currTime.tm_hour >= RST_HR && currTime.tm_min >= RST_MIN) {
      resetFunc();
    } else if (currTime.tm_hour == 0 && currTime.tm_min == 0) {
      didReset = false;
    }
  }
}
