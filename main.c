#include <stdint.h>
#include <tm4c123gh6pm.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <math.h>
float longitude1,latitude1,longitude2,latitude2,longitude_init,latitude_init,longitude_final,latitude_final,total_dist,x;
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
