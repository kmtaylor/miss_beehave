import machine, time
import uos, pyb
from umodbus.serial import ModbusRTU

led = machine.Pin('CPX')
for i in range(20):
    led.value(not led.value())
    time.sleep(0.1)

uos.dupterm(None, 1)
client = ModbusRTU(5)

def toggle_led(reg_type, address, val):
    led.value(not led.value())

client.add_hreg(0xF000, on_set_cb = toggle_led)

while True:
    result = client.process()
