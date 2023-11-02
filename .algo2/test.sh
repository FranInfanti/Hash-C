#!/bin/bash
#
PREFIX=".algo2"

if [ -f "banner.txt" ] ; then
    cat $PREFIX/banner.txt
    echo ""
fi

echo "Linter"
echo "======"
echo ""

curl -ss "https://raw.githubusercontent.com/torvalds/linux/master/.clang-format" -o .clang-format
clang-format --style=file --Werror --dry-run $( find . \( -iname '*.h' -o -iname '*.c' \) ) > /dev/null 2>&1

LINTER_RESULT=$?

if [ $LINTER_RESULT -ne 0 ]; then
    echo -e "\033[1;31m¡ERROR CORRIENDO CLANG:FORMAT! Revisa tu entrega, el estilo de código no fue respetado\033[0m\n"
else
    echo -e "\033[1;32mLINTER OK\033[0m\n"
fi


echo "Compilación y pruebas"
echo "=====================\n"

make -f makefile clean 2>&1
timeout 30s make -f makefile valgrind-chanutron 2>&1

RETVAL=$?
#Ojo con esto. Pa2mmf devuelve 0 para éxito, 2 para error, Pa2mm devuelve 0 para éxito y >0 para error. Si fallan justo 124 pruebas, va a salir como timeout. Creo.
if [ $RETVAL -eq 2 ]; then
    echo ""
    echo -e "\033[1;31m¡ERROR CORRIENDO VALGRIND! Revisa tu entrega, las pruebas de la CÁTEDRA no pasan.\033[0m"
    echo ""
    exit 1
fi

if [ $RETVAL -eq 124 ]; then
    echo ""
    echo -e "\033[1;31m¡TIMEOUT! Revisa tu entrega, las pruebas de la CÁTEDRA no pasan.\033[0m"
    echo ""
    exit 2;
fi

echo ""
echo "Parece que tu TP pasa todas las pruebas de la cátedra... Pasemos ahora a las tuyas."
echo ""


make -f makefile clean 2>&1
timeout 30s make -f makefile valgrind-alumno 2>&1

RETVAL=$?
#Ojo con esto. Pa2mmf devuelve 0 para éxito, 2 para error, Pa2mm devuelve 0 para éxito y >0 para error. Si fallan justo 124 pruebas, va a salir como timeout. Creo.
if [ $RETVAL -eq 2 ]; then
    echo ""
    echo -e "\033[1;31m¡ERROR CORRIENDO VALGRIND! Revisa tu entrega, tus pruebas no pasan.\033[0m"
    echo ""
    exit 1
fi

if [ $RETVAL -eq 124 ]; then
    echo ""
    echo -e "\033[1;31m¡TIMEOUT! Revisa tu entrega, tus pruebas no pasan.\033[0m"
    echo ""
    exit 2;
fi

echo ""
echo "Parece que tu TP pasa tus pruebas también."
echo ""
echo ""
echo "TODAS LAS PRUEBAS SE EJECUTARON CON ÉXITO, ¡FELICITACIONES!"
echo ""
echo ""

if [ $LINTER_RESULT -ne 0 ]; then
    echo -e "\033[1;31m¡PERO EL FORMATO NO ES CORRECTO! Revisa tu entrega, el estilo de código no fue respetado\033[0m\n"
    exit 1
else
    cat $PREFIX/success.txt
fi

exit 0
