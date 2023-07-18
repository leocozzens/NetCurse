import java.io.IOException;
import java.io.InputStream;

public class Listener implements Runnable {
    private int messageSize;
    private SocketConnection activeConnection;
    private InputStream serverInStream;

    public Listener(InputStream serverInStream, SocketConnection activeConnection, int messageSize) {
        this.activeConnection = activeConnection;
        this.serverInStream = serverInStream;
        this.messageSize = messageSize;
    }
    
    @Override
    public void run() {
        byte[] buffer = new byte[messageSize * 10];
        receiveData(buffer);
    }
    private void receiveData(byte[] buffer) {
        int bytesRead;
        try {
            while((bytesRead = serverInStream.read(buffer)) != -1) {
                String data = new String(buffer, 0, bytesRead);
                System.out.println("Received: " + data);
            }
        }
        catch(IOException e) {
            System.err.println("Error receiving data from server.\n" + e);
            activeConnection.killConn();
        }
    }
}