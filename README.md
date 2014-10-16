<!DOCTYPE HTML PUBLIC "-//W3C//DTD HTML 4.01 Transitional//EN">
<html>
  <head>
    <meta http-equiv="content-type" content="text/html;
      charset=windows-1252">
    <title></title>
  </head>
  <body>
    <b><big>Phant database library for Spark </big><br>
    </b>
    <p> </p>
    <p>Supports clearing and writing data to Phant data streams</p>
    Scott Piette, Piette Technologies, LTD - scott.piette@gmail.com<br>
    Copyright 2014 Piette Technologies, LTD<br>
    <br>
    Project supported by Open Source Beehives Project<a
      href="http://www.opensourcebeehives.net">
      http://www.opensourcebeehives.net</a><br>
    <br>
    Oct 16, 2014&nbsp; - Initial release for Spark Core<br>
    <br>
    <table border="1" cellpadding="2" cellspacing="2" width="100%">
      <tbody>
        <tr>
          <td valign="top">VERSION:</td>
          <td valign="top">0.2<br>
          </td>
        </tr>
        <tr>
          <td valign="top">PURPOSE:</td>
          <td valign="top">Phant database communication library for
            Spark<br>
          </td>
        </tr>
        <tr>
          <td valign="top">LICENSE:</td>
          <td valign="top">GPL v3 (http://www.gnu.org/licenses/gpl.html)</td>
        </tr>
      </tbody>
    </table>
    <br>
    <p> BASED ON:<br>
      Phant Arduino Library <a href="https://github.com/niesteszeck">https://github.com/sparkfun/phant-arduino</a><br>
    </p>
    <p>PHANT GITHUB: &nbsp; &nbsp; &nbsp;<a
        href="https://github.com/sparkfun/phant">
        https://github.com/sparkfun/phant</a><br>
    </p>
    METHODS:
    <table height="58" border="1" cellpadding="2" cellspacing="2"
      width="904">
      <tbody>
        <tr>
          <td valign="top">sendData(method)<br>
          </td>
          <td valign="top">sends data to Phant server using the method
            passed, defaults to POST<br>
          </td>
        </tr>
        <tr>
          <td valign="top">addData(name, value)<br>
          </td>
          <td valign="top">adds a data=value pair to the stream.&nbsp;
            After you have added all data send the stream using sendData<br>
          </td>
        </tr>
        <tr>
          <td valign="top">getError()<br>
          </td>
          <td valign="top">returns the human readable text from the
            Phant server resulting from the last sendData<br>
          </td>
        </tr>
      </tbody>
    </table>
    <br>
    NOTES:<br>
    <table border="1" cellpadding="2" cellspacing="2" width="100%">
      <tbody>
        <tr>
          <td>1</td>
          <td> Does not use dynamic Strings as that can cause memory
            fragmentation</td>
        </tr>
        <tr>
          <td>2</td>
          <td> HTTP Header is consolidated into one 256 byte data buffer
            to improve performance</td>
        </tr>
        <tr>
          <td>3</td>
          <td> POST and CLEAR use the same header buffer - modified
            before sending</td>
        </tr>
        <tr>
          <td>4</td>
          <td> All POST data gets added to a 512 byte internal content
            buffer</td>
        </tr>
        <tr>
          <td>5</td>
          <td> HTTP and Phant responses from server are read using block
            reads into the remaining space from internal content buffer,
            or if space &lt; 128 bytes we read using single byte reads
            from stream</td>
        </tr>
        <tr>
          <td>6</td>
          <td> sendData returns 1=success from Phant server or 0=failure
            if connect fails, no HTTP response, or Phant server returns
            status = 0</td>
        </tr>
        <tr>
          <td>7</td>
          <td> Currently only supports CLEAR STREAM,&nbsp; and POST
            methods<br>
          </td>
        </tr>
        <tr>
          <td>8</td>
          <td> There is a 64 byte buffer for the Phant response message.
            Use getError() to get the Phant response text.</td>
        </tr>
        <tr>
          <td>9</td>
          <td> The response buffer is used as a temporary buffer for
            addData so it's only valid after the sendData call and
            before any addData calls</td>
        </tr>
      </tbody>
    </table>
    <br>
    <br>
    <p> </p>
  </body>
</html>
