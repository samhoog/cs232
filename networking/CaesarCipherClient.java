//
//  CaesarCipherClient.java
//  CS232 Client for a Caesar Cipher
//
//  Created by Sam Hoogewind on 4/19/22.
//

import java.io.BufferedReader;
import java.io.InputStreamReader;
import java.io.PrintWriter;
import java.net.Socket;
import java.util.Scanner;
import java.io.*; 

public class CaesarCipherClient
{
    public static void main( String[] args )
    {
        if( args.length < 2 )
        {
            System.out.println( "Usage: CaesarCipherClient <server> <port>" );
            System.exit( 0 );
        }
        
        // Split the arguments
        String server = args[ 0 ];
        Integer port = Integer.parseInt(args[ 1 ]);

        try
        {
            // Connect to the server
            Socket client = new Socket( server, port );

            // Create input and output streams to read from and write to the server
            PrintWriter out = new PrintWriter( client.getOutputStream(), true );
            BufferedReader in = new BufferedReader( new InputStreamReader( client.getInputStream() ) );
            BufferedReader userIn = new BufferedReader ( new InputStreamReader( System.in));

            System.out.println("Loading contents of: " + server + " on port: " + port);

             // Display welcome message and prompt for the rotation amount
            System.out.println("Welcome to the Caeser Cipher Client!");
            System.out.println("Please enter a rotation amount between 1 and 25.");

            // Get the rotation amount
            Scanner keyboard = new Scanner(System.in);
            Integer rotation = keyboard.nextInt();

            // Make sure the rotation amount is valid
            if (rotation > 25 || rotation < 1) {
                System.out.println("Rotation amount is not valid.");
                System.exit( 0 );
            }

            // Send the rotation amount to the server
            out.println(String.valueOf(rotation));
            // print out the server's response
            System.out.println(in.readLine());

            // Read data from the user to encrypt
            System.out.println("Please enter what you want to encrypt or type quit to exit: ");
            String inputLine;

            // When the line is valid and not quit
            while ((inputLine = userIn.readLine()) != null && !inputLine.equals("quit")) {
                // Send the line to the server
                out.println(inputLine);
                // Output the encrypted line from the server
                System.out.println(in.readLine());
                // Reprompt the user
                System.out.println("Please enter what you want to encrypt or type quit to exit: ");
            }

            // Close our streams
            in.close();
            out.close();
            client.close();
            userIn.close();
        }
        catch( Exception e )
        {
            e.printStackTrace();
        }
    }
}