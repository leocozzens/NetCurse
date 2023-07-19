// Local classes
import connection.SocketConnection;

public class ExitHandler {
    public static void handleExit(SocketConnection activeConn) {
        OnExit exitFunc = new OnExit(activeConn);
        Thread exitThread = new Thread(exitFunc);

        Runtime.getRuntime().addShutdownHook(exitThread);
    }

    private static class OnExit implements Runnable {
        private SocketConnection activeConn;

        public OnExit(SocketConnection activeConn) {
            this.activeConn = activeConn;
        }

        @Override
        public void run() {
            this.activeConn.killConn();
            System.err.println("\nReceived interruption. Performing cleanup.");
        }
    }
}