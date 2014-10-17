/*
 * PietteTech_Phant.cpp
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
 *
 */

#include "PietteTech_Phant.h"

//#define SERIAL_DEBUG  1		// 1 = timing, 2 = sensor

#if defined(SERIAL_DEBUG)
#define D(x) x
#else
#define D(x)
#endif
#if (SERIAL_DEBUG > 1)
#define D2(x) x
#else
#define D2(x)
#endif

// Constructor
Phant::Stream::Stream(String host, String publicKey, String privateKey, unsigned int port)
{
    _host = host;
    _port = port;
    _pub = publicKey;
    _prv = privateKey;
}

// Initializers that should be called only once in the `setup()` function
int Phant::Stream::begin(){
    if (!_client) _client = TCPClient();

    // Clear out buffers;
    _params[0] = NULL;
    _header[0] = NULL;
    _response[0] = NULL;

    // Setup the HTTP header
    strcat(_header, "POST   /input/");	// NOTE DELETE and POST will fit here
    strcat(_header, _pub.c_str());	// Add public key
    strcat(_header, " HTTP/1.1\nUser-Agent: SparkCoreLibrary\nHost: ");
    strcat(_header, _host.c_str());	// Add host
    strcat(_header, "\nConnection: close\nPhant-Private-Key: ");
    strcat(_header, _prv.c_str());	// Add private key
    _clearPos = strlen(_header);
    strcat(_header, "\nContent-Type: application/x-www-form-urlencoded\nContent-Length: 000\n\n");
    _lenPos = strlen(_header) - 5;

    return 1;
}

// Main API functions that the library provides
// typically called in `loop()` or `setup()` functions

// add the new data pair in the buffer checking to be sure it will fit
// returns 1=success, 0=failure - no room in buffer
int Phant::Stream::addBuf() {
    int _len = strlen(_params);
    int _size = strlen(_response);
    if (_size + _len > (sizeof(_params) - 1)) return 0;
    strcat(_params, _response);
    return _size;
}

//	add(field, string)
//		function adds a char field to the sending queue.
//		Warning: string type data needs to be urlencoded beforehand
int Phant::Stream::add(char *field, char *data) {
    sprintf(_response, "&%s=%s", field, data);
    return addBuf();
}

//	add(field, char)
//		function adds a char field to the sending queue.
int Phant::Stream::add(char *field, char data) {
    sprintf(_response, "&%s=%c", field, data);
    return addBuf();
}

//	add(field, byte)
//		function adds a byte field to the sending queue.
int Phant::Stream::add(char *field, byte data) {
  sprintf(_response, "&%s=%#x", field, data);
  return addBuf();
}

//	add(field, unsignedInt)
//		function adds a unsigned int field to the sending queue.
int Phant::Stream::add(char *field, unsigned int data) {
  sprintf(_response, "&%s=%u", field, data);
  return addBuf();
}

//	add(field, unsignedLong)
//		function adds a unsigned long field to the sending queue.
int Phant::Stream::add(char *field, unsigned long data) {
  sprintf(_response, "&%s=%u", field, data);
  return addBuf();
}

//	add(field, int)
//		function adds a int field to the sending queue.
int Phant::Stream::add(char *field, int data) {
    sprintf(_response, "&%s=%d", field, data);
    return addBuf();
}

//	add(field, long)
//		function adds a long field to the sending queue.
int Phant::Stream::add(char *field, long data) {
  sprintf(_response, "&%s=%d", field, data);
  return addBuf();
}

//	add(field, float)
//		function adds a float field to the sending queue.
int Phant::Stream::add(char *field, float data, uint8_t prec, uint8_t width) {
    char fmt[15];
    sprintf(fmt, "&%%s=%%%d.%df", width, prec);
    sprintf(_response, fmt, field, data);
    return addBuf();
}

//	add(field, double)
//		function adds a float field to the sending queue.
int Phant::Stream::add(char *field, double data, uint8_t prec, uint8_t width) {
    char fmt[15];
    sprintf(fmt, "&%%s=%%%d.%df", width, prec);
    sprintf(_response, fmt, field, data);
    return addBuf();
}

/**************************************************************************/
/*
	Function:  sendData(phantMethod)

	Main TCP function.  It opens the connection, waits for it to
	connect, calls either clearStream or post for the header / data
	send and waits for the response.

	TODO:  Scan the response from the phant server and return status

	The timeout delay can be adjusted shorter if needed.
 */
/**************************************************************************/

#define MAX_CONNECT_TIMEOUT 100

