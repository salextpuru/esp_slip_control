#ifndef AUTH_MODE_H
#define AUTH_MODE_H

#include "user_interface.h"

/**
 * @brief Получить строку с названием режима авторизации
 */
const char* getStrFromAuth ( AUTH_MODE auth_mode );

/**
 * @brief Получить код режима авторизации по названию режима.
 * 
 * @return AUTH_MODE - код или -1, если неправильный режим
 */
AUTH_MODE  getAuthFromStr (const char* s);

/**
 * @brief Записывает в буфер s все возможные режимы авторизации через запятую 
 * 	Смотрите, чтобы в s хватило места, иначе быть бедэ...
 */
void getAuthNames(char* s);


#endif // AUTH_MODE_H
