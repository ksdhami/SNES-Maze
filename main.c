// Karndeep Dhami
// 10031989
// CPSC 359
// Assignment 4

// This program will initialize a frame buffer for a 1024 x 768 display and draw a maze on it
// A user will be able to use a SNES controller and controller a square, navigating through the maze
// As the user is traversing the maze, they will be indicated by a red square
// Once the user is at the end of the maze, they will be indicated by a green square
// The game begins by the user pressing the START button
// The user can use the directional button on the SNES controller to control the square
// At the end of the maze the player can select START again the go back to the beginning and play again 

// SNES Controller and Frame Buffer example code provided by Dr. Leonard Manzara was used and modified
// 08_SNESCONTROLLER and 09_FRAMEBUFFER files were used 

// Included header files
#include "uart.h"
#include "framebuffer.h"
#include "gpio.h"
#include "systimer.h"

unsigned short get_SNES();
void init_GPIO9_to_output();
void set_GPIO9();
void clear_GPIO9();
void init_GPIO11_to_output();
void set_GPIO11();
void clear_GPIO11();
void init_GPIO10_to_input();
unsigned int get_GPIO10();

// Maze; 0 indicates positions the user can travel 1 indicates walls
unsigned int maze[12][16] = {
        {1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1}, 
        {1, 0, 1, 0, 1, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1},
        {0, 0, 0, 0, 1, 0, 1, 0, 1, 0, 0, 0, 1, 1, 0, 1},
        {1, 0, 1, 1, 1, 0, 1, 0, 1, 1, 1, 1, 1, 1, 0, 1},
        {1, 0, 0, 1, 0, 0, 1, 0, 0, 0, 0, 0, 0, 1, 0, 1},
        {1, 1, 0, 0, 0, 1, 1, 1, 1, 1, 0, 1, 1, 1, 0, 1},
        {1, 0, 0, 1, 0, 1, 0, 0, 0, 1, 0, 1, 0, 0, 0, 1},
        {1, 0, 1, 1, 0, 1, 0, 1, 1, 1, 0, 1, 0, 1, 1, 1},
        {1, 0, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 0},
        {1, 1, 0, 1, 0, 0, 0, 1, 0, 1, 0, 1, 1, 1, 0, 1},
        {1, 0, 0, 1, 0, 1, 0, 0, 0, 1, 0, 0, 0, 0, 0, 1},
        {1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1}};
        

// Variables to keep track or current and next position of player
int playerRow;
int playerColumn;
int checkPlayerRow;
int checkPlayerColumn;

// Colours and buttons
#define WHITE   0x00FFFFFF
#define RED     0x00FF0000
#define GREEN   0x00008000
#define START   (1<<3)
#define LEFT    (1<<6)
#define RIGHT   (1<<7)
#define UP      (1<<4)
#define DOWN    (1<<5)


////////////////////////////////////////////////////////////////////////////////
//
//  Function:       main
//
//  Arguments:      none
//
//  Returns:        void
//
//  Description:    This function initializes the UART terminal and initializes
//                  a frame buffer for a 1024 x 768 display. Each pixel in the
//                  frame buffer is 32 bits in size, which encodes an RGB value
//                  (plus an 8-bit alpha channel that is not used). The program
//                  then draws and displays an 18 x 12 checker board pattern.
//
////////////////////////////////////////////////////////////////////////////////

