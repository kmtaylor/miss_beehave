import machine, time, uos, as5600
from umodbus.serial import ModbusRTU

#Setup pins
machine.Pin(machine.Pin.board.OUT1,  mode=machine.Pin.ALT, alt=5) # SCL 
machine.Pin(machine.Pin.board.ESTOP, mode=machine.Pin.ALT, alt=6) # SDA out
machine.Pin(machine.Pin.board.P1X,   mode=machine.Pin.ALT, alt=6) # SDA in
machine.Pin(machine.Pin.board.P10X,  mode=machine.Pin.ALT, alt=6) # Required dummy sclk

led = machine.Pin('CPX')

def as5600_poll(t):
    as5600.poll()

as5600_timer = machine.Timer(-1, freq = 1000, callback = as5600_poll)

uos.dupterm(None, 1)
client = ModbusRTU(5)

while True:
    result = client.process()
    client.set_hreg(4, as5600.get_status())
    client.set_hreg(5, as5600.get_pos())
