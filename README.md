Конспект проекта по фаззинг-тестированию библиотеки cJSON (Linux Manjaro)
Цель проекта

Цель проекта – проверить надёжность и безопасность библиотеки cJSON при обработке некорректных или случайных входных данных методом фаззинг-тестирования. Это позволяет выявить потенциальные ошибки и уязвимости, которые не обнаруживаются стандартными тестами.

Инструменты и технологии

Clang – компилятор C/C++ с поддержкой инструментария фаззинга (LLVM libFuzzer) и санитайзеров.

CMake – система автоматизации сборки, использована для конфигурации и сборки проекта.

libFuzzer – встроенный в Clang механизм coverage-guided фаззинг-тестирования библиотек.

CLion – интегрированная среда разработки (IDE) для C/C++, использована для разработки и отладки кода.

Git – система контроля версий, применялась для управления исходным кодом и сохранения историй изменений проекта.

Этапы работы

Установка среды – Настройка окружения на Linux Manjaro. Установлены необходимые инструменты: Clang, CMake и связанные зависимости. Проект cJSON склонирован из репозитория с помощью Git. Среда разработки CLion настроена для работы с исходным кодом.

Сборка библиотеки с фаззинг-флагами – Библиотека cJSON скомпилирована с использованием Clang и CMake, с включением специальных флагов для фаззинга. В процесс сборки интегрированы AddressSanitizer и coverage-инструментирование, что позволяет обнаруживать ошибки памяти. В результате получена отладочная сборка cJSON, готовая для fuzz-тестирования.

Разработка harness-функции – Написана специальная функция-харнес (входная точка для фаззера), которая принимает сгенерированные libFuzzer входные данные. Внутри harness-функции эти данные интерпретируются как JSON-строка и передаются в парсер библиотеки (например, вызывается cJSON_Parse). После обработки результата выполняется очистка (освобождение выделенной памяти) для корректного учета возможных утечек.

Запуск фаззинга – Скомпонованное fuzz-исполняемое приложение запущено для непрерывного тестирования. LibFuzzer генерирует множество случайных и граничных вариантов JSON-данных, передавая их в harness-функцию. Во время выполнения фаззинга осуществляется мониторинг с помощью AddressSanitizer – автоматически отслеживаются аварийные сбои, утечки памяти и другие аномалии. Тесты выполнялись до исчерпания новых путей или появления сбоев.

Фиксация результатов – По ходу фаззинг-тестирования результаты автоматически протоколировались. При обнаружении ошибочных ситуаций сохранялись отчёты (стектрейсы, сообщения AddressSanitizer) и входные данные, вызвавшие сбой, в отдельные файлы для последующего анализа. Все полученные существенные результаты и наблюдения были зафиксированы в отчётных документах и в репозитории Git (в виде issues или заметок по изменениям).

Общие итоги

Фаззинг-тестирование подтвердило корректность и устойчивость библиотеки cJSON под нагрузкой некорректных данных. За время тестирования не выявлено критических сбоев или утечек памяти, что свидетельствует о высокой надёжности парсера JSON. Проект продемонстрировал эффективность использования связки Clang + libFuzzer + AddressSanitizer для автоматизированного поиска дефектов. Методика фаззинга рекомендовалась к интеграции в процесс разработки и тестирования компонентов на C/C++ для раннего обнаружения потенциальных уязвимостей и повышения общего качества программного обеспечения.

cJSON Library Fuzz Testing Project Summary (Linux Manjaro)
Project Goal

The goal of this project was to assess the reliability and security of the cJSON library by means of fuzz testing on a Linux Manjaro platform. Fuzzing (automated random input testing) was used to expose potential bugs or vulnerabilities in cJSON’s JSON parsing that would not be caught by standard testing techniques.

Tools and Technologies

Clang – C/C++ compiler with support for LLVM libFuzzer and sanitizers (used for instrumenting the code for fuzz testing).

CMake – Build automation system used to configure and generate build files for the project.

libFuzzer – In-process, coverage-guided fuzzing engine (part of LLVM) used to generate inputs and drive the fuzz testing of the library.

CLion – Integrated Development Environment for C/C++ (used for code development and debugging convenience).

Git – Version control system used to manage the source code and track project changes.

Work Stages

Environment Setup – Prepared the Linux Manjaro environment and installed all necessary tools (Clang, CMake, etc.). The cJSON library source code was cloned from its Git repository. The CLion IDE was configured for the project to facilitate development and configuration.

Building cJSON with Fuzzing Flags – Compiled the cJSON library from source using Clang and CMake with appropriate compiler flags for fuzzing. The build enabled AddressSanitizer and coverage instrumentation to catch memory errors and support libFuzzer. This resulted in a special instrumented build of cJSON ready for fuzz testing.

Writing the Fuzz Harness – Implemented a custom fuzzing harness function (the libFuzzer entry point) to feed random input data into cJSON’s parser. The harness takes the fuzzer-generated byte data, interprets it as a JSON string, and calls the cJSON parse function (e.g. cJSON_Parse). After parsing, proper cleanup (e.g. deallocating any created JSON objects) is performed to avoid false positives from memory leaks.

Executing the Fuzzing Process – Ran the compiled fuzzing binary to begin continuous testing. libFuzzer generated a wide range of random and edge-case JSON inputs and passed them to the harness in a tight loop. Throughout execution, AddressSanitizer monitored the program for any crashes, memory allocation errors, or undefined behaviors. The fuzzer was allowed to run until no new coverage was being found or until a failure was observed.

Recording Results – Results were logged and collected throughout the fuzzing run. In case of any abnormal termination or error detection, detailed AddressSanitizer reports (stack traces and error descriptions) were automatically captured. Any input that caused a crash or issue was saved for analysis and reproduction. All significant findings and observations were documented in project reports and noted in the Git repository (for example, via issue tracking or commit notes).

Overall Results

The fuzz testing project demonstrated the robustness of the cJSON library against malformed or random inputs. No critical crashes or memory leaks were observed during the testing period, indicating a high level of reliability in cJSON’s JSON parsing implementation. This project highlighted the effectiveness of using Clang’s libFuzzer with AddressSanitizer for uncovering potential defects in C libraries. It is recommended to integrate fuzz testing into the regular development and quality assurance process for C/C++ components to ensure early detection of vulnerabilities and to further improve software quality.

GitHub Repository Description (English):
Fuzz testing project for the cJSON library (a lightweight C JSON parser) on Linux Manjaro using Clang’s libFuzzer and AddressSanitizer. The project involves building cJSON with fuzzing instrumentation, implementing a custom fuzz harness, and running continuous fuzzing to detect potential memory errors. Technologies: Clang, CMake, libFuzzer, CLion, Git.
