/**
 * weathernode
 *
 * weathernode is part of ShopNet
 *
 * Copyright 2014 zach wick <zach@zachwick.com>
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 **/

#include <SPI.h>
#include <Ethernet.h>
#include <Wire.h> // I2C library
#include "MPL3115A2.h" // Pressure Sensor
#include "HTU21D.h" // Humidity Sensor

// MAC Address for the weathernode
byte mac[] = { 0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xED };
String mac_string = "90A2DA0F4BD0";

// IP Address for the weathernode
// This will be used if the weathernode does not get an IP
// from a DHCP server on the network.
IPAddress ip(192, 168, 1, 103);

// IP Address of the ShopNet server
// In zwick's shop, the server has the following IP address
IPAddress server(192,168,1,100);
// If hardcoding an IP for the server isn't what you want,
// then you can use a hostname for the server and rely on DNS
// char server[] = "doorman.local";

// Initialize the Ethernet client library
EthernetClient client;

// Pressure Sensor Class
MPL3115A2 pressure_sensor;

// Humidity Sensor Class
HTU21D humidity_sensor;

// Hardware Pin Definitions
const byte STAT1 = 7; // Weather Shield Status LED #1
const byte STAT2 = 8; // Weather Shield Status LED #2
const byte REFERENCE_3V3 = A3;
const byte LIGHT = A1;

float humidity = 0; // % Relative Humidity
float tempf = 0;    // F Temperature
float pressure = 0;
float light_lvl = 455; // analog value from [0, 1023]

void
setup() {
	Serial.begin(9600);
	while (!Serial) {
		; // Wait for the serial port to connect
		; // Needed for Leonardo only
	}

  	// Set the IP of the weathernode manually since no DHCP
	Ethernet.begin(mac, ip);
        Serial.print("Ethernet Object Started\n");

	// Let the Ethernet Shield initialize
	delay(1000);

	// Set Up Hardware Pins
	pinMode (STAT1, OUTPUT);
	pinMode (STAT2, OUTPUT);
	pinMode (REFERENCE_3V3, INPUT);
	pinMode (LIGHT, INPUT);
	
	// Configure the pressure sensor
	pressure_sensor.begin(); // Turn the sensor on
	// Measure pressure in Pascals from 20 to 110 kPa
	pressure_sensor.setModeBarometer();
	// Set Oversample to the recommended 128
	pressure_sensor.setOversampleRate(7);
	// Enable all three pressure and temp event flags
	pressure_sensor.enableEventFlags();

	// Configure the humidity sensor
	humidity_sensor.begin();

	// Slow blink the status LEDs
	digitalWrite(STAT1, HIGH);
	digitalWrite(STAT2, HIGH);
	delay(1000);
	digitalWrite(STAT1, LOW);
	digitalWrite(STAT2, LOW);
}

void
loop() {
	// Read the Sparkfun Weather Shield
	read_weather();
        print_weather(); // debug only
        
	// Send the Weather Shield data to the ShopNet server
	if (client.connect(server,80)) {
		send_data();
	}

	// Delay for 5 seconds
	delay(5000);
}

void
send_data() {
        String request_body = String("{\"macaddr\":\"" + mac_string + "\",");
	request_body += "\"temp\":\"" + String(int(tempf*10),DEC) + "\",";
	request_body += "\"humidity\":\"" + String(int(humidity*10),DEC) + "\",";
	request_body += "\"pressure\":\"" + String(int(pressure*10),DEC) + "\",";
	request_body += "\"light\":\"" + String(int(light_lvl*10),DEC) + "\"}";
  
  	String request = String("POST /datapoints/ HTTP/1.1\n");
        request += String("Host: 192.168.1.100\n");
        request += String("Connection: close\n");
        request += String("Content-Type: application/json\n");
        request += String("Content-Length: ") + String(request_body.length(),DEC) + "\n";
        request += request_body;
        
        Serial.println("\n\nSending Data:");
        Serial.println(request);
	client.println(request);
        client.stop();
        
}

void
read_weather() {
	// Read humidity
	humidity = humidity_sensor.readHumidity();
	
	// Read temperature (from pressure sensor)
	tempf = pressure_sensor.readTempF();

	// Read pressure
	pressure = pressure_sensor.readPressure();

	// Read light_lvl
	light_lvl = get_light_level();
}

/**
 * Returns the voltage of the light sensor based on 3.3V logic
 * This allows us to ignore what VCC might be (an Arduino -> USB has VCC of
 * 4.5 - 5.2V)
 */
float
get_light_level() {
	float operatingVoltage = analogRead(REFERENCE_3V3);
	float lightSensor = analogRead(LIGHT);
	
	operatingVoltage = 3.3 / operatingVoltage;

	lightSensor = operatingVoltage * lightSensor;

	return lightSensor;
}

void print_weather()
{
  Serial.println();
  Serial.print("humidity=");
  Serial.print(humidity, 1);
  Serial.print(",tempf=");
  Serial.print(tempf, 1);
  Serial.print(",pressure=");
  Serial.print(pressure, 2);
  Serial.print(",light_lvl=");
  Serial.print(light_lvl, 2);
  Serial.print(",");
  Serial.println("#");

}

