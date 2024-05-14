extern "C" {
    extern void i2c_setup(uint8_t scl, uint8_t sda);
    extern void i2c_poll(void);
    extern uint16_t i2c_get_data(void);
}
