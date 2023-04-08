/* Helper for Rugged Circuits Industrial Shield */

#include <Arduino.h>
#include <Wire.h>

#define ADC_I2C_ADDR    0x48

uint16_t rugged_shield_read_adc(uint8_t channel) {
    uint16_t val;

    Wire.beginTransmission(ADC_I2C_ADDR);
    Wire.write(0x00); /* Conversion register */
    Wire.endTransmission();
    Wire.requestFrom(ADC_I2C_ADDR, 2);
    val  = Wire.read() << 4;
    val += Wire.read() >> 4;

    /* Start next conversion */
    Wire.beginTransmission(ADC_I2C_ADDR);
    Wire.write(0x01); /* Configuration register */
    Wire.write(0xC5 | (channel << 4));
    Wire.write(0x83); /* Defaults */
    Wire.endTransmission();

    return val;
}

void rugged_shield_setup(void) {
    Wire.begin();
    Wire.beginTransmission(0x21);
    Wire.write(0x00);
    Wire.write(0xC0); /* Configure port A as output */
    Wire.endTransmission();
}

void rugged_shield_write(uint8_t val) {
    Wire.beginTransmission(0x21);
    Wire.write(0x12);
    Wire.write(val);
    Wire.endTransmission();
}

