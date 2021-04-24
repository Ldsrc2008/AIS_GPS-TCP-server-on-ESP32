# AIS_GPS-TCP-server-on-ESP32
Transport serial GPS and AIS NEMA to TCP

I have a open CPN installed on PC, and a Open plotter on Rasperry Pi, both have wifi.
The idea is to use esp32 as TCP server and it's multiple serial ports to get GPS and NEMA from serials, and transport to TCP port.
So that my Pi or PC can receive data from wifi connection.
components:
1. GPS module
 There are many GPS　module available if you looking into drone DIY martket, I use one like this, with antenna integrated.
 3.3V serial interface, output NEMA sentences.
 
![image](https://user-images.githubusercontent.com/2297151/115944969-bc65f380-a4eb-11eb-8123-530caa02d19a.png)


2. AIS module
 There are very few choice, a cheap solution is to use SDR on Pi as AIS decoder, but this will quite complicate if you use it on windowsPC.
 And with a USB connected to Pi or PC, your placement of PC will be limited due to USB cable and antenna cable.
 I found a module from China:
 
 https://item.taobao.com/item.htm?spm=a312a.7700824.w4004-2690632040.12.7d0225e75FRzIe&id=521807253726
 
![image](https://user-images.githubusercontent.com/2297151/115945755-a8bc8c00-a4ef-11eb-85f0-dc6811931139.png)
![image](https://user-images.githubusercontent.com/2297151/115945763-aeb26d00-a4ef-11eb-87ab-9aa7459ecee3.png)

it's powered with 3.3V, connection is quite simple, the serial out put is 38400, 8N1, price is about 180 RMB, not really cheap, but the easier and faster way to start my test
