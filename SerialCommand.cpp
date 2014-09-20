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
#include "SerialCommand.h"

/**
 * Constructor makes sure some things are set and cleared.
 */
CommandHandler::CommandHandler( SerialCommand &cmd )
  : serialCmd(cmd),      // Constructor
    commandList(NULL),
    commandCount(0),
    defaultHandler(NULL)
{
}

/**
 * Adds a sub command handler function to the list of available commands.
 * This is used for matching a found token in the buffer, 
 */
void CommandHandler::addHandler(const char *command, CommandHandler& handler ) {

  commandList = (SerialCommandCallback *) realloc(commandList, (commandCount + 1) * sizeof(SerialCommandCallback));
  strncpy(commandList[commandCount].command, command, SERIALCOMMAND_MAXCOMMANDLENGTH);
  commandList[commandCount].function1 = NULL;
  commandList[commandCount].function2 = NULL;
  commandList[commandCount].hand = &handler;
  commandCount++;
}

/**
 * Adds a "command" and a delegate function to the list of available commands.
 * This is used for matching a found token in the buffer, and gives the pointer
 * to the handler function to deal with it.
 */
void CommandHandler::addCommand(const char *command, CommandCallbackSimple callback) {

  commandList = (SerialCommandCallback *) realloc(commandList, (commandCount + 1) * sizeof(SerialCommandCallback));
  strncpy(commandList[commandCount].command, command, SERIALCOMMAND_MAXCOMMANDLENGTH);
  commandList[commandCount].function1 = callback;
  commandList[commandCount].function2 = NULL;
  commandList[commandCount].hand = NULL;
  commandCount++;
}

/**
 * Adds a "command" and a delegate function to the list of available commands.
 * This is used for matching a found token in the buffer, and gives the pointer
 * to the handler function to deal with it.
 */
void CommandHandler::addCommand(const char *command, CommandCallback callback) {

  commandList = (SerialCommandCallback *) realloc(commandList, (commandCount + 1) * sizeof(SerialCommandCallback));
  strncpy(commandList[commandCount].command, command, SERIALCOMMAND_MAXCOMMANDLENGTH);
  commandList[commandCount].function1 = NULL;
  commandList[commandCount].function2 = callback;
  commandList[commandCount].hand = NULL;
  commandCount++;
}

/**
 * This sets up a handler to be called in the event that the receveived command string
 * isn't in the list of commands.
 */
void CommandHandler::setDefault(CommandCallback callback) {
  defaultHandler = callback;
}

/**
 * Handles the decision of the command to execute based on the next token in the stream
 */
void CommandHandler::handle () {

    boolean matched = false;
    char *command = serialCmd.nextToken();
    
    if (command != NULL) {
        for (int i = 0; i < commandCount; i++) {
        
          // Compare the found command against the list of known commands for a match
          if (strncmp(command, commandList[i].command, SERIALCOMMAND_MAXCOMMANDLENGTH) == 0) {
          
            // Execute the stored handler function for the command
            if ( *commandList[i].function1 != NULL ) {
            	(*commandList[i].function1)();
            } else if ( *commandList[i].function2 != NULL ) {
            	(*commandList[i].function2)( &serialCmd );
        	} else if ( (commandList[i].hand) != NULL ) {
            	(*commandList[i].hand).handle();
        	}
            matched = true;
            break;
          }
        }
    }
    if (!matched && (defaultHandler != NULL)) {
      // not matched, call the default handler
      (*defaultHandler)( &serialCmd );
    }
}

/**
 * Constructor makes sure some things are set.
 */
SerialCommand::SerialCommand()
  : term('\n'),           // default terminator for commands, newline character
    last(NULL),
    firsttoken(true)
{
    strcpy(delim, " "); // strtok_r needs a null-terminated string
	clearBuffer();
}

void SerialCommand::setHandler( CommandHandler& cmdHand ) {
  cmdHandle = &cmdHand;
}

/**
 * This checks the Serial stream for characters, and assembles them into a buffer.
 * When the terminator character (default '\n') is seen, it starts parsing the
 * buffer for a prefix command, and calls handlers setup by addCommand() member
 */
void SerialCommand::readSerial() {
  while (Serial.available() > 0) {

    char inChar = Serial.read();   // Read single available character, there may be more waiting
    
    if (inChar == term) {     // Check for the terminator (default '\r') meaning end of command
	  cmdHandle->handle();
      clearBuffer();
    }

    else if (isprint(inChar)) {     // Only printable characters into the buffer
      if (bufPos < SERIALCOMMAND_BUFFER) {
        buffer[bufPos++] = inChar;  // Put character into buffer
        buffer[bufPos] = '\0';      // Null terminate
      }
    }
  }
}

/*
 * Clear the input buffer.
 */
void SerialCommand::clearBuffer() {
  buffer[0] = '\0';
  bufPos = 0;
  firsttoken=true;
  lastTok=NULL;
}

/**
 * Retrieve the next token ("word" or "argument") from the command buffer.
 * Returns NULL if no more tokens exist.
 */ 
char* SerialCommand::nextToken() {
	if (firsttoken) {
		firsttoken=false;
		lastTok = strtok_r(buffer, delim, &last);   // Search for command at start of buffer
	} else {
		lastTok = strtok_r(NULL, delim, &last);
	}
	return lastTok;
}

/**
 * Retrieve the next token ("word" or "argument") from the command buffer.
 * Returns NULL if no more tokens exist.
 */ 
char* SerialCommand::lastToken() {
	return lastTok;
}

