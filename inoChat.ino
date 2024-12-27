#include <WiFi.h>
#include <ArduinoJson.h>
#include <ArduinoHttpClient.h>
JsonDocument conf;
JsonDocument request;
JsonDocument user1;
JsonDocument user2;
JsonDocument user3;
JsonDocument user4;
JsonDocument assi1;
JsonDocument assi2;
JsonDocument assi3;
JsonDocument systemPrompts;

String pubHostname;
String pubSsid;
String pubPassword;
String pubOpenAiKey;
String pubGptSystemPrompt;
String pubOpenAIModel;
String assistantIn = "0";

WiFiClient wifi;
HttpClient http = HttpClient(wifi, "http://api.openai.com", 443);

void setup() {
  Serial.begin(9600); //yummy

  String waitForStart = "";
  while(waitForStart == "") {
    waitForStart = Serial.readString();
  }
  delay(1000);
  Serial.println("Welcome to the inoChat pre-Alpha!\nParsing config placed at /config.json on the SD card.");
  delay(2000);

  
  //temporary
  String jsonConfigData = "{\"hostname\":\"ESP32BK\",\"SSID\":\"COOL!Box\",\"wifiPassword\":\"01892533340\",\"model\":\"gpt-3.5-turbo\",\"openAiKey\":\"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\",\"systemPrompt\":\"You are a Speech recognizing Chat bot. Have a conversation with the user.\"}";

//----------parsing config file------------

  DeserializationError error = deserializeJson(conf, jsonConfigData);
  if (error) {
    Serial.print(F("deserializeJson() failed: "));
    Serial.println(error.c_str());
    return;
  }



  const char* hostname = conf["hostname"];
  const char* ssid = conf["SSID"];
  const char* password = conf["wifiPassword"];
  const char* openAiKey = conf["openAiKey"];
  const char* gptSystemPrompt = conf["systemPrompt"];
  const char* openAIModel = conf["model"];

  pubHostname = hostname;
  pubSsid = ssid;
  pubPassword = password;
  pubOpenAiKey = openAiKey;
  pubGptSystemPrompt = gptSystemPrompt;
  pubOpenAIModel = openAIModel;

//---------end config parsing------------

  //test
  Serial.println("Successfully Parsed JSON config file: \n \n");

  Serial.print("Hostname: ");
  Serial.println(hostname);
  Serial.print("Network SSID: ");
  Serial.println(ssid);
  Serial.print("Network Password: ");
  Serial.println(password);
  Serial.print("OpenApi Api key: ");
  Serial.println(openAiKey);
  Serial.print("GPT system Prompt: ");
  Serial.println(gptSystemPrompt);
  Serial.print("GPT Model: ");
  Serial.println(openAIModel);
  Serial.println("\n\n");

  
//----------wifi setup and connection------
  Serial.println("\n Setting up WIFI \n");
    WiFi.setHostname(hostname);
    WiFi.mode(WIFI_STA);
    WiFi.begin(ssid, password);
    Serial.println("Connecting");
    while(WiFi.status() != WL_CONNECTED) {
        Serial.print(".");
        delay(100);
     }
    Serial.println("\nConnected and shiii");
//---------------wifi setup end------------
 delay(2000); 

  
}

void loop() {
 //read for user input
  Serial.println("Provide user Input please.");
  String userIn;
  userIn = getUserInput();
  Serial.println("User Input received!");
  delay(500);
  Serial.println("Generating Response....");
  String GPTResponse = getResponse(userIn);
  Serial.println(GPTResponse);
}

String getUserInput() {
  String input = "";
  while(input == "") {
    input = Serial.readString();
  }
  input.replace("\n", "");
  return input;
}

String getResponse(String input) {
//--------convert input into json----------
  String jsonInput = generateJson(input, assistantIn);
  Serial.println("Json generated:");
  delay(500);
  Serial.println(jsonInput);
  assistantIn = "0";

//--------------end conversion-------------
// send httprequest using ArduinohttpClient
 
  http.beginRequest();
  http.post("/v1/chat/completions");
  http.sendHeader("Content-Type", "application/json");
  String authString = "Bearer " + pubOpenAiKey;
  http.sendHeader("Authorization", authString);

  http.beginBody();
  http.print(jsonInput);
  http.endRequest();

//--------------end request----------------

// receive response using ArduinohttpClient
    
  int statusCode = http.responseStatusCode();
  String response = http.responseBody();

//---------end reponse receiving-----------
  return response;
  }

//---------start json generation-----------

String generateJson(String userInput, String assistantInput) {

  request.clear();

  if(userInput != "0")  {
    if (!user4.isNull()) {
      backShiftallUser();
      user4["role"] = "user";
      user4["content"] = userInput;
    }
    else {
      user4["role"] = "user";
      user4["content"] = userInput;
      if(user3.isNull()) {
        backshiftUser();
      }
    }
  }


  if (assistantInput != "0"){
    if(!assi3.isNull()){
      backshiftallAssi();
      assi3["role"] = "assistant";
      assi3["content"] = assistantInput;
    } 
    else {
      assi3["role"] = "assistant";
      assi3["content"] = assistantInput;
      if(assi2.isNull()) {
        backshiftAssi();
      }
    }
  }
  
  request["model"] = pubOpenAIModel;
  systemPrompts["role"] = "system";
  systemPrompts["content"] = pubGptSystemPrompt;
  request["messages"].add(systemPrompts);
  if(!user1.isNull()) {request["messages"].add(user1);} //wtf is this code
  if(!assi1.isNull()) {request["messages"].add(assi1);}
  if(!user2.isNull()) {request["messages"].add(user2);}
  if(!assi2.isNull()) {request["messages"].add(assi2);}
  if(!user3.isNull()) {request["messages"].add(user3);}
  if(!assi3.isNull()) {request["messages"].add(assi3);}
  if(!user4.isNull()) {request["messages"].add(user4);}

  String finishedRequest;
  serializeJsonPretty(request, finishedRequest);
  return finishedRequest;
}
void backShiftallUser() {
  user1 = user2;
  user2 = user3;
  user3 = user4;
}
void backshiftUser() { //start backshiftUser
  user3 = user4;
  user4.clear();
  if(user2.isNull()){
    user2 = user3;
    user3.clear();
    if(user1.isNull()) {
      user1 = user2;
      user2.clear(); //i want to combine my head with a wall
    }

  }

}//end backshiftUser

void backshiftallAssi() {
  assi1 = assi2;
  assi2 = assi3;
}
void backshiftAssi() {
  assi2 = assi3;
  assi3.clear();
  if(assi1.isNull()) {
    assi1 = assi2;
    assi2.clear();
  }
}
//---------end json generation-----------