int Phant::Stream::sendData(int phantMethod) {
    int _ret;
    int _timeout = 0;
    int _status = -1;
    int length = _host.length()+1;
    char charBuffer[length];
    _host.toCharArray(charBuffer,length);

    D(Serial.print("Starting client.connect(");)
    D(Serial.print(_host);)
    D(Serial.print(":");)
    D(Serial.print(_port);)
    D(Serial.println(")");)

    // Open the connection to the host
    if(_client.connect(charBuffer,_port)) {
	D(Serial.println("After client.connect()");)
	while ( _timeout < MAX_CONNECT_TIMEOUT && !_client.connected()) {
	    D(if (!_timeout) Serial.print("Waiting for connect");)
	    D(else Serial.print(".");)
	    delay(10);
	    ++_timeout;
	}
	// Setup the header and send the phant request
	D(if (_timeout) Serial.println("");)
	D(Serial.print("Sending");)
	switch (phantMethod) {
	  case PHANT_POST_METHOD:
	    D(Serial.print(" post\r\n");)
	    post();
	    break;
	  case PHANT_CLEAR_METHOD:
	    D(Serial.print(" stream clear\r\n");)
	    clearStream();
	    break;
	}

	// wait for response
	_timeout = 0;
	while (_timeout < MAX_CONNECT_TIMEOUT && !(_ret = _client.available())) {
	    D(if (!_timeout) Serial.print("\r\nWaiting for response");)
	    D(else Serial.print(".");)
	    delay(100);
	    ++_timeout;
	}
	D(Serial.println();)

	// If we have a response from the server lets process it
	if (_ret) {
	    bool _found = false;  // set to true when we have reached the end of the http response
	    int _count = 0; // This keeps track of how many /n /r we have received
	    memset(_response, 0, sizeof(_response));  // empty the response buffer

	    // if we have enough space left over in _params lets use it
	    int _size = (sizeof(_params) - strlen(_params) - 1);
	    if (_size > 128 ) {  // We have at lest 128 bytes so lets use it

		// Lets see how much buffer we have remaining on the _params array
		D(Serial.println("using _params as buffer");)

		// Use what's left in the _params buffer
		char *_buf = &_params[strlen(_params) + 1];
		int _len;

		// read a block from the TCP stream
		while ((_len = _client.read((uint8_t *)_buf, _size)) != -1) {
		    // Search thru the block looking for the http response end
		    // and keep the Phant response code
		    if (!_found || _status == -1) {
			for (char *_s = _buf; _len > 0; _s++, _len--) {
			    D(Serial.print(*_s);)
			    if (*_s == '\r' || *_s == '\n') {
				_count++;
				if (_count == 4) _found = true;
			    }
			    else _count = 0;
			    if (_found && (_status == -1) && (*_s == '0' || *_s == '1')) { // we have found the phant response
				_status = *_s - '0';
				strncpy(_response, _s, (_len < sizeof(_response) ? _len : sizeof(_response)));
				break;  // found phant response, skip rest of buffer
			    }
			}	// scanning buffer
		    } // only scan buffer if we don't have the status
		}; // continue reading TCP stream until no data is available
	    } else {  // We don't have any extra buffer so
		// Search thru the http response reading one byte at a time
		// and keep the Phant response code
		char _c; // character read from stream
		char *_r = _response; // beginning of response buffer
		char *_q = _r + sizeof(_response) - 1;  // end of response buffer
		while (int8_t(_c = _client.read()) != -1) {
		    D(Serial.print(_c);)
		    if (_c == '\r' || _c == '\n') {
			_count++;
			if (_count == 4) _found = true;	// we have found the end of http response
		    }
		    else _count = 0;
		    if (_found && (_status == -1) && (_c == '0' || _c == '1')) // we have found the phant response
			_status = _c - '0';
		    if (_status != -1) {
			// Copy the error text message to our response buffer
			if ((_c != '\n' || _c != '\r') && _r < _q) *_r++ = _c;
		    }
		}; // keep reading until no data is available
	    }
	    D(Serial.print("Phant Status = ");)
	    D(Serial.println(_status);)
	    D(Serial.println(_response);)
	}
	else {
	    D(Serial.println("\r\nNo Response.");)
	    _status = 0;
	}
	_client.flush();
	_client.stop();
    }
    else {
      _status = 0;	// error on connect
      D(Serial.print("Failed to connect");)
    }

    // Empty the params array of we had success
    if (_status)
        _params[0] = NULL;

    return _status;
}

/**************************************************************************/
/*
	Function:  getError

	Returns the pointer to the response error message
	The contents of this buffer is only valid after a call to
	sendData and before any subsequent calls to addData
 */
/**************************************************************************/
char *Phant::Stream::getError() {
    return _response;
}

/**************************************************************************/
/*
	Function:  post

	Setup the static header for the post
	We share this header with the clearStream, so there are
	a few things that need to be changed such as the length
	in the message body.

	We send the header and message in two calls to _client.print()
 */
/**************************************************************************/
void Phant::Stream::post() {
  size_t numBytes;

  // Put the POST in the header
  memcpy(_header, "POST  ", 6);

  // Make sure we have the newline here
  memcpy(&_header[_clearPos], "\nCo", 3);

  // Lets insert the proper length
  int _len = strlen(&_params[1]);
  char _lenBuf[4];
  sprintf(_lenBuf, "%3d", _len);
  strncpy(&_header[_lenPos], _lenBuf, 3);

  numBytes = _client.write((uint8_t*)_header, strlen(_header));
  D(Serial.print("Header write sent [");)
  D(Serial.print(numBytes);)
  D(Serial.print("/");)
  D(Serial.print(strlen(_header));)
  D(Serial.println("]");)
  numBytes = _client.write((uint8_t*)&_params[1], strlen(&_params[1]));
  D(Serial.print("Message write sent [");)
  D(Serial.print(numBytes);)
  D(Serial.print("/");)
  D(Serial.print(strlen(&_params[1]));)
  D(Serial.println("]");)

  D(Serial.print(_header);)
  D(Serial.print(&_params[1]);)

}

/**************************************************************************/
/*
	Function:  clearStream

	Setup the static header for clearString
	We share this header with the post, so there are
	a few things that need to be changed

	We send the header in one call to _client.print()
*/
/**************************************************************************/
void Phant::Stream::clearStream() {
    size_t numBytes;

    char _s[] = {'\n','\n',0x00};

    // Put the DELETE in the header
    memcpy(_header, "DELETE", 6);

    // Make sure we terminate the header properly for the clear command
    memcpy(&_header[_clearPos], _s, 3);

    numBytes = _client.write((uint8_t*)_header, strlen(_header));
    D(Serial.print("Header write sent [");)
    D(Serial.print(numBytes);)
    D(Serial.print("/");)
    D(Serial.print(strlen(_header));)
    D(Serial.println("]");)
    D(Serial.print(_header);)

}
