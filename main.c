#include <avr/io.h>
#include <util/delay.h>
#include <stdio.h>
#include <stdint.h>

#define PIN_SCE   PD7
#define PIN_RESET PD6
#define PIN_DC    PD5
#define PIN_SDIN  PD4
#define PIN_SCLK  PD3

#define LCD_C     0x00
#define LCD_D     0x01

#define LCD_X     84
#define LCD_Y     48

#define PORT_PCD8544 PORTD
#define DDR_PCD8544  DDRD

static const uint8_t ASCII[][5] =
{
	{0x00, 0x00, 0x00, 0x00, 0x00} // 20
	,{0x00, 0x00, 0x5f, 0x00, 0x00} // 21 !
	,{0x00, 0x07, 0x00, 0x07, 0x00} // 22 "
	,{0x14, 0x7f, 0x14, 0x7f, 0x14} // 23 #
	,{0x24, 0x2a, 0x7f, 0x2a, 0x12} // 24 $
	,{0x23, 0x13, 0x08, 0x64, 0x62} // 25 %
	,{0x36, 0x49, 0x55, 0x22, 0x50} // 26 &
	,{0x00, 0x05, 0x03, 0x00, 0x00} // 27 '
	,{0x00, 0x1c, 0x22, 0x41, 0x00} // 28 (
	,{0x00, 0x41, 0x22, 0x1c, 0x00} // 29 )
	,{0x14, 0x08, 0x3e, 0x08, 0x14} // 2a *
	,{0x08, 0x08, 0x3e, 0x08, 0x08} // 2b +
	,{0x00, 0x50, 0x30, 0x00, 0x00} // 2c ,
	,{0x08, 0x08, 0x08, 0x08, 0x08} // 2d -
	,{0x00, 0x60, 0x60, 0x00, 0x00} // 2e .
	,{0x20, 0x10, 0x08, 0x04, 0x02} // 2f /
	,{0x3e, 0x51, 0x49, 0x45, 0x3e} // 30 0
	,{0x00, 0x42, 0x7f, 0x40, 0x00} // 31 1
	,{0x42, 0x61, 0x51, 0x49, 0x46} // 32 2
	,{0x21, 0x41, 0x45, 0x4b, 0x31} // 33 3
	,{0x18, 0x14, 0x12, 0x7f, 0x10} // 34 4
	,{0x27, 0x45, 0x45, 0x45, 0x39} // 35 5
	,{0x3c, 0x4a, 0x49, 0x49, 0x30} // 36 6
	,{0x01, 0x71, 0x09, 0x05, 0x03} // 37 7
	,{0x36, 0x49, 0x49, 0x49, 0x36} // 38 8
	,{0x06, 0x49, 0x49, 0x29, 0x1e} // 39 9
	,{0x00, 0x36, 0x36, 0x00, 0x00} // 3a :
	,{0x00, 0x56, 0x36, 0x00, 0x00} // 3b ;
	,{0x08, 0x14, 0x22, 0x41, 0x00} // 3c <
	,{0x14, 0x14, 0x14, 0x14, 0x14} // 3d =
	,{0x00, 0x41, 0x22, 0x14, 0x08} // 3e >
	,{0x02, 0x01, 0x51, 0x09, 0x06} // 3f ?
	,{0x32, 0x49, 0x79, 0x41, 0x3e} // 40 @
	,{0x7e, 0x11, 0x11, 0x11, 0x7e} // 41 A
	,{0x7f, 0x49, 0x49, 0x49, 0x36} // 42 B
	,{0x3e, 0x41, 0x41, 0x41, 0x22} // 43 C
	,{0x7f, 0x41, 0x41, 0x22, 0x1c} // 44 D
	,{0x7f, 0x49, 0x49, 0x49, 0x41} // 45 E
	,{0x7f, 0x09, 0x09, 0x09, 0x01} // 46 F
	,{0x3e, 0x41, 0x49, 0x49, 0x7a} // 47 G
	,{0x7f, 0x08, 0x08, 0x08, 0x7f} // 48 H
	,{0x00, 0x41, 0x7f, 0x41, 0x00} // 49 I
	,{0x20, 0x40, 0x41, 0x3f, 0x01} // 4a J
	,{0x7f, 0x08, 0x14, 0x22, 0x41} // 4b K
	,{0x7f, 0x40, 0x40, 0x40, 0x40} // 4c L
	,{0x7f, 0x02, 0x0c, 0x02, 0x7f} // 4d M
	,{0x7f, 0x04, 0x08, 0x10, 0x7f} // 4e N
	,{0x3e, 0x41, 0x41, 0x41, 0x3e} // 4f O
	,{0x7f, 0x09, 0x09, 0x09, 0x06} // 50 P
	,{0x3e, 0x41, 0x51, 0x21, 0x5e} // 51 Q
	,{0x7f, 0x09, 0x19, 0x29, 0x46} // 52 R
	,{0x46, 0x49, 0x49, 0x49, 0x31} // 53 S
	,{0x01, 0x01, 0x7f, 0x01, 0x01} // 54 T
	,{0x3f, 0x40, 0x40, 0x40, 0x3f} // 55 U
	,{0x1f, 0x20, 0x40, 0x20, 0x1f} // 56 V
	,{0x3f, 0x40, 0x38, 0x40, 0x3f} // 57 W
	,{0x63, 0x14, 0x08, 0x14, 0x63} // 58 X
	,{0x07, 0x08, 0x70, 0x08, 0x07} // 59 Y
	,{0x61, 0x51, 0x49, 0x45, 0x43} // 5a Z
	,{0x00, 0x7f, 0x41, 0x41, 0x00} // 5b [
	,{0x02, 0x04, 0x08, 0x10, 0x20} // 5c ?
	,{0x00, 0x41, 0x41, 0x7f, 0x00} // 5d ]
	,{0x04, 0x02, 0x01, 0x02, 0x04} // 5e ^
	,{0x40, 0x40, 0x40, 0x40, 0x40} // 5f _
	,{0x00, 0x01, 0x02, 0x04, 0x00} // 60 `
	,{0x20, 0x54, 0x54, 0x54, 0x78} // 61 a
	,{0x7f, 0x48, 0x44, 0x44, 0x38} // 62 b
	,{0x38, 0x44, 0x44, 0x44, 0x20} // 63 c
	,{0x38, 0x44, 0x44, 0x48, 0x7f} // 64 d
	,{0x38, 0x54, 0x54, 0x54, 0x18} // 65 e
	,{0x08, 0x7e, 0x09, 0x01, 0x02} // 66 f
	,{0x0c, 0x52, 0x52, 0x52, 0x3e} // 67 g
	,{0x7f, 0x08, 0x04, 0x04, 0x78} // 68 h
	,{0x00, 0x44, 0x7d, 0x40, 0x00} // 69 i
	,{0x20, 0x40, 0x44, 0x3d, 0x00} // 6a j
	,{0x7f, 0x10, 0x28, 0x44, 0x00} // 6b k
	,{0x00, 0x41, 0x7f, 0x40, 0x00} // 6c l
	,{0x7c, 0x04, 0x18, 0x04, 0x78} // 6d m
	,{0x7c, 0x08, 0x04, 0x04, 0x78} // 6e n
	,{0x38, 0x44, 0x44, 0x44, 0x38} // 6f o
	,{0x7c, 0x14, 0x14, 0x14, 0x08} // 70 p
	,{0x08, 0x14, 0x14, 0x18, 0x7c} // 71 q
	,{0x7c, 0x08, 0x04, 0x04, 0x08} // 72 r
	,{0x48, 0x54, 0x54, 0x54, 0x20} // 73 s
	,{0x04, 0x3f, 0x44, 0x40, 0x20} // 74 t
	,{0x3c, 0x40, 0x40, 0x20, 0x7c} // 75 u
	,{0x1c, 0x20, 0x40, 0x20, 0x1c} // 76 v
	,{0x3c, 0x40, 0x30, 0x40, 0x3c} // 77 w
	,{0x44, 0x28, 0x10, 0x28, 0x44} // 78 x
	,{0x0c, 0x50, 0x50, 0x50, 0x3c} // 79 y
	,{0x44, 0x64, 0x54, 0x4c, 0x44} // 7a z
	,{0x00, 0x08, 0x36, 0x41, 0x00} // 7b {
	,{0x00, 0x00, 0x7f, 0x00, 0x00} // 7c |
	,{0x00, 0x41, 0x36, 0x08, 0x00} // 7d }
	,{0x10, 0x08, 0x08, 0x10, 0x08} // 7e <
	,{0x78, 0x46, 0x41, 0x46, 0x78} // 7f >
};


