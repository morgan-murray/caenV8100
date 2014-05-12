#ifndef V8100_H
#define V8100_H

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include "ftd2xx.h"

#include <iostream>
#include <cstring>
#include <string>
#include <sstream>

#define RESPONSE_TIME 1 // the number of seconds that the board needs
                        // to respond to a normal request
#define BUFFER_SIZE 512
#define PRINT_ERR(name, err) fprintf(stderr,"Function %s failed with error code %lu in line %d of file %s\n", name, err, __LINE__, __FILE__)

// Header file for C++ module related to CAEN N1470 4-channel HV NIM module
// Note that the documentation switches between iset and ilim for the same quantity
// We restrict ourselves to iset for consistency.

class V8100{
	
 private:

  // Board ID
  int BD_;
  // Device handle
  FT_HANDLE dev_; 

  std::string crate_name_, crate_status_, num_chan_, ps_temp_, fan_speed1_;
  std::string fan_speed2_, fan_speed3_, fan_temp_;

  // Forms a command to send to the module
  // Takes a name from the private list above, a target of what to replace and a replacement
  // Returns a char* to the command that should be sent to the module.
  char * formCommand(std::string, std::string, std::string);
  // Forms a command to send to the module in the case where the command string is already well-formed
  char * formCommand(std::string);

  // Writes a command to the V8100 and checks to make sure that it is written.
  // Takes a command string and returns 0 if no problems.
  int writeCommand(char *);

  // Get response from the board following a command
  // Takes std::string pointer to store response string 
  // Return 0 if response arrives, non-zero if response failed. In that case, response string set to NULL
  int getResponse(std::string *);

  // Parse the response and determine if error
  // If error, call parseError()
  // Return 0 if OK, non-zero if error.
  int parseResponse(std::string *, int type, double *value);
  int parseResponse(std::string *response, int type, std::string * value);

  // Takes a channel number as argument and checks that it is within [0,3]
  void channelCheck(int);

  

 public:

  V8100(int);

  // Prints the current status to stdout
  // Returns status field
  double getCrateStatus();
  // Parses the crate's status field supplied as an argument
  void parseCrateStatus(double);

  
  std::string * getCrateName();
  // Gets the number of channels that can be read out on the crate.
  double getNumberChannels();
  // Gets the temperature of the power supply and fan tray in the crate.
  double getPSTemp();
  double getFansTemp();
  // speed must be an array of 3 doubles.
  double *getFanSpeeds(double * speed); 

  // Make the connection to the physical module. Sets private connected variable.
  int makeConnection();
  int dropConnection();

  // Returns 0 on success, non-zero on failure. Takes a channel number [0->3]

  // Prints Board name to stdout
  // returns -1 in case of error
  std::string * readCrateName();

};

#endif
