#include <ESP8266WiFi.h>
#include <NTPClient.h>
#include <WiFiUdp.h>
#include <time.h>

#define LED D5

//#define PT_DEBUG

char* ssid = "22-23-UNC-PSK";
const char* password = "ReachHighAndDreamBig";

WiFiUDP ntpUDP;
NTPClient timeClient(ntpUDP, "pool.ntp.org");

#define SECS_IN_MIN 60
#define SECS_IN_HR 3600
#define SECS_IN_DAY 86400
#define TIME_OFFSET -18000
struct tm currTime;
int lastSec = -1;
bool isNextPrime = false;

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
  delay(1000);
  pinMode(LED, OUTPUT);
  turnOffLED();
  Serial.begin(115200);
  Serial.println("Beginning setup...");
  Serial.print("Joining WiFi network with SSID ");
  Serial.print(ssid);
  Serial.print(" and password ");
  Serial.print(password);
  Serial.print("...\n");
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);
  Serial.print("Connected to WiFi network. Checking WiFi for successful Internet connection");
  while (WiFi.status() != WL_CONNECTED) {
    Serial.print(".");
    delay(500);
  }
  Serial.print("\n");
  Serial.println("Internet connection successful. Syncing RTC with NTP server...");
  timeClient.begin();
  timeClient.setTimeOffset(TIME_OFFSET);
  Serial.println("RTC synced with NTP server. Setup complete.");
}
#else
void setup() {
  delay(1000);
  pinMode(LED, OUTPUT);
  turnOffLED();
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
  }
  timeClient.begin();
  timeClient.setTimeOffset(TIME_OFFSET);
}
#endif

void loop() {
  timeClient.update();
  time_t epochTime = timeClient.getEpochTime();
  struct tm *currTime = gmtime ((time_t *)&epochTime);
  int currSec = currTime->tm_sec;
  
  if (currSec != lastSec) {
    lastSec = currSec;
    if (isNextPrime) {
      turnOnLED();
    } else {
      turnOffLED();
    }
    
    unsigned long nextSecsIntoYear = 1 + currSec + (currTime->tm_min*SECS_IN_MIN) + (currTime->tm_hour*SECS_IN_HR) + (currTime->tm_yday*SECS_IN_DAY);
    isNextPrime = isPrime(nextSecsIntoYear);
    
    #ifdef PT_DEBUG
    Serial.print("Next secs: ");
    Serial.print(nextSecsIntoYear);
    if (isNextPrime) {
      Serial.print(" [PRIME]");
    }
    Serial.println();
    #endif
  }
}