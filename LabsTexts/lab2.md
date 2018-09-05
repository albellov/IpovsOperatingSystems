# Лабораторная работа №2

## Преподготовка

Для начала необходимо сделать fork [2ой лабораторной работы](https://github.com/IpovsOperatingSystems/Lab2). Выполнить update.sh, как в лабораторной работе 0. А также импортировать проект в Codenvy.

## Задание 1

### Необходимые знания

1. Как менять занчения переменных внутри функций в Си.

### Задание
В папке swap лежит 3 файла: swap.c, swap.h и main.c. Ваша задача закончить метод Swap в swap.c, так, чтобы он менял местами два символа. Скомпилировать программу. Если вы все сделали верно, то программа, которую вы собрали выведет "b a".

### Ресурсы

1. [Как компилировать простые программы с gcc (заменить сс на gcc).](https://www.classes.cs.uchicago.edu/archive/2014/winter/51081-1/LabFAQ/lab3/compile.html)

## Задание 2

### Необходимые знания

1. Выделение и освобождение памяти в куче Си.
2. В чем разница между стеком и кучей (прямо в задании не потребуется, но я спрошу).
3. Использование аргументов командной строки


### Задание

В папке revert\_string содержатся файлы main.c, revert\_string.h, revert\_string.c. Вам необходимо реализовать метод RevertString в revert_string.c, который должен переворачивать данную пользователем строку.
Изучить код main.c, скомпилировать программу, рассказать, как она работает и, что делает.

### Ресурсы

1. [Кратко про выделение/освобождение памяти и указатели.](https://computer.howstuffworks.com/c29.htm)
2. [Разница между стеком и кучей.](https://stackoverflow.com/questions/79923/what-and-where-are-the-stack-and-heap)
3. [Кратко про аргументы командной строки от создателей gcc.](https://www.gnu.org/software/libc/manual/html_node/Program-Arguments.html)
4. [Пример с аргументами командной строки.](https://www.cprogramming.com/tutorial/c/lesson14.html)

## Задание 3

### Необходимые знания

1. Основы работы компилятора: препроцессор, компилятор, линковщик. Их роли и порядок работы.
2. Что такое статическая и динамическая линковка. В чем разница?
3. Опции компилятора: `-I, -L, -l, -shared, -o, -с, -fPIC`
4. Утилита ar.
5. Переменная окружения `LD_LIBRARY_PATH`


### Задание

В **задании 2**, вы написали маленькую библиотеку с одной функцией, переворота строки. Тем не менее этот код уже можно переиспользовать, а чтобы это было удобнее делать, его необходимо вынести в библиотеку. Ваше задание скомпилировать статическую и динамическую библиотеки с RevertString и залинковать их в приложения с main.c.

***Получится две программы, первая будет использовать статическую билиотеку, а вторая динамическую.*** 

### Ресурсы

1. [Как создавать статические и динамические библиотеки и линковаться на них.](https://renenyffenegger.ch/notes/development/languages/C-C-plus-plus/GCC/create-libraries/index)

## Задание 4

### Необходимые знания

3. Переменная окружения `LD_LIBRARY_PATH`

### Задание

Если вы зайдете в update.sh, то увидите, слежующую строчку `sudo apt -y install libcunit1 libcunit1-doc libcunit1-dev`. Это установка в вашу систему библиотеки для юнит тестирования на языке Си - CUnit. В папочке test вы найдете tests.c - готовую программу, которая запускает несколько юнит тестов для функции RevertString. Ваша задача, скомпилировать эту программу с динамической библиотекой из **Задания 3**. Важно, что программа с тестами и программа из **Задания 2** должны использовать одну библиотеку (один файл). 

P.S. Разумеется, тесты должны проходить :)

```

CUnit - A unit testing framework for C - Version 2.1-3
http://cunit.sourceforge.net/


Suite: Suite
  Test: test of RevertString function ...passed

Run Summary:    Type  Total    Ran Passed Failed Inactive
              suites      1      1    n/a      0        0
               tests      1      1      1      0        0
             asserts      4      4      4      0      n/a

Elapsed time =    0.000 seconds
```


### Ресурсы

1. [Как cлинковаться с CUnit.](https://mysnippets443.wordpress.com/2015/03/07/ubuntu-install-cunit/)

## Перед тем, как сдавать

Залейте ваш код в ваш репозиторий на GitHub. Убедитесь, что вы не добавляете в репозиторий бинарные файлы (программы, утилиты, библиотеки и т.д.).

<!--
## Задание 5* (Необязательно, за допополнительные баллы)

###Необходимые знания

1. Как пользоваться интернетом.

###Задание

В папке bin лежит динамическая библиотека libsimple.so. С помощью утилить `objdump` определить, какие функции она содержит.

###Ресурсы

1. [Аргументы коммандной строки для objdump](https://sourceware.org/binutils/docs/binutils/objdump.html)
-->

 