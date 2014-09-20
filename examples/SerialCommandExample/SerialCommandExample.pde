// Demo Code for SerialCommand Library
//
// Steven Cogswell
// May 2011
//
// Mark Pruden 
// July 2014
// Demonstrated enhancements to library

#include <SerialCommand.h>

#define arduinoLED 13   // Arduino LED on board

static SerialCommand  sCmd   = SerialCommand();       // The demo SerialCommand object
static CommandHandler sHand1 = CommandHandler(sCmd);  // the main command handler
static CommandHandler sHand2 = CommandHandler(sCmd);  // the sub command handler for LED command

void setup() {
  
  pinMode(arduinoLED, OUTPUT);      // Configure the onboard LED for output
  digitalWrite(arduinoLED, LOW);    // default to LED off

  Serial.begin(9600);
  while (! Serial );

  // set the main handler for serial command
  sCmd.setHandler(sHand1);

  // Setup callbacks for the main SerialCommand. These are the main commands
  sHand1.addCommand("ON",    LED_on);          // Turns LED on
  sHand1.addCommand("OFF",   LED_off);         // Turns LED off
  sHand1.addCommand("HELLO", sayHello);        // Echos the string argument back
  sHand1.addHandler("LED",   sHand2);          // REGISTERS a SUB Handler for LED command
  sHand1.addCommand("P",     processCommand);  // Converts two arguments to integers and echos them back
  sHand1.setDefault(         unrecognized);    // Handler for command that isn't matched  (says "What?")
  
  // Setup the command for the LED command. LED is a command from the handler above that calls this handler
  sHand2.addCommand("ON",    LED_on);          // Turns LED on
  sHand2.addCommand("OFF",   LED_off);         // Turns LED off
  sHand2.setDefault(         unrecognized);    // Handler for command that isn't matched  (says "What?")
  
  Serial.println("Ready");
}

void loop() {
  sCmd.readSerial();     // We don't do much, just process serial commands
}


void LED_on() {
  Serial.println("LED on");
  digitalWrite(arduinoLED, HIGH);
}

void LED_off() {
  Serial.println("LED off");
  digitalWrite(arduinoLED, LOW);
}

void sayHello(SerialCommand *cmd) {
  char *arg = cmd->nextToken();    // Get the next argument from the SerialCommand object buffer
  if (arg != NULL) {    // As long as it existed, take it
    Serial.print("Hello ");
    Serial.println(arg);
  }
  else {
    Serial.println("Hello, whoever you are");
  }
}

void processCommand(SerialCommand *cmd) {
  Serial.println("We're in processCommand");

  char *arg = cmd->nextToken();
  if (arg != NULL) {
    Serial.print("First argument was: ");
    Serial.println( atoi(arg) );
  }
  else {
    Serial.println("No arguments");
  }

  arg = cmd->nextToken();
  if (arg != NULL) {
    Serial.print("Second argument was: ");
    Serial.println( atol(arg) );
  }
  else {
    Serial.println("No second argument");
  }
}

// This gets set as the default handler, and gets called when no other command matches.
void unrecognized(SerialCommand *cmd) {
  Serial.print("Unknown Argument '");
  Serial.print(cmd->lastToken());
  Serial.println("'");
}
