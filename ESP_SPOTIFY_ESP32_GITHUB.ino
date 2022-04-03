/*******************************************************************
Spotify parts:
    Written by Brian Lough
    YouTube: https://www.youtube.com/brianlough
    Tindie: https://www.tindie.com/stores/brianlough/
    Twitter: https://twitter.com/witnessmenow

Other code by Heikki Hietala    
 *******************************************************************/

// ----------------------------
// Standard Libraries
// ----------------------------

#if defined(ESP8266)
#include <ESP8266WiFi.h>
#elif defined(ESP32)
#include <WiFi.h>
#endif
#include <stdlib.h>
#include <WiFiClientSecure.h>
#include <GxEPD.h>
#include <GxGDEH029A1/GxGDEH029A1.h>      // 2.9" b/w
#include <GxIO/GxIO_SPI/GxIO_SPI.h>
#include <GxIO/GxIO.h>

//fonts
#include <Fonts/FreeMonoBold9pt7b.h>
#include <Fonts/FreeMonoBold12pt7b.h>
#include <Fonts/FreeMonoBold18pt7b.h>
#include <Fonts/FreeMonoBold24pt7b.h>

#include <Fonts/FreeSans9pt7b.h>
#include <Fonts/FreeSans12pt7b.h>
#include <Fonts/FreeSans18pt7b.h>
#include <Fonts/FreeSans24pt7b.h>

#include <Fonts/FreeSansBold9pt7b.h>
#include <Fonts/FreeSansBold12pt7b.h>
#include <Fonts/FreeSansBold18pt7b.h>
#include <Fonts/FreeSansBold24pt7b.h>

#include <Fonts/FreeSansBoldOblique9pt7b.h>
#include <Fonts/FreeSansBoldOblique12pt7b.h>
#include <Fonts/FreeSansBoldOblique18pt7b.h>
#include <Fonts/FreeSansBoldOblique24pt7b.h>

const char* maxfontmono1 = "FreeMonoBold24pt7b";
const GFXfont* maxfmono = &FreeMonoBold24pt7b;
const char* midfontmono2 = "FreeMonoBold18pt7b";
const GFXfont* midfmono = &FreeMonoBold18pt7b;
const char* smallfontmono3 = "FreeMonoBold12pt7b";
const GFXfont* smallfmono = &FreeMonoBold12pt7b;
const char* tinyfontmono4 = "FreeMonoBold9pt7b";
const GFXfont* tinyfmono = &FreeMonoBold9pt7b;

const char* maxfontsans1 = "FreeSans24pt7b";
const GFXfont* maxfsans = &FreeSans24pt7b;
const char* midfontsans2 = "FreeSans18pt7b";
const GFXfont* midfsans = &FreeSans18pt7b;
const char* smallfontsans3 = "FreeSans12pt7b";
const GFXfont* smallfsans = &FreeSans12pt7b;
const char* tinyfontsans4 = "FreeSans9pt7b";
const GFXfont* tinyfsans = &FreeSans9pt7b;

const char* maxfontsansbold1 = "FreeSansBold24pt7b";
const GFXfont* maxfsansbold = &FreeSansBold24pt7b;
const char* midfontsansbold2 = "FreeSansBold18pt7b";
const GFXfont* midfsansbold = &FreeSansBold18pt7b;
const char* smallfontsansbold3 = "FreeSansBold12pt7b";
const GFXfont* smallfsansbold = &FreeSansBold12pt7b;
const char* tinyfontsansbold4 = "FreeSansBold9pt7b";
const GFXfont* tinyfsansbold = &FreeSansBold9pt7b;

const char* maxfontsansboldOblique1 = "FreeSansBoldOblique24pt7b";
const GFXfont* maxfsansboldOblique = &FreeSansBoldOblique24pt7b;
const char* midfontsansboldOblique2 = "FreeSansBoldOblique18pt7b";
const GFXfont* midfsansboldOblique = &FreeSansBoldOblique18pt7b;
const char* smallfontsansboldOblique3 = "FreeSansBoldOblique12pt7b";
const GFXfont* smallfsansboldOblique = &FreeSansBoldOblique12pt7b;
const char* tinyfontsansboldOblique4 = "FreeSansBoldOblique9pt7b";
const GFXfont* tinyfsansboldOblique = &FreeSansBoldOblique9pt7b;

// constructor for AVR Arduino, copy from GxEPD_Example else
GxIO_Class io(SPI, /*CS=*/ SS, /*DC=*/ 17, /*RST=*/ 16); // arbitrary selection of 8, 9 selected for default of GxEPD_Class
GxEPD_Class display(io, /*RST=*/ 16, /*BUSY=*/ 4); // default selection of (9), 7


// ----------------------------
// Additional Libraries - each one of these will need to be installed.
// ----------------------------

#include <SpotifyArduino.h>
// Library for connecting to the Spotify API

// Install from Github
// https://github.com/witnessmenow/spotify-api-arduino

// including a "spotify_server_cert" variable
// header is included as part of the SpotifyArduino libary
#include <SpotifyArduinoCert.h>

#include <ArduinoJson.h>
// Library used for parsing Json from the API responses

// Search for "Arduino Json" in the Arduino Library manager
// https://github.com/bblanchon/ArduinoJson

//------- Replace the following! ------

char ssid[] = "YOUR_SSID";         // your network SSID (name)
char password[] = "YOUR_PASSWORD"; // your network password

