extern "C" {
    extern void as5600_setup(uint8_t scl, uint8_t sda_out, uint8_t sda_in, uint8_t sclk);
    extern void as5600_poll(void);
    extern uint16_t as5600_get_pos(void);
    extern uint16_t as5600_get_status(void);
}
