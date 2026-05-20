#include <Arduino.h>
#include <MAX7456.h>

#define CS 12
#define SCK 11
#define MISO 10
#define MOSI 9

const char* CALLSIGN = "KV0R";





SPIClass bus;
MAX7456 max7456(&bus, CS);

void setup() {
    bus.setSCK(SCK);
    bus.setMISO(MISO);
    bus.setMOSI(MOSI);

    SerialUSB.begin(115200); while(!SerialUSB){};
    pinMode(LED_BUILTIN, OUTPUT); digitalWrite(LED_BUILTIN, HIGH);

    SerialUSB.print("Starting MAX7456 Interface...");
    max7456.begin();
    SerialUSB.println("Done!");

    delay(50);

    SerialUSB.print("Reading Status Register...");
    uint8_t status = max7456.readSTAT();
    SerialUSB.print("Done! Returned "); SerialUSB.println(status, BIN);

    delay(50);

    SerialUSB.println("");
    // Build VM0 Register
    // Page 25 of Datasheet
    max7456.vm0_standard_select = 0; // NTSC
    max7456.vm0_sync_select_1 = 0; // Autoselect
    max7456.vm0_sync_select_0 = 0; // Don't care, but external
    max7456.vm0_enable_osd = 1; // Do enable OSD
    max7456.vm0_video_buffer_enable = 1; // start displaying at next VSYNC (kinda irrelevent)
    max7456.vm0_software_reset = 0; // DO NOT RESET
    max7456.vm0_output_enable = 0; // do output
    uint8_t vm0_write = max7456.packVM0();
    SerialUSB.print("Writing VM0 Register...");
    uint8_t vm0_read = max7456.writeRegister(VM0_VIDEO_MODE_0, vm0_write);
    SerialUSB.print("Done! ");
    if(vm0_read == vm0_write){
        SerialUSB.print("Matched! ");
    } else{
        SerialUSB.print("Did not match. ");
    } SerialUSB.print("Received: "); SerialUSB.println(vm0_read, BIN);

    SerialUSB.println("");
    // Build VM1 Register
    // Page 26 of Datasheet
    max7456.vm1_background_mode = 0;
    max7456.vm1_background_brightness = 0b000;
    max7456.vm1_blink_time = 0b00;
    max7456.vm1_blink_duty_cycle = 0b00;
    uint8_t vm1_write = max7456.packVM1();
    SerialUSB.print("Writing VM1 Register...");
    uint8_t vm1_read = max7456.writeRegister(VM1_VIDEO_MODE_1, vm1_write);
    SerialUSB.print("Done! ");
    if(vm1_read == vm1_write){
        SerialUSB.print("Matched! ");
    } else{
        SerialUSB.print("Did not match. ");
    } SerialUSB.print("Received: "); SerialUSB.println(vm1_read, BIN);

    SerialUSB.println("");
    max7456.horizontal_offset = 0b100000; // zero offset
    uint8_t hos_write = max7456.packHOS();
    SerialUSB.print("Writing HOS Register...");
    uint8_t hos_read = max7456.writeRegister(HOS_HORIZONTAL_OFFSET, hos_write);
    SerialUSB.print("Done! ");
    if(hos_read == hos_write){
        SerialUSB.print("Matched! ");
    } else{
        SerialUSB.print("Did not match. ");
    } SerialUSB.print("Received: "); SerialUSB.println(hos_read, BIN);

    SerialUSB.println("");
    max7456.vertical_offset = 0b10000; // zero offset
    uint8_t vos_write = max7456.packVOS();
    SerialUSB.print("Writing VOS Register...");
    uint8_t vos_read = max7456.writeRegister(VOS_VERTICAL_OFFSET, vos_write);
    SerialUSB.print("Done! ");
    if(vos_read == vos_write){
        SerialUSB.print("Matched! ");
    } else{
        SerialUSB.print("Did not match. ");
    } SerialUSB.print("Received: "); SerialUSB.println(vos_read, BIN);

    SerialUSB.println("");
    max7456.dmm_operation_mode = 1; // 16 bit mode (technically 8 bit mode but)
    max7456.dmm_local_background_control = 0;
    max7456.dmm_blink_bit = 0;
    max7456.dmm_invert_bit = 0;
    max7456.dmm_clear_display_memory = 0;
    max7456.dmm_vertical_sync_clear = 0;
    max7456.dmm_auto_increment_mode = 0;
    uint8_t dmm_write = max7456.packDMM();
    SerialUSB.print("Writing DMM Register...");
    uint8_t dmm_read = max7456.writeRegister(DMM_DISPLAY_MEMORY_MODE, dmm_write);
    SerialUSB.print("Done! ");
    if(dmm_read == dmm_write){
        SerialUSB.print("Matched! ");
    } else{
        SerialUSB.print("Did not match. ");
    } SerialUSB.print("Received: "); SerialUSB.println(dmm_read, BIN);

    SerialUSB.println("");
    max7456.osd_rise_time = 0b010;
    max7456.osd_switch_time = 0b010;
    uint8_t osdm_write = max7456.packOSDM();
    SerialUSB.print("Writing OSDM Register...");
    uint8_t osdm_read = max7456.writeRegister(OSDM_OSD_INSERTION_MUX, osdm_write);
    SerialUSB.print("Done! ");
    if(osdm_read == osdm_write){
        SerialUSB.print("Matched! ");
    } else{
        SerialUSB.print("Did not match. ");
    } SerialUSB.print("Received: "); SerialUSB.println(osdm_read, BIN);

    SerialUSB.println("");
    uint8_t row_brightness_value = 0b00000001; // 0% blacks and 100% whites (Page 35)
    SerialUSB.println("Writing Row Brightness Registers...");
    for(int i = 0; i<= 15; i++){
        Serial.print("Row " + String(i) + " ");
        uint8_t read = max7456.writeRegister((ROW_BRIGHTNESS + i), row_brightness_value);
        SerialUSB.print("Done! ");
        if(read == row_brightness_value){
            SerialUSB.print("Matched! ");
        } else{
            SerialUSB.print("Did not match. ");
        }
        SerialUSB.print("Received: "); SerialUSB.println(read, BIN);
    }
    SerialUSB.println("Row brightness finished.");

    max7456.osd_black_level = 0;
    SerialUSB.print("Writing OSDBL Register...");
    bool success_osdbl = max7456.writeOSDBL();
    SerialUSB.print("Done! ");
    if(success_osdbl){
        SerialUSB.println("Matched!");
    } else {
        SerialUSB.println("Did not match.");
    }
}

void loop() {

}

