#include <stdint.h>
#include <tm4c123gh6pm.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <math.h>
float longitude1,latitude1,longitude2,latitude2,longitude_init,latitude_init,longitude_final,latitude_final,total_dist,x;


void UART1_Transmitter(unsigned char data){
    while((UART1_FR_R & (1<<5)) != 0){} // wait until Tx buffer not full
    UART1_DR_R = data;                  // before giving it another byte
}

void systick_intialization(int reload)
{
    NVIC_ST_CTRL_R=0; //disable timer
    NVIC_ST_RELOAD_R=reload-1;
    NVIC_ST_CURRENT_R =0;
    NVIC_ST_CTRL_R = 5; //enable timer module and adjust clock of timer
}
void delay(int sec, char x)
{
    int i;
    if (x=='m')
    {systick_intialization(16000);}
    else if (x == 'u')
    {systick_intialization(16);}
    else
    {systick_intialization(16000000);}

    for(i = 0; i< sec; i++)
    {
        while( (NVIC_ST_CTRL_R &= 0x10000)==0 ){}
    }
}
float act_coordinate(float coordinate)
{
    float d =coordinate/100; //2832.1834/100 =28.321834
int degree_part= d;//28
float minuite_part = (coordinate-(degree_part*100) )/60;
float fixvalue= degree_part+minuite_part;
return fixvalue;
}
float degr_Rad(float deg) {
  return deg * (3.14159265359/180);
}
float Distance(float latI,float lonI,float latF,float lonF)
    {
  int R = 6371; // Radius of the earth in km
  float dLat = degr_Rad(latF-latI);
  float dLon = degr_Rad(lonF-lonI);
  float a = sin(dLat/2) * sin(dLat/2) +
          cos(degr_Rad(latI)) * cos(degr_Rad(latF)) *
          sin(dLon/2) * sin(dLon/2);
  float c = 2 * atan2(sqrt(a),sqrt(1-a));
  float d = R * c; // Distance in km
    return d ;
}

char* GET_NMEA(void){
         int i;
     char str[30] = {0};
                while(1)
            {   for (i=0; i<6; i++)
                {
                while ((UART1_FR_R & 0x40) != 1){} //wait until Rx FIFO is full
                str[i] = UART1_DR_R;
                }//$Gpxxx
if (strcmp(str, "$GPRMC") == 0){
   break;}
}
i = 6;
while (UART1_DR_R != '$')
{
    while ((UART1_FR_R & 0x40) != 1){} //wait until Rx FIFO is full
            str[i] = UART1_DR_R;
            i++;
        }

return str;
}
void led_indicate_distance(float d){
if (d >= 100)
{
GPIO_PORTF_DATA_R |= 0X02;
}
else
{
GPIO_PORTF_DATA_R   &= ~0x0E;
}
}

