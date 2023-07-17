import java.io.IOException;
import java.io.OutputStream;
import java.net.Socket;

public class Connection {
    private Socket sock;
    private OutputStream serverStream;

    public Connection(String serverIP, int serverPort) {
        try {
            this.sock = new Socket(serverIP, serverPort);
            this.serverStream = sock.getOutputStream();
        }
        catch(Exception e) {
            System.out.println("Error establishing socket: " + e);
        }
    }
    public void sendToServer(String outData) {
        try {
            this.serverStream.write(outData.getBytes());
            this.serverStream.flush();
        }
        catch(IOException e) {
            System.out.println("Error sending data to server: " + e);
        }
    }
}
