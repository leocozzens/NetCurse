public class ExitHandler {
    public static void handleExit(StayAlive KALoop) {
        OnExit exitFunc = new OnExit(KALoop);
        Thread exitThread = new Thread(exitFunc);

        Runtime.getRuntime().addShutdownHook(exitThread);
    }

    private static class OnExit implements Runnable {
        private StayAlive KALoop;

        public OnExit(StayAlive KALoop) {
            this.KALoop = KALoop;
        }

        @Override
        public void run() {
            this.KALoop.sendKill();
            System.err.println("\nReceived interruption. Performing cleanup.");
        }
    }
}