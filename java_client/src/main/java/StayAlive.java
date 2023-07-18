public class StayAlive implements Runnable {
    private static final int KA_SIZE = 1;
    private static final int SLEEP_MS = 100;
    private static final String HEADER = "KEEP";
    private static final String FOOTER = "ALVE";
    private static final String ALIVE = "\n";
    private static final String DEAD = "\0";

    private String connStat;

    private Connection serverConn;
    private MessageFactory beatFactory;

    public StayAlive(Connection serverConn) {
        this.serverConn = serverConn;
        this.connStat = ALIVE;
        this.beatFactory = new MessageFactory(HEADER, FOOTER, KA_SIZE);
    }

    @Override
    public void run() {
        while(true) {
        this.beatFactory.createMessage(connStat);
            serverConn.sendToServer(beatFactory.getData());
            try {
                Thread.sleep(SLEEP_MS);
            }
            catch(Exception e) {}
        }
    }

    public void sendKill() {
        this.connStat = DEAD;
    }
}
