/* Libreria propia para los sensores SHT40 para  TEMPERATURA Y HUMEDAD
   Por Alan Yeray Olivas

   Header donde se hacen todas las declaraciones

   Rs485

   Tipo de dato: Sensado definido en .cpp libreria SHT40

   */

#ifndef _SHT40_H
#define _SHT40_H
#include <Arduino.h>

class SHT40
{

public:
    const uint8_t adrSensor_x33 = 0x33;
    const uint8_t adrSensor_x34 = 0x34;
    const uint8_t adrSensor_x37 = 0x37;
    const uint8_t adrSensor_x36 = 0x36;
    const uint8_t adrSensor_x35 = 0x35;
    const uint8_t adrSensor_x31 = 0x31;
    const uint8_t adrSensor_x32 = 0x32;

    struct Sensado
    { // estructura para almacenar los valores de temperatura y humedad
        float FT;
        float FRH;
    };

    struct Sensado dataSensor31 = {0, 0}, dataSensor32 = {0, 0}, dataSensor33 = {0, 0}, dataSensor34 = {0, 0}, dataSensor35 = {0, 0}, dataSensor36 = {0, 0}, dataSensor37 = {0, 0};

    SHT40();  // constructor
    ~SHT40(); // destructor
    void setup(uint8_t Pinn);
    void ResetBus();
    void SendGetSerial(uint8_t Nsensor); // retorna dos valores

    uint8_t getDataSensores(uint8_t Nsensor);
    float FTx = 0, FRHx = 0;
    uint8_t _EnTxPinn; // pin de habilitacion de transmision

private:
    void LeerSerial(uint8_t Nsensor);
    void Formulas();
    void impresion_trama();
    void CRC();
    unsigned char crcF, crcF2;

protected:
    int16_t Temp1 = 0, Hum1 = 0, Temp2 = 0, Hum2 = 0, HCRC = 0, TCRC = 0;
    uint8_t Nvar;
    float ST = 0, SRH = 0;
    byte buffer[6]; // buffer para almacenar los datos recibidos
};

#endif // _SHT40_H
