#include <WiFi.h>
#include <HTTPClient.h>
#include "credentials.h"
#include <ESP32_MySQL.h>


#define ESP32_MYSQL_DEBUG_PORT      Serial

// Debug Level from 0 to 4
#define _ESP32_MYSQL_LOGLEVEL_      4 // uh... is 4 chattier?

// MySQL Server IP address
const char* SERVER_HOST = "192.168.4.32";
const int   SERVER_PORT = 3306; //default port

const unsigned long LOG_INTERVAL_MS = 10UL * 1000UL;  // 10 seconds

IPAddress server(SERVER_HOST);
uint16_t server_port = SERVER_PORT;    // MySQL server port (default : 3306)

char default_database[] = "Observations";           //default DB
char default_table[]    = "Observation";          //default table

// SQWL INSERT string
String INSERT_SQL = String("INSERT INTO Observations.Observation (value, valueType) VALUES 37.4, 'other'");

ESP32_MySQL_Connection conn((Client *)&client);

ESP32_MySQL_Query *query_mem;




void setup() {
  Serial.begin(115200);
  while (!Serial && millis() < 5000); // wait for serial port to connect
  
    ESP32_MYSQL_DISPLAY1("\nStarting Basic_Insert_ESP on", ARDUINO_BOARD);

  // Begin WiFi section
  ESP32_MYSQL_DISPLAY1("Connecting to", ssid);
  
  WiFi.begin(ssid, pass);
  
  while (WiFi.status() != WL_CONNECTED) 
  {
    delay(500);
    ESP32_MYSQL_DISPLAY0(".");
  }

  // print out info about the connection:
  ESP32_MYSQL_DISPLAY1("Connected to network. My IP address is:", WiFi.localIP());

#if USE_TLS
  #if USING_HOST_NAME
    conn.enable_tls(true, server);   // Request TLS and pass hostname for SNI
  #else
    conn.enable_tls(true);
  #endif
#endif

  ESP32_MYSQL_DISPLAY3("Connecting to SQL Server @", server, ", Port =", server_port);
  ESP32_MYSQL_DISPLAY5("User =", user, ", PW =", password, ", DB =", default_database);
}

void runInsert()
{
  // Initiate the query class instance
  ESP32_MySQL_Query query_mem = ESP32_MySQL_Query(&conn);

  if (conn.connected())
  {
    ESP32_MYSQL_DISPLAY(INSERT_SQL);
    
    // Execute the query
    if ( !query_mem.execute(INSERT_SQL.c_str()) )
    {
      ESP32_MYSQL_DISPLAY("Insert error");
    }
    else
    {
      ESP32_MYSQL_DISPLAY("Data Inserted.");
    }
  }
  else
  {
    ESP32_MYSQL_DISPLAY("Disconnected from Server. Can't insert.");
  }
}

void loop()
{
  ESP32_MYSQL_DISPLAY("Connecting...");
  
  //if (conn.connect(server, server_port, user, password))
  if (conn.connectNonBlocking(server, server_port, user, password) != RESULT_FAIL)
  {
    delay(500);
    runInsert();
    conn.close();                     // close the connection
  } 
  else 
  {
    ESP32_MYSQL_DISPLAY("\nConnect failed. Trying again on next iteration.");
  }

  ESP32_MYSQL_DISPLAY("\nSleeping...");
  ESP32_MYSQL_DISPLAY("================================================");
 
  delay(10000); // every 10 seconds
}