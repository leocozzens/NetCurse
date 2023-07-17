import java.util.Scanner;

public class Main {
    public static void main(String args[]) {
        final String USERDATA_HEADER = "USER";
        final String USERDATA_FOOTER = "ENDU";
        final String PROMPT = "-> ";

        Scanner in = new Scanner(System.in);
        String serverIP;
        if(args.length > 0) serverIP = args[0];
        else serverIP = "0.0.0.0";
        int serverPort = 9002;

        Connection serverConn = new Connection(serverIP, serverPort);
        while(true) {
            Message outData = new Message(USERDATA_HEADER, USERDATA_FOOTER, in, PROMPT);
            serverConn.sendToServer(outData.getData());
            System.out.println("Message sent: " + outData.getMessage());
        }
    }
}
