/* Libreria propia  Para adquirir los datos sensados atraves del adc

  Alan Yeray Olivas

  Definiciones e implementaciones

    Programa para  recepcion datos analogicos del Sensor Anemometro FH400 de la marca degreeC
    Donde se optiene Temperatura, velocidad de aire y humedad ademas de calcular el flujo de aire
    FH400 3B12 - Responde cada 400ms
    El sensor necesita de 19 a 29 volts y <15mA
    Se necesitan tres adcs

GPIO32 (ADC1_CH4)
GPIO33 (ADC1_CH5)
GPIO34 (ADC1_CH6)
GPIO35 (ADC1_CH7)
GPIO36 (ADC1_CH0)
GPIO39 (ADC1_CH3)

With the Voutput not clamped option, the output voltage is limited by the input supply voltage minus the
internal operational amplifier VO output swing limit (typically .3V).

 v2.0
*/

#include <FH400.h>

FH400::FH400()
{
}

FH400::~FH400()
{
}

void FH400::setup(uint8_t pinBlanco, uint8_t pinVerde, uint8_t pinCafe)
{
  pinMode(pinBlanco, INPUT);
  pinMode(pinVerde, INPUT);
  pinMode(pinCafe, INPUT);
  _pinBlanco = pinBlanco;
  _pinVerde = pinVerde;
  _pinCafe = pinCafe;
  // analogSetWidth(12);                            // 12 bits de resolucion (ya que el adc es de 12 bits)
  // analogSetPinAttenuation(_pinBlanco, ADC_11db); //2.5dB attenuation (ADC_ATTEN_DB_2_5) gives full-scale voltage 1.5V

  // 6dB attenuation (ADC_ATTEN_DB_6) gives full-scale voltage 2.2V
  //  VDD de   --volts
}

float FH400::conversion_velocidad()
{
  /* Application airflow range is 100 – 2000 fpm.
     Output voltage at 1000 fpm is 5.0V.
     At airflow velocities exceeding 1000 fpm, the output voltage should not exceed 5.0V.
     Checar data sheet
     */
  const uint16_t velocity_high_range = 10160; //  mm/s
  const uint8_t muestreo = 20;                // cantidad de muestras a tomar para promediar
  uint16_t aux2;
  float aux = 0;

  for (uint8_t i = 0; i < muestreo; i++)
  {
    aux2 = analogRead(_pinBlanco);
    aux = aux + ((aux2 * velocity_high_range) / (vdcMax * 1000));
  }
  blanco_velocidad = aux / muestreo; // division para promediar

  velocidad_resultado = (float)blanco_velocidad / 1000; // cambiamos la escala a m/s
  Serial.printf("fh400:sin Filtro Velocidad: %0.2f mm/s | %0.2f m/s\n", blanco_velocidad, velocidad_resultado);
  return velocidad_resultado;
}

float FH400::filtro_viento()
{
//Filtro Media Móvil como Pasa Bajos
//An=a*M+(1-a)*An
//alpha 1: Sin filtro
//alpha 0: Filtrado totalmente
//alpha clásico 0.05
const uint16_t velocity_high_range = 10160; //  mm/s


   adc_raw = analogRead(_pinBlanco);
   adc_filtrado = (alpha*adc_raw) + ((1-alpha)*adc_filtrado);

   /* Serial.println();
   Serial.print(adc_raw);
   Serial.print(",");  
   Serial.println(adc_filtrado); */

  float velocidad_resultado =   ((adc_filtrado * velocity_high_range) / (vdcMax * 1000));
  //Serial.printf("lib fh400:con filtro Velocidad: %0.2f mm/s | %0.2f m/s \n", velocidad_resultado, velocidad_resultado/ 1000);
  

  return velocidad_resultado / 1000;
}

float FH400::conversion_temp()
{ // Function to convert the voltage to temperature

  const uint8_t CentrigradosMax = 100; // Application temperature range is 0 – 100 °C
  const uint8_t muestreo = 5;        // cantidad de datos a tomar para promediar

  float aux, aux2;
  for (uint8_t i = 0; i < muestreo; i++)
  {

    aux2 = analogRead(_pinVerde);
    aux = aux + ((aux2 * CentrigradosMax) / vdcMax);

    // compensacion segun datasheet
    /*  if (velocidad_resultado > 0.5)
     {
       aux = aux + (((aux2 * CentrigradosMax) / vdcMax) + 1);
     }
     else
     {
       aux = aux + (((aux2 * CentrigradosMax) / vdcMax) + 2);
     } */
  }
  verde_temperatura = aux / muestreo;

  // compensacion necesario, segun datasheet
  float temperatura_resultado = verde_temperatura / 1000; // cambiamos us escala
  // Serial.printf("fh400: Temperatura: %f \n", temperatura_resultado);
  return temperatura_resultado;
}

float FH400::conversion_hum()
{
  const uint8_t humedad_range_high = 100; // porcentaje de humedad
  const uint8_t muestreo = 100;           // cantidad de datos a tomar para promediar */

  float aux;
  for (uint8_t i = 0; i < muestreo; i++)
  {
    // voltage = analogRead(_pinCafe) * (3.3 / 4095.0);
    aux = aux + (analogRead(_pinCafe) * humedad_range_high) / vdcMax;
    // Serial.printf("fh400: hum [%d]aux:%f \n", i,aux);
  }
  cafe_humedad = aux / muestreo;

  float humedad_resultado = cafe_humedad / 1000;
  // Serial.printf("fh400: Humedad:%f   \n", humedad_resultado);
  return humedad_resultado;
}
