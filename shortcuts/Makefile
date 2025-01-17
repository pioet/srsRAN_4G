.PHONY= all clean

all:
	@echo "This is makefile for running srsran_4g"

epc:
	sudo ../build/srsepc/src/srsepc

enb:
	../build/srsenb/src/srsenb --rf.device_name=zmq --rf.device_args="fail_on_disconnect=true,tx_port=tcp://*:2000,rx_port=tcp://localhost:2001,id=enb,base_srate=23.04e6"

ue:
	sudo ../build/srsue/src/srsue --rf.device_name=zmq --rf.device_args="tx_port=tcp://*:2001,rx_port=tcp://localhost:2000,id=ue,base_srate=23.04e6" --gw.netns=ue1 --log.all_level=debug --log.filename=./ue1.log

clean:
	rm -f *.log

compile:
	@cd ../build && make -j8

install:
	@cd ../build && sudo make install