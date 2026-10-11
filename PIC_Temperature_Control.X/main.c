#include <xc.h>

#define _XTAL_FREQ 10000000UL     // 10 MHz crystal


// CONFIGURATION BITS

#pragma config FOSC = HS
#pragma config WDTE = OFF
#pragma config PWRTE = ON
#pragma config BOREN = ON
#pragma config LVP = OFF
#pragma config CPD = OFF
#pragma config WRT = OFF
#pragma config CP = OFF


// LCD CONNECTIONS

// RS -> RD0 -> Physical pin 19
// E  -> RD1 -> Physical pin 20
// D4 -> RD4 -> Physical pin 27
// D5 -> RD5 -> Physical pin 28
// D6 -> RD6 -> Physical pin 29
// D7 -> RD7 -> Physical pin 30


#define LCD_RS PORTDbits.RD0
#define LCD_EN PORTDbits.RD1

#define LCD_D4 PORTDbits.RD4
#define LCD_D5 PORTDbits.RD5
#define LCD_D6 PORTDbits.RD6
#define LCD_D7 PORTDbits.RD7



// DHT11 CONNECTION

// DATA -> RB0 -> Physical pin 33

#define DHT_DATA PORTBbits.RB0
#define DHT_TRIS TRISBbits.TRISB0


// FAN CONTROL

// FAN_CTRL -> RC2 -> Physical pin 17

#define FAN PORTCbits.RC2


// LCD FUNCTIONS

void LCD_Pulse(void)
{
    LCD_EN = 1;
    __delay_us(2);

    LCD_EN = 0;
    __delay_us(2);
}


void LCD_SendNibble(unsigned char data)
{
    LCD_D4 = (data >> 0) & 1;
    LCD_D5 = (data >> 1) & 1;
    LCD_D6 = (data >> 2) & 1;
    LCD_D7 = (data >> 3) & 1;

    LCD_Pulse();
}


void LCD_Command(unsigned char cmd)
{
    LCD_RS = 0;

    LCD_SendNibble(cmd >> 4);
    LCD_SendNibble(cmd & 0x0F);

    __delay_ms(2);
}


void LCD_Char(unsigned char data)
{
    LCD_RS = 1;

    LCD_SendNibble(data >> 4);
    LCD_SendNibble(data & 0x0F);

    __delay_us(50);
}


void LCD_String(const char *str)
{
    while (*str)
    {
        LCD_Char(*str++);
    }
}


void LCD_SetCursor(unsigned char row, unsigned char column)
{
    if (row == 1)
    {
        LCD_Command(0x80 + column - 1);
    }
    else
    {
        LCD_Command(0xC0 + column - 1);
    }
}


void LCD_Clear(void)
{
    LCD_Command(0x01);
    __delay_ms(2);
}


void LCD_Init(void)
{
    // LCD pins as outputs
    TRISDbits.TRISD0 = 0;
    TRISDbits.TRISD1 = 0;

    TRISDbits.TRISD4 = 0;
    TRISDbits.TRISD5 = 0;
    TRISDbits.TRISD6 = 0;
    TRISDbits.TRISD7 = 0;

    LCD_RS = 0;
    LCD_EN = 0;

    __delay_ms(30);


    // Initialize LCD into 4-bit mode
    LCD_SendNibble(0x03);
    __delay_ms(5);

    LCD_SendNibble(0x03);
    __delay_us(150);

    LCD_SendNibble(0x03);
    __delay_us(150);

    LCD_SendNibble(0x02);


    // 4-bit, 2-line mode
    LCD_Command(0x28);

    // Display ON, cursor OFF
    LCD_Command(0x0C);

    // Move cursor right
    LCD_Command(0x06);

    // Clear display
    LCD_Command(0x01);

    __delay_ms(2);
}



// DHT11 WAIT FUNCTION


unsigned char DHT_Wait(unsigned char state,
                       unsigned int timeout)
{
    while (DHT_DATA != state)
    {
        if (timeout == 0)
        {
            return 0;
        }

        timeout--;

        __delay_us(1);
    }

    return 1;
}


// READ ONE BYTE FROM DHT11

unsigned char DHT11_ReadByte(unsigned char *result)
{
    unsigned char i;
    unsigned char data = 0;

    for (i = 0; i < 8; i++)
    {
        // Wait for beginning of HIGH pulse
        if (!DHT_Wait(1, 150))
        {
            return 0;
        }

        __delay_us(40);

        // Shift previous bits left
        data <<= 1;

        // If still HIGH after 40 us -> logic 1
        if (DHT_DATA)
        {
            data |= 1;
        }

        // Wait for line to go LOW
        if (!DHT_Wait(0, 150))
        {
            return 0;
        }
    }


    *result = data;

    return 1;
}


