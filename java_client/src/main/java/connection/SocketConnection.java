package connection;

// JDK classes
import java.io.IOException;
import java.io.OutputStream;
import java.net.Socket;

// Local classes
import connection.handler.Listener;
import connection.handler.StayAlive;

public class SocketConnection {
    private OutputStream serverStream;
    private Socket sock;
    private boolean connAlive;

    public SocketConnection(String serverIP, int serverPort, int messageSize) {
        try {
            this.sock = new Socket(serverIP, serverPort);
            this.serverStream = sock.getOutputStream();
            makeThread(new Listener(sock.getInputStream(), this, messageSize));
        }
        catch(IOException e) {
            System.err.println("Error establishing connection.\n" + e);
            System.exit(-1);
        }

        makeThread(new StayAlive(this));
        this.connAlive = true;
    }

    public void makeThread(Runnable threadClass) {
        Thread newThread = new Thread(threadClass);
        newThread.start();
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
