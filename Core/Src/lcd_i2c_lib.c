#include "lcd_i2c_lib.h"
/*-----------------------------------Налаштування----------------------------------*/

#define Adress 0x27 << 1
extern I2C_HandleTypeDef hi2c1;
bool backlight = true;
char lcd_tx_buffer[40] = { 0, };
uint8_t global_buffer = 0;

/*-----------------------------------Налаштування----------------------------------*/

/*============================Допоміжні функції============================*/
/*-------------Функція для відправки данних при ініціалізації дисплея-------------*/
static void lcd_Send_init_Data(uint8_t *init_Data) {
	if (backlight) {
		*init_Data |= 0x08;
	} else {
		*init_Data &= ~0x08;
	}
	*init_Data |= 0x04;
	HAL_I2C_Master_Transmit(&hi2c1, Adress, init_Data, 1, 10);
	HAL_Delay(5);
	*init_Data &= ~0x04;
	HAL_I2C_Master_Transmit(&hi2c1, Adress, init_Data, 1, 10);
	HAL_Delay(5);
}
/*-------------Функція для відправки данних при ініціалізації дисплея-------------*/

/*--------------------Функція відправки байта інформації на дисплей---------------*/
static void lcd_Write_byte(uint8_t Data) {
	HAL_I2C_Master_Transmit(&hi2c1, Adress, &Data, 1, 10);
}
/*--------------------Функція відправки байта інформації на дисплей---------------*/

/*----------------------Функція відправки пів байта інформації--------------------*/
static void lcd_Send_cmd(uint8_t Data) {
	Data <<= 4;
	lcd_Write_byte(global_buffer |= 0x04);
	lcd_Write_byte(global_buffer | Data);
	lcd_Write_byte(global_buffer &= ~0x04);
}
/*----------------------Функція відправки пів байта інформації--------------------*/

/*----------------------Функція відправки байта данних----------------------------*/
static void lcd_Send_data_symbol(uint8_t Data, uint8_t mode) {
	if (mode == 0) {
		lcd_Write_byte(global_buffer &= ~0x01); // RS = 0
	} else {
		lcd_Write_byte(global_buffer |= 0x01); // RS = 1
	}
	uint8_t MSB_Data = 0;
	MSB_Data = Data >> 4;
	lcd_Send_cmd(MSB_Data);
	lcd_Send_cmd(Data);
}
/*----------------------Функція відправки байта данних----------------------------*/

/*----------------------Основная функція для відправки данних---------------------*/
static void lcd_Send_data(uint8_t *Data) {

	if (backlight) {
		*Data |= 0x08;
	} else {
		*Data &= ~0x08;
	}
	*Data |= 0x04;
	HAL_I2C_Master_Transmit(&hi2c1, Adress, Data, 1, 10);
	*Data &= ~0x04;
	HAL_I2C_Master_Transmit(&hi2c1, Adress, Data, 1, 10);
}

/*----------------------Основная функція для відправки данних---------------------*/
/*============================Допоміжні функції============================*/

