/* Libreria propia  Para controlar el variador de Frecuencia TECO L510 con la Tarjeta Arduino uno
  usando protocolo Modbus Rtu

  Alan Yeray Olivas

  RS485
  Definiciones e implementaciones

  */

#include <TECO_L510.h>


TECO_L510::TECO_L510()
{   
}

TECO_L510::~TECO_L510()
{  
}

void TECO_L510::setup( uint8_t Pinn )
{// pin de habilitacion de transmision
  pinMode(Pinn, OUTPUT); 
  _EnTxPinn = Pinn;
}

void TECO_L510::runVariador()
{
   // Run Forward Command
  // 01 06 25 01 00 01 12 C6
  digitalWrite(_EnTxPinn, HIGH);

  Serial2.write(0x01);// ID
  Serial2.write(0x06);// Fuction Code
  Serial2.write(0x25);// Starting Address #1 MSB
  Serial2.write(0x01);// Starting Address #2 LSB eeror en 2
  Serial2.write(0x00);// Send data #1  MSB
  Serial2.write(0x01);// Send data #2 LSB
  Serial2.write(0x12);// crc #1
  Serial2.write(0xC6);// crc #2
  Serial2.flush();
  vTaskDelay(pdMS_TO_TICKS(vtiempo_delay)); 
  digitalWrite(_EnTxPinn, LOW); // RS485 como receptor
  recepcion_impresion_trama(); 
}

void TECO_L510::stopVariador()
{
  // Stop Command
  // 01 06 25 01 00 00 D3 06
  digitalWrite(_EnTxPinn, HIGH); // rs modo transmisor
  Serial2.write(0x01); // ID
  Serial2.write(0x06); // Fuction Code
  Serial2.write(0x25); // Starting Address #1 MSB
  Serial2.write(0x01); // Starting Address #2 LSB
  Serial2.write(0x00); // Send data #1  MSB
  Serial2.write(0x00); // Send data #2 LSB
  Serial2.write(0xD3); // crc #1
  Serial2.write(0x06); // crc #2
  Serial2.flush();
  vTaskDelay(pdMS_TO_TICKS(vtiempo_delay)); 
  digitalWrite(_EnTxPinn, LOW); // rs modo recepto
  recepcion_impresion_trama();
}

uint8_t TECO_L510::set_freq_variador(uint16_t freq)  //
{ // funcion para establecer consigna de frecuencia de forma manual y codigo
  // Maxima frecuencia motor con variador es de 60.00hz donde el variador los representa como 6000
    uint16_t last_freq =0;
  if ( freq <= 7000) 
  {
  uint8_t buffer_variable[] = {0x01, 0x06, 0x25, 0x02, highByte(freq), lowByte(freq)}; // ejemplo: toma 0xAB y 0xCD de 0xABCD
  size_t len = sizeof(buffer_variable) / sizeof(buffer_variable[0]);
  uint16_t crc = calculateCRC16(buffer_variable, len);

  digitalWrite(_EnTxPinn, HIGH); // rs modo transmisor
  Serial2.write(0x01);// ID
  Serial2.write(0x06);// Fuction Code
  Serial2.write(0x25);// Starting Address #1 MSB
  Serial2.write(0x02);// Starting Address #2 LSB
  Serial2.write(highByte(freq));// Send data #1  MSB
  Serial2.write(lowByte(freq));// Send data #2 LSB
  Serial2.write(lowByte(crc));// crc #2   // para el variador se debe invertir el orden (A little endian)
  Serial2.write(highByte(crc));// crc #1
  Serial2.flush();
  
  vTaskDelay(pdMS_TO_TICKS(vtiempo_delay)); 
  digitalWrite(_EnTxPinn, LOW); // rs modo receptor
  recepcion_frecuencia_trama();

  
  return 1; // si se pudo enviar la trama
  }else{
    return 0; //no se pudo enviar la trama
  }
}



