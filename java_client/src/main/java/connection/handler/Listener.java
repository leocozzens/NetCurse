package connection.handler;

// JDK classes
import java.io.IOException;
import java.io.InputStream;

// Local classes
import connection.SocketConnection;
import message.handler.Parser;

public class Listener implements Runnable {
    private SocketConnection activeConnection;
    private InputStream serverInStream;
    private Parser messageParser;
    private int messageSize;

    public Listener(InputStream serverInStream, SocketConnection activeConnection, int messageSize) {
        this.activeConnection = activeConnection;
        this.serverInStream = serverInStream;
        this.messageParser = new Parser();
        this.messageSize = messageSize;
    }
    
    @Override
    public void run() {
        byte[] buffer = new byte[messageSize * 10];
        try {
            while(true) {
                this.messageParser.interpretData(receiveData(buffer));
            }
        }
        catch(IOException e) {
            System.err.println("Error receiving data from server.\n" + e);
            activeConnection.killConn();
        }
    }

    private String receiveData(byte[] buffer) throws IOException {
        int bytesRead;
        bytesRead = serverInStream.read(buffer);
        if(bytesRead == -1) throw new IOException("Failed to read from input stream");
        return new String(buffer, 0, bytesRead);
    }
}