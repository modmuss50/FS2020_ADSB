import net
import nativesockets

var socketThread: Thread[void]
var messageChannel: Channel[string]

proc socketThreadFunc() {.thread.} =
    var socket = newSocket()
    socket.bindAddr(Port(30003))
    socket.getFd().setBlocking(false)
    socket.listen()

    echo "Listening for TCP connections on port 30003"

    # Based off https://blog.tejasjadhav.xyz/simple-chat-server-in-nim-using-sockets/
    var clients: seq[Socket] = @[]
    while true:
        try:
            var client: Socket = new(Socket)
            socket.accept(client)
            clients.add(client)
            echo "TCP client connected"
        except OSError:
            discard

        var clientsToRemove: seq[int] = @[]

        for index, client in clients:
            try:
                let message: string = client.recvLine(timeout = 1)
                if message == "":
                    clientsToRemove.add(index)
            except TimeoutError:
                discard

        for index in clientsToRemove:
            clients.del(index)
            stdout.writeLine("Server: client disconnected")

        let message = messageChannel.tryRecv()
        if message.dataAvailable:
            for index, client in clients:
                client.send(message.msg)

    socket.close()

proc send*(message: string) =
    messageChannel.send(message)

proc start*() =
    messageChannel.open()
    socketThread.createThread(socketThreadFunc)