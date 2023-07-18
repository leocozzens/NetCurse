import java.util.Scanner;

public class MessageFactory {
    private final int MESSAGE_SIZE = 256;
    private String header;
    private String footer;

    private String outMessage;
    private String outData;

    public MessageFactory(String header, String footer) {
        this.header = header;
        this.footer = footer;
    }

    public void createMessage(Scanner in, String prompt) {
        System.out.print(prompt);
        try {
            this.outMessage = in.nextLine();
        }
        catch(Exception e) {
            e.printStackTrace();
        }
        constructMessage();
    }

    public void createMessage(String message) {
        this.outMessage = message;
        constructMessage();
    }
    
    private void constructMessage() {
        StringBuilder messageBuilder = new StringBuilder(this.header);
        messageBuilder.append(this.outMessage);
        messageBuilder.append('\0');
        for(int i = this.outMessage.length() + 1; i < MESSAGE_SIZE; i++) {
            messageBuilder.append(' ');
        }
        messageBuilder.append(this.footer);
        this.outData = messageBuilder.toString();
    }

    public String getData() {
        return outData;
    }
    public String getMessage() {
        return outMessage;
    }
}