uint16_t TECO_L510::calculateCRC16(uint8_t *data, size_t length)
{
  uint16_t crc = 0xFFFF;
  uint16_t i;
  //0xA001 Modbus Polinomio 
  for (i = 0; i < length; i++) {
    crc ^= data[i];
    for (int j = 0; j < 8; j++) {
      if (crc & 0x0001) {
        crc = (crc >> 1) ^ 0xA001; 
      } else {
        crc >>= 1;
      }
    }
  }

  return crc;
}

void TECO_L510::recepcion_impresion_trama() // Si hubo recepcion de datos entonces hubo un error en el envio o trama
{ 
   if ( Serial2.available() > 0){
    Serial.println("Teco: Error en Trama: ");
    for (int i = 0; i < 8; i++)
    {
      buffer[i] = Serial2.read();
      delay(25);
    }
    Serial.println(" ");
    Serial.print(buffer[0], HEX);// ID
    Serial.print(" ");
    Serial.print(buffer[1], HEX);// Fuction Code
    Serial.print(" ");
    Serial.print(buffer[2], HEX);// Starting Address #1 MSB
    Serial.print(" ");
    Serial.print(buffer[3], HEX);// Starting Address #2 LSB
    Serial.print(" ");
    Serial.print(buffer[4], HEX);// Send data #1  MSB
    Serial.print(" ");
    Serial.print(buffer[5], HEX);// Send data #2 LSB
    Serial.print(" ");
    Serial.print(buffer[6], HEX);// crc #2   // para el variador se debe invertir el orden (A little endian)
    Serial.print(" ");
    Serial.println(buffer[7], HEX);// crc #1

    if ( comparacion_recepcion_trama_freq_crc16() == 0 ){
      Serial.println("Teco: Error en CRC16, reenviando trama..falta agregar funcion");
      //conversion_frecuencia_variable();
    }
  
  }else
  {
    // No hubo recepcion de datos entonces la trama fue enviada correctamente
  }  
}

void TECO_L510::recepcion_frecuencia_trama()
{
    //buffer exclusivo para el valor de la frecuencia variable
  if ( Serial2.available() > 0){
    Serial.println("Teco: Error recepcion en Trama (): ");
    for (int i = 0; i < 9; i++)
    {
      buffer[i] = Serial2.read();
      delay(25);
    }
    Serial.print(bufferF[0], HEX);// ID
    Serial.print(" ");
    Serial.print(bufferF[1], HEX);// Fuction Code
    Serial.print(" ");
    Serial.print(bufferF[2], HEX);// Starting Address #1 MSB
    Serial.print(" ");
    Serial.print(bufferF[3], HEX);// Starting Address #2 LSB
    Serial.print(" ");
    Serial.print(bufferF[4], HEX);// Send data #1  MSB
    Serial.print(" ");
    Serial.print(bufferF[5], HEX);// Send data #2 LSB
    Serial.print(" ");
    Serial.print(bufferF[6], HEX);// crc #2   // para el variador se debe invertir el orden (A little endian)
    Serial.print(" ");
    Serial.println(bufferF[7], HEX);// crc #1

    if ( comparacion_recepcion_trama_freq_crc16() == 0 ){
      Serial.println("Teco: Error en CRC16, reenviando trama..falta agregar funcion x2");
      //conversion_frecuencia_variable();
    }
 
  }
  else{
    // No hubo recepcion de datos entonces la trama fue enviada correctamente
  }
}

uint8_t TECO_L510::comparacion_recepcion_trama_freq_crc16()
{
  uint8_t crc16_ok;
  uint8_t buffer_variable[] = {bufferF[0], bufferF[1],bufferF[2], bufferF[3], bufferF[4], bufferF[5]}; // ejemplo: toma 0xAB y 0xCD de 0xABCD
  size_t len = sizeof(buffer_variable) / sizeof(buffer_variable[0]);
  uint16_t crc = calculateCRC16(buffer_variable, len); 
  if ( (lowByte(crc) << 8 | highByte(crc)) == (bufferF[6] << 8 | bufferF[7]) ){// inversion a little endian
    //Serial.println("CRC16 OK");
    crc16_ok = 1;
  }else {
    //Serial.println("CRC16 ERROR");
    crc16_ok = 0;
  }
  return  crc16_ok;  
}

