import java.io.IOException;
import java.io.InputStream;

public class Listener implements Runnable {
    private SocketConnection activeConnection;
    private InputStream serverInStream;
    private int messageSize;

    public Listener(InputStream serverInStream, SocketConnection activeConnection, int messageSize) {
        this.activeConnection = activeConnection;
        this.serverInStream = serverInStream;
        this.messageSize = messageSize;
    }
    
    @Override
    public void run() {
        byte[] buffer = new byte[messageSize * 10];
        try {
            while(true) {
                interpretData(receiveData(buffer));
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

    private void interpretData(String inData) {
        System.out.println("Received: " + inData);
    }
}