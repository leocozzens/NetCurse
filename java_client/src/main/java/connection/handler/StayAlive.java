package connection.handler;

// Local classes
import connection.SocketConnection;
import message.MessageFactory;

public class StayAlive implements Runnable {
    private static final int KA_SIZE = 1;
    private static final int SLEEP_MS = 100;
    private static final String HEADER = "KEEP";
    private static final String FOOTER = "ALVE";
    private static final String ALIVE = "\n";
    private static final String DEAD = "\0";

    private SocketConnection serverConn;
    private MessageFactory kaFactory;

    public StayAlive(SocketConnection serverConn) {
        this.serverConn = serverConn;
        this.kaFactory = new MessageFactory(HEADER, FOOTER, KA_SIZE);
    }

    @Override
    public void run() {
        while(this.serverConn.checkAlive()) {
            sendKeepAlive(ALIVE);
            try {
                Thread.sleep(SLEEP_MS);
            }
            catch(InterruptedException e) {}
        }
        sendKeepAlive(DEAD);
    }

    public void sendKeepAlive(String status) {
        this.kaFactory.createMessage(status);
        serverConn.sendToServer(kaFactory.getData());
    }
}
