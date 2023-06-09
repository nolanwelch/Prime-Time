#include <ESP8266WiFi.h>
#include <NTPClient.h>
#include <WiFiUdp.h>
#include <time.h>

#define LED D5

//#define PT_DEBUG

char* ssid = "YourWiFiNameHere";
const char* password = "YourPasswordHere";

#define SECS_IN_MIN 60
#define SECS_IN_HR 3600
#define SECS_IN_DAY 86400
#define GMT_OFFSET 0 // Offset from GMT in seconds
#define SYNC_INTERVAL 30000 // NTP fetch interval in ms
#define RESYNC_SLEEP 300000 // Time to sleep after failed sync in ms
struct tm currTime;
int lastSec = -1;
bool isNextPrime = false;

WiFiUDP ntpUDP;
NTPClient timeClient(ntpUDP, "pool.ntp.org", GMT_OFFSET, SYNC_INTERVAL);

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

void debugPrint(var val) {
  #ifdef PT_DEBUG
  Serial.print(val);
  #endif
}

void debugPrintln(var val) {
  #ifdef PT_DEBUG
  Serial.println(val);
  #endif
}

void setup() {
  delay(1000);
  pinMode(LED, OUTPUT);
  turnOffLED();
  Serial.begin(115200);
  debugPrintln("Beginning setup...");
  debugPrint("Joining WiFi network with SSID ");
  debugPrint(ssid);
  debugPrint(" and password ");
  debugPrint(password);
  debugPrint("...\n");
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);
  debugPrint("Connected to WiFi network. Checking WiFi for successful Internet connection");
  while (WiFi.status() != WL_CONNECTED) {
    debugPrint(".");
    delay(500);
  }
  debugPrint("\n");
  debugPrintln("Internet connection successful. Starting NTP client...");
  timeClient.begin();
  debugPrintln("NTP client started. Setup complete.");
}

void loop() {
  bool updated = timeClient.update();
  
  if (!updated) {
    debugPrintln("Unable to sync client with NTP server. Sleeping before reattempting.");
    delay(RESYNC_SLEEP);
    continue;
  }

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
    
    debugPrint("Next secs: ");
    debugPrint(nextSecsIntoYear);
    if (isNextPrime) {
      debugPrint(" [PRIME]");
    }
    debugPrintln();
  }
}
