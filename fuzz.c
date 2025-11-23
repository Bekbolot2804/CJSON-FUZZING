#include "cJSON.h"
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

// Функция для фаззинга
int LLVMFuzzerTestOneInput(const uint8_t *data, size_t size) {
    if (size < 1) return 0;

    // Создаем строку для парсинга
    char *json_string = malloc(size + 1);
    if (!json_string) return 0;
    memcpy(json_string, data, size);
    json_string[size] = '\0';

    // Тест 1: Парсинг JSON
    cJSON *parsed_json = cJSON_Parse(json_string);

    if (parsed_json != NULL) {
        // Тест 2: Проверка различных типов данных
        cJSON *array_item = cJSON_GetArrayItem(parsed_json, 0);
        cJSON *object_item = cJSON_GetObjectItem(parsed_json, "test");

        // Тест 3: Сериализация обратно в строку
        char *printed_json = cJSON_Print(parsed_json);
        if (printed_json) {
            cJSON_free(printed_json);
        }

        char *unformatted_json = cJSON_PrintUnformatted(parsed_json);
        if (unformatted_json) {
            cJSON_free(unformatted_json);
        }

        // Тест 4: Работа с массивами
        int array_size = cJSON_GetArraySize(parsed_json);
        for (int i = 0; i < array_size && i < 10; i++) {
            cJSON_GetArrayItem(parsed_json, i);
        }

        // Тест 5: Создание и модификация объектов
        cJSON *test_obj = cJSON_CreateObject();
        if (test_obj) {
            cJSON_AddStringToObject(test_obj, "fuzz_string", json_string);
            cJSON_AddNumberToObject(test_obj, "fuzz_number", (double)size);
            cJSON_AddBoolToObject(test_obj, "fuzz_bool", size % 2);

            // Тест 6: Потенциально опасные операции со строками
            cJSON *string_item = cJSON_GetObjectItem(test_obj, "fuzz_string");
            if (cJSON_IsString(string_item)) {
                char *new_val = malloc(size + 1);
                if (new_val) {
                    memcpy(new_val, data, size);
                    new_val[size] = '\0';
                    cJSON_SetValuestring(string_item, new_val);
                    free(new_val);
                }
            }

            cJSON_Delete(test_obj);
        }

        // Тест 7: Дублирование JSON
        cJSON *duplicated = cJSON_Duplicate(parsed_json, 1);
        if (duplicated) {
            cJSON_Delete(duplicated);
        }

        // Тест 8: Минификация
        char *minify_test = malloc(size + 1);
        if (minify_test) {
            memcpy(minify_test, data, size);
            minify_test[size] = '\0';
            cJSON_Minify(minify_test);
            free(minify_test);
        }

        cJSON_Delete(parsed_json);
    } else {
        // Тест 9: Минификация даже при неудачном парсинге
        char *minify_input = malloc(size + 1);
        if (minify_input) {
            memcpy(minify_input, data, size);
            minify_input[size] = '\0';
            cJSON_Minify(minify_input);
            free(minify_input);
        }
    }

    // Тест 10: Создание JSON с потенциально опасными значениями
    cJSON *root = cJSON_CreateObject();
    if (root) {
        // Создаем строку с потенциально проблемным содержимым
        char *problematic_string = malloc(size + 1);
        if (problematic_string) {
            memcpy(problematic_string, data, size);
            problematic_string[size] = '\0';

            cJSON_AddStringToObject(root, "data", problematic_string);

            // Специальный тест для cJSON_SetValuestring
            cJSON *string_obj = cJSON_GetObjectItem(root, "data");
            if (string_obj && cJSON_IsString(string_obj)) {
                if (size % 100 == 0) {
                    // Потенциально опасная операция
                    string_obj->valuestring = NULL;
                    cJSON_SetValuestring(string_obj, "test");
                } else {
                    cJSON_SetValuestring(string_obj, problematic_string);
                }
            }

            free(problematic_string);
        }

        // Добавляем числа с особыми значениями - используем переносимые способы
        double nan_value = 0.0 / 0.0;  // Это создаст NaN
        double inf_value = 1.0 / 0.0;  // Это создаст Infinity
        double neg_inf_value = -1.0 / 0.0;  // Это создаст -Infinity

        cJSON_AddNumberToObject(root, "nan", nan_value);
        cJSON_AddNumberToObject(root, "inf", inf_value);
        cJSON_AddNumberToObject(root, "neg_inf", neg_inf_value);
        cJSON_AddNumberToObject(root, "large_num", 1e308);
        cJSON_AddNumberToObject(root, "small_num", -1e308);

        // Создаем вложенные структуры - исправленная версия
        cJSON *array = cJSON_CreateArray();
        if (array) {
            for (int i = 0; i < 5 && i < (int)size; i++) {
                cJSON *number_item = cJSON_CreateNumber((double)data[i]);
                if (number_item) {
                    cJSON_AddItemToArray(array, number_item);
                }
            }
            cJSON_AddItemToObject(root, "nested_array", array);
        }

        char *result = cJSON_Print(root);
        if (result) {
            cJSON_free(result);
        }

        cJSON_Delete(root);
    }

    free(json_string);
    return 0;
}

