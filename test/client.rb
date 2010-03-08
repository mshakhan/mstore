require 'socket'

module Mstore
  class Client
    attr_reader :port
    BUFFER_LEN = 1024
    
    def initialize(port)
      @port = port
      @socket = TCPSocket.new 'localhost', port
    end
    
    def get(key)
      send_command "GET #{key};"
    end
    
    def put(key, value)
      send_command "PUT #{key};#{value}"
    end
    
    protected
    
    def send_command(command)
      @socket.send command, 0
      @socket.recv BUFFER_LEN    
    end
  end # Client
end # Mstore
