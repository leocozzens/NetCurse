import java.util.Scanner;

public class Main {
    private static final String USERDATA_HEADER = "USER";
    private static final String USERDATA_FOOTER = "ENDU";
    private static final String PROMPT = "-> ";
    public static void main(String args[]) {
        MessageFactory outData = new MessageFactory(USERDATA_HEADER, USERDATA_FOOTER);
        Scanner in = new Scanner(System.in);

        String serverIP;
        if(args.length > 0) serverIP = args[0];
        else serverIP = "0.0.0.0";
        int serverPort = 9002;
        Connection serverConn = new Connection(serverIP, serverPort);

        while(true) {
            outData.createMessage(in, PROMPT);
            serverConn.sendToServer(outData.getData());
            System.out.println("Message sent: " + outData.getMessage());
        }
    }
}