char clientId[] = "YOUR_CLIENT_ID";     // Your client ID of your spotify APP
char clientSecret[] = "YOUR_SECRET_ID"; // Your client Secret of your spotify APP (Do Not share this!)

// Country code, including this is advisable
#define SPOTIFY_MARKET "FI"

#define SPOTIFY_REFRESH_TOKEN "YOUR_REFRESH_TOKEN"

//------- ---------------------- ------

WiFiClientSecure client;
SpotifyArduino spotify(client, clientId, clientSecret, SPOTIFY_REFRESH_TOKEN);

unsigned long delayBetweenRequests = 3000; // Time between requests (3 seconds)
unsigned long requestDueTime;               //time when request due

String myArtist, mySong, myOldSong, myAlbum;

boolean paused = false;

void setup()
{
  /******************************* START EPAPER SETUP ***************************************/
  display.init();

  // comment out next line to have no or minimal Adafruit_GFX code

  /************************************ END EPAPER SETUP ****************************/
  display.setRotation(1);
  display.fillScreen(GxEPD_WHITE);
  display.setTextColor(GxEPD_BLACK);
  display.setFont(smallfsansbold);
  display.setCursor(30, 30);
  display.println("Spotify Song Display");
  display.println();
  display.setCursor(60, 70);
  display.setFont(tinyfsans);
  display.println("Entering network and");
  display.setCursor(60, 90);
  display.println("getting Spotify data...");
  display.update();  Serial.begin(115200);
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);
  Serial.println("");

  // Wait for connection
  while (WiFi.status() != WL_CONNECTED)
  {
    delay(500);
    Serial.print(".");
  }

  Serial.println("");
  Serial.print("Connected to ");
  Serial.println(ssid);
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());

  // Handle HTTPS Verification
#if defined(ESP8266)
  client.setFingerprint(SPOTIFY_FINGERPRINT); // These expire every few months
#elif defined(ESP32)
  client.setCACert(spotify_server_cert);
#endif
  // ... or don't!
  //client.setInsecure();

  // If you want to enable some extra debugging
  // uncomment the "#define SPOTIFY_DEBUG" in ArduinoSpotify.h


  Serial.println("Refreshing Access Tokens");
  if (!spotify.refreshAccessToken())
  {
    Serial.println("Failed to get access tokens");
  }

}

void drawSpotifyData()
{
//  if (paused = true) {
//    display.fillScreen(GxEPD_WHITE);
//    display.setTextColor(GxEPD_BLACK);
//    display.setFont(tinyfsans);
//    display.setCursor(0, 20);
//    display.println("Spotify is not playing a song.");
//    display.update();
//  }
//  else {
    display.setFont(tinyfsans);
    display.setCursor(0, 20);
    display.println("Song: " + mySong);
    display.println();
    display.println("Artist: " + myArtist);
    display.println("Album: " + myAlbum);
}


void printCurrentlyPlayingToSerial(CurrentlyPlaying currentlyPlaying)
{
  // Use the details in this method or if you want to store them
  // make sure you copy them (using something like strncpy)
  // const char* artist =

  Serial.println("-------- - Currently Playing -------- -");

  Serial.print("Is Playing: ");
  if (currentlyPlaying.isPlaying)
  {
    paused = false;
    Serial.println("Yes");
  }
  else
  {
    paused = true;
    Serial.println("No");
  }

  Serial.print("Track: ");
  Serial.println(currentlyPlaying.trackName);
  mySong = currentlyPlaying.trackName;
  Serial.println("Artists: ");
  for (int i = 0; i < currentlyPlaying.numArtists; i++)
  {
    Serial.print("Name: ");
    Serial.println(currentlyPlaying.artists[i].artistName);
  }
  myArtist = currentlyPlaying.artists[0].artistName;

  Serial.print("Album: ");
  Serial.println(currentlyPlaying.albumName);
  myAlbum = currentlyPlaying.albumName;

  long progress = currentlyPlaying.progressMs; // duration passed in the song
  long duration = currentlyPlaying.durationMs; // Length of Song
  Serial.print("Elapsed time of song (ms): ");
  Serial.print(progress);
  Serial.print(" of ");
  Serial.println(duration);
  Serial.println();
  if (mySong != myOldSong) {
    display.drawPaged(drawSpotifyData); // version for AVR using paged drawing, works also on other processors
  }


  //delayBetweenRequests = (duration - progress) + 1000;

  float percentage = ((float)progress / (float)duration) * 100;
  int clampedPercentage = (int)percentage;
  Serial.print(" < ");
  for (int j = 0; j < 50; j++)
  {
    if (clampedPercentage >= (j * 2))
    {
      Serial.print(" = ");
    }
    else
    {
      Serial.print(" - ");
    }
  }
  Serial.println(" > ");
  Serial.println();
  myOldSong = mySong;
}
void loop()
{
  if (millis() > requestDueTime)
  {
    Serial.print("Free Heap: ");
    Serial.println(ESP.getFreeHeap());

    Serial.println("getting currently playing song: ");
    // Market can be excluded if you want e.g. spotify.getCurrentlyPlaying()
    int status = spotify.getCurrentlyPlaying(printCurrentlyPlayingToSerial, SPOTIFY_MARKET);
    if (status == 200)
    {
      Serial.println("Successfully got currently playing");
    }
    else if (status == 204)
    {
      Serial.println("Doesn't seem to be anything playing");
    }
    else
    {
      Serial.print("Error: ");
      Serial.println(status);
    }
    requestDueTime = millis() + delayBetweenRequests;
    Serial.println(delayBetweenRequests);
  }
}
