/*
 * phanttest.ino
 *
 * Written by:
 * Scott Piette (Piette Technologies) scott.piette@gmail.com
 * Copyright 2014 Piette Technologies, LTD
 * Developed for the Open Source Beehives Project
 *       (http://www.opensourcebeehives.net)
 *
 * This software is released under the following license:
 *	GPL v3 (http://www.gnu.org/licenses/gpl.html)
 *
 * Modeled after SparkFun Phant Arduino Library
 * https://github.com/sparkfun/phant-arduino
 *
 * NOTES:
 * 	1.  Does not use dynamic Strings as that can cause memory fragmentation
 * 	2.  HTTP Header is consolidated into one 256 byte data buffer to improve performance
 *	3.  POST and CLEAR use the same header buffer, modified before sending
 *	4.  All POST data gets added to a 512 byte internal content buffer
 *	5.  HTTP and Phant responses from server are read using block reads into the
 *	    the remaining space from internal content buffer or space < 128 bytes we read
 *	    using single byte reads from stream
 * 	6.  sendData returns 1=success from Phant server or 0=failure if connect fails,
 * 	    no HTTP response, or Phant server returns status = 0
 *	7.  Currently only supports CLEAR STREAM, and POST messages
 *	8.  There is a 64 byte buffer for the Phant response message. Use getError() to
 *	    get the Phant response text.
 *	9.  The response buffer is used as a temporary buffer in addData so it's only
 *	    valid after the sendData call and before any addData calls
 * 	10. Recommend you create your own data stream at data.sparkfun.com
 *	11. Recommend you use your own public and private keys in the object initialization
 *
 */

#include <application.h>
#include "PietteTech_Phant/PietteTech_Phant.h"

// Phant::Stream stream1("data.sparkfun.com", <<public key>>, <<private key>>);
// view the data @ http://data.sparkfun.com/streams/G2EroylGG1tmmoX44KKn
Phant::Stream stream1("data.sparkfun.com", "G2EroylGG1tmmoX44KKn", "NWnYaNkll6TaaMw7744v");

int _ret;
int _retry;
int _loopcount;
unsigned long _ms;

void setup() {
    Serial.begin(9600);

    while(!Serial.available()) {
	Serial.println("Press any key to begin");
	delay(1000);
    }

    Serial.println("PHANT test v1.0");

    while (!WiFi.ready()) {
	Serial.println("Waiting on WiFi.");
	delay(500);
    }

    // Phant intitialization
    _ms = millis();
    stream1.begin();

    Serial.print("Local IP Address = ");
    Serial.println(WiFi.localIP());

    // clear previous stream values
    _retry = 0;
    while (_retry < 5 && !(_ret = stream1.sendData(PHANT_CLEAR_METHOD))) { delay(500); _retry++; }
    float _f = (millis() - _ms) / 1000.0;
    if (_ret)
    	Serial.print("Stream successfully cleared");
    else
    	Serial.print("Stream could not be cleared");
    if (_retry) {
	Serial.print(" Retries = "); Serial.print(_retry);
    }
    Serial.print(" - time = "); Serial.print(_f, 1); Serial.println("s");

}

void loop() {

#if 0
    //Adding a bad string field to test error
    char sTest1[] = "Hello World";
    stream1.add("st_ing", sTest1);
#endif

    //Adding a string field to the stream
    char sTest[] = "Hello World";
    stream1.add("string", sTest);

    //Adding a char field to the stream
    char cTest = 'e';
    stream1.add("char", cTest);

    //Adding a byte field to the stream
    byte bTest = 0xD0;
    stream1.add("byte", bTest);

    //Adding a int field to the stream
    int iTest = -rand();
    stream1.add("int", iTest);

    //Adding a long field to the stream
    long lTest = -186000L;
    stream1.add("long", lTest);

    //Adding a unsigned int field to the stream
    unsigned int uiTest = rand();
    stream1.add("uint", uiTest);

    //Adding a unsigned long field to the stream
    unsigned long ulTest = 186000L;
    stream1.add("ulong", ulTest);

    //Adding a float field to the stream
    float fTest = 1.117;
    stream1.add("float", fTest);

    //Adding a float field to the stream specifying precision
    stream1.add("float_2", fTest, 3);

    //Adding a double field to the stream
    double dTest = 11.23444556677;
    stream1.add("double", dTest);

    _retry = 0;
    _ret = 0;
    _ms = millis();
    while (_retry < 5 && !(_ret = stream1.sendData())) { delay(500); _retry++; }
    float _f = (millis() - _ms) / 1000.0;

    Serial.print("Post ["); Serial.print(_loopcount++); Serial.print("] ");
    if (_ret)
	Serial.print("successfully sent");
    else {
	Serial.print("could not be sent due to error (");
	Serial.print(stream1.getError());
	Serial.print(")");
	stream1.begin();  // reset stream to remove previous contents
    }
    if (_retry) {
	Serial.print(" retries = "); Serial.print(_retry);
    }

    Serial.print(" - time = "); Serial.print(_f, 1); Serial.println("s");
    delay(5000);
}
