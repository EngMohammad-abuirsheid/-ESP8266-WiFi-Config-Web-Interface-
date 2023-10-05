#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>
#include <EEPROM.h>

ESP8266WebServer server(80);

// Struct to store Wi-Fi credentials
struct WifiCredentials {
  char ssid[30];
  char password[30];
} wifiCredentials = {};

bool shouldSaveConfig = false;
bool connectRequested = false;

// Function declarations
void saveConfig();
String listAvailableNetworks();

void setup() {
  Serial.begin(115200);
  EEPROM.begin(sizeof(WifiCredentials));

  // Load saved Wi-Fi credentials from EEPROM
  EEPROM.get(0, wifiCredentials);

  // Start in AP mode to configure Wi-Fi
  WiFi.mode(WIFI_AP);
  WiFi.softAP("SEB3AWY", "12345678");

  // Define the configuration page
  server.on("/", HTTP_GET, handleConfigPortal);
  server.on("/save", HTTP_POST, handleSaveConfig);
  server.on("/connect", HTTP_POST, handleConnect);

  server.begin();
}

void loop() {
  server.handleClient();

  if (shouldSaveConfig) {
    saveConfig();
    shouldSaveConfig = false;
  }

  // Check if a connection request has been made
  if (connectRequested) {
    // Switch to STA mode and connect to Wi-Fi
    WiFi.mode(WIFI_STA);
    WiFi.begin(wifiCredentials.ssid, wifiCredentials.password);
    Serial.println("Connecting to Wi-Fi...");
    connectRequested = false; // Reset the connection request flag
  }

  // Check if Wi-Fi is connected
  if (WiFi.status() == WL_CONNECTED) {
    // Print Wi-Fi information
    Serial.println("Connected to Wi-Fi Network: " + String(WiFi.SSID()));
    Serial.print("IP Address: ");
    Serial.println(WiFi.localIP());
    // Your code to perform tasks when connected to Wi-Fi goes here
  } else {
    // If not connected, print a message
    Serial.println("Not connected to Wi-Fi.");
  }
  delay(5000);
}


void handleConfigPortal() {
  String html = "<html><head>";
  html += "<meta charset='UTF-8'>";
  html += "<meta name='viewport' content='width=device-width, initial-scale=1.0'>";
  html += "<title>ESP8266 Configuration</title>";
  html += "<style>";
  html += "body { font-family: Arial, sans-serif; background-color: #f2f2f2; text-align: center; margin: 0; padding: 0; }";
  html += "h1 { color: #333; }";
  html += "form { background-color: #fff; border: 1px solid #ccc; padding: 20px; border-radius: 5px; box-shadow: 0px 0px 10px rgba(0, 0, 0, 0.2); margin: 20px auto; max-width: 400px; }";
  html += "label { display: block; font-weight: bold; margin-bottom: 10px; }";
  html += "input[type='text'], input[type='password'] { width: 100%; padding: 10px; margin-bottom: 15px; border: 1px solid #ccc; border-radius: 4px; }";
  html += "input[type='submit'] { background-color: #007bff; color: #fff; border: none; padding: 12px 20px; border-radius: 4px; cursor: pointer; }";
  html += "input[type='submit']:hover { background-color: #0056b3; }";
  html += ".networks { text-align: left; }";
  html += ".network { margin-bottom: 10px; }";
  html += "footer { font-size: 14px; margin-top: 20px; color: #777; }";
  html += "</style>";
  html += "</head><body>";
  html += "<h1>ESP8266 Configuration</h1>";
  html += "<form method='post' action='/save'>";
  html += "<label for='ssid'>SSID:</label>";
  html += "<input type='text' id='ssid' name='ssid' value='" + String(wifiCredentials.ssid) + "'><br>";
  html += "<label for='password'>Password:</label>";
  html += "<input type='password' id='password' name='password' value='" + String(wifiCredentials.password) + "'><br>";
  html += "<input type='submit' value='Save'>";
  html += "</form>";
  html += "<h2>Available Wi-Fi Networks:</h2>";
  html += listAvailableNetworks();
  html += "<footer>";
  html += "<p>Made by Se3awy's Hand</p>";
  html += "<p>Made by Mohammad Abu Irsheid</p>";
  html += "</footer>";
  html += "</body></html>";

  server.send(200, "text/html", html);
}


void handleSaveConfig() {
  String ssid = server.arg("ssid");
  String password = server.arg("password");

  // Save Wi-Fi credentials
  strncpy(wifiCredentials.ssid, ssid.c_str(), sizeof(wifiCredentials.ssid));
  strncpy(wifiCredentials.password, password.c_str(), sizeof(wifiCredentials.password));

  shouldSaveConfig = true;

  String html = "<html><body>";
  html += "<h1>Configuration Saved</h1>";
  html += "<p>ESP8266 will now attempt to connect to the configured network.</p>";
  html += "</body></html>";

  server.send(200, "text/html", html);
}

void handleConnect() {
  connectRequested = true;
  String html = "<html><body>";
  html += "<h1>Connecting to Wi-Fi...</h1>";
  html += "</body></html>";

  server.send(200, "text/html", html);
  server.send(200, "text/html", html);
}

void saveConfig() {
  EEPROM.put(0, wifiCredentials);
  EEPROM.commit();
  Serial.println("Wi-Fi credentials saved.");
}

String listAvailableNetworks() {
  String networks;
  int n = WiFi.scanNetworks();

  for (int i = 0; i < n; ++i) {
    networks += "<input type='radio' name='ssid' value='" + WiFi.SSID(i) + "'> ";
    networks += WiFi.SSID(i) + " (Signal: " + String(WiFi.RSSI(i)) + " dBm)<br>";
  }

  return networks;
}
