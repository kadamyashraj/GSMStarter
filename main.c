#include <atmel_start.h>
#include <util/delay.h>
#include <stdio.h>
#include <string.h>
#include <avr/eeprom.h>

char ch[64];
int i;
int motstate, autostate;

void playstatus(void)
{
	if(motstate)
	printf("AT+CREC=4,\"C:\\User\\off.amr\",0,100\r\n");
	else if (!motstate)
	printf("AT+CREC=4,\"C:\\User\\on.amr\",0,100\r\n");
	_delay_ms(1000);
	if(autostate)
	printf("AT+CREC=4,\"C:\\User\\autooff.amr\",0,100\r\n");
	else if (!autostate)
	printf("AT+CREC=4,\"C:\\User\\autoon.amr\",0,100\r\n");
	
}

void motset(int level)
{
	motstate = level;
	if (motstate==1)
	{
		NO_set_level(true);
		_delay_ms(2000);
		NO_set_level(false);
	}
	else if (motstate==0)
	{
		NC_set_level(true);
		_delay_ms(2000);
		NC_set_level(false);
	}
}


int main(void)
{
	/* Initializes MCU, drivers and middleware */
	atmel_start_init();
	
	DDRD |= (1<<DDD4);
	
	printf("AT+DDET=1\r\n");              //Enable DMTF
	autostate = eeprom_read_byte(0x01);   //Read data from EEPROM
	motstate = autostate;
	motset(motstate);            //Set motor
	Auto_ind_set_level(autostate);        //Set auto
	

	/* Replace with your application code */
	while (1)
	{
		
		i=0;
		
		memset(ch, 0, sizeof(ch));
		
		while (1)
		{
			ch[i]= USART_0_read();
			if (ch[i]=='\n') break;
			i++;
		}
		
		if (strstr(ch,"RING"))
		{
			PIND |= (1<<PIND4);
			printf("ATA\r\n");
			_delay_ms(1000);
			playstatus();
			
		}
		
		if(strstr(ch,"+DTMF: 1"))
		{
			//PIND |= (1<<PIND4);
			printf("AT+CREC=5\r\n");
			motset(1); // Switch on motor
			printf("AT+CREC=4,\"C:\\User\\onstatus.amr\",0,100\r\n");
		}
		
		if(strstr(ch,"+DTMF: 2"))
		{
			printf("AT+CREC=5\r\n");
			//PIND |= (1<<PIND4);
			motset(0); // Switch off motor
			printf("AT+CREC=4,\"C:\\User\\offstatus.amr\",0,100\r\n");
		}
		
		if(strstr(ch,"+DTMF: 4"))
		{
			printf("AT+CREC=5\r\n");
			eeprom_write_byte(0x01,1);
			printf("AT+CREC=4,\"C:\\User\\autoonstatus.amr\",0,100\r\n");
			Auto_ind_set_level(true);
		}
		
		if(strstr(ch,"+DTMF: 5"))
		{
			printf("AT+CREC=5\r\n");
			eeprom_write_byte(0x01,0);
			printf("AT+CREC=4,\"C:\\User\\autooffstatus.amr\",0,100\r\n");
			Auto_ind_set_level(false);
		}
	}
}