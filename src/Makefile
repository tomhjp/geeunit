OPENGL_LIBS = -lglut -lGL -lGLU

# returns g++
CXX = $(shell wx-config --cxx)

SRC = logsim.cc names.cc scanner.cc network.cc parser.cc monitor.cc devices.cc userint.cc gui.cc guitest.cc

L_OBJECTS = logsim.o names.o scanner.o network.o parser.o monitor.o devices.o userint.o gui.o error.o device.o warning.o

G_OBJECTS = guitest.o names.o network.o monitor.o devices.o gui.o

PT_OBJECTS = names.o network.o monitor.o devices.o parser.o error.o warning.o parsertest.o device.o scanner.o

NT_OBJECTS = namestest.o names.o

ST_OBJECTS = scannertest.o scanner.o names.o

# implementation

.SUFFIXES:	.o .cc

.cc.o :
	$(CXX) -c `wx-config --cxxflags` -g -o $@ $<

all:    logsim guitest

logsim:	$(L_OBJECTS)
	$(CXX) -o logsim $(L_OBJECTS) `wx-config --libs --gl_libs` $(OPENGL_LIBS)

guitest: $(G_OBJECTS)
	$(CXX) -o guitest $(G_OBJECTS) `wx-config --libs --gl_libs` $(OPENGL_LIBS)

parsertest: $(PT_OBJECTS)
	$(CXX) -o parsertest $(PT_OBJECTS)

namestest: $(NT_OBJECTS)
	$(CXX) -o namestest $(NT_OBJECTS)

scannertest: $(ST_OBJECTS)
	$(CXX) -o scannertest $(ST_OBJECTS)

clean:
	rm -f *.o logsim guitest

depend:
	makedepend $(SRC)

# DO NOT DELETE
# additional dependencies
logsim.o: logsim.h names.h devices.h network.h monitor.h parser.h userint.h error.h
logsim.o: gui.h
names.o: names.h
network.o: network.h names.h
parser.o: parser.h names.h network.h devices.h monitor.h
monitor.o: monitor.h names.h network.h devices.h
devices.o: devices.h names.h network.h
userint.o: userint.h names.h network.h devices.h monitor.h
gui.o: gui.h names.h devices.h network.h monitor.h
guitest.o: guitest.h names.h devices.h network.h monitor.h gui.h
namestest.o: namestest.h names.h
scannertest.o: scannertest.h scanner.h names.h
parsertest.o: names.h devices.h monitor.h network.h typedefs.h scanner.h parser.h error.h
parser.o: scanner.h parser.h error.h warning.h names.h network.h devices.h device.h
error.o: error.h names.h
device.o: device.h
scanner.o: scanner.h
