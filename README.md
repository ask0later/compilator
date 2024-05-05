# Компилятор

### Предисловие

Этот проект является продолжением моего прошлого [проекта](https://github.com/ask0later/language.git).

Текстовый файл с кодом на моём языке преобразовывался в `AST-дерева`, а затем в инструкции `ассемблера`, написанного мной ранее, и исполнялся на эмуляторе процессора.

Сейчас же на вход будет подаваться уже сгенерированное AST-дерево, на выходе должна быть исполняемая программа определенного формата, который поддерживается компьютером.


## Проект делится на две части:
1. Из формата `AST-дерева` (Abstract Syntax Tree) перейти к собственному формату `IR` (Intermediate Representation).
2. Из IR-формата перейти к исполняемому рабочему ELF-файлу.


## AST -> IR

IR-инструкция приближена к ассесблерной инструкцией. 