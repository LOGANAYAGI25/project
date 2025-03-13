#include<lpc21xx.h>

#define SCL_EN 0X00000010
#define SDA_EN 0X00000040
#define CCLK 60000000
#define PCLK CCLK/4
#define I2C_SPEED 100000
#define LOADVAL ((PCLK/I2C_SPEED)/2)

#define AA_BIT 2
#define SI_BIT 3
#define STO_BIT 4
#define STA_BIT 5
#define I2EN_BIT 6

typedef unsigned char u8;

void i2c_eeprom_write(u8,u8,u8);
u8   i2c_eeprom_read(u8,u8);


void init_i2c(void);
void i2c_start(void);
void i2c_stop(void);
void i2c_restart(void);
void i2c_write(u8);

u8 i2c_ack(void);
u8 i2c_nack(void);
u8 i2c_masterack(void);

void i2c_eeprom_write(u8 slaveAddr,u8 wBuffAddr,u8 dat)
{
  i2c_start();	
  i2c_write(slaveAddr<<1); 
	i2c_write(wBuffAddr);   
	i2c_write(dat);  
	i2c_stop();
	delay_ms(10);
}			 

u8 i2c_eeprom_read(u8 slaveAddr,u8 rBuffAddr)
{
	u8 dat;
	i2c_start();	
  i2c_write(slaveAddr<<1);
	i2c_write(rBuffAddr);    
	i2c_restart();	
	i2c_write(slaveAddr<<1|1); 
  dat=i2c_nack();	
	i2c_stop();
	return dat;
}

void init_i2c(void)
{
	  PINSEL0=(1<<4)|(1<<6);
	 I2SCLL=75;
	 I2SCLH=75;
	 I2CONSET=1<<I2EN_BIT; 
}	

void i2c_start(void)
{
  I2CONSET=1<<STA_BIT;
	while(((I2CONSET>>SI_BIT)&1)==0);
	I2CONCLR=1<<STA_BIT;
}	

void i2c_restart(void)
{
	I2CONSET=1<<STA_BIT;
	I2CONCLR=1<<SI_BIT;
	while(((I2CONSET>>SI_BIT)&1)==0);
	I2CONCLR=1<<STA_BIT;
}	

void i2c_write(u8 dat)
{
	I2DAT=dat;	
	I2CONCLR = 1<<SI_BIT;
	while(((I2CONSET>>SI_BIT)&1)==0);
	
}	

void i2c_stop(void)
{	
	I2CONSET=1<<STO_BIT;
	I2CONCLR = 1<<SI_BIT;
	
}

u8 i2c_nack(void)
{	
	I2CONSET = 0x00; 
  I2CONCLR = 1<<SI_BIT;
	while(((I2CONSET>>SI_BIT)&1)==0);
	return I2DAT;
}

u8 i2c_masterack(void)
{	
	I2CONSET = 1<<2; 
  I2CONCLR = 1<<SI_BIT;
	while(((I2CONSET>>SI_BIT)&1)==0);
	I2CONCLR = 1<<2; 
 	return I2DAT;
}	
