/* Libreria propia  Para adquirir los datos sensados atraves del adc

  Alan Yeray Olivas

  Sensor responde cada 400ms

  Definiciones e implementaciones

  */

#ifndef _FH400_H
#define _FH400_H
#include <Arduino.h>

class FH400
{

public:
    struct sensor_fh400
    {
        float speed;
        float temp;
        float hum;
    };
    sensor_fh400 datafh400;

    FH400();                                                          // Constructor
    ~FH400();                                                         // Destructor
    void setup(uint8_t pinBlanco, uint8_t pinVerde, uint8_t pinCafe); // Config Pines Input
    float conversion_velocidad();                                     // devuelve en m/s
    float filtro_viento();                                            // adc con filtro
    float conversion_temp();
    float conversion_hum(); // devuelve la humedad

private:
    uint8_t _pinBlanco; // pines genericos para adc su sensor
    uint8_t _pinVerde;
    uint8_t _pinCafe;

protected:
    int adc_filtrado = 0;        // Filtro Media Móvil como Pasa Bajos
    int adc_raw = 0;             // Filtro Media Móvil como Pasa Bajos
    float alpha = 0.50;          // aumentar el valor para disminuir el filtro y aumentar la velocidad en cambios de mediciones
    float blanco_velocidad = 0;  // Valor de referencia para velocidad
    float verde_temperatura = 0; // Valor de referencia para temperatura
    float cafe_humedad = 0;
    const uint8_t vdcMax = 10; // Output Voltage at 100°C should be 5.0V.
    float velocidad_resultado; // variable global necesaria para la compensacion
};

#endif // _FH400_H