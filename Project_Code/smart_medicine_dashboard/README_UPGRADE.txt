SMART MEDICINE DISPENSER - UPGRADED DASHBOARD

This package preserves the existing dashboard UI styling and updates the schedule UI to 6 slots.

ESP32 firmware is supplied separately as Smart_Medicine_Dispenser_ESP32.ino.

Important setup:
1. Edit WIFI_SSID and WIFI_PASSWORD in the .ino file.
2. Install Arduino libraries: ESP32Servo, LiquidCrystal_I2C, RTClib (Adafruit).
3. DS3231 RTC uses a separate I2C bus on GPIO 4 (SDA) and GPIO 5 (SCL).
4. LCD uses SDA GPIO 32 and SCL GPIO 33. Confirm your board exposes these pins and use an I2C level shifter if the LCD backpack pulls SDA/SCL up to 5V.
5. Open this folder's terminal and run: npm start
6. Connect laptop and ESP32 to the same Wi-Fi; enter the ESP32 IP shown in Serial Monitor.
7. The dashboard syncs browser time to the DS3231. Keep the dashboard open for automatic time sync, or set the RTC another way if the dashboard is not running.

The supplied dashboard zip intentionally excludes node_modules and private .env credentials. The existing WhatsApp backend source is retained, but its credentials must be configured locally in whatsapp/.env. No notification backend is called by this firmware yet; dashboard status polling updates medicine history.
