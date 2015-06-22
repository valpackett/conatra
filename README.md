# conatra [![unlicense](https://img.shields.io/badge/un-license-green.svg?style=flat)](http://unlicense.org)

It's an easy to use C/C++ library for implementing [CoAP] / [CoRE] on microcontrollers, featuring [Sinatra]-style syntax!

- CoAP is the Constrained Application Protocol, which is basically simplified and very compact binary HTTP over UDP.
- CoRE is the Constrained RESTful Environments Link Format, which is basically the HTTP Link header as a response, not as a header.

This allows you to create RESTful interfaces on IoT (Internet of Things) devices.

In the box:

- **<conatra.h>** // a Sinatra-style DSL for the [microcoap] library
- **<EtherCard+coap.h>** // boilerplate for connecting the [EtherCard] library (driver for the very popular [ENC28J60] Ethernet module) with the [microcoap] library

[Sinatra]: http://www.sinatrarb.com
[CoAP]: http://coap.technology
[CoRE]: https://tools.ietf.org/html/rfc6690
[microcoap]: https://github.com/1248/microcoap
[EtherCard]: https://github.com/jcw/ethercard
[ENC28J60]: http://www.ebay.com/sch/i.html?_nkw=enc28j60

## Usage

### Arduino

1. Install the microcoap library into the Arduino IDE;
2. Remove `main-posix.c` and `endpoints.c` from the `libraries/microcoap` folder, because the IDE tries to compile all the things for no good reason;
3. Install the library for your network module (EtherCard) into the Arduino IDE;
4. Install conatra into the Arduino IDE;
5. Use all of that stuff in your sketches like this:

```arduino
#include <EtherCard.h>
#include <coap.h>
#include <EtherCard+coap.h>

byte Ethernet::buffer[400];
static uint8_t mymac[] = { 0x74, 0x69, 0x69, 0x2D, 0x30, 0x31 };

void setup(void) {
  if (ether.begin(sizeof Ethernet::buffer, mymac, 4) == 0)
    Serial.println(F("Eth"));
  if (!ether.dhcpSetup())
    Serial.println(F("DHCP"));
  ether.udpServerListenOnPort(&udpCallback, 5683);
  coap_setup();
}

void loop(void) {
  ether.packetLoop(ether.packetReceive());
  delay(50);
}

#define ROUTES \
ROUTE(hello, COAP_METHOD_GET, URL("tests", "hello"), ";if=\"test\"", { \
  char response[3] = "Hi"; \
  CONTENT(COAP_CONTENTTYPE_TEXT_PLAIN, response); \
})

#include <conatra.h>
```

For a full example, see the `examples/iotweather/iotweather.ino` sketch.


## Contributing

Please feel free to submit pull requests!
Bugfixes and simple non-breaking improvements will be accepted without any questions :-)

By participating in this project you agree to follow the [Contributor Code of Conduct](http://contributor-covenant.org/version/1/1/0/).

## License

This is free and unencumbered software released into the public domain.  
For more information, please refer to the `UNLICENSE` file or [unlicense.org](http://unlicense.org).
