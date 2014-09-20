/**
 * SerialCommand - A Wiring/Arduino library to tokenize and parse commands
 * received over a serial port.
 * 
 * Copyright (C) 2014 Mark Pruden
 * Copyright (C) 2012 Stefan Rado
 * Copyright (C) 2011 Steven Cogswell <steven.cogswell@gmail.com>
 *                    http://husks.wordpress.com
 * 
 * Version 20140120
 * 
 * This library is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 * 
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License
 * along with this library.  If not, see <http://www.gnu.org/licenses/>.
 */
#ifndef SerialCommand_h
#define SerialCommand_h

#if defined(WIRING) && WIRING >= 100
  #include <Wiring.h>
#elif defined(ARDUINO) && ARDUINO >= 100
  #include <Arduino.h>
#else
  #include <WProgram.h>
#endif
#include <string.h>

// Size of the input buffer in bytes (maximum length of one command plus arguments)
#define SERIALCOMMAND_BUFFER 48

// Maximum length of a command excluding the terminating null
#define SERIALCOMMAND_MAXCOMMANDLENGTH 12

//forward declarations
class SerialCommand;

// definitions of callback for handling commands
typedef void (*CommandCallbackSimple)();
typedef void (*CommandCallback)(SerialCommand*);

// The class that handles the processing of Commands
class CommandHandler {
  public:
  
    CommandHandler( SerialCommand& cmd );  // Constructor, must associate the SerialCommand 

    void addCommand( const char *command, CommandCallbackSimple callback);  // Add a command to the list of commands to process.
    void addCommand( const char *command, CommandCallback callback);  // Add a command to the list of commands to process.
    void addHandler( const char *command, CommandHandler& handler );  // Add a sub command handler for a command.
    void setDefault( CommandCallback callback );   // A handler to call when no valid command received.

	void handle (); // called by SerialCommand to process a command, dont call this in your code.

  private:

	SerialCommand& serialCmd; 	// The SerialCommand from which tokens are read
    CommandCallback defaultHandler;  // Pointer to the default handler function

    // Data structure to hold Command/Handler function key-value pairs
    struct SerialCommandCallback {
      char command[SERIALCOMMAND_MAXCOMMANDLENGTH + 1];
      CommandCallbackSimple function1;
      CommandCallback function2;
      CommandHandler *hand;
    };  

    SerialCommandCallback *commandList;   // Actual definition for command/handler array
    byte commandCount; // size of the above structure
};

// This is the main class that 
class SerialCommand {
  public:

    SerialCommand( );      // Main Constructor
    
    void setHandler( CommandHandler& cmdHand );    // Sets the main handler
    
    void readSerial();    // Main entry point, to read from serial and process.
    
    void clearBuffer();   // Clears the input buffer.
	char* nextToken();    // parses and gets the next token from the input
	char* lastToken();    // gets the last token read from the input

  private:

	CommandHandler* cmdHandle;	// The default command Handler for processing the commands
	
    char term;          // Character that signals end of command (default '\n')
    char buffer[SERIALCOMMAND_BUFFER + 1]; // Buffer of stored characters while waiting for terminator character
    byte bufPos;        // Current position in the buffer
    
    char delim[2]; 		// null-terminated list of character to be used as delimeters for tokenizing (default " ")
    char *last;         // State variable used by strtok_r during processing
    char *lastTok;      // returned from call to last token
    boolean firsttoken; // denotes first call to string tokeniser.
};

#endif //SerialCommand_h
