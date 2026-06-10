#pragma once

#include <Arduino.h>
#include <SPI.h>

#include "registerDefs.h"


class MAX7456
{
    public:
        MAX7456(SPIClass* SpiDevice, uint8_t CHIP_SELECT) :
            spi(SpiDevice), cs(CHIP_SELECT) {}

        uint8_t begin()
        {
            pinMode(cs, OUTPUT);
            digitalWrite(cs, HIGH);
            spi->begin();
            return 0;
        }

        uint8_t update(){return 0;};

        // =========================
        // STAT
        // =========================
        uint8_t readSTAT()
        {
            spi->beginTransaction(settings);
            digitalWrite(cs, LOW);
            spi->transfer(STATUS);
            uint8_t status = spi->transfer(0x00);
            digitalWrite(cs, HIGH);
            spi->endTransaction();

            return status;
        };

        uint8_t writeRegister(uint8_t address, uint8_t value)
        {
            spi->beginTransaction(settings);
            digitalWrite(cs, LOW);
            // write value
            spi->transfer(address);
            spi->transfer(value);
            // read it back
            spi->transfer((address | READ_OFFSET));
            uint8_t returnVal = spi->transfer(0x00);
            digitalWrite(cs, HIGH);
            spi->endTransaction();

            return returnVal;
        }

        // =========================
        // VM0
        // =========================

        uint8_t packVM0() const
        {
            uint8_t reg = 0;
            reg |= (vm0_standard_select ? 1 : 0) << 6;
            reg |= (vm0_sync_select_1 ? 1 : 0) << 5;
            reg |= (vm0_sync_select_0 ? 1 : 0) << 4;
            reg |= (vm0_enable_osd ? 1 : 0) << 3;
            reg |= (vm0_video_buffer_enable ? 1 : 0) << 2;
            reg |= (vm0_software_reset ? 1 : 0) << 1;
            reg |= (vm0_output_enable ? 1 : 0) << 0;
            return reg;
        }

        void unpackVM0(uint8_t reg)
        {
            vm0_standard_select    = (reg >> 6) & 0x1;
            vm0_sync_select_1      = (reg >> 5) & 0x1;
            vm0_sync_select_0      = (reg >> 4) & 0x1;
            vm0_enable_osd         = (reg >> 3) & 0x1;
            vm0_video_buffer_enable= (reg >> 2) & 0x1;
            vm0_software_reset     = (reg >> 1) & 0x1;
            vm0_output_enable      = (reg >> 0) & 0x1;
        }

        // =========================
        // VM1
        // =========================

        uint8_t packVM1() const
        {
            uint8_t reg = 0;
            reg |= (vm1_background_mode ? 1: 0) << 7;
            reg |= (vm1_background_brightness & 0x7) << 4;
            reg |= (vm1_blink_time & 0x3) << 2;
            reg |= (vm1_blink_duty_cycle ? 1 : 0) << 0;
            return reg;
        }

        void unpackVM1(uint8_t reg)
        {
            vm1_background_mode         = (reg >> 7) & 0x1;
            vm1_background_brightness   = (reg >> 4) & 0x7;
            vm1_blink_time              = (reg >> 2) & 0x3;
            vm1_blink_duty_cycle        = (reg >> 0) & 0x3;
        }

        // =========================
        // Horizontal / Vertical Offset
        // =========================

        uint8_t packHOS() const { return horizontal_offset & 0x3F; }
        void unpackHOS(uint8_t v) { horizontal_offset = v; }

        uint8_t packVOS() const { return vertical_offset & 0x1F; }
        void unpackVOS(uint8_t v) { vertical_offset = v; }

        // =========================
        // Display Memory Mode
        // =========================

