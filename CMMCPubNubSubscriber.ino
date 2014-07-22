
#include <SPI.h>
#include <Ethernet.h>
#include <PubNub.h>
#include <aJSON.h>

// Some Ethernet shields have a MAC address printed on a sticker on the shield;
// fill in that address here, or choose your own at random:
byte mac[] = { 0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xED };

const int subLedPin = 4;

aJsonObject* jsonObject;

String jsonString;
char jsonPrimitiveString[50];

char pubkey[] = "pub-c-0a5aa9d7-02cc-4f29-9c97-4d3601d30656";
char subkey[] = "sub-c-b47bf35e-a98b-11e2-821b-12313f022c90";
const static char channel[] = "my_channel";
char uuid[] = "xxxxxxxx-xxxx-4444-9999-xxxxxxxxxxxx";

void random_uuid() {
	randomSeed(analogRead(4) + millis() * 1024);
	snprintf(uuid, sizeof(uuid), "%04lx%04lx-%04lx-4444-9999-%04lx%04lx%04lx",
		random(0x10000), random(0x10000), random(0x10000),
		random(0x10000), random(0x10000), random(0x10000));
}

void setup()
{
	pinMode(subLedPin, OUTPUT);
	digitalWrite(subLedPin, LOW);

        jsonString = String("");
	Serial.begin(9600);
	Serial.println("Serial set up");

	while (!Ethernet.begin(mac)) {
		Serial.println("Ethernet setup error");
		delay(1000);
	}
	Serial.println("Ethernet set up");

	PubNub.begin(pubkey, subkey);
	random_uuid();
	PubNub.set_uuid(uuid);
	Serial.println("PubNub set up");
}

void flash(int ledPin)
{
	/* Flash LED three times. */
	for (int i = 0; i < 3; i++) {
		digitalWrite(ledPin, HIGH);
		delay(100);
		digitalWrite(ledPin, LOW);
		delay(100);
	}
}

void loop()
{
	Ethernet.maintain();

	PubSubClient *client;

	Serial.println("waiting for a message (subscribe)");
	client = PubNub.subscribe(channel);
	if (!client) {
		Serial.println("subscription error");
		delay(1000);
		return;
	}

	Serial.print("Received: ");

        jsonString = String("");
        
	while (client->wait_for_data()) {
		char c = client->read();
//		Serial.println(c);
                jsonString.concat(c);
                Serial.println(jsonString);
	}

        Serial.println("===========");
      
        jsonString = jsonString.substring(1, jsonString.length()-1);
        
        jsonString.toCharArray(jsonPrimitiveString, jsonString.length()+1);
        Serial.println(jsonPrimitiveString);
        
        jsonObject = aJson.parse(jsonPrimitiveString);
        aJsonObject *item = aJson.getObjectItem(jsonObject, "switch");
        Serial.println(item->valuestring);       
        if (strcmp(item->valuestring, "on") == 0) {
          Serial.println("ON");          
          flash(subLedPin);
        }
        else {
          Serial.println("OOOFFFFF");
        }
	client->stop();
	Serial.println();


	delay(200);
}
