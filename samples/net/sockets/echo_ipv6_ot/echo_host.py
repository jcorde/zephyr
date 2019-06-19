import socket
import sys
HOST = sys.argv[1]
PORT = int(sys.argv[2])

tcp = socket.socket(socket.AF_INET6, socket.SOCK_STREAM)
orig = (HOST,PORT)

tcp.bind(orig)
tcp.listen(1)
print "Listening"

while True:
    conn, cliente = tcp.accept()
    print "Connected to", cliente
    try:
        while True:
            msg = conn.recv(1024)
            if not msg: break
            print cliente, "Length:", len(msg)
            conn.send(msg)
    except Exception as e:
        conn.close()
        raise e
    
    conn.close()