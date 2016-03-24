msg_s:
	g++ -o syncClient OpelHeader.cc OpelBT.cc TempSock.cc sync_client.cc -lbluetooth
msg_c:
	g++ -o syncServer OpelHeader.cc OpelBT.cc TempSock.cc sync_server.cc -lbluetooth

.PHONY: clean

clean:
	rm ./syncServer ./syncClient
