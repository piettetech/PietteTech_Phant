/*
 * PietteTech_Phant.h
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

#ifndef __PHANT_H__
#define __PHANT_H__

#include "application.h"

//  Post and Clear are the only methods supported in this library
#define PHANT_POST_METHOD	1
#define PHANT_CLEAR_METHOD	2

#define PHANT_DEFAULT_PORT	80
#define PHANT_SOCKET_NUMBER	6

namespace Phant
{
    class Stream
    {
        private:
	    String _pub;
	    String _prv;
	    String _host;
	    unsigned int _port;
	    uint8_t _lenPos;	// location in header to store the length of message body (max 999)
	    uint8_t _clearPos;	// location in header that changes between post and clearStrem
	    char _header[256];	// static buffer for building header on call to begin
	    char _params[512];	// buffer for data name & value fields in message body
	    char _response[64]; // buffer for phant response text message
	    TCPClient _client;

	    int addBuf();	// adds to the _params buffer checking first to make sure it fits
	    void post();	// configures and sends post header and data
	    void clearStream();	// configures and sends clearStream header

	public:
	    Stream(String host, String publicKey, String privateKey, unsigned int port = PHANT_DEFAULT_PORT);
	    int begin();	// clears buffers and status flags
	    int sendData(int phantMethod = PHANT_POST_METHOD);
	    char *getError();
	    int add(char *field, char *data);
	    int add(char *field, char data);
	    int add(char *field, byte data);
	    int add(char *field, int data);
	    int add(char *field, long data);
	    int add(char *field, unsigned int data);
	    int add(char *field, unsigned long data);
	    int add(char *field, float data, uint8_t prec = 1, uint8_t width = 1);
	    int add(char *field, double data, uint8_t prec = 4, uint8_t width = 1);
    };
}
#endif
