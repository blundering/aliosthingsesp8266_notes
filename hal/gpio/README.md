1. 新增hal/gpio封装文件
  `platform/mcu/esp8266/hal/gpio.c` 

  [文件地址](gpio.c)


2. 文件`platform/mcu/esp8266/esp8266.mk`中增加引用
  `$(NAME)_SOURCES  += hal/gpio.c`

  ![mkfile添加引用](../../resource/img/hal_gpio/mk_file.jpg)

