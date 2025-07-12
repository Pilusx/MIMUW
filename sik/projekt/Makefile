AR = ar
CC = g++
CXXFLAGS=-std=c++17 -Wall -Wextra -O2

LOGFLAGS+=-DLOG_UDP # namespace udp (level UDP)
LOGFLAGS+=-DLOG_TCP # namespace tcp (level TCP)
LOGFLAGS+=-DLOG_TTY -DLOG_CLIENT # client (level CLIENT)
LOGFLAGS+=-DLOG_SERVER # server (level SERVER)
LOGFLAGS+=-DLOG_THREADS # threads (level BASE)
LOGFLAGS+=-DLOG_CNODES # lient node (level NODES)

#CXXFLAGS+=${LOGFLAGS}

OBJ_DIR=static
LIBS_OTHER=-lboost_filesystem -lboost_program_options -lboost_regex -lboost_system -lpthread
BASE_OBJS=base/Net.o base/Threads.o base/Mem.o proto/Encoder.o proto/Info.o proto/Buffer.o proto/Split.o proto/Validate.o
TCP_OBJS=${BASE_OBJS} tcp/client.o tcp/server.o
TESTS_OBJS= ${BASE_OBJS} main/test_split.o
OBJS=${TCP_OBJS} fs/fs.o fs/index.o udp/join.o udp/socket.o
SERVER_OBJS= ${OBJS} nodes/ServerWorker.o main/server.o
CLIENT_OBJS= ${OBJS} nodes/TTYNode.o nodes/ClientWorker.o main/client.o

FILTER=--filter=-legal/copyright,-build/include_order,-whitespace,-runtime/references,-build/c++11,-runtime/threadsafe

all: netstore-client netstore-server

${OBJ_DIR}/%.o: %.cc
	@echo "Compiling file $<" 
	@mkdir -p $(dir $@)
	$(CC) -c $(CXXFLAGS) $< -o $@

netstore-server: ${foreach f,${SERVER_OBJS},${OBJ_DIR}/$f}
	@echo "Building server"
	$(CC) ${foreach f,${SERVER_OBJS},${OBJ_DIR}/$f} $(LIBS_OTHER) -o netstore-server 

netstore-client: ${foreach f,${CLIENT_OBJS},${OBJ_DIR}/$f}
	@echo "Building client"
	$(CC) ${foreach f,${CLIENT_OBJS},${OBJ_DIR}/$f} $(LIBS_OTHER) -o netstore-client

split-test: ${foreach f,${TEST_OBJS},${OBJ_DIR}/$f}
	@echo "Building split-test"
	$(CC) ${foreach f,${TESTS_OBJS},${OBJ_DIR}/$f} $(LIBS_OTHER) -o split-test


tcp-client-recv: ${TCP_OBJS} tcp-test/tcp-client-recv.o
	@echo "Building tcp-client-recv"
	${CC} ${foreach f,${TCP_OBJS}, ${OBJ_DIR}/$f} ${OBJ_DIR}/tcp-test/tcp-client-recv.o ${LIBS_OTHER} -o tcp-client-recv

tcp-client-send: ${TCP_OBJS} tcp-test/tcp-client-send.o
	@echo "Building tcp-client-send"
	${CC} ${foreach f,${TCP_OBJS}, ${OBJ_DIR}/$f} ${OBJ_DIR}/tcp-test/tcp-client-send.o ${LIBS_OTHER} -o tcp-client-send

tcp-server-recv: ${TCP_OBJS} tcp-test/tcp-server-recv.o
	@echo "Building tcp-test"
	${CC} ${foreach f,${TCP_OBJS}, ${OBJ_DIR}/$f} ${OBJ_DIR}/tcp-test/tcp-server-recv.o ${LIBS_OTHER} -o tcp-server-recv

tcp-server-send: ${TCP_OBJS} tcp-test/tcp-server-send.o
	@echo "Building tcp-test"
	${CC} ${foreach f,${TCP_OBJS}, ${OBJ_DIR}/$f} ${OBJ_DIR}/tcp-test/tcp-server-send.o ${LIBS_OTHER} -o tcp-server-send

tcp-test: tcp-client-recv tcp-client-send tcp-server-recv tcp-server-send

lint:
	clang-format -i -sort-includes **/*.hpp **/*.cc
	cpplint --recursive --extensions=hpp,cc $(FILTER) .

doc:
	doxygen Doxyfile

clean:
	@echo "Removing server"
	rm -f netstore-server 
	@echo "Removing client"
	rm -f netstore-client
	@echo "Removing split-test"
	rm -f split-test
	@echo "Removing temp"
	rm -f *~ */*~
	@echo "Removing doc"
	rm -rf html
	@echo "Removing tcp-test"
	rm -f tcp-client-recv tcp-client-send tcp-server-recv tcp-server-send
	@echo "Removing static"
	rm -rf $(OBJ_DIR)
