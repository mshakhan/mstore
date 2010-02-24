require 'socket'

host = 'localhost'
port = ARGV[1]

s = TCPSocket.open(host, port)
# TODO