void initial(){
    SYSCTL_RCGCGPIO_R |= 0x32;
    SYSCTL_RCGCUART_R = SYSCTL_RCGCUART_R1 ;// UART1 CLK ENABLED
    UART1_CTL_R &= ~ UART_CTL_UARTEN; //UART1 DISABLED
    UART1_IBRD_R = 104; //(16MHz/(16*9600)) INTEGER VALUE
    UART1_FBRD_R = 11; // ((FRACTION OF IBRD *64) +0.5) INTEGER VALUE
    UART1_LCRH_R |= 0x70;   // FIFO ENABLED, WORD LENGTH = 8bit
    UART1_CTL_R |= 0x301; // TX,RX, UARTEN ENABLED
    SYSCTL_RCGCUART_R = SYSCTL_RCGCUART_R7 ; //UART2 CLK ENABLED
    UART7_CTL_R &= ~ UART_CTL_UARTEN; //UART7 DISABLED
    UART7_IBRD_R = 104; //(16MHz/(16*9600)) INTEGER VALUE
    UART7_FBRD_R = 11; // ((FRACTION OF IBRD *64) +0.5) INTEGER VALUE
    UART7_LCRH_R |= 0x70;   // FIFO ENABLED, WORD LENGTH = 8bit
    UART7_CTL_R |= 0x301; // TX,RX, UARTEN ENABLED
    while(!(SYSCTL_PRGPIO_R &0x32)){}
//PORT F  (FOR LEDS & SWITCH1 & LCD ENABLE ,PF0 )
    GPIO_PORTF_LOCK_R = 0x4C4F434B;
    GPIO_PORTF_CR_R |= 0x1F;
    GPIO_PORTF_DIR_R |= 0x0E;
    GPIO_PORTF_DEN_R |= 0x1F;
    GPIO_PORTF_AMSEL_R = 0;
    GPIO_PORTF_AFSEL_R = 0;
    GPIO_PORTF_PCTL_R = 0;
    GPIO_PORTF_PUR_R = 0xFF;
//PORT B    (LCD 4,5,6,7 PINS & GPS B0,B1 & (RS,RW) B2,B3)
    GPIO_PORTB_CR_R = 0xFF;
    GPIO_PORTB_DIR_R |= 0xFE;
    GPIO_PORTB_DEN_R |= 0xFF;
    GPIO_PORTB_AMSEL_R = 0;
    GPIO_PORTB_AFSEL_R = 0x03;
    GPIO_PORTB_PCTL_R = 0x11;
    GPIO_PORTB_PUR_R = 0xFF;
//PORT E (BLUETOOTH E0,E1 & LCD 2,3,4,5 PINS )
    GPIO_PORTE_CR_R = 0xFF;
    GPIO_PORTE_DIR_R |= 0xFE;
    GPIO_PORTE_DEN_R |= 0xFF;
    GPIO_PORTE_AMSEL_R = 0;
    GPIO_PORTE_AFSEL_R = 0x03;
    GPIO_PORTE_PCTL_R = 0x11;
    GPIO_PORTE_PUR_R = 0xFF;
    LCD_COMMAND(0x01); //CLEAR
    LCD_COMMAND(0x02); //HOME
    LCD_COMMAND(0x80); //START OF FIRST LINE
    LCD_COMMAND(0x0E); //DISPLAY ON CURSOR BLINKING
}
int main(){
        initial();
        char bluetooth_data [20]={0};
        int z;
        for (z = 0 ; z<20 ;z++)
             {
                 while (UART7_FR_R & 0x04 == 0){};
                  bluetooth_data[z] = UART7_DR_R ;
             }
        sscanf(bluetooth_data,"%f;%f",&latitude_final,&longitude_final);
        // final coordinates will be taken through bluetooth data
        while(1){
            if ((GPIO_PORTF_DATA_R &= 0x10) == 0){
            char* Str= GET_NMEA();
                   char modifier[20]= {};
                   char number [20] ={0};
                   char letter1;
                   float latitude;
                   char letter2;
                   float longitude;
                   sscanf( Str , "%[^,],%[^,],%c,%f,%c,%f", modifier, number,&letter1,&latitude,&letter2,&longitude);

               float lon =act_coordinate(longitude);
               float lat =act_coordinate(latitude);
               latitude1 = lat;
               longitude1 = lon;
               latitude_init = lat;
               longitude_init = lon;
               break;}
        }
        while(1){

        char* Str= GET_NMEA();
        char modifier[20]= {};
        char number [20] ={0};
        char letter1;
        float latitude;
        char letter2;
        float longitude;
        sscanf( Str , "%[^,],%[^,],%c,%f,%c,%f", modifier, number,&letter1,&latitude,&letter2,&longitude);

    float lon =act_coordinate(longitude);
    float lat =act_coordinate(latitude);
    latitude2 = lat;
    longitude2 = lon;

    x = Distance(latitude1,longitude1,latitude2,longitude2);
    total_dist += x;
        longitude1 = longitude2;
        latitude1 = latitude2;
    led_indicate_distance(total_dist);
    if (GPIO_PORTF_DATA_R & 0x02 == 1)
    {break;}
        }

       return 0;}
