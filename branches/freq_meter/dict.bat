del .\h\r_Dict.h
del .\src\r_Dict.cpp

cd .\h

rootcint -f ..\src\r_Dict.cpp -c main.h Linkdef.h

move ..\src\r_Dict.h .

cd ..