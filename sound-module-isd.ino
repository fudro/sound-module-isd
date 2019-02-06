#include <SoftwareSerial.h>


/******HARDWARE SETUP********************************************
 * Connect the software serial TX pin to 5V using a 1K pullup resistor.
 * Connect Pin 4 of the Sound Module to the software serial TX pin.
 * Connect Vin of the sound module to Vin on Arduino. Be sure to use external power supply of 6-12V.
 * Connect Vss of the sound module to GND.
 * ************************************************/
SoftwareSerial sound(4, 5); //Create the software serial port (first argument RX, second argument TX)

/*******SOUND MODULE COMMAND REFERENCE**********************
 * Complete Sound Module documentation can be found here: https://www.adrirobot.it/sound_module/SoundModManBv1_1.pdf
 * When the Sound Module is powered on, it will start in "locked" mode where recording is not enabled (siginified by flashing Red and Green LED).
 * Commands can be sent as an array of characters or as a string. MUST CHOOSE ONLY ONE METHOD AND COMMENT OUT THE OTHER OPTION!! See examples within the code at the command declaration.
 * The following commands can be used with the sound module. The "0" character is always a zero.
 * 
 * !SM0N          - Enables record mode on the module and sets the module in Playback mode. Press Black button on module to switch modes.
 * !SM0P[marker]  - Plays back sound at the designated address (set by "marker"). The "marker" parameter defines a specific time address within the sound module's allotted 60 seconds of memory. The time address is caluclated as: "marker" multiplied by 0.4 seconds.
 * !SM0M[marker]  - Places an End Of Message marker at the designated position. Can be used to mark the end of a sound clip or to divide a recording into multiple clips. It is best practice to send this command with a parameter of 149 at least once to prevent accidental overflow when playing sound near the end of the memory.
 * 
 * ********************************************************/

//CHOOSE A COMMAND OPTION (Comment out unused option):
//char command[5] = {'!', 'S', 'M', '0', 'N'};    //OPTION A: Send command as array of individual characters. See command reference above.
char command[] = "!SM0N";                         //OPTION B: Send command as a string (char array)

uint8_t marker = 0;       //Set marker for the time address in the sound memory. Set to zero if the command does not require a parameter. Values MUST be between 0 and 149. Zero will set the marker to the beginning of memory, 149 will set the marker to 59.6 seconds - the end of usable memory. Do not record at or beyond 149 or a memory overflow condition will occur.
//int commandLength = 5;  //Convenience variable (assign here for the entire program) to store the length of the serial transmission in bytes. Value MUST match the actual length of the char array for the command.
int enabled = 0;          //Use a flag variable so the program loop will only run the serial transmission once
char received;            //FOR LOOPBACK ONLY: A single byte variable to store and monitor the received serial transmission via a loopback circuit - connect the software serial TX pin to the RX pin of Arduino hardware UART (pin 0).


void setup() {
  //Open serial communication and set data rate on the Arduino hardware UART:
  Serial.begin(4800);   //For loopback monitoring, make sure this rate matches the rate of the Sound Module.
  Serial.println("Sound Module Test!");   //Print to the monitor to signify the start of the program
  
  //Set the data rate for the SoftwareSerial port
  sound.begin(4800);    //Set baud rate to an acceptable rate. Sound Module documentation uses sample code based on the Basic Stamp 2: either 4800 or 9600 8N1 (8 Data bits, Non-Parity, 1 Stop bit, TRUE logic: non-inverted LOW = 0 and HIGH = 1). https://www.parallax.com/go/PBASICHelp/Content/LanguageTopics/Commands/SEROUT.htm
  delay(100);           
}

void loop() {
  if (enabled == 0) {         //Check flag to see if transmission has not been sent
    enabled = 1;              //Update flag to show that transmission has been sent
    sound.write(command);     //Use the write() function to serially send the command array as a series of bytes
    sound.write(marker);      //Send the paraeter value as a byte. If marker is set to zero, commands that do not require a parameter will ignore this second transmission.

/**********FOR LOOPBACK MONITORING ONLY******************************/
    if (Serial.available()) {
      for (int i = 0; i < sizeof(command) / sizeof(char); i++){   //Use "sizeof" calculation to make sure the for loop matches the length of the command.
        received = Serial.read();
        Serial.print(received, BIN);    //Display the received transmission as a binary value. Can also use DEC to show value as a decimal, or use no format modifier to display the ASCII character.
        Serial.print("\t");             //Separate each transmitted byte by a tab
      }
    }
/*******************************************************************/

    Serial.println();
    Serial.print("Sent Command!");      //Print to monitor to signify that transmission has finished. Review moinitor output to check for errors.
  }
}

void printBits(byte myByte){                //Helper function to display the received transmision as a full 8 bit byte including leading zeros - by default leading zeros are truncated when displyed in the serial monitor.
 for(byte mask = 0x80; mask; mask >>= 1){   //hexadecimal value 0x80 is equivalent to decimal 127 which is equivalent to 10000000 in binary. The single 1 as the MSB is used for bit wise comparison after each bit shift to the right.
   if(mask  & myByte)
       Serial.print('1');
   else
       Serial.print('0');
 }
}
