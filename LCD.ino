#include <Wire.h> // Library for I2C communication

#include <LiquidCrystal_I2C.h> // Library for LCD
LiquidCrystal_I2C lcd(0x27, 20, 4);

void lcdSetup()
{
  lcd.init();
  lcd.backlight();
  Wire.begin(); // Start the I2C
  
  lcd.setCursor(0, 0);
  lcd.print("    Bienvenido a  ");
  lcd.setCursor(0, 1);
  lcd.print("      Soatech     ");
  delay(3000);
  lcd.clear();
}
