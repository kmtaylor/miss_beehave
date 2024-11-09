import machine, as5600

class HeadServo:
    def __init__(self, freq, pwm_channel, dir_pin):
        # I2C pins: (SCL, SDA out, SDA in, required dummy sclk)
        machine.Pin('OUT1',  mode=machine.Pin.ALT, alt=5)
        machine.Pin('ESTOP', mode=machine.Pin.ALT, alt=6)
        machine.Pin('P1X',   mode=machine.Pin.ALT, alt=6)
        machine.Pin('P10X',  mode=machine.Pin.ALT, alt=6)

        self.timer = machine.Timer(-1, freq=freq, callback=self.process)
        self.count = 0
        self.freq = freq
        self.pwm_channel = pwm_channel
        self.dir_pin = dir_pin

    def process(self, timer):
        if self.count == self.freq - 1:
            #print(as5600.get_pos())
            self.count = 0

        self.count += 1
        as5600.poll()

    def get_status(self):
        return as5600.get_status()

    def get_pos(self):
        return as5600.get_pos()

    def set_dir(self, dir):
        self.dir_pin.value(dir)

    def set_pwm(self, pwm):
        self.pwm_channel.pulse_width_percent(pwm)
