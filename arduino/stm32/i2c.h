extern "C" {
    extern void i2c_setup(uint8_t scl, uint8_t sda_out, uint8_t sda_in, uint8_t sclk);
    extern void i2c_poll(void);
    extern uint16_t i2c_get_data(uint8_t byte);
}
