# usage : 
# make -f makefile
# make -f makefile clean

dir_bin = .

SRC1 = socket.cc
SRC2 = tcp_client_01.cc
SRC3 = tcp_server_01.cc

OBJ1 = socket.o
OBJ2 = tcp_client_01.o
OBJ3 = tcp_server_01.o

EXE1 = $(dir_bin)/tcp_client_01
EXE2 = $(dir_bin)/tcp_server_01

OBJs = $(OBJ1) $(OBJ2) $(OBJ3)
EXEs = $(EXE1) $(EXE2)

CXX = g++
CXX_FLAG = -g -Wall -std=c++0x
LINK_FLAG = -lstdc++ 

all:  $(EXEs) 

test : run1 

$(EXE1) : $(OBJ1) $(OBJ2)  
	$(CXX) $(LINK_FLAG) -o $(EXE1) $(OBJ1) $(OBJ2) $(LIB)

$(EXE2) : $(OBJ1) $(OBJ3)  
	$(CXX) $(LINK_FLAG) -o $(EXE2) $(OBJ1) $(OBJ3) $(LIB)

$(OBJ1) : $(SRC1)
	$(CXX) $(INC) $(CXX_FLAG) -c $(SRC1) -o $(OBJ1)

$(OBJ2) : $(SRC2)
	$(CXX) $(INC) $(CXX_FLAG) -c $(SRC2) -o $(OBJ2)

$(OBJ3) : $(SRC3)
	$(CXX) $(INC) $(CXX_FLAG) -c $(SRC3) -o $(OBJ3)

clean:
	rm -f $(OBJs) $(EXEs)



