/*

MicMask is intended to utilize a ESP32 along with an I2S amp and I2S mic to amplify voice locally within a mask. Additional IIR filters are imagined to be working as well.

*/

// Setup time
#include "driver/i2s.h"
const i2s_port_t I2S_PORT_IN = I2S_NUM_0; // this is the mic port.
const i2s_port_t I2S_PORT_OUT = I2S_NUM_1; // this is the amp port.


void setup() {
    Serial.begin(115200);
    esp_err_t err;

    // Microphone I2S Config
    i2s_config_t i2s_config_in ;
  
    i2s_config_in.mode = i2s_mode_t(I2S_MODE_MASTER | I2S_MODE_RX); // Receive, not transfer
    i2s_config_in.sample_rate = 16000;                         // 16KHz
    i2s_config_in.bits_per_sample = I2S_BITS_PER_SAMPLE_32BIT; // mic spits 32 bit values
    i2s_config_in.channel_format = I2S_CHANNEL_FMT_ONLY_RIGHT; // use right channel
    i2s_config_in.communication_format = i2s_comm_format_t(I2S_COMM_FORMAT_STAND_I2S );
    i2s_config_in.intr_alloc_flags = ESP_INTR_FLAG_LEVEL1;    // Interrupt level 1
    i2s_config_in.dma_buf_count = 4;                           // number of buffers
    i2s_config_in.dma_buf_len = 512;                             // 512 samples per buffer (minimum)

    // Microphone I2S Pin Config
    i2s_pin_config_t pin_config_in = {
		.bck_io_num = GPIO_NUM_17,
		.ws_io_num = GPIO_NUM_18,
		.data_out_num = I2S_PIN_NO_CHANGE,
		.data_in_num = GPIO_NUM_5
	};

    // Configuring the I2S driver and pins.
    // This function must be called before any I2S driver read/write operations.

    // installing mic.
    err = i2s_driver_install(I2S_PORT_IN, &i2s_config_in, 0, NULL);
    if (err != ESP_OK) {
        Serial.printf("Failed installing mic driver: %d\n", err);
        while (true);
    }
    err = i2s_set_pin(I2S_PORT_IN, &pin_config_in);
    if (err != ESP_OK) {
        Serial.printf("Failed setting mic pin: %d\n", err);
        while (true);
    }
    Serial.println("I2S mic driver installed.");

     // Speaker I2S config
    i2s_config_t i2s_config_out ;
    i2s_config_out.mode = i2s_mode_t(I2S_MODE_MASTER | I2S_MODE_TX); // Transmit, not receive
    i2s_config_out.sample_rate = 16000;                  // 16KHz
    i2s_config_out.bits_per_sample = I2S_BITS_PER_SAMPLE_16BIT; 
    i2s_config_out.channel_format = I2S_CHANNEL_FMT_ONLY_RIGHT; // use right channel
    i2s_config_out.communication_format = i2s_comm_format_t(I2S_COMM_FORMAT_STAND_I2S),
    i2s_config_out.intr_alloc_flags = ESP_INTR_FLAG_LEVEL1;     // Interrupt level 1
    i2s_config_out.dma_buf_count = 4;                          // number of buffers
    i2s_config_out.dma_buf_len = 512;                              // 512 samples per buffer (1/2 max)
    i2s_config_out.use_apll = 0;                                // Disabled
    i2s_config_out.tx_desc_auto_clear = true;
    i2s_config_out.fixed_mclk = I2S_PIN_NO_CHANGE;
  
    i2s_pin_config_t pin_config_out = {
			.bck_io_num = GPIO_NUM_26,
			.ws_io_num = GPIO_NUM_25,
			.data_out_num = GPIO_NUM_22,
			.data_in_num = GPIO_NUM_23
	};

    // Configuring the I2S driver and pins.
    // This function must be called before any I2S driver read/write operations.

    // installing speaker.
    err = i2s_driver_install(I2S_PORT_OUT, &i2s_config_out, 0, NULL);
    if (err != ESP_OK) {
        Serial.printf("Failed installing amp driver: %d\n", err);
        while (true);
    }
    err = i2s_set_pin(I2S_PORT_OUT, &pin_config_out);
    if (err != ESP_OK) {
        Serial.printf("Failed setting amp pin: %d\n", err);
        while (true);
    }
    Serial.println("I2S amp driver installed.");

}

void loop() {
  
    uint16_t buf_len = 512;
    char *inputbuffer;
    inputbuffer = (char*)calloc(buf_len, sizeof(char));
    esp_err_t err;
    int bytes_written = 0;
    while(true){
        
        char *buf_ptr_read = inputbuffer;
        char *buf_ptr_write = inputbuffer;

        size_t bytes_read = 0;
        while(bytes_read==0){
            
            err = i2s_read(I2S_PORT_IN,inputbuffer,buf_len,&bytes_read,0);

        }

        uint32_t samples_read = bytes_read / 2 / (I2S_BITS_PER_SAMPLE_32BIT / 8);
        for( int i = 0; i < samples_read; i++) {

            buf_ptr_write[0] = buf_ptr_read[2]; // mid
			buf_ptr_write[1] = buf_ptr_read[3]; // high

			// right
			buf_ptr_write[2] = buf_ptr_write[0]; // mid
			buf_ptr_write[3] = buf_ptr_write[1]; // high


            // let's make a shift


			buf_ptr_write += 2 * (I2S_BITS_PER_SAMPLE_16BIT / 8);
			buf_ptr_read += 2 * (I2S_BITS_PER_SAMPLE_32BIT / 8);
        }

        // now echo to the speaker
        bytes_written = samples_read * 2 * (I2S_BITS_PER_CHAN_16BIT /8);
        size_t writtenbytes;
        for( int i = 0; i< samples_read; i++){

            // let's write each 32 bit sample.
            uint32_t s32;

            s32 = inputbuffer[i] << 24 + inputbuffer[i+1] << 16 + inputbuffer[i+2]<< 8 + inputbuffer[i+3];
            Serial.println(s32);
            i2s_write(I2S_PORT_OUT,(const char*) &s32,sizeof(uint32_t),&writtenbytes,50);

        }
            
            Serial.println(bytes_read);
            Serial.println(samples_read);
    }






  
}
