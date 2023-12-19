/* Libreria propia para los sensores SHT40 para  TEMPERATURA Y HUMEDAD 
   Por Alan Yeray Olivas 

   Definiciones e implementaciones

   #include <freertos/task.h>
   vTaskDelay(pdMS_TO_TICKS(100));
   */

#include <SHT40.h>
#include <freertos/task.h>
#include <HardwareSerial.h>


SHT40::SHT40() // de la clase sht40 exite un metodo SHT40
{   
}

SHT40::~SHT40()   // Destructor
{   
}

void SHT40::setup(uint8_t Pinn)// configuracion EnTxPin
{
  _EnTxPinn = Pinn;
  pinMode(Pinn, OUTPUT); //esta instruccion tambien esta inicializada en la libreria TECO_L510.h
}


void SHT40::SendGetSerial(uint8_t Nsensor)//Retornas Temp y Hum
{ // Valor en Hexadecimal
  digitalWrite(_EnTxPinn, HIGH);
  ResetBus();
  vTaskDelay(pdMS_TO_TICKS(50));
  Serial2.write(0x55);
  Serial2.write(Nsensor); // Nsensor  0x32, 0x33, 0x34, 0x35
  Serial2.write(0xF1);
  Serial2.flush(); // esperamos a que se envien los datos
  Serial.print("Sht40: Addr. Sensor: 0x");
  Serial.println(Nsensor, HEX);
  LeerSerial( Nsensor);
  getDataSensores(Nsensor); 
  
}


void SHT40::LeerSerial(uint8_t Nsensor)
{
  bool TrueCRC1 = false;
  bool TrueCRC2 = false;
  digitalWrite(_EnTxPinn, LOW); // RS485 como receptor
  vTaskDelay(pdMS_TO_TICKS(25));

  if (Serial2.available() > 0)
  {
    for (int i = 0; i < 6; i++)
    {
      buffer[i] = Serial2.read();
      vTaskDelay(pdMS_TO_TICKS(25));
    }
    //impresion_trama();  
    TCRC = buffer[2];
    HCRC = buffer[5];
    CRC();
    if (TCRC == crcF){TrueCRC1 = true; }//Serial.print("-TCRC:"); Serial.println(String(TrueCRC1));
    if (HCRC == crcF2){TrueCRC2 = true; }//Serial.print("-HCRC:"); Serial.println(String(TrueCRC2));
    Formulas(); 
 } 
  else
  {
    Serial.println("Sht40: No hay datos recibidos en 0x");Serial.print(Nsensor, HEX);
  }
}

void SHT40::ResetBus()
{
  Serial2.write(0x66);
  Serial2.write(0x66);
  Serial2.write(0x66);  
}

void SHT40::CRC()
{
     // crc calculator
  // comparar los valores del crc calculado con el que envian si no es correcto volver a mandar la solicitud de temperatura
  // hasta que sea correcto
  unsigned char crc = 0xFF;
  int i=0;
  bool t_control = false;
  for (int k = 0; k < 2; k++)// se debe repetir dos veces, uno para el bytes de temperatura  y otros para humedad
  {

    if (t_control == false) 
    {
      for ( i = 0; i < 2; i++)
      {
        crc ^= buffer[i];
        for (int j = 0; j < 8; j++)
        {
          if ((crc & 0x80) != 0)
          {
            crc = (unsigned char)((crc << 1) ^ 0x31);
          }
          else
          {
            crc <<= 1;
          }
        } 
        if( i == 1){ // i = 1
          crcF= crc;
          //Serial.print("1CRC= ");
          //Serial.println(crcF, HEX);
          t_control = true;
          }    
      }
      
    }
    else // true
      {
        crc = 0xFF;
        for ( i = 3; i < 5; i++)
        {
          crc ^= buffer[i];
          for (int j = 0; j < 8; j++)
          {
            if ((crc & 0x80) != 0)
            {
             crc = (unsigned char)((crc << 1) ^ 0x31);
            }
          else
            {
            crc <<= 1;
            } 
          } 
          if( i == 4){ // i = 4
            crcF2= crc;
            //Serial.print("4CRC= ");
            //Serial.println(crcF2, HEX);
          }      
        }  
      }
  } 
}

void SHT40::Formulas()
{
  Temp1 = buffer[0];
  Temp2 = buffer[1];
  Hum1 = buffer[3];
  Hum2 = buffer[4];

  ST = (Temp1 * 256) + Temp2;
  SRH = (Hum1 * 256) + Hum2;

  float ft2 = ST / 65535;

  FTx = -45 + (175 * ft2);
  FRHx = (-6 + 125 * (SRH / 65535));

  //Serial.print("Sht40 lib : Temperatura: ");
  //Serial.print( FTx);
  //Serial.println("°C ");
  //Serial.print("Sht40 lib: Humedad: ");
  //Serial.print(FRHx);
  //Serial.println(" %RH ");
}

uint8_t SHT40::getDataSensores( uint8_t Nsensor)
{
  if(Nsensor == adrSensor_x31){  dataSensor31.FT = FTx; dataSensor31.FRH = FRHx; return 1;} 
  else if (Nsensor == adrSensor_x32){ dataSensor32.FT = FTx; dataSensor32.FRH = FRHx; return 1;} 
  else if (Nsensor == adrSensor_x33){ dataSensor33.FT = FTx; dataSensor33.FRH = FRHx; return 1;}
  else if (Nsensor == adrSensor_x34){ dataSensor34.FT = FTx; dataSensor34.FRH = FRHx; return 1;}
  else if (Nsensor == adrSensor_x35){ dataSensor35.FT = FTx; dataSensor35.FRH = FRHx; return 1;}
  else if (Nsensor == adrSensor_x36){ dataSensor36.FT = FTx; dataSensor36.FRH = FRHx; return 1;}
  else if (Nsensor == adrSensor_x37){ dataSensor37.FT = FTx; dataSensor37.FRH = FRHx; return 1;}
  else { // se metio otro parametro o no sucecdio return 0; 
        return 0;
        }
}

    


void SHT40::impresion_trama()
{
  Serial.print("Sht40: Trama Recibida: ");
  Serial.print(buffer[0], HEX);
  Serial.print(" ");
  Serial.print(buffer[1], HEX);
  Serial.print(" ");
  Serial.print(buffer[2], HEX);
  Serial.print(" ");
  Serial.print(buffer[3], HEX);
  Serial.print(" ");
  Serial.print(buffer[4], HEX);
  Serial.print(" ");
  Serial.println(buffer[5], HEX);
}


