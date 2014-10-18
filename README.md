# ShopNet WeatherNode
The ShopNet WeatherNode monitors the ambient environment and sends
data to an instance of shopnet-server. The WeatherNode monitors the
following environmental parameters:

* air temperature
* barometric pressure
* relative humidity
* incident light

# Configuring the WeatherNode
As with all ShopNet nodes, configuration is achieved by modifying the
source code and re-flashing the node. There are a few places that need
to changed in the source code on a per physical basis:
## MAC Address
The MAC address of the weathernode is unique to the networking
hardware and should be changed in the code to the MAC address printed
(or on a sticker) of the networking hardware that you are using. In
the file `weathernode.ino` the corresponding lines are lines 36
and 37. 

    byte mac[] = { 0x90, 0xA2, 0xDA, 0x0F, 0x4B, 0xD0 };
    String mac_string = "90A2DA0F4BD0";

## WeatherNode IP Address
If your network uses DHCP, then this part is easy because you get a
network unique IP for free. If your network doesn't use DHCP, then
line 42 needs to be uncommented and the values need to be set to
something that makes sense for your network. The first two octets
should probably remain as `192` and `168`. The third octect will be
the same as any other devices connected to the network; On my shop's
network (see why the project is called "ShopNet"?) this third octet is
`10`. The fourth and final octect needs to be network unique and can
be any value less than 256 (but cannot be already be on your network).

    IPAddress ip(192, 168, 1, 103);

## ShopNet-Server IP Address
Line 46 needs to be changed so that it has the value of the IP address
at which your instance of shopnet-server is running. Alternatively,
you can use the hostname of your shopnet-server instance, but I am
using the IP because I am not sure that I set my shop's network up
correctly. 

    IPAddress server(192,168,10,247);

Line 159, where the HTTP POST of the sensor data is sent to your
shopnet-server instance, you must change the "HOST" line to whatever
makes sense for your shopnet-server instance. You need to put in
either the IP address of your shopnet-server instance, or the hostname
if you used that in the previous change.

## Sample Rate
The sample rate is set by using a `delay` at the end of the `loop`
function. This is extremely crude and will hopefully be changed
someday. In the meantime, feel free to adjust the sample time to
whatever you see fit. You can sample as often as you wish, but if you
sample too fast, you may run into issues with your shopnet-server not
being able to respond fast enough and/or the weathernode not being
able to send data quick enough.

# Licenses
weathernode uses the following libraries each with their own
licensing:
- HTU21D : ([Beerware License](https://en.wikipedia.org/wiki/Beerware))
- MPL3115A2 :
  ([Creative Commons Share-alike 3.0](http://creativecommons.org/licenses/by-sa/3.0/)

In addition to initially using the Sparkfun Weather Shield hardware,
weathernode's code is derived from their Weather_Shield example code
which is also licensed under the "Beerware license."

However, it is important to nodet that weathernode's code is licensed
under the GNU GPLv3 or later. The full text of the license can be read
in the `COPYING.txt` file in the weathernode source directory.