// READ DHT11

// DHT11 sends:

// Byte 1 -> Humidity integer
// Byte 2 -> Humidity decimal
// Byte 3 -> Temperature integer
// Byte 4 -> Temperature decimal
// Byte 5 -> Checksum


unsigned char DHT11_Read(unsigned char *humidityInt,
                         unsigned char *humidityDec,
                         unsigned char *temperatureInt,
                         unsigned char *temperatureDec)
{
    unsigned char checksum;


    // SEND START SIGNAL

    // DATA pin becomes output
    DHT_TRIS = 0;

    // Pull DATA LOW for at least 18 ms
    DHT_DATA = 0;

    __delay_ms(20);

    // Pull HIGH briefly
    DHT_DATA = 1;

    __delay_us(30);

    // Release line -> input
    DHT_TRIS = 1;

    // WAIT FOR DHT11 RESPONSE

    // DHT11 pulls line LOW
    if (!DHT_Wait(0, 200))
    {
        return 0;
    }


    // Then DHT11 pulls line HIGH
    if (!DHT_Wait(1, 200))
    {
        return 0;
    }

    // Then LOW before first data bit
    if (!DHT_Wait(0, 200))
    {
        return 0;
    }

    // READ 40 BITS

    if (!DHT11_ReadByte(humidityInt))
    {
        return 0;
    }


    if (!DHT11_ReadByte(humidityDec))
    {
        return 0;
    }


    if (!DHT11_ReadByte(temperatureInt))
    {
        return 0;
    }


    if (!DHT11_ReadByte(temperatureDec))
    {
        return 0;
    }


    if (!DHT11_ReadByte(&checksum))
    {
        return 0;
    }


    // CHECK CHECKSUM

    if ((unsigned char)(
            *humidityInt +
            *humidityDec +
            *temperatureInt +
            *temperatureDec) != checksum)
    {
        return 0;
    }


    return 1;
}


// PRINT UNSIGNED INTEGER ON LCD

void LCD_PrintUInt(unsigned int number)
{
    char buffer[6];

    unsigned char i = 0;


    if (number == 0)
    {
        LCD_Char('0');

        return;
    }


    while (number > 0)
    {
        buffer[i] = (number % 10) + '0';

        number = number / 10;

        i++;
    }


    while (i > 0)
    {
        i--;

        LCD_Char(buffer[i]);
    }
}


// MAIN PROGRAM

void main(void)
{
    unsigned char humidityInt;
    unsigned char humidityDec;

    unsigned char temperatureInt;
    unsigned char temperatureDec;

    // INITIALIZE DHT11

    DHT_TRIS = 1;

    // INITIALIZE FAN CONTROL
    // RC2 / physical pin 17

    TRISCbits.TRISC2 = 0;

    // Fan OFF initially
    FAN = 0;

    // INITIALIZE LCD

    LCD_Init();

    // Startup message
    LCD_SetCursor(1, 1);

    LCD_String("DHT11 Monitor");


    LCD_SetCursor(2, 1);

    LCD_String("Starting...");


    __delay_ms(2000);


    LCD_Clear();

    // MAIN LOOP

    while (1)
    {
        // Read DHT11
        if (DHT11_Read(&humidityInt,
                       &humidityDec,
                       &temperatureInt,
                       &temperatureDec))
        {


            // FAN CONTROL
            //
            // Temperature > 35 C -> Fan ON
            // Temperature <=35 C -> Fan OFF


            if (temperatureInt > 35)
            {
                FAN = 1;
            }
            else
            {
                FAN = 0;
            }

            // LCD ROW 1
            // TEMPERATURE

            LCD_SetCursor(1, 1);

            LCD_String("Temp: ");

            LCD_PrintUInt(temperatureInt);

            LCD_Char('.');

            LCD_PrintUInt(temperatureDec);

            LCD_Char(0xDF);      

            LCD_Char('C');

            // Clear remaining characters
            LCD_String("   ");

            // LCD ROW 2
            // HUMIDITY

            LCD_SetCursor(2, 1);

            LCD_String("Hum : ");

            LCD_PrintUInt(humidityInt);

            LCD_Char('.');

            LCD_PrintUInt(humidityDec);

            LCD_Char('%');

            // Clear remaining characters
            LCD_String("    ");
        }

        else
        {

            // DHT11 READ ERROR

            FAN = 0;


            LCD_SetCursor(1, 1);

            LCD_String("DHT11 ERROR     ");


            LCD_SetCursor(2, 1);

            LCD_String("Check sensor    ");
        }


        // DHT11 should not be read continuously
        __delay_ms(2000);
    }
}