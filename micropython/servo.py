import machine, as5600

class HeadServo:
    def __init__(self, freq, pwm_channel, dir_pin, pid_config = None):
        if pid_config:
            self.pid_max = pid_config[0]
            self.pid_p = pid_config[1]
            self.pid_i = pid_config[2]
        else:
            self.pid_max = 90
            self.pid_p = 10
            self.pid_i = 0.1

        # I2C pins: (SCL, SDA out, SDA in, required dummy sclk)
        machine.Pin('OUT1',  mode=machine.Pin.ALT, alt=5)
        machine.Pin('ESTOP', mode=machine.Pin.ALT, alt=6)
        machine.Pin('P1X',   mode=machine.Pin.ALT, alt=6)
        machine.Pin('P10X',  mode=machine.Pin.ALT, alt=6)

        self.timer = machine.Timer(-1, freq=freq, callback=self.process)
        self.pwm_channel = pwm_channel
        self.dir_pin = dir_pin
        self.enable = False

        self.set_pos = 0
        self.cur_pos = 0
        self.cor_p = 0
        self.cor_i = 0
        self.cor_pi = 0

    def process(self, timer):
        self.cur_pos = as5600.get_pos()
        err = self.set_pos - self.cur_pos

        self.cor_p = self.pid_p * err
        self.cor_p = max(-self.pid_max, min(self.cor_p, self.pid_max))
        self.cor_i += self.pid_i * err
        self.cor_i = max(-self.pid_max - self.cor_p, 
                         min(self.cor_i, self.pid_max - self.cor_p))
        self.cor_pi = self.cor_p + self.cor_i

        if self.enable:
            self.set_dir(self.cor_pi < 0)
            self.set_pwm(abs(self.cor_pi))

        as5600.poll()

    def print(self):
        print(f"Set: {self.set_pos}, Pos: {self.cur_pos}, " +
              f"cor_p: {self.cor_p}, cor_i: {self.cor_i}, " +
              f"cor_pi: {self.cor_pi}")

    def get_status(self):
        return as5600.get_status()

    def set_dir(self, dir):
        self.dir_pin.value(dir)

    def set_pwm(self, pwm):
        self.pwm_channel.pulse_width_percent(pwm)
