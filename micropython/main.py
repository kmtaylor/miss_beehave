import pyb, uos
from umodbus.serial import ModbusRTU
from servo import HeadServo

pwm_timer = pyb.Timer(9, freq=1000)
pwm_channel = pwm_timer.channel(2, pyb.Timer.PWM, pin=pyb.Pin('DIC'))
dir_pin = pyb.Pin('CPC')
servo = HeadServo(1000, pwm_channel, dir_pin)

mb = ModbusRTU(5)
mb.set_hreg(1, 0) # PWM
mb.set_hreg(2, 0) # Direction

def modbus_main():
    uos.dupterm(None, 1)
    while True:
        result = mb.process()
        servo.set_pwm(mb.get_hreg(1))
        servo.set_dir(mb.get_hreg(2))
        mb.set_hreg(4, servo.get_status())
        mb.set_hreg(5, servo.get_pos())

#modbus_main()
