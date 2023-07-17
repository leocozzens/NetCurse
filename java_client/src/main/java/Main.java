import java.util.Scanner;

public class Main {
    public static void main(String args[]) {
        Scanner in = new Scanner(System.in);
        String serverIP;
        if(args.length > 0) serverIP = args[0];
        else serverIP = "0.0.0.0";
        int serverPort = 9002;
        Connection serverConn = new Connection(serverIP, serverPort);
        while(true) {
            Message outData = new Message("USER", "ENDU", in, "-> ");
            serverConn.sendToServer(outData.getData());
            System.out.println("Message sent: " + outData.getMessage());
        }
    }
}