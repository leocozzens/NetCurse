import java.util.Scanner;

public class Message {
    private String outMessage;
    private String outData;
    final int messageSize = 256;

    public Message(String header, String footer, Scanner in, String prompt) {
        System.out.print(prompt);
        try {
            this.outMessage = in.nextLine();
        }
        catch(Exception e) {
            e.printStackTrace();
        }
        createMessage(header, footer);
    }
    
    public Message(String header, String footer, String message) {
        this.outMessage = message;
        createMessage(header, footer);
    }

    private void createMessage(String header, String footer) {
        StringBuilder messageBuilder = new StringBuilder(header);
        messageBuilder.append(this.outMessage);
        messageBuilder.append('\0');
        for(int i = this.outMessage.length() + 1; i < messageSize; i++) {
            messageBuilder.append(' ');
        }
        messageBuilder.append(footer);
        this.outData = messageBuilder.toString();
    }

    public String getData() {
        return outData;
    }
    public String getMessage() {
        return outMessage;
    }
}