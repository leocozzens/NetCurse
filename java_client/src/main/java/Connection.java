import java.io.IOException;
import java.io.OutputStream;
import java.net.Socket;

public class Connection {
    // private boolean connAlive;
    private Socket sock;
    private OutputStream serverStream;

    public Connection(String serverIP, int serverPort) {
        try {
            this.sock = new Socket(serverIP, serverPort);
            this.serverStream = sock.getOutputStream();
        }
        catch(IOException e) {
            System.err.println("Error establishing connection.\n" + e);
            System.exit(1);
        }
        // this.connAlive = true;
    }
    public void sendToServer(String outData) {
        try {
            this.serverStream.write(outData.getBytes());
            this.serverStream.flush();
        }
        catch(IOException e) {
            System.err.println("Error sending data to server.\n" + e);
        }
    }
}