        uint8_t packDMM() const 
        { 
            uint8_t reg = 0;
            reg |= (dmm_operation_mode ? 1 : 0) << 6;
            reg |= (dmm_local_background_control ? 1 : 0) << 5;
            reg |= (dmm_blink_bit ? 1 : 0) << 4;
            reg |= (dmm_invert_bit ? 1 : 0) << 3;
            reg |= (dmm_clear_display_memory ? 1 : 0) << 2;
            reg |= (dmm_vertical_sync_clear ? 1 : 0) << 1;
            reg |= (dmm_auto_increment_mode ? 1 : 0) << 0;
            return reg; 
        }
        void unpackDMM(uint8_t reg) 
        {
            dmm_operation_mode              = (reg >> 6) & 0x1;
            dmm_local_background_control    = (reg >> 5) & 0x1;
            dmm_blink_bit                   = (reg >> 4) & 0x1;
            dmm_invert_bit                  = (reg >> 3) & 0x1;
            dmm_clear_display_memory        = (reg >> 2) & 0x1;
            dmm_vertical_sync_clear         = (reg >> 1) & 0x1;
            dmm_auto_increment_mode         = (reg >> 0) & 0x1;
        }

        // =========================
        // OSD Insertion Mux
        // =========================

        uint8_t packOSDM() const
        {
            uint8_t reg = 0;
            reg |= (osd_rise_time & 0x7) << 3;
            reg |= (osd_switch_time & 0x7);
            return reg;
        }

        void unpackOSDM(uint8_t reg)
        {
            osd_rise_time   = (reg >> 3) & 0x7;
            osd_switch_time = reg & 0x7;
        }

        // =========================
        // Row Brightness
        // =========================

        uint8_t packRowBrightness(uint8_t row) const
        {
            return row_brightness[row] & 0x0F;
        }

        void unpackRowBrightness(uint8_t row, uint8_t value)
        {
            row_brightness[row] = value & 0x0F;
        }

        // =========================
        // OSD Black Level
        // =========================

        uint8_t packOSDBL() const
        {
            uint8_t reg;
            reg |= (osd_black_level ? 1 : 0) << 4;
            return reg;
        }

        void unpackOSDBL(uint8_t reg)
        {
            osd_black_level = reg;
        }

        bool writeOSDBL()
        {
            // this byte is slightly weird, look at Page 36 of DS
            // we first need to read the byte to get values for bits 0-3
            spi->beginTransaction(settings);
            digitalWrite(cs, LOW);
            spi->transfer(OSDM_OSD_INSERTION_MUX_READ);
            uint8_t reg = spi->transfer(0x00);
            // we then modify the register with our desired value in bit 4 and send it
            reg = ((osd_black_level ? 1 : 0) << 4);
            spi->transfer(OSDM_OSD_INSERTION_MUX);
            spi->transfer(reg);
            // read it again
            spi->transfer(OSDM_OSD_INSERTION_MUX_READ);
            uint8_t reg_read = spi->transfer(0x00);
            return (reg == reg_read);
        }

        // =========================
        // Configuration Variables
        // =========================

        // --- VM0 ---
        bool vm0_standard_select;      // Bit 6
        bool vm0_sync_select_1;        // Bit 5
        bool vm0_sync_select_0;        // Bit 4
        bool vm0_enable_osd;           // Bit 3
        bool vm0_video_buffer_enable;  // Bit 2
        bool vm0_software_reset;       // Bit 1
        bool vm0_output_enable;        // Bit 0

        // --- VM1 ---
        bool vm1_background_mode;       // Bit 7
        uint8_t vm1_background_brightness;   // Bits 6:4
        uint8_t vm1_blink_time;         // Bits 3:2
        uint8_t vm1_blink_duty_cycle;   // Bits 1:0

        // --- Offsets ---
        uint8_t horizontal_offset;     // HOS
        uint8_t vertical_offset;       // VOS

        // --- Display Memory Mode ---
        bool dmm_operation_mode;            // Bit 6
        bool dmm_local_background_control;  // Bit 5
        bool dmm_blink_bit;                 // Bit 4
        bool dmm_invert_bit;                // Bit 3
        bool dmm_clear_display_memory;      // Bit 2
        bool dmm_vertical_sync_clear;       // Bit 1
        bool dmm_auto_increment_mode;       // Bit 0

        // --- OSD Insertion Mux ---
        uint8_t osd_rise_time;         // Bits 5:3
        uint8_t osd_switch_time;       // Bits 2:0

        // --- Row Brightness ---
        uint8_t row_brightness[16];    // RB0–RB15 (lower 4 bits used)

        // --- OSD Black Level ---
        bool osd_black_level;


    private:
        SPIClass* spi;
        SPISettings settings = SPISettings(5, MSBFIRST, SPI_MODE1);
        uint8_t cs;
};