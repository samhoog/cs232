//
//  CaesarCipherServer.java
//  CS232 Server for a Caesar Cipher
//
//  Created by Sam Hoogewind on 4/19/22.
//

import java.io.BufferedReader;
import java.io.InputStreamReader;
import java.io.PrintStream;
import java.io.PrintWriter;
import java.io.DataOutputStream;
import java.net.Socket;
import java.net.ServerSocket;
import java.util.Date;

import javax.crypto.EncryptedPrivateKeyInfo;

public class CaesarCipherServer
{
    public static void main( String[] args )
    {
        // check the argument length
        if( args.length < 1 )
        {
            System.out.println( "Usage: CaesarCipherServer <port>" );
            System.exit( 0 );
        }

        // set the port value
        Integer port = Integer.parseInt(args[ 0 ]);

        try
        {
            // Create a ServerSocket with that port
            ServerSocket server = new ServerSocket( port );

            while( true )
            {
                // accept any clients
                Socket client = server.accept();

                // print out the time and date
                System.out.println(new Date().toString() + ": connected to " + client.getRemoteSocketAddress().toString());

                // start a thread for that client
                new MultiThread(client).start();
            }

        }
        catch( Exception e )
        {
            e.printStackTrace();
        }
    }
}

// https://codejava.net/java-se/networking/java-socket-server-examples-tcp-ip
class MultiThread extends Thread {
    public Socket multiSocket;

    public MultiThread(Socket socket) {
        // set the socket to a variable for later use
        this.multiSocket = socket;
    }

    public void run() {
        try {
            // Create input and output streams to read from and write to the client
            PrintWriter out = new PrintWriter( multiSocket.getOutputStream(), true );
            BufferedReader in = new BufferedReader ( new InputStreamReader( multiSocket.getInputStream() ));

            // read in the rotation amount from the client
            String inputLine;
            inputLine = in.readLine();
            int rotation = Integer.valueOf(inputLine);

            // check if the rotation amount is valid
            if (rotation > 25 || rotation < 1) {
                System.out.println("Rotation amount is not valid.");
                multiSocket.close();
            }

            // send the rotation amount back to the client
            out.println(rotation);

            // read in each line from the client unless it's 'quit'
            while ((inputLine = in.readLine()) != null && !inputLine.equals("quit")) {
                // send back each encrypted message
                out.println(encrypt(inputLine, rotation));
            }

            // print out the time and date
            System.out.println(new Date().toString() + multiSocket.getRemoteSocketAddress().toString() + " disconnecting.");
            // close the socket
            multiSocket.close();
        }

        // catch any exceptions
        catch ( Exception e ) {
            System.out.println("Error occurred, closing thread.");
        }
    }

    // https://www.baeldung.com/java-caesar-cipher
    public static StringBuilder encrypt(String line, int rotation) {
        StringBuilder encryptedText = new StringBuilder();
        
        // For each character in the line
        for (int i = 0; i < line.length(); i++) {
            
            // Get the character
            char character = line.charAt(i);
            char newCharacter = ' ';
            
            // If the character is a letter
            if (Character.isLetter(character)) {

                // If the character is upper case
                if (Character.isUpperCase(character)) {
                    newCharacter = (char) ('A' + (character - 'A' + rotation) % 26);
                
                // If the character is lower case
                } else if (Character.isLowerCase(character)) {
                    newCharacter = (char) ('a' + (character - 'a' + rotation) % 26);
                }

                // Append the new character
                encryptedText.append(newCharacter);

            // Otherwise just append the unchanged character
            } else {
                encryptedText.append(character);
            }
        }

        // Return the encrypted text
        return encryptedText;
    }

}