// Альтернативная версия для более агрессивного тестирования
void aggressive_cjson_fuzz(const uint8_t *data, size_t size) {
    if (size < 1) return;

    // Тест парсинга с разными опциями
    if (size > 0) {
        char *input_str = malloc(size + 1);
        if (input_str) {
            memcpy(input_str, data, size);
            input_str[size] = '\0';

            cJSON_ParseWithOpts(input_str, NULL, 1);
            cJSON_ParseWithLengthOpts(input_str, size, NULL, 0);
            cJSON_ParseWithLength(input_str, size);

            free(input_str);
        }
    }

    // Тест создания объектов с большими вложениями
    cJSON *deep = cJSON_CreateObject();
    cJSON *current = deep;
    for (int i = 0; i < 100 && i < (int)size; i++) {
        cJSON *next = cJSON_CreateObject();
        if (next) {
            cJSON_AddItemToObject(current, "child", next);
            current = next;

            // Добавляем данные из фаззера
            char key[2] = {(char)data[i % size], '\0'};
            cJSON_AddNumberToObject(current, key, (double)data[i % size]);
        } else {
            break;
        }
    }
    if (deep) {
        cJSON_Delete(deep);
    }
}

// Дополнительная функция для тестирования конкретных уязвимостей
void test_known_vulnerabilities() {
    // Тест для уязвимости в cJSON_SetValuestring
    cJSON *obj = cJSON_CreateString("test");
    if (obj) {
        // Симулируем поврежденное состояние
        obj->valuestring = NULL;
        cJSON_SetValuestring(obj, "new_value");
        cJSON_Delete(obj);
    }

    // Тест для уязвимостей с числами
    double nan_val = 0.0 / 0.0;
    cJSON *num = cJSON_CreateNumber(nan_val);
    if (num) {
        char *printed = cJSON_Print(num);
        if (printed) {
            cJSON_free(printed);
        }
        cJSON_Delete(num);
    }
}

// Простая функция для ручного тестирования
// int main() {
//     // Простой тест
//     const char *test_json = "{\"test\": \"value\", \"array\": [1, 2, 3]}";
//     LLVMFuzzerTestOneInput((const uint8_t*)test_json, strlen(test_json));
//
//     // Тест известных уязвимостей
//     test_known_vulnerabilities();
//
//     return 0;
// }
























// #include <stdint.h>
// #include <stddef.h>
// #include <stdlib.h>
// #include <string.h>
// #include "cJSON.h"
//
// // 1. Разбор входных данных JSON через cJSON_ParseWithLength
// int LLVMFuzzerTestOneInput(const uint8_t *data, size_t size) {
//     // Парсим JSON из буфера `data` длиной `size`
//     cJSON *json = cJSON_ParseWithLength((const char*)data, size);
//     if (json == NULL) {
//         // Если разбор не удался, выходим (ошибки парсинга ожидаемы для нерелевантных входов)
//         return 0;
//     }
//
//     // 2. Если парсинг успешен, выполняем расширенные операции над полученным JSON:
//
//     // 2a. Печать JSON-данных без форматирования (строкой)
//     char *jsonStr = cJSON_PrintUnformatted(json);
//     if (jsonStr != NULL) {
//         // Освобождаем память, выделенную под строку JSON
//         free(jsonStr);
//     }
//
//     // 2b. Установка нового строкового значения случайному узлу JSON-дерева
//     cJSON *targetNode = json;
//     if (json->child != NULL) {
//         // Если корневой элемент имеет дочерние узлы (объект или массив), выберем один из них
//         int childCount = 0;
//         for (cJSON *child = json->child; child != NULL; child = child->next) {
//             childCount++;
//         }
//         // Вычисляем индекс случайного дочернего узла на основе данных ввода (для детерминизма)
//         int index = 0;
//         if (size > 0) {
//             index = data[0] % childCount;
//         }
//         // Итерируем до выбранного индекса
//         targetNode = json->child;
//         for (int i = 0; i < index && targetNode->next != NULL; ++i) {
//             targetNode = targetNode->next;
//         }
//     }
//     // Задаём этому узлу новое строковое значение
//     cJSON_SetValuestring(targetNode, "fuzz");
//
//     // 2c. Дублирование всего JSON-объекта (глубокое копирование дерева)
//     cJSON *dup = cJSON_Duplicate(json, 1);  // второй параметр '1' означает рекурсивное копирование всех детей
//
//     // 2d. Удаление случайного элемента из JSON-объекта (если корень является объектом)
//     if (cJSON_IsObject(json)) {
//         // Подсчитываем количество полей в объекте
//         int childCount = 0;
//         for (cJSON *child = json->child; child != NULL; child = child->next) {
//             childCount++;
//         }
//         if (childCount > 0) {
//             // Выбираем случайный индекс поля для удаления (на основе второго байта данных ввода при наличии)
//             int index = 0;
//             if (size > 1) {
//                 index = data[1] % childCount;
//             }
//             // Находим соответствующий элемент с данным индексом
//             cJSON *child = json->child;
//             for (int i = 0; i < index && child->next != NULL; ++i) {
//                 child = child->next;
//             }
//             if (child != NULL && child->string != NULL) {
//                 // Удаляем выбранный элемент по его ключу (имени) из объекта (с учётом регистра ключа)
//                 cJSON_DeleteItemFromObjectCaseSensitive(json, child->string);
//             }
//         }
//     }
//
//     // 3. Освобождение всех выделенных ресурсов
//     cJSON_Delete(json);       // удаляем исходное JSON-дерево
//     if (dup != NULL) {
//         cJSON_Delete(dup);    // удаляем дубликат JSON-дерева (если был успешно создан)
//     }
//     return 0;
// }