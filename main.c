#include <stdint.h>
#include <tm4c123gh6pm.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <math.h>
float longitude1,latitude1,longitude2,latitude2,longitude_init,latitude_init,longitude_final,latitude_final,total_dist,x;

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
