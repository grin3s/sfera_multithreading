import socket
import time
IP = "127.0.0.1"
Port = 3100

s = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
s.settimeout(1)
s.connect((IP, Port))
s.sendall("hello\n")
time.sleep(2)
s.close()