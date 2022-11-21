import sys
import subprocess
import time as tm
import socket

serv_ip            = "128.110.218.186"
control_plane_port = int(sys.argv[1]) 

sock = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
try:
    sock.connect((serv_ip, control_plane_port))
except:
    print("Connection to server failed")
    exit()




#/./tests/netperf_client client.config 100 1 128.110.218.186 5 10 1
#[config, nflows, nthreads, server_ip, time, payload, depth]
command   = "tests/netperf_client"
config    = "client.config"
#nflow     = "10"
nthreads  = "1"
server_ip = "128.110.218.186"
time      = "10"
payload   = "10"
depth     = "1"
tm.sleep(1)
encoding = 'utf-8'

payload_size = 64
num_flows = 10

file1 = open("test_final.txt", "w")
file1.write("Threads: {} ------ Time: {} ------ Depth: {}\n\n\n\n".format(nthreads, time, depth))

#for payload_size in [64, 4096, 8192]:
while payload_size < 32000:
    file1.write("payload_size: {}\n".format(payload_size))
    num_flows    = 10
    while num_flows < 151:
    #for num_flows in [10, 50 ,150]:
        avg = 0
        i = 0
        while i < 3:
            print("Iteration round: {}\n".format(i))
            try:
                data = "start"
                sock.send(data.encode())
                print("waiting to receive")
                data_from_server = sock.recv(1024)
                print("received")
                tm.sleep(1)
                process = subprocess.check_output([command, config, str(num_flows), nthreads, server_ip, time, str(payload_size), depth])
                output = process.decode(encoding)
            except subprocess.CalledProcessError:
                file1.write("SEG FAULT!!!!!!!!!!\n")
                output = ""
            for line in output.splitlines():
                if "Through" in line:
                    a = [int(s) for s in line.split() if s.isdigit()][0]
                    avg += a
                    i += 1

            data = "done"
            sock.send(data.encode())
            kill_signal = sock.recv(1024)

        file1.write("num_flows {}     ".format(num_flows))
        file1.write("Throughput: {}\n".format(avg/3))
        num_flows += 20
        file1.write("-----------------------\n")
        file1.flush()
    payload_size *= 2
    file1.write("\n\n")
    