void main()
{

    // Player start position
    playerRow = 2;
    playerColumn = 0;

    
    // start at [2][0]
    // end at [8][11]


    //char c;
    unsigned short data, currentState = 0xFFFF;

    // Initialize the UART terminal
    uart_init();

    // SNES CONTROLLER
    // Set up GPIO pin #9 for output (LATCH output)
    init_GPIO9_to_output();
    // Set up GPIO pin #11 for output (CLOCK output)
    init_GPIO11_to_output(); 
    // Set up GPIO pin #10 for input (DATA input)
    init_GPIO10_to_input();
    // Clear the LATCH line (GPIO 9) to low
    clear_GPIO9();
    // Set CLOCK line (GPIO 11) to high
    set_GPIO11();

    // FRAME BUFFER
    // Initialize the frame buffer
    initFrameBuffer();
    // Draw on the frame buffer and display it
    displayFrameBuffer();    

    // Loop forever, echoing characters received from the console
    while (1) {

        // Current position 
        checkPlayerRow = playerRow;
        checkPlayerColumn = playerColumn;

        // Read data from the SNES controller
    	data = get_SNES();

        // Write out data if the state of the controller has changed
        if (data != currentState) {
            // Record the state of the controller
            currentState = data;

            if((data & UP) == UP) { // LEFT ARROW
                checkPlayerRow = checkPlayerRow - 1;
                if (checkPlayerRow >= 0 && checkPlayerRow <= 11) {
                    if(maze[checkPlayerRow][playerColumn] == 0) {
                        // Recolour
                        drawSquare(playerRow * 64, playerColumn * 64, 64, WHITE);
                        // New colour
                        playerRow--;
                        drawSquare(playerRow * 64, playerColumn * 64, 64, RED);
                    }
                }

            } else if ((data & DOWN) == DOWN) { // RIGHT ARROW
                checkPlayerRow = checkPlayerRow + 1;
                if (checkPlayerRow >= 0 && checkPlayerRow <= 11) {
                    if(maze[checkPlayerRow][playerColumn] == 0) {
                        // Recolour
                        drawSquare(playerRow * 64, playerColumn * 64, 64, WHITE);
                        // New colour
                        playerRow++;
                       
                        // END
                        if((playerRow == 8) && (playerColumn == 15)) {
                            drawSquare(playerRow * 64, playerColumn * 64, 64, GREEN);
                        } else {
                            drawSquare(playerRow * 64, playerColumn * 64, 64, RED);
                        }
                    }
                }

            } else if ((data & LEFT) == LEFT) { // UP ARROW
                checkPlayerColumn = checkPlayerColumn - 1;
                if (checkPlayerColumn >= 0 && checkPlayerColumn <= 15) {
                    if(maze[playerRow][checkPlayerColumn] == 0) {
                        // Recolour
                        drawSquare(playerRow * 64, playerColumn * 64, 64, WHITE);
                        // New colour
                        playerColumn--;
                        drawSquare(playerRow * 64, playerColumn * 64, 64, RED);
                    }
                }
            
            } else if ((data & RIGHT) == RIGHT) { // DOWN ARROW
                checkPlayerColumn = checkPlayerColumn + 1;
                if (checkPlayerColumn >= 0 && checkPlayerColumn <= 15) {
                    if(maze[playerRow][checkPlayerColumn] == 0) {
                        // Recolour
                        drawSquare(playerRow * 64, playerColumn * 64, 64, WHITE);
                        // New colour
                        playerColumn++;

                        if((playerRow == 8) && (playerColumn == 15)) {
                            drawSquare(playerRow * 64, playerColumn * 64, 64, GREEN);
                        } else {
                            drawSquare(playerRow * 64, playerColumn * 64, 64, RED);
                        }
                    }
                }

            } else if (data == START && (playerRow == 2) && (playerColumn == 0)) { // START BUTTON AND BEGINNING 
                drawSquare(playerRow * 64, playerColumn * 64, 64, RED);


            } 
            else if ((playerRow == 8) && (playerColumn == 15 ) && data == START) { // START BUTTON AND END
                displayFrameBuffer();
                playerColumn = 0;
                playerRow = 2;
                drawSquare(playerRow * 64, playerColumn * 64, 64, RED);

            }
        }
            
        // Delay 1/30th of a second
        microsecond_delay(33333);
    }
}



// SNES CONTROLLER INPUT AND OUTPUT 

////////////////////////////////////////////////////////////////////////////////
//
//  Function:       get_SNES
//
//  Arguments:      none
//
//  Returns:        A short integer with the button presses encoded with 16
//                  bits. 1 means pressed, and 0 means unpressed. Bit 0 is
//                  button B, Bit 1 is button Y, etc. up to Bit 11, which is
//                  button R. Bits 12-15 are always 0.
//
//  Description:    This function samples the button presses on the SNES
//                  controller, and returns an encoding of these in a 16-bit
//                  integer. We assume that the CLOCK output is already high,
//                  and set the LATCH output to high for 12 microseconds. This
//                  causes the controller to latch the values of the button
//                  presses into its internal register. We then clock this data
//                  to the CPU over the DATA line in a serial fashion, by
//                  pulsing the CLOCK line low 16 times. We read the data on
//                  the falling edge of the clock. The rising edge of the clock
//                  causes the controller to output the next bit of serial data
//                  to be place on the DATA line. The clock cycle is 12
//                  microseconds long, so the clock is low for 6 microseconds,
//                  and then high for 6 microseconds. 
//
////////////////////////////////////////////////////////////////////////////////

unsigned short get_SNES()
{
    int i;
    unsigned short data = 0;
    unsigned int value;
	
	
    // Set LATCH to high for 12 microseconds. This causes the controller to
    // latch the values of button presses into its internal register. The
    // first serial bit also becomes available on the DATA line.
    set_GPIO9();
    microsecond_delay(12);
    clear_GPIO9();
	
    // Output 16 clock pulses, and read 16 bits of serial data
    for (i = 0; i < 16; i++) {
	// Delay 6 microseconds (half a cycle)
	microsecond_delay(6);
		
	// Clear the CLOCK line (creates a falling edge)
	clear_GPIO11();
		
	// Read the value on the input DATA line
	value = get_GPIO10();
		
	// Store the bit read. Note we convert a 0 (which indicates a button
	// press) to a 1 in the returned 16-bit integer. Unpressed buttons
	// will be encoded as a 0.
	if (value == 0) {
	    data |= (0x1 << i);
	}
		
	// Delay 6 microseconds (half a cycle)
	microsecond_delay(6);
		
	// Set the CLOCK to 1 (creates a rising edge). This causes the
	// controller to output the next bit, which we read half a
	// cycle later.
	set_GPIO11();
    }
	
    // Return the encoded data
    return data;
}



