from pykeyboard import PyKeyboard
import bluetooth

uuid="4CC4A570-3212-11E3-BBA6-AC1945E34500"
k = PyKeyboard()

def runServer():
	serverSocket = bluetooth.BluetoothSocket( bluetooth.RFCOMM )
	port = bluetooth.PORT_ANY
	serverSocket.bind(("", port))
	print "Listening for connections on port: ", port   
	serverSocket.listen(1)
	port=serverSocket.getsockname()[1]

    #the missing piece
	bluetooth.advertise_service( serverSocket, "SampleServer",
					   service_id = uuid,
					   service_classes = [ uuid, bluetooth.SERIAL_PORT_CLASS ],
					   profiles = [ bluetooth.SERIAL_PORT_PROFILE ] 
						)

	inputSocket, address=serverSocket.accept()
	print "Got connection with" , address

	last_data = 0.0
	while True:
		data = inputSocket.recv(5)
		if float(data) > 1.29:
			k.tap_key("a")
		elif float(data) < -1.29:
			k.tap_key("d")


		print "received [%s] \n " % data

	inputSocket.close()
	serverSocket.close()  

runServer()

