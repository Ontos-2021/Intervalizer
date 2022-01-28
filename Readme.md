# Intervalizer
Intervalizer es un proyecto basado en Arduino para entrenar el reconocimiento de intervalos musicales

## Buzzer
Para generar el sonido utilizamos un buzzer pasivo, el cual necesita de corriente y de una frecuencia de onda en particular. 
A diferencia del *buzzer pasivo*, existe el *buzzer activo* el cual posee un oscilador incorporado por lo que sólo necesita corriente para generar un tono.

### Función tone()
La función tone() nos permite generar un tono con el buzzer.

### Syntax

`tone(pin, frequency)`  
`tone(pin, frequency, duration)`
