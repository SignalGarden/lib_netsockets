# usage : 
# make -f makefile
# make -f makefile clean

dir_bin = .

SRC1 = socket.cc
SRC2 = http.cc
SRC3 = ftp.cc
SRC_MAIN1 = tcp_client_01.cc
SRC_MAIN2 = tcp_server_01.cc
SRC_MAIN3 = http_client.cc

OBJ1 = socket.o
OBJ2 = http.o
OBJ3 = ftp.o
OBJ_MAIN1 = tcp_client_01.o
OBJ_MAIN2 = tcp_server_01.o
OBJ_MAIN3 = http_client.o

EXE1 = $(dir_bin)/tcp_client_01
EXE2 = $(dir_bin)/tcp_server_01
EXE3 = $(dir_bin)/http_client

OBJs = $(OBJ1) $(OBJ2) $(OBJ3) $(OBJ_MAIN1) $(OBJ_MAIN2) $(OBJ_MAIN3)
EXEs = $(EXE1) $(EXE2) $(EXE3)

CXX = g++
CXX_FLAG = -g -Wall -std=c++0x
LINK_FLAG = -lstdc++ 

all:  $(EXEs) 

test : run1 

$(EXE1) : $(OBJ1) $(OBJ2) $(OBJ3) $(OBJ_MAIN1)
	$(CXX) $(LINK_FLAG) -o $(EXE1) $(OBJ1) $(OBJ2) $(OBJ3) $(OBJ_MAIN1) $(LIB)

$(EXE2) : $(OBJ1) $(OBJ2) $(OBJ3) $(OBJ_MAIN2) 
	$(CXX) $(LINK_FLAG) -o $(EXE2) $(OBJ1) $(OBJ2) $(OBJ3) $(OBJ_MAIN2) $(LIB)

$(EXE3) : $(OBJ1) $(OBJ2) $(OBJ3) $(OBJ_MAIN3) 
	$(CXX) $(LINK_FLAG) -o $(EXE3) $(OBJ1) $(OBJ2) $(OBJ3) $(OBJ_MAIN3) $(LIB)

$(OBJ1) : $(SRC1)
	$(CXX) $(INC) $(CXX_FLAG) -c $(SRC1) -o $(OBJ1)

$(OBJ2) : $(SRC2)
	$(CXX) $(INC) $(CXX_FLAG) -c $(SRC2) -o $(OBJ2)

$(OBJ3) : $(SRC3)
	$(CXX) $(INC) $(CXX_FLAG) -c $(SRC3) -o $(OBJ3)

$(OBJ_MAIN1) : $(SRC_MAIN1)
	$(CXX) $(INC) $(CXX_FLAG) -c $(SRC_MAIN1) -o $(OBJ_MAIN1)

$(OBJ_MAIN2) : $(SRC_MAIN2)
	$(CXX) $(INC) $(CXX_FLAG) -c $(SRC_MAIN2) -o $(OBJ_MAIN2)

$(OBJ_MAIN3) : $(SRC_MAIN3)
	$(CXX) $(INC) $(CXX_FLAG) -c $(SRC_MAIN3) -o $(OBJ_MAIN3)

clean:
	rm -f $(OBJs) $(EXEs)



