## Задание
Лампочка с выключателем - кнопка SAFE на плате отладчика подключена к выводу `GPIO_PIN(PORT_A, 11)` микроконтроллера. Напишите программу, которая по нажатию кнопки включала и выключала бы светодиод:
1. с использованием периодического опроса кнопок в цикле;
2. обрабатывая нажатие кнопки в прерывании с обработкой "дребезга" путем отключения прерываний;
3. светодиод должен не просто включаться, а мигать с заданной частотой, длинное нажатие кнопки переключает частоту миганий.