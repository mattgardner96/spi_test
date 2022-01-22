#include "pico/stdlib.h"
#include "pico-ssd1306/ssd1306.h"
#include "pico-ssd1306/textRenderer/TextRenderer.h"
#include "hardware/i2c.h"
#include "pico/printf.h"

// Use the namespace for convenience
using namespace pico_ssd1306;

int value;

int main(){
    // Init i2c0 controller         //changed to i2c1
    i2c_init(i2c1, 1000000);
    // Set up pins 12 and 13
    gpio_set_function(2, GPIO_FUNC_I2C);            //changed from 12 -> 2
    gpio_set_function(3, GPIO_FUNC_I2C);            //changed from 13 -> 3
    gpio_pull_up(2);                                //changed from 12 -> 2
    gpio_pull_up(3);                                //changed from 13 -> 3

    // If you don't do anything before initializing a display pi pico is too fast and starts sending
    // commands before the screen controller had time to set itself up, so we add an artificial delay for
    // ssd1306 to set itself up
    sleep_ms(250);

    // Create a new display object at address 0x3D and size of 128x64
    SSD1306 display = SSD1306(i2c1, 0x3C, Size::W128xH32);

    // Here we rotate the display by 180 degrees, so that it's not upside down from my perspective
    // If your screen is upside down try setting it to 1 or 0
    display.setOrientation(0);

    char buffer[] = "Testing"; // init the buffer
    value = 0;

    while(1) {
        // clear whatever is on the screen (old text)
        drawText(&display, font_12x16, buffer, 0, 0, pico_ssd1306::WriteMode::SUBTRACT); // add the text
        display.sendBuffer(); // write to display

        // convert to a char*
        sprintf(buffer,"%d",value); 

        // Draw text on display
        // After passing a pointer to display, we need to tell the function what font and text to use
        // Available fonts are listed in textRenderer's readme
        // Last we tell this function where to anchor the text
        // Anchor means top left of what we draw
        drawText(&display, font_12x16, buffer, 0, 0, pico_ssd1306::WriteMode::ADD); // add the text
        display.sendBuffer(); // write to display

        //wait one second
        sleep_ms(1000);

        //increment the value on the display
        value++;
    }
}