////////////////////////////////////////////////////////////////////////////////
//
//  Function:       init_GPIO9_to_output
//
//  Arguments:      none
//
//  Returns:        void
//
//  Description:    This function sets GPIO pin 9 to an output pin without
//                  any pull-up or pull-down resistors.
//
////////////////////////////////////////////////////////////////////////////////

void init_GPIO9_to_output()
{
    register unsigned int r;
    
    
    // Get the current contents of the GPIO Function Select Register 0
    r = *GPFSEL0;

    // Clear bits 27 - 29. This is the field FSEL9, which maps to GPIO pin 9.
    // We clear the bits by ANDing with a 000 bit pattern in the field.
    r &= ~(0x7 << 27);

    // Set the field FSEL9 to 001, which sets pin 9 to an output pin.
    // We do so by ORing the bit pattern 001 into the field.
    r |= (0x1 << 27);

    // Write the modified bit pattern back to the
    // GPIO Function Select Register 0
    *GPFSEL0 = r;

    // Disable the pull-up/pull-down control line for GPIO pin 9. We follow the
    // procedure outlined on page 101 of the BCM2837 ARM Peripherals manual. The
    // internal pull-up and pull-down resistor isn't needed for an output pin.

    // Disable pull-up/pull-down by setting bits 0:1
    // to 00 in the GPIO Pull-Up/Down Register 
    *GPPUD = 0x0;

    // Wait 150 cycles to provide the required set-up time 
    // for the control signal
    r = 150;
    while (r--) {
	asm volatile("nop");
    }

    // Write to the GPIO Pull-Up/Down Clock Register 0, using a 1 on bit 9 to
    // clock in the control signal for GPIO pin 9. Note that all other pins
    // will retain their previous state.
    *GPPUDCLK0 = (0x1 << 9);

    // Wait 150 cycles to provide the required hold time
    // for the control signal
    r = 150;
    while (r--) {
        asm volatile("nop");
    }

    // Clear all bits in the GPIO Pull-Up/Down Clock Register 0
    // in order to remove the clock
    *GPPUDCLK0 = 0;
}



////////////////////////////////////////////////////////////////////////////////
//
//  Function:       set_GPIO9
//
//  Arguments:      none
//
//  Returns:        void
//
//  Description:    This function sets the GPIO output pin 9
//                  to a 1 (high) level.
//
////////////////////////////////////////////////////////////////////////////////

void set_GPIO9()
{
    register unsigned int r;
	  
    // Put a 1 into the SET9 field of the GPIO Pin Output Set Register 0
    r = (0x1 << 9);
    *GPSET0 = r;
}



////////////////////////////////////////////////////////////////////////////////
//
//  Function:       clear_GPIO9
//
//  Arguments:      none
//
//  Returns:        void
//
//  Description:    This function clears the GPIO output pin 9
//                  to a 0 (low) level.
//
////////////////////////////////////////////////////////////////////////////////

void clear_GPIO9()
{
    register unsigned int r;
	  
    // Put a 1 into the CLR9 field of the GPIO Pin Output Clear Register 0
    r = (0x1 << 9);
    *GPCLR0 = r;
}



////////////////////////////////////////////////////////////////////////////////
//
//  Function:       init_GPIO11_to_output
//
//  Arguments:      none
//
//  Returns:        void
//
//  Description:    This function sets GPIO pin 11 to an output pin without
//                  any pull-up or pull-down resistors.
//
////////////////////////////////////////////////////////////////////////////////

