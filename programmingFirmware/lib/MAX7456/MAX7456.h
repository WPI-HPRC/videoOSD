#pragma once

#include <Arduino.h>
#include <SPI.h>

#include "registerDefs.h"


class MAX7456
{
    public:
        MAX7456(SPIClass* SpiDevice, uint8_t CHIP_SELECT) :
            spi(SpiDevice), cs(CHIP_SELECT) {}

        uint8_t begin();

        uint8_t update();

        // =========================
        // VM0
        // =========================

        uint8_t packVM0() const
        {
            uint8_t reg = 0;
            reg |= (vm0_sync_select_1 ? 1 : 0) << 5;
            reg |= (vm0_sync_select_0 ? 1 : 0) << 4;
            reg |= (vm0_enable_osd ? 1 : 0) << 3;
            reg |= (vm0_video_buffer_enable ? 1 : 0) << 2;
            reg |= (vm0_software_reset ? 1 : 0) << 1;
            return reg;
        }

        void unpackVM0(uint8_t reg)
        {
            vm0_sync_select_1      = (reg >> 5) & 0x1;
            vm0_sync_select_0      = (reg >> 4) & 0x1;
            vm0_enable_osd         = (reg >> 3) & 0x1;
            vm0_video_buffer_enable= (reg >> 2) & 0x1;
            vm0_software_reset     = (reg >> 1) & 0x1;
        }

        // =========================
        // VM1
        // =========================

        uint8_t packVM1() const
        {
            uint8_t reg = 0;
            reg |= (vm1_background_mode & 0x7) << 4;
            reg |= (vm1_blink_enable ? 1 : 0) << 3;
            return reg;
        }

        void unpackVM1(uint8_t reg)
        {
            vm1_background_mode = (reg >> 4) & 0x7;
            vm1_blink_enable    = (reg >> 3) & 0x1;
        }

        // =========================
        // Horizontal / Vertical Offset
        // =========================

        uint8_t packHOS() const { return horizontal_offset; }
        void unpackHOS(uint8_t v) { horizontal_offset = v; }

        uint8_t packVOS() const { return vertical_offset; }
        void unpackVOS(uint8_t v) { vertical_offset = v; }

        // =========================
        // Display Memory Mode
        // =========================

        uint8_t packDMM() const { return display_memory_mode; }
        void unpackDMM(uint8_t reg) { display_memory_mode = reg; }

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
            return osd_black_level;
        }

        void unpackOSDBL(uint8_t reg)
        {
            osd_black_level = reg;
        }


    private:
        SPIClass* spi;
        uint8_t cs;

        // =========================
        // Configuration Variables
        // =========================

        // --- VM0 ---
        bool vm0_sync_select_1;        // Bit 5
        bool vm0_sync_select_0;        // Bit 4
        bool vm0_enable_osd;           // Bit 3
        bool vm0_video_buffer_enable;  // Bit 2
        bool vm0_software_reset;       // Bit 1

        // --- VM1 ---
        uint8_t vm1_background_mode;   // Bits 6:4
        bool vm1_blink_enable;         // Bit 3

        // --- Offsets ---
        uint8_t horizontal_offset;     // HOS
        uint8_t vertical_offset;       // VOS

        // --- Display Memory Mode ---
        uint8_t display_memory_mode;   // DMM (raw for now)

        // --- OSD Insertion Mux ---
        uint8_t osd_rise_time;         // Bits 5:3
        uint8_t osd_switch_time;       // Bits 2:0

        // --- Row Brightness ---
        uint8_t row_brightness[16];    // RB0–RB15 (lower 4 bits used)

        // --- OSD Black Level ---
        uint8_t osd_black_level;

};