PWD = $(shell pwd)
FIFO_DIR = $(PWD)/fifo
MQUEUE_DIR = $(PWD)/mqueue
SHM_DIR = $(PWD)/shm
SOCKET_DIR = $(PWD)/socket

default: $(FIFO_DIR)/fifo $(MQUEUE_DIR)/mqueue $(SHM_DIR)/shm $(SOCKET_DIR)/socket

$(FIFO_DIR)/fifo:
	cd $(FIFO_DIR); make

$(MQUEUE_DIR)/mqueue:
	cd $(MQUEUE_DIR); make

$(SHM_DIR)/shm:
	cd $(SHM_DIR); make

$(SOCKET_DIR)/socket:
	cd $(SOCKET_DIR); make

clean:
	cd $(FIFO_DIR); make clean
	cd $(MQUEUE_DIR); make clean
	cd $(SHM_DIR); make clean
	cd $(SOCKET_DIR); make clean
	rm -f *.csv

