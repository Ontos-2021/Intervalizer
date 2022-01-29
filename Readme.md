# Intervalizer
Intervalizer es un proyecto basado en Arduino para entrenar el reconocimiento de intervalos musicales.
Consiste en un disposito electrónico con el que a través de un juego podrás fortalecer tu oído musical.

## ¿Qué son los intervalos musicales?

La música está compuesta de sonidos, y estos sonidos (por lo menos la música occidental) son 12. Estos sonidos están *a cierta distancia* entre sí, esa distancia son los **intervalos**.

[Acá puedes ver un breve video de **Jaime Altozano** explicando qué es un intervalo](https://youtu.be/KbIRXKP6GfY "Video de YouTube")

## ¿Por qué son importantes los intervalos?

Al entrar a una escuela de música o al conservatorio, una de las primeras cosas con las que uno se encuentra son cosas como *terceras mayores*, *terceras menores*, *quinta justa*, *tritono*, etc. Y no es cosa rara ya que de los intervalos de compone la música.

Los intervalos son las piezas fundamentales con las que se construye la música. De los intervalos derivan arpegios, melodías, acordes, armonías, etc. 

Por lo tanto, si un músico puede reconocer y entonar los **12 intervalos** con facilidad, podrá dominar el solfeo musical, tocar y reconocer cualquier pieza de oído. 

## ¿Cómo funcionaría *Intervalizer*?

El juego emitirá diferentes intervalos en diferentes niveles. Primero estarán los intervalos más *consonantes* como la *quinta justa*,  la *cuarta justa*,   *la tercera mayor*, y en niveles más difíciles se irán incluyendo intervalos más *disonantes*, cómo la *séptima mayor*, *la cuarta aumentada o tritono*, la *segunda menor*, etc.
A través de los distintos niveles uno obtendrá puntaje al reconocer correctamente los intervalos. También al final de cada nivel, arrojará una estadística de cuales intervalos están más fuertes y cuáles más débiles.

### ¿Cuales son los intervalos?

Estos son 12, sería:

- *Unísono* (Este intervalo en realidad sería el mismo sonido. Incluyendo el *unísono* serían 13 intervalos)
- *Segunda Menor*
- *Segunda Mayor*
- *Tercera Menor*
- *Tercera Mayor*
- *Cuarta Justa*
- *Cuarta aumentada* o *Tritono*
- *Quinta Justa*
- *Sexta Menor*
- *Sexta Mayor*
- *Séptima Menor*
- *Septima Mayor*
- Y por último la *Octava*

### Buzzer
Para generar el sonido utilizamos un buzzer pasivo, el cual necesita de corriente y de una frecuencia de onda en particular. 
A diferencia del *buzzer pasivo*, existe el *buzzer activo* el cual posee un oscilador incorporado el cuál sólo necesita corriente para generar un tono, pero no es adecuado para este proyecto ya que trae un tono por defecto.

### Función `tone()`
La función tone() nos permite generar un tono con el buzzer pasivo.

#### Syntaxis 

`tone(pin, frequency)`  
`tone(pin, frequency, duration)`

## Referencias
[Música con Arduino](https://juegosrobotica.es/musica-con-arduino/#)

[Documentación función `random()` en Arduino](https://www.arduino.cc/reference/en/language/functions/random-numbers/random/ "Documentación oficial")

[Documentación función `tone()` en Arduino](https://www.arduino.cc/reference/en/language/functions/advanced-io/tone/ "Documentación oficial")

[Documentación de array en Arduino](https://www.arduino.cc/reference/en/language/variables/data-types/array/ "Documentación oficial")

[Uso de la función `sizeof()`](https://learn.parallax.com/tutorials/robot/shield-bot/robotics-board-education-shield-arduino/chapter-4-boe-shield-bot-4#:~:text=Using%20the%20sizeof%20Function&text=It%20can%20tell%20you%20both,of%20elements%20in%20the%20array.)

[Matriz de botones](https://www.youtube.com/watch?v=G82X3Z7S5uM "Video de YouTube")

[Teclado Keypad 4x4](https://www.youtube.com/watch?v=9ligsi5Bgv8 "Video de YouTube")

[Pantalla Oled Arduino](https://www.youtube.com/watch?v=cnrV3xJf4po&t=353 "Video de YouTube")