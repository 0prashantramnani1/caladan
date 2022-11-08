import sys
import subprocess
import socket
import time 

control_plane_port = int(sys.argv[1])

#Creating the socket
sock = socket.socket(socket.AF_INET, socket.SOCK_STREAM)

#Binding and listening to all interfaces
serv_addr = ("0.0.0.0", control_plane_port)
sock.bind(serv_addr)

sock.listen()
conn, addr = sock.accept()

#./tests/netperf_server server.config 1 10
command      = "tests/netperf_server"
config       = "server.config"
nthreads     = "1"
payload_size = 10 

while True:
    data_from_client = conn.recv(1024)
    process = subprocess.Popen([command, config, nthreads, str(payload_size)])
    data = "started"
    conn.send(data.encode())
    print("waiting to receive")
    data_from_client = conn.recv(1024)
    kill = subprocess.Popen(["kill", str(process.pid)])

    data = "killed"
    conn.send(data.encode())
#conn.send(data_from_client)
sock.close()