uint16_t adc_read(uint8_t channel);
void adc_init(void);
void pcd8544_init(void);
void pcd8544_send(uint8_t dc, uint8_t data);
void pcd8544_send_char(uint8_t ch);
void pcd8544_clear(void);
void pcd8544_print_at(char *str, uint8_t x, uint8_t y);
void print_value(double value, uint8_t x, uint8_t y);

int main(void)
{
	pcd8544_init();
	pcd8544_clear();
	adc_init();
	
	double amps;
	double volts;
	double power;
	float Vofs = 0.136f;
	
	for (;;)
	{
		amps = adc_read(0) * 5.0f / 1023.0f - Vofs;
		volts = adc_read(1) * 5.0f / 1023.0f;
		if (amps < 0.0f)
		{
			amps = 0.0f;
		}
		amps = amps * 2.9412f;
		volts = volts * 6.2f - 0.005f * amps;
		power = amps * volts;
		
		//pcd8544_clear();
		
		pcd8544_print_at("U=", 1, 2);
		print_value(volts, 3, 2);
		pcd8544_print_at("V", 10, 2);
		
		if (amps > 10.0f)
		{
			pcd8544_print_at(">10A!", 3, 3);
		}
		else
		{
			pcd8544_print_at("I=", 1, 3);
			print_value(amps, 3, 3);
			pcd8544_print_at("A", 10, 3);
		}
		
		pcd8544_print_at("P=", 1, 4);
		print_value(power, 3, 4);
		pcd8544_print_at("W", 10, 4);
		
		_delay_ms(300);
	}

	return 0;
}

