#include <WiFi.h>
#include <PubSubClient.h>
#include <Wire.h>
#include <LiquidCrystal_I2C.h>

const char *ssid = "";     // SSID da rede
const char *password = ""; // Senha da rede

const char *mqttServer = "localhost";
const int mqttPort = 1883;

const int btnAUp = 15;
const int btnADown = 2;
const int btnBUp = 4;
const int btnBDown = 16;
const int btnReset = 17;

int scoreA = 0;
int scoreB = 0;

WiFiClient espClient;
PubSubClient client(espClient);
LiquidCrystal_I2C lcd(0x27, 16, 2);

void connectWiFi()
{
    WiFi.begin(ssid, password);
    while (WiFi.status() != WL_CONNECTED)
    {
        delay(500);
    }
}

void connectMQTT()
{
    while (!client.connected())
    {
        if (client.connect("ESP32_Placar"))
        {
            // Conectado
        }
        else
        {
            delay(5000);
        }
    }
}

void publishScore()
{
    String payload = "{\"scoreA\": " + String(scoreA) + ", \"scoreB\": " + String(scoreB) + "}";
    client.publish("/esp32_placar/attrs", payload.c_str());
}

void updateDisplay()
{
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("Time A: " + String(scoreA));
    lcd.setCursor(0, 1);
    lcd.print("Time B: " + String(scoreB));
}

void setup()
{
    Serial.begin(115200);

    pinMode(btnAUp, INPUT_PULLUP);
    pinMode(btnADown, INPUT_PULLUP);
    pinMode(btnBUp, INPUT_PULLUP);
    pinMode(btnBDown, INPUT_PULLUP);
    pinMode(btnReset, INPUT_PULLUP);

    lcd.init();
    lcd.backlight();
    updateDisplay();

    connectWiFi();
    client.setServer(mqttServer, mqttPort);
    connectMQTT();
    publishScore();
}

void loop()
{
    if (!client.connected())
        connectMQTT();
    client.loop();

    if (digitalRead(btnAUp) == LOW)
    {
        scoreA++;
        delay(300);
        publishScore();
        updateDisplay();
    }
    if (digitalRead(btnADown) == LOW)
    {
        scoreA = max(0, scoreA - 1);
        delay(300);
        publishScore();
        updateDisplay();
    }
    if (digitalRead(btnBUp) == LOW)
    {
        scoreB++;
        delay(300);
        publishScore();
        updateDisplay();
    }
    if (digitalRead(btnBDown) == LOW)
    {
        scoreB = max(0, scoreB - 1);
        delay(300);
        publishScore();
        updateDisplay();
    }
    if (digitalRead(btnReset) == LOW)
    {
        scoreA = 0;
        scoreB = 0;
        delay(300);
        publishScore();
        updateDisplay();
    }
}
