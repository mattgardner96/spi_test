#include "pico/stdlib.h"
#include "pico-ssd1306/ssd1306.h"
#include "pico-ssd1306/textRenderer/TextRenderer.h"
#include "pico/printf.h"
#include "hardware/spi.h"

#define SPI_CSN_PIN 1

// Use the namespace for convenience
using namespace pico_ssd1306;

int value,write_bytes;
char buffer[] = "Testing"; // init the buffer

int main() {

    stdio_init_all();

    // Init i2c controller
    i2c_init(i2c1, 1000000);
    
    gpio_set_function(PICO_DEFAULT_I2C_SDA_PIN, GPIO_FUNC_I2C);
    gpio_set_function(PICO_DEFAULT_I2C_SCL_PIN, GPIO_FUNC_I2C);
    gpio_pull_up(PICO_DEFAULT_I2C_SDA_PIN);
    gpio_pull_up(PICO_DEFAULT_I2C_SCL_PIN);

    // If you don't do anything before initializing a display pi pico is too fast and starts sending
    // commands before the screen controller had time to set itself up, so we add an artificial delay for
    // ssd1306 to set itself up
    sleep_ms(5000);

    // Create a new display object at address 0x3D and size of 128x64
    SSD1306 display = SSD1306(i2c1, 0x3C, Size::W128xH32);

    // Here we rotate the display by 180 degrees, so that it's not upside down from my perspective
    // If your screen is upside down try setting it to 1 or 0
    display.setOrientation(0);

    value = 0;

    // SPI CONFIGURATION
    printf("%s","Initializing SPI\n");
    spi_init(spi0,1000*1000); //initialize spi0 at 1MHz
    gpio_set_function(PICO_DEFAULT_SPI_SCK_PIN,GPIO_FUNC_SPI);
    gpio_set_function(PICO_DEFAULT_SPI_TX_PIN,GPIO_FUNC_SPI);
    gpio_set_function(PICO_DEFAULT_SPI_RX_PIN,GPIO_FUNC_SPI);
    gpio_set_function(SPI_CSN_PIN, GPIO_FUNC_SPI);

    spi_set_format(spi0,8,SPI_CPOL_0,SPI_CPHA_0,SPI_LSB_FIRST);

    while(1) {
        // clear whatever is on the screen (old text)
        drawText(&display, font_12x16, buffer, 0, 0, pico_ssd1306::WriteMode::SUBTRACT); // add the text
        display.sendBuffer(); // write to display

        // convert the value to a text char*
        sprintf(buffer,"%d\0",value);

        printf("(uint8_t*) buffer value: %0x\n",*((uint8_t*)buffer));
        printf("value contains: %#x\n",value);

        // Draw text on display
        // After passing a pointer to display, we need to tell the function what font and text to use
        // Available fonts are listed in textRenderer's readme
        // Last we tell this function where to anchor the text
        // Anchor means top left of what we draw

        // show the text
        drawText(&display, font_12x16, buffer, 0, 0, pico_ssd1306::WriteMode::ADD); // add the text
        display.sendBuffer(); // write to display

        if(spi_is_writable(spi0)) {
            printf("%s","Writing to SPI\n");
            write_bytes = spi_write_blocking(spi0,(uint8_t*)buffer,4);
            // printf("wrote %d bytes\n",write_bytes);
            printf("wrote %d bytes of (uint8_t*)buffer: %#x\n",write_bytes,*buffer);
        } else {
            // printf("%s", "SPI not writable\n");
        }

        //wait one second
        sleep_ms(1000);

        //increment the value on the display
        value++;
    }
}