#include<LPC21XX.H>
#include "delay.h"
#include "eeprom.h"
#include "rtc.h"

#define ir1 14
#define ir2 15
#define led1 17

#define LCD_D (0X0f)<<20
#define RS 1<<17
#define RW 1<<18
#define E 1<<19

void LCD_INIT(void);
void lcd_command(unsigned char);
void lcd_data(unsigned char);
void lcd_rotate(char s[50]);
void lcd_str(char s[50],int);
void lcd_integer(int);
void visitor_counter(void);

char tot[]="TOTAL=0";
char v[]="ENTRY  ";
char u[]="EXIT   ";
char in[]="E:";
char out[]="O:";
char current[]="C:";
int main()
{
   char a[50]="BIDIRECTIONAL VISITOR COUNTER                  ";
   char id[]="ID:V24CE2L1";
//unsigned char c;

   LCD_INIT();

   lcd_str(id,0Xc2);
   lcd_rotate(a);

   lcd_command(0X01);
   init_i2c();
			 /* i2c_eeprom_write(0X50,0X00,'A');
			  c=i2c_eeprom_read(0X50,0X00);
			  delay_milliseconds(100);
			  lcd_command(0X80);
			  lcd_data(c);
			  delay_milliseconds(1000);	*/
			
   visitor_counter();
}
void LCD_INIT()
{
  IODIR1|=LCD_D|RS|RW|E;
  lcd_command(0X01);
  lcd_command(0X02);
  lcd_command(0X0C);
  lcd_command(0X28);
  lcd_command(0X80);
}

void lcd_command(unsigned char cmd)
{
  IOCLR1=LCD_D;
  IOSET1=(cmd&0Xf0)<<16;
  IOCLR1=RS;
  IOCLR1=RW;
  IOSET1=E;
  delay_ms(2);
  IOCLR1=E;
	 
  IOCLR1=LCD_D;
  IOSET1=(cmd&0X0f)<<20;
  IOCLR1=RS;
  IOCLR1=RW;
  IOSET1=E;
  delay_ms(2);
  IOCLR1=E;
}

void lcd_data(unsigned char d)
{
  IOCLR1=LCD_D;
  IOSET1=(d&0Xf0)<<16;
  IOSET1=RS;
  IOCLR1=RW;
  IOSET1=E;
  delay_ms(2);
  IOCLR1=E;
	  
  IOCLR1=LCD_D;
  IOSET1=(d&0X0f)<<20;
  IOSET1=RS;
  IOCLR1=RW;
  IOSET1=E;
  delay_ms(2);
  IOCLR1=E;
}

void lcd_rotate(char s[50])
{
  int i=0,len=0,j,k=0;
  while(s[len]!='\0')
  len++;
  while(k<30)
  {
    delay_ms(100);
    for(j=0;j<16;j++)
    {
      lcd_command(0X80+j);
      lcd_data(s[(i+j)%len]);
    }
    i=(i+1)%len;
    delay_ms(100);
    k++;
 }
}

void lcd_str(char s[50],int j)
{
  int i=0;
  for(i=0;s[i]!=0;i++)
  {
   lcd_command(j);

   lcd_data(s[i]);
   j++;
  }
}

void visitor_counter(void)
{
  unsigned char min,sec,hr;
  int a,incnt=0,outcnt=0,total=0;
  IODIR0|=1<<led1;
  IOSET0=1<<led1;

   i2c_rtc_write(0X68,0X02,0X12);
   i2c_rtc_write(0X68,0X01,0X00);
   i2c_rtc_write(0X68,0X00,0X00);

   while(1)
   {
    hr=i2c_rtc_read(0X68,0X02);
		 lcd_command(0x88);
		 lcd_data((hr/16)+48);
		 lcd_data((hr%16)+48);
		 lcd_data(':');
		 //delay_milliseconds(200);

	     min=i2c_rtc_read(0X68,0X01);
		 lcd_command(0x8b);
		 lcd_data((min/16)+48);
		 lcd_data((min%16)+48);
		 lcd_data(':');
		 //delay_milliseconds(200);

	     sec=i2c_rtc_read(0X68,0X00);
		 lcd_command(0x8e);
		 lcd_data((sec/16)+48);
		 lcd_data((sec%16)+48);
		 delay_ms(200);

		 if(min==0X01)
		 {
		   
		   i2c_eeprom_write(0X50,0X00,0X00);
		   
		   incnt=0;
		   outcnt=0;

		   i2c_rtc_write(0X68,0X02,0X00);
		   i2c_rtc_write(0X68,0X01,0X00);
		   i2c_rtc_write(0X68,0X00,0X00);
		} 
   		  
       if(incnt==0)
       {
         a=i2c_eeprom_read(0X50,0X00);
       }

       if(((IOPIN0>>ir1)&1)==0)
       {
         incnt++;

//	delay_milliseconds(200);
//	lcd_command(0X01);

         lcd_str(v,0X80);
   
         lcd_str(out,0Xc6);
         lcd_str(current,0Xca);
	     lcd_command(0Xcc);

 //     lcd_data((incnt/10)+48);
//	  lcd_data((incnt%10)+48);

         lcd_integer(incnt);
	     while(((IOPIN0>>ir1)&1)==0);
       }
       if(((IOPIN0>>ir2)&1)==0)
       {
         if(outcnt<incnt)
         {
   	        outcnt++;

    	//delay_milliseconds(200);   
//	lcd_data((outcnt/10)+48);
//	lcd_data((outcnt%10)+48); 
 
            lcd_str(u,0X80);
		 
			while(((IOPIN0>>ir1)&1)==0);
         }
       }
        total=incnt-outcnt;
        if(total==0)
        {     
           IOSET0=1<<led1;
           lcd_command(0X01);
           lcd_str(tot,0X80);
           delay_ms(200); 
        }
        else
        {
           IOCLR0=1<<led1;
          // delay_ms(100);
	       lcd_command(0Xc8);
           lcd_integer(outcnt);
	       lcd_str(in,0Xc0);
		   lcd_command(0Xc2);
		   lcd_integer(a+incnt);


	       i2c_eeprom_write(0X50,0X00,a+incnt);
        }
   }
}

void lcd_integer(int a)
{
			unsigned char b[10];
			int i=0;
			if(a==0){
			  lcd_data('0');}
			if(a<0){
		    	a=-a;}
			while(a!=0)
			{
		    	b[i++]=a%10;
			    a=a/10;
			}
			for(--i;i>=0;i--){
			  lcd_data(b[i]+48);}
}
