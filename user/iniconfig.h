#ifndef INICONFIG_H
#define INICONFIG_H

#include "os_type.h"
#include "spi_flash.h"
#include "user_interface.h"

enum {
	// Длина имени и параметра
	maxParLen=24
};

typedef enum answOnOffErr{
	answErr = -1,
	answOff = 0,
	answOn  = 1
}answOnOffErr;

// Чтобы нам удобно было - сделаем каждый параметр строкой, что не более  символа
// Имя его такое же
typedef struct cfgPar{
	// Имя параметра (сохраняется на флешке)
	uint8_t name[maxParLen];
	// Значение параметра (сохраняется на флешке)
	uint8_t param[maxParLen];
	// Подсказка (не сохраняется на флешке)
	char* hint;
} cfgPar;

cfgPar* getCfgPar(const char* name);

char* getCfgParHint(const char* name);

cfgPar* setCfgPar(const char* name, const char* par);

/**
 * @brief Получить указатель на массив текущих параметров
 * 
 * @return const cfgPar*
 */
cfgPar* paramSet();

/**
 * @brief Получить количество текущих параметров
 * 
 * @return int
 */
int	paramSetSize();

/**
 * @brief сохранить параметры на флешке
 */
void saveCfgPar();

/**
 * @brief загрузить параметры. Если они косячные  - установить по умолчанию и сохранить
 */
void loadCfgPar();

/**
 * @brief Установить параметры по умолчанию (но не сохранять!)
 */
void setDefaults();

/**
 * @brief Установить конкретный параметр по умолчанию  (но не сохранять!)
 * 
 * @param name - имя параметра
 */
void setDefaultPar(const char* name);

/**
 * @brief Получить IP из строки.
 * 
 * @return true - ok, false-ошибка
 */
bool str2ip4(const char* str, ip_addr_t* ip4addr);

/**
 * @brief Получить IP по имени параметра
 * 
 * @return true - ok, false-ошибка
 */
bool par2ip4(const char* name, ip_addr_t* ip4addr);
 
/**
 * @brief Сконвертить строку в целое
 * 
 * @return true - ok, false-ошибка
 */
bool str2int(const char* str, int32_t* number);

/**
 * @brief Получить параметр-целое по имени
 * 
 * @return true - ok, false-ошибка
 */
bool par2int(const char* name, int32_t* number);

/**
 * @brief Ответ нам нужен - 'on' иль 'off' иль 'error' при ошибке!
 */
answOnOffErr getAnswOnOff(const char* name);

#endif // INICONFIG_H
