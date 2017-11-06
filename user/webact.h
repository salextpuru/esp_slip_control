#ifndef WEBACT_H
#define WEBACT_H

#include <stdint.h>

/**
* @brief Страницу тут мы делаем и буфер под ней мы выделяем.
* 	Коннект пришел когда - мы начинаем тут работать.
*/
void onInitWeb();

/**
* @brief Здесь синтезируем Страницу мы и указатель на неё мы возвращаем.
* 
* @return uint8_t*
*/
uint8_t* getPageWeb();

/**
* @brief Закончена работа, стерта память... Сюда при дисконнекте попадаем.
*/
void onDoneWeb();

/**
* @brief Запрос от веба отработать
*/
void onGETWeb ( char *data, unsigned short length );

#endif // WEBACT_H