/*-------------------------Функція ініціалізації дисплея-------------------------*/
/// Функція ініціалізації дисплея
void lcd_Init(void) {
	/*========Power on========*/
	uint8_t tx_buffer = 0x30;
	/*========Wait for more than 15 ms after Vcc rises to 4.5V========*/
	HAL_Delay(15);
	/*========BF can not be checked before this instruction.========*/
	/*========Function set ( Interface is 8 bits long.========*/
	lcd_Send_init_Data(&tx_buffer);
	/*========Wait for more 4.1 ms========*/
	HAL_Delay(5);
	/*========BF can not be checked before this instruction.========*/
	/*========Function set ( Interface is 8 bits long.========*/
	lcd_Send_init_Data(&tx_buffer);
	/*========Wait for more 100 microsec========*/
	HAL_Delay(1);
	/*========BF can not be checked before this instruction.========*/
	/*========Function set ( Interface is 8 bits long.========*/
	lcd_Send_init_Data(&tx_buffer);

	/*========Вмикаємо 4х-бітний інтерфейс========*/
	tx_buffer = 0x20;
	lcd_Send_init_Data(&tx_buffer);
	/*========Вмикаємо 4х-бітний інтерфейс========*/

	/*======2 строки, шрифт 5х8======*/
	tx_buffer = 0x20;
	lcd_Send_init_Data(&tx_buffer);
	tx_buffer = 0x80;
	lcd_Send_init_Data(&tx_buffer);
	/*======2 строки, шрифт 5х8======*/

	/*========Вимкнути дисплей========*/
	tx_buffer = 0x00;
	lcd_Send_init_Data(&tx_buffer);
	tx_buffer = 0x80;
	lcd_Send_init_Data(&tx_buffer);
	/*========Вимкнути дисплей========*/

	/*========Очистить дисплей========*/
	tx_buffer = 0x00;
	lcd_Send_init_Data(&tx_buffer);
	tx_buffer = 0x10;
	lcd_Send_init_Data(&tx_buffer);
	/*========Очистить дисплей========*/

	/*========Режим зсува курсора========*/
	tx_buffer = 0x00;
	lcd_Send_init_Data(&tx_buffer);
	tx_buffer = 0x30;
	lcd_Send_init_Data(&tx_buffer);
	/*========Режим зсува курсора========*/

	/*========ініціалізації завершена. Включить дисплей========*/
	tx_buffer = 0x00;
	lcd_Send_init_Data(&tx_buffer);
	tx_buffer = 0xC0;
	lcd_Send_init_Data(&tx_buffer);
	/*========ініціалізації завершена. Включить дисплей========*/
}

/*-------------------------Функція ініціалізації дисплея-------------------------*/

/*-------------------------Функція виводу символа на дисплей---------------------*/
void lcd_Print_symbol(uint8_t symbol) {
	uint8_t command;
	command = ((symbol & 0xf0) | 0x09);
	lcd_Send_data(&command);
	command = ((symbol & 0x0f) << 4) | 0x09;
	lcd_Send_data(&command);
}
/*-------------------------Функція виводу символа на дисплей---------------------*/

/*-------------------------Функція виводу тексту на дисплей----------------------*/
void lcd_Print_text(char *message) {
	for (int i = 0; i < strlen(message); i++) {
		lcd_Print_symbol(message[i]);
	}
}
/*-------------------------Функція виводу тексту на дисплей----------------------*/

/*-------------------Функція встановки курсора для вивода тексту----------------*/
void lcd_SetCursor(uint8_t x, uint8_t y) {
	uint8_t command, adr;
	if (y > 3)
		y = 3;
	if (x > 39)
		x = 39;
	if (y == 0) {
		adr = x;
	}
	if (y == 1) {
		adr = x + 0x40;
	}
	if (y == 2) {
		adr = x + 0x14;
	}
	if (y == 3) {
		adr = x + 0x54;
	}
	command = ((adr & 0xf0) | 0x80);
	lcd_Send_data(&command);

	command = (adr << 4);
	lcd_Send_data(&command);

}
/*-------------------Функція встановки курсора для вивода тексту----------------*/

/*---------------------Функція включення/виключення підсвітки--------------------*/
void lcd_Backlight(bool state) {
	if (state) {
		backlight = true;
	} else {
		backlight = false;
	}
}
/*---------------------Функція включення/виключення підсвітки--------------------*/

/*-------------------------Функція очищення дисплея-------------------------------*/

void lcd_Clean(void) {
	uint8_t tx_buffer = 0x00;
	lcd_Send_init_Data(&tx_buffer);
	tx_buffer = 0x10;
	lcd_Send_init_Data(&tx_buffer);

}
/*-------------------------Функція очищення дисплея-------------------------------*/