void init_GPIO11_to_output()
{
    register unsigned int r;
    
    
    // Get the current contents of the GPIO Function Select Register 1
    r = *GPFSEL1;

    // Clear bits 3 - 5. This is the field FSEL11, which maps to GPIO pin 11.
    // We clear the bits by ANDing with a 000 bit pattern in the field.
    r &= ~(0x7 << 3);

    // Set the field FSEL11 to 001, which sets pin 9 to an output pin.
    // We do so by ORing the bit pattern 001 into the field.
    r |= (0x1 << 3);

    // Write the modified bit pattern back to the
    // GPIO Function Select Register 1
    *GPFSEL1 = r;

    // Disable the pull-up/pull-down control line for GPIO pin 11. We follow the
    // procedure outlined on page 101 of the BCM2837 ARM Peripherals manual. The
    // internal pull-up and pull-down resistor isn't needed for an output pin.

    // Disable pull-up/pull-down by setting bits 0:1
    // to 00 in the GPIO Pull-Up/Down Register 
    *GPPUD = 0x0;

    // Wait 150 cycles to provide the required set-up time 
    // for the control signal
    r = 150;
    while (r--) {
	asm volatile("nop");
    }

    // Write to the GPIO Pull-Up/Down Clock Register 0, using a 1 on bit 11 to
    // clock in the control signal for GPIO pin 11. Note that all other pins
    // will retain their previous state.
    *GPPUDCLK0 = (0x1 << 11);

    // Wait 150 cycles to provide the required hold time
    // for the control signal
    r = 150;
    while (r--) {
        asm volatile("nop");
    }

    // Clear all bits in the GPIO Pull-Up/Down Clock Register 0
    // in order to remove the clock
    *GPPUDCLK0 = 0;
}



////////////////////////////////////////////////////////////////////////////////
//
//  Function:       set_GPIO11
//
//  Arguments:      none
//
//  Returns:        void
//
//  Description:    This function sets the GPIO output pin 11
//                  to a 1 (high) level.
//
////////////////////////////////////////////////////////////////////////////////

void set_GPIO11()
{
    register unsigned int r;
	  
    // Put a 1 into the SET11 field of the GPIO Pin Output Set Register 0
    r = (0x1 << 11);
    *GPSET0 = r;
}



////////////////////////////////////////////////////////////////////////////////
//
//  Function:       clear_GPIO11
//
//  Arguments:      none
//
//  Returns:        void
//
//  Description:    This function clears the GPIO output pin 11
//                  to a 0 (low) level.
//
////////////////////////////////////////////////////////////////////////////////

void clear_GPIO11()
{
    register unsigned int r;
	  
    // Put a 1 into the CLR11 field of the GPIO Pin Output Clear Register 0
    r = (0x1 << 11);
    *GPCLR0 = r;
}



////////////////////////////////////////////////////////////////////////////////
//
//  Function:       init_GPIO10_to_input
//
//  Arguments:      none
//
//  Returns:        void
//
//  Description:    This function sets GPIO pin 10 to an input pin without
//                  any internal pull-up or pull-down resistors. Note that
//                  a pull-down (or pull-up) resistor must be used externally
//                  on the bread board circuit connected to the pin. Be sure
//                  that the pin high level is 3.3V (definitely NOT 5V).
//
////////////////////////////////////////////////////////////////////////////////

void init_GPIO10_to_input()
{
    register unsigned int r;
    
    
    // Get the current contents of the GPIO Function Select Register 1
    r = *GPFSEL1;

    // Clear bits 0 - 2. This is the field FSEL10, which maps to GPIO pin 10.
    // We clear the bits by ANDing with a 000 bit pattern in the field. This
    // sets the pin to be an input pin.
    r &= ~(0x7 << 0);

    // Write the modified bit pattern back to the
    // GPIO Function Select Register 1
    *GPFSEL1 = r;

    // Disable the pull-up/pull-down control line for GPIO pin 10. We follow the
    // procedure outlined on page 101 of the BCM2837 ARM Peripherals manual. We
    // will pull down the pin using an external resistor connected to ground.

    // Disable internal pull-up/pull-down by setting bits 0:1
    // to 00 in the GPIO Pull-Up/Down Register 
    *GPPUD = 0x0;

    // Wait 150 cycles to provide the required set-up time 
    // for the control signal
    r = 150;
    while (r--) {
        asm volatile("nop");
    }

    // Write to the GPIO Pull-Up/Down Clock Register 0, using a 1 on bit 10 to
    // clock in the control signal for GPIO pin 10. Note that all other pins
    // will retain their previous state.
    *GPPUDCLK0 = (0x1 << 10);

    // Wait 150 cycles to provide the required hold time
    // for the control signal
    r = 150;
    while (r--) {
        asm volatile("nop");
    }

    // Clear all bits in the GPIO Pull-Up/Down Clock Register 0
    // in order to remove the clock
    *GPPUDCLK0 = 0;
}



////////////////////////////////////////////////////////////////////////////////
//
//  Function:       get_GPIO10
//
//  Arguments:      none
//
//  Returns:        1 if the pin level is high, and 0 if the pin level is low.
//
//  Description:    This function gets the current value of pin 10.
//
////////////////////////////////////////////////////////////////////////////////

unsigned int get_GPIO10()
{
    register unsigned int r;
	  
	  
    // Get the current contents of the GPIO Pin Level Register 0
    r = *GPLEV0;
	  
    // Isolate pin 10, and return its value (a 0 if low, or a 1 if high)
    return ((r >> 10) & 0x1);
}

