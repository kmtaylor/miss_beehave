import pyb, uos, time
from umodbus.serial import ModbusRTU
from servo import HeadServo

pwm_timer = pyb.Timer(9, freq=1000)
pwm_channel = pwm_timer.channel(2, pyb.Timer.PWM, pin=pyb.Pin('DIC'))
dir_pin = pyb.Pin('CPC')
head_servo = HeadServo(1000, pwm_channel, dir_pin)

def servo_print():
    while True:
        head_servo.print()
        time.sleep(0.5)

def servo_pos(reg_type, address, val):
    head_servo.set_pos = val[0]
    head_servo.enable = True

mb = ModbusRTU(5)
mb.add_hreg(1, on_set_cb=servo_pos) # Position

def modbus_main():
    uos.dupterm(None, 1)
    while True:
        mb.set_hreg(2, head_servo.cur_pos)
        mb.set_hreg(3, round(head_servo.cor_p))
        mb.set_hreg(4, round(head_servo.cor_i))
        mb.set_hreg(5, round(head_servo.cor_pi))
        mb.set_hreg(6, head_servo.get_status())
        mb.process()

#servo_pos(None, None, [800])
#servo_print()
modbus_main()
