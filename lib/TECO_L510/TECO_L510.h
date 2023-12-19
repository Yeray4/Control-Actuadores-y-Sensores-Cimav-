/* Libreria propia  Para controlar el variador de Frecuencia TECO L510 con la Tarjeta Arduino uno
  usando protocolo Modbus Rtu

  Alan Yeray Olivas

  RS485
   Header donde se hacen todas las declaraciones 

   40ms delay entre comandos

  */

 #ifndef _TECO_L510_H
 #define _TECO_L510_H
 #include <Arduino.h>
 
 class TECO_L510
 {  
    public:

        
        TECO_L510(); // constructor
        ~TECO_L510(); // destructor
        void setup( uint8_t Pinn ); //Config Pines Input
        void runVariador();
        void stopVariador();
        uint8_t set_freq_variador( uint16_t freq );
        //uint16_t last_bytes_freq = 0;
    
    private:
        uint16_t calculateCRC16(uint8_t *data, size_t length);
        void recepcion_impresion_trama();
        void recepcion_frecuencia_trama();
        uint8_t comparacion_recepcion_trama_freq_crc16();
        uint8_t _EnTxPinn; //21

    protected:
        uint16_t crc;
        
        int var1, var2;// c1 = 0;
        byte buffer[8], bufferF[8];
        uint16_t vtiempo_delay = 80; // delay necesario para que funciones el variador en el Esp32
 };

 #endif // _TECO_L510_H
