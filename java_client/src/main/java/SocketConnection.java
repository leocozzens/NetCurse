import java.io.IOException;
import java.io.OutputStream;
import java.net.Socket;

public class SocketConnection {
    private Socket sock;
    private boolean connAlive;

    private Listener recver;
    private StayAlive KALoop;
    private OutputStream serverStream;

    public SocketConnection(String serverIP, int serverPort, int messageSize) {
        try {
            this.sock = new Socket(serverIP, serverPort);
            this.serverStream = sock.getOutputStream();

            this.recver = new Listener(sock.getInputStream(), this, messageSize);
            Thread recvThread = new Thread(this.recver);
            recvThread.start();

            this.KALoop = new StayAlive(this);
            Thread KAThread = new Thread(this.KALoop);
            KAThread.start();
        }
        catch(IOException e) {
            System.err.println("Error establishing connection.\n" + e);
            System.exit(-1);
        }

        this.connAlive = true;
    }

    public boolean sendToServer(String outData) {
        if(!this.checkAlive()) return false;
        try {
            this.serverStream.write(outData.getBytes());
            this.serverStream.flush();
        }
        catch(IOException e) {
            System.err.println("Error sending data to server.\n" + e);
            return false;
        }
        return true;
    }

    public boolean checkAlive() {
        return this.connAlive;
    }

    public void killConn() {
        this.connAlive = false;
    }
}