void print_value(double value, uint8_t x, uint8_t y)
{
	int k = 0;
	char buffer[4];
	uint16_t f1 = value;
	uint16_t f2 = value*10-f1*10;
	uint16_t f3 = value*100-f1*100-f2*10;;
	snprintf(buffer, sizeof(buffer), "%u", f1);
	if (value >= 100)
	{
		k = 1;
	}
	if (value >= 10)
	{
		pcd8544_print_at(buffer, x, y);
	}
	else
	{
		pcd8544_print_at("0", x, y);
		pcd8544_print_at(buffer, x+1, y);
	}
	pcd8544_print_at(".", x+k+2, y);
	snprintf(buffer, sizeof(buffer), "%u", f2);
	pcd8544_print_at(buffer, x+k+3, y);
	snprintf(buffer, sizeof(buffer), "%u", f3);
	pcd8544_print_at(buffer, x+k+4, y);
}
void adc_init(void)
{
	ADMUX = (1 << REFS0);
	//ADMUX = (1 << REFS1) | (1 << REFS0);
	ADCSRA = (1 << ADEN) | (1 << ADPS2) | (1 << ADPS1) | (1 << ADPS0);
}

uint16_t adc_read(uint8_t channel)
{
	channel &= 0x07;
	ADMUX = (ADMUX & 0xF8) | channel;
	ADCSRA |= (1 << ADSC);
	while (ADCSRA & (1 << ADSC));
	return ADC;
}

void pcd8544_print_at(char *str, uint8_t x, uint8_t y)
{
	x=x%12; y=y%6;
	pcd8544_send(LCD_C, 0x40+y);
	pcd8544_send(LCD_C, 0x80+x*7);

	while (*str)
	{
		pcd8544_send_char(*str++);
	}
}
void pcd8544_init(void)
{
	// GPIO Setup
	DDR_PCD8544 |= (1<<PIN_SCE) | (1<<PIN_RESET) | (1<<PIN_DC) | (1<<PIN_SDIN) | (1<<PIN_SCLK);
	PORT_PCD8544&=~(1<<PIN_RESET);
	PORT_PCD8544|=(1<<PIN_RESET);
	pcd8544_send(LCD_C, 0x21 );  // LCD Extended Commands.
	pcd8544_send(LCD_C, 0xBA );  // Set LCD Vop (Contrast).
	pcd8544_send(LCD_C, 0x04 );  // Set Temp coefficent. //0x04
	pcd8544_send(LCD_C, 0x14 );  // LCD bias mode 1:48. //0x13
	pcd8544_send(LCD_C, 0x20 );  // LCD Basic Commands
	pcd8544_send(LCD_C, 0x0C );  // LCD in normal mode.
}

void pcd8544_send(uint8_t dc, uint8_t data)
{
	uint8_t i;
	if (dc == LCD_D)
	PORT_PCD8544 |= (1<<PIN_DC);
	else
	PORT_PCD8544 &= ~(1<<PIN_DC);

	PORT_PCD8544&=~(1<<PIN_SCE);
	for (i=0; i<8; i++)
	{
		PORT_PCD8544=(data & 0x80) ? PORT_PCD8544 | (1<<PIN_SDIN) : PORT_PCD8544 & ~(1<<PIN_SDIN);

		data=(data<<1);

		PORT_PCD8544|=(1<<PIN_SCLK);
		PORT_PCD8544&=~(1<<PIN_SCLK);
	}
	PORT_PCD8544|=(1<<PIN_SCE);
}

void pcd8544_send_char(uint8_t ch)
{
	int i;
	if (ch >= 0x20 && ch <= 0x80)
	{
		pcd8544_send(LCD_D, 0x00);
		for (i = 0; i < 5; i++)
		{
			pcd8544_send(LCD_D,  ASCII[ch - 0x20][i]);
		}
		pcd8544_send(LCD_D, 0x00);
	}
}

void pcd8544_clear(void)
{
	int i;
	for (i=0; i < LCD_X * LCD_Y / 8; i++)
	{
		pcd8544_send(LCD_D, 0x00);
	}
}
