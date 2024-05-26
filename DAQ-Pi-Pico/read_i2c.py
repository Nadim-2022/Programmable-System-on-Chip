from machine import Pin, I2C
from ssd1306 import SSD1306_I2C
import utime


i2c = I2C(1, scl=Pin(15), sda=Pin(14), freq=400000) 

# Address of the I2C slave (PSoC 5)
slave_address = 0x08

WIDTH =128
HEIGHT= 64
x = 0
y = 0
oled = SSD1306_I2C(WIDTH,HEIGHT,i2c)

# make true to read full buffer
fullbuff = False

while True:
    
    if not fullbuff:
        data = i2c.readfrom(slave_address, 4)

        decimal_data = [int.from_bytes(data[i:i+1], 'little') for i in range(4)]

        print("Received data from PSoC 5:", decimal_data)
        oled.fill(0) 
        oled.text(f"Temp1: {decimal_data[0]} C", 0, 0)
        oled.text(f"Temp2: {decimal_data[1]} C", 0, 10)
        oled.text(f"Temp3: {decimal_data[2]} C", 0, 20)
        oled.text(f"Temp4: {decimal_data[3]} C", 0, 30)
    else:
        data = i2c.readfrom(slave_address, 40)

        decimal_data = [int.from_bytes(data[i:i+1], 'little') for i in range(40)]

        print("Received data from PSoC 5:", decimal_data)
        oled.fill(0) 
        oled.text(f"Temp1: {decimal_data[9]} C", 0, 0)
        oled.text(f"Temp2: {decimal_data[19]} C", 0, 10)
        oled.text(f"Temp3: {decimal_data[29]} C", 0, 20)
        oled.text(f"Temp4: {decimal_data[39]} C", 0, 30)
    
    oled.show()

    utime.sleep_ms(1000)
    