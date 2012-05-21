#---------------------------------------------------
LDFLAGS	= `root-config --cflags --glibs`
CMP	= gcc
CFLAGS	= -w -Wno-deprecated -O2
LIBS	= -lThread -lftd2xx
HDRS	=  ./h
SRCS	= ./src/*.cpp
OUT	= ndaq_gui

all: build

build:
		@echo "Compiling..."
		@$(CMP) $(CFLAGS) -o $(OUT) $(SRCS) -I $(HDRS) $(LIBS) $(LDFLAGS)
		@echo "Done!"

dict:;
		@echo "Generating dictionary..."
		@cd ./h; \
		rootcint -f r_Dict.cpp -c main.h Linkdef.h
		@mv ./h/r_Dict.cpp ./src
		@echo "Done!"
#---------------------------------------------------
