import java.util.Scanner;

public class MessageFactory {
    private static final char TERMINATOR = '\0';
    private static final char EMPTY_CHAR = ' ';

    private int messageSize;
    private StringBuilder messageBuilder;
    private String header;
    private String footer;

    private String outMessage;
    private String outData;
    public MessageFactory(String header, String footer, int messageSize) {
        this.messageBuilder = new StringBuilder();
        this.messageSize = messageSize;
        this.header = header;
        this.footer = footer;
    }

    public void createMessage(Scanner in, String prompt) {
        System.out.print(prompt);
        try {
            this.outMessage = in.nextLine();
        }
        catch(Exception e) {
            System.err.println("Unable to read input, please try again.");
        }
        constructMessage();
    }

    public void createMessage(String message) {
        this.outMessage = message;
        constructMessage();
    }
    
    private void constructMessage() {
        this.messageBuilder.append(header);
        int messageLen = Math.min(this.outMessage.length(), messageSize);
        this.messageBuilder.append(this.outMessage.substring(0, messageLen));
        if(messageLen < messageSize) {
            this.messageBuilder.append(TERMINATOR);
            messageLen++;
        }
        for(int i = messageLen; i < this.messageSize; i++) {
            this.messageBuilder.append(EMPTY_CHAR);
        }
        this.messageBuilder.append(this.footer);
        this.outData = this.messageBuilder.toString();
        this.messageBuilder.setLength(0);
    }

    public String getData() {
        return outData;
    }
    public String getMessage() {
        return outMessage;
    }
}