all: mavlink_serial send_quad_commands mavlink_socket send_socket_commands

mavlink_serial: mavlink_serial.o
	g++ mavlink_serial.o -o mavlink_serial

mavlink_serial.o: mavlink_serial.cpp
	g++ -I mavlink/include/mavlink/v1.0 -c mavlink_serial.cpp
	
mavlink_socket: mavlink_socket.o
	g++ mavlink_socket.o -o mavlink_socket

mavlink_socket.o: mavlink_socket.cpp
	g++ -I mavlink/include/mavlink/v1.0 -c mavlink_socket.cpp

send_quad_commands: send_quad_commands.o
	g++ send_quad_commands.o -o send_quad_commands

send_quad_commands.o: send_quad_commands.cpp
	g++ -I mavlink/include/mavlink/v1.0 -c send_quad_commands.cpp
	
send_socket_commands: send_socket_commands.o
	g++ send_socket_commands.o -o send_socket_commands

send_socket_commands.o: send_socket_commands.cpp
	g++ -I mavlink/include/mavlink/v1.0 -c send_socket_commands.cpp


clean:
	rm -rf *o mavlink_serial send_quad_commands mavlink_socket send_socket_commands