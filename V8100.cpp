
#include "V8100.h"

// Default constructor. Make everything apart from the board number zero and get actual values in initialize function
// String commands are taken from the N1470 manual available at caen.it
V8100::V8100(int boardNumber) : 
  BD_(boardNumber),
  crate_name_("$CMD:MON,CH:8,PAR:CRNAME"),
  crate_status_("$CMD:MON,CH:8,PAR:CRSTAT"),
  num_chan_("$CMD:MON,CH:8,PAR:NUMCH"),
  ps_temp_("$CMD:MON,CH:8,PAR:PSTEMP"),
  fan_speed1_("$CMD:MON,CH:8,PAR:FAN1"),
  fan_speed2_("$CMD:MON,CH:8,PAR:FAN2"),
  fan_speed3_("$CMD:MON,CH:8,PAR:FAN3"),
  fan_temp_("$CMD:MON,CH:8,PAR:FUTEMP"){

#ifdef DEBUG
  std::cerr << "Initialised V8100 module" << std::endl;
#endif
};


char * V8100::formCommand(std::string command, std::string target, std::string replacement){

  int position;
  char * cmd;
  unsigned int bufLen;

  std::string newCommand;

  position = command.find(target);
  if (position < 0){
    std::cerr << "Target string " << target << " not found" << std::endl;
    exit(1);
  }
  newCommand = command.replace(position, target.size(), replacement);
  
  // V8100 accepts C-style strings, now convert from std::string to c_str style
  // need to append a Windows-style line ending (\<cr>\<lf>) to make N1470 realise it's the end of the command
  bufLen = newCommand.size() + 3;
  cmd = (char *)malloc(bufLen);

  if (cmd == NULL){
    fprintf(stderr,"No memory allocation possible!\n");
    return NULL;
  }
  
  std::strcpy(cmd,command.c_str());
  strncat(cmd,"\r\n",2);
  
  return cmd;
 
} 

char * V8100::formCommand(std::string command){

  char * cmd;
  unsigned int bufLen;

  // V8100 accepts C-style strings, now convert from std::string to c_str style
  // need to append a Windows-style line ending (\<cr>\<lf>) to make N1470 realise it's the end of the command
  bufLen = command.size() + 3;
  cmd = (char *)malloc(bufLen);

  if (cmd == NULL){
    fprintf(stderr,"No memory allocation possible!\n");
    return NULL;
  }
  
  std::strcpy(cmd,command.c_str());
  strncat(cmd,"\r\n",2);
  
  return cmd;
} 

  

int V8100::makeConnection(){

#ifndef NO_DEVICE
  
  unsigned long ret;
  
  if ((ret = FT_Open(0, &dev_)) != FT_OK){
    
    PRINT_ERR("FT_Open", ret);
    return -1;
    
  }

  if ((ret = FT_SetBaudRate(dev_, FT_BAUD_9600)) != FT_OK){
    
    PRINT_ERR("FT_SetBaudRate",ret);
    return -2;
    
  }
  
  
  if ((ret = FT_SetDataCharacteristics(dev_, FT_BITS_8, FT_STOP_BITS_1, FT_PARITY_NONE )) != FT_OK){
    
    PRINT_ERR("FT_SetDataCharacteristics",ret);
    return -3;
    
  }
  
  
  if ((ret = FT_Purge(dev_, (FT_PURGE_RX & FT_PURGE_TX))) != FT_OK){
    
    PRINT_ERR("FT_Purge", ret);
    return -4;
    
  }
  
#endif

#ifdef DEBUG
  fprintf(stderr,"Connected to the device with Board ID: %d\n",BD_);
#endif
	
  return 0;
  
};	


int V8100::dropConnection(){

#ifndef NO_DEVICE

	unsigned long ret;

	if ((ret = FT_Purge(dev_, (FT_PURGE_RX & FT_PURGE_TX))) != 0){

		PRINT_ERR("FT_Purge", ret);
		return -1;

	}


	if ((ret = FT_Close(dev_)) != FT_OK){

		PRINT_ERR("FT_Close", ret);
		return -2;
	}

#endif

#ifdef DEBUG
	fprintf(stderr,"Disconnected from the device with Board ID: %d\n",BD_);
#endif
	return 0;
};	



void V8100::channelCheck(int channel){

  if (channel < 0 || channel >=4){
    std::cerr << "Channel call of " << channel << " not understood" << std::endl;
    exit(1);
  }

}
    
int V8100::writeCommand(char *cmd){

  int bufLen, bufWrit, ret;
  bufLen = strlen(cmd); 

#ifdef DEBUG_MAX
  std::cerr << "Writing the following command to the device: " << cmd << std::endl;
#endif

#ifndef NO_DEVICE

  if ((ret = FT_Write(dev_,cmd,bufLen,(LPDWORD) &bufWrit)) != FT_OK){

    PRINT_ERR("FT_Write", (long unsigned)ret);
    free(cmd);	
    return -1;
  }

#else
  std::cerr << "Faking successful write" << std::endl;
  bufWrit = bufLen;
#endif
  
  if(bufWrit != bufLen){
    fprintf(stderr, "Buffersize mismatch: bufLen %u \t bufWrit %u\n",bufLen,bufWrit);
    return -1;
  }

 return 0;

}

double V8100::getCrateStatus(){
  
  char * cmd;
  std::string *response = new std::string();

  double status;
 
  cmd = this->formCommand(crate_status_);

#ifdef DEBUG_MAX
  std::cerr << "Getting the status of the crate" << std::endl;
#endif

 if (writeCommand(cmd) != 0){

    std::cerr << "There was a problem writing the command to read out the crate status" << std::endl;
    free(cmd);
    delete(response);
    exit(1);
  }

  if (getResponse(response) != 0){
    
    fprintf(stderr,"Could not get response\n");
    free(cmd);
    delete(response);
    exit(1);
  }

#ifdef DEBUG_MAX
  std::cout << "Printing response:" << std::endl;
  std::cout << (*response) << std::endl;
#endif

#ifdef DEBUG_MAX
  std::cout << "Parsing response:" << std::endl;
#endif

  if (parseResponse(response,2,&status) != 0){
    std::cerr << "Could not parse response" << std::endl;
  }

#ifdef DEBUG
  fprintf(stderr,"Status was %x\n",(unsigned)status);
#endif

  // No memory leaks!                                                      
  free(cmd);
  delete(response);
  parseCrateStatus(status);
  return status;

}


std::string * V8100::getCrateName(){
  
  char * cmd;
  std::string *response = new std::string();
  std::string * name = new std::string();

  cmd = this->formCommand(crate_name_);

#ifdef DEBUG_MAX
  std::cerr << "Getting the name of the crate" << std::endl;
#endif

 if (writeCommand(cmd) != 0){

    std::cerr << "There was a problem writing the command to read out the crate name" << std::endl;
    free(cmd);
    delete(response);
    delete(name);
    exit(1);
  }

  if (getResponse(response) != 0){
    
    fprintf(stderr,"Could not get response\n");
    free(cmd);
    delete(response);
    delete(name);
    exit(1);
  }

#ifdef DEBUG_MAX
  std::cout << "Printing response:" << std::endl;
  std::cout << (*response) << std::endl;
#endif

#ifdef DEBUG_MAX
  std::cout << "Parsing response:" << std::endl;
#endif

  if (parseResponse(response,2,name) != 0){
    std::cerr << "Could not parse response" << std::endl;
  }

#ifdef DEBUG
  std::cerr << "Name was " << *name << std::endl;
#endif

  // No memory leaks!                                                      
  free(cmd);
  return name;

}

double V8100::getNumberChannels(){
  
  char * cmd;
  std::string *response = new std::string();
  double num;
 
  cmd = this->formCommand(num_chan_);

#ifdef DEBUG_MAX
  std::cerr << "Getting the number of channels in the crate" << std::endl;
#endif

 if (writeCommand(cmd) != 0){

    std::cerr << "There was a problem writing the command to read out the number of channels" << std::endl;
    free(cmd);
    delete(response);
    exit(1);
  }

  if (getResponse(response) != 0){
    
    fprintf(stderr,"Could not get response\n");
    free(cmd);
    delete(response);
    exit(1);
  }

#ifdef DEBUG_MAX
  std::cout << "Printing response:" << std::endl;
  std::cout << (*response) << std::endl;
#endif

#ifdef DEBUG_MAX
  std::cout << "Parsing response:" << std::endl;
#endif

  if (parseResponse(response,2,&num) != 0){
    std::cerr << "Could not parse response" << std::endl;
  }

#ifdef DEBUG
  fprintf(stderr,"Number of channels was %lf\n",num);
#endif

  // No memory leaks!                                                      
  free(cmd);
  delete(response);
  return num;

}

int V8100::getResponse(std::string * accumulator){
  
  char * buf; 
  
  DWORD bufLenWd, bufWrtWd, status, bufRead;
  int ret;

  buf = (char *)calloc(BUFFER_SIZE, sizeof(char));

  bufLenWd = 0;
  bufWrtWd = 0;
  status = 0;
  bufRead = 0;
  ret = 0;

  sleep(RESPONSE_TIME);
  
#ifdef DEBUG_MAX
  std::cout << "bufLenWd BufWrtWd Status" << std::endl;
  std::cout << bufLenWd << " " << bufWrtWd << " " << status << std::endl;
#endif

  do {

    if((ret = FT_GetStatus(dev_, &bufLenWd, &bufWrtWd, &status)) != FT_OK){
      PRINT_ERR("FT_GetStatus",(long unsigned)ret);
      return 1;
    }
      
    if (bufLenWd > BUFFER_SIZE){

      buf = (char *)realloc(buf,bufLenWd);
      
      if (buf == NULL){

	fprintf(stderr,"OOM ERROR on buffer length %ul\n",bufLenWd);
	return 1;
      }
    }      
    
    if((ret = FT_Read(dev_,buf, bufLenWd, &bufRead))!=FT_OK){
      PRINT_ERR("FT_Read",(long unsigned)ret);
      return 1;
    }

    // Null-terminate the response
    buf[bufLenWd] = '\0';

#ifdef DEBUG_MAX
    std::cerr << "Accumulating buffer" << std::endl;	    
    puts(buf);
#endif
    accumulator->append(buf);
  }  while (bufLenWd!=0);
  
  return 0; 
}

int V8100::parseResponse(std::string *response, int type, double *value){

  // should tokenise response string at this point in case there is more than one response in the buffer!
  int loc;	
  loc = response->find("OK");
  if (loc == -1)
    {	
      std::cerr << loc << std::endl;
      std::cerr << "Something has gone wrong. Command failed!" << std::endl;
      return -9;
    }		

  if(type == 2){

    if (sscanf(response->substr(loc).c_str(),"OK,VAL:%lf",value) != 1){
      
      std::cerr << "Could not interpret a value from the response: " << *response << std::endl;
      return -2;
    }			
  }
  
  return 0;
  
}


int V8100::parseResponse(std::string *response, int type, std::string * value){

  // should tokenise response string at this point in case there is more than one response in the buffer!
  int loc;
  char buf[80];

  loc = response->find("OK");
  if (loc == -1)
    {	
      std::cerr << loc << std::endl;
      std::cerr << "Something has gone wrong. Command failed!" << std::endl;
      return -9;
    }		

  if(type == 2){

    if (sscanf(response->substr(loc).c_str(),"OK,VAL:%s",buf) != 1){
      std::cerr << "Could not interpret a value from the response: " << *response << std::endl;
      return -2;
    }			
  }
  

  *value = buf;
  return 0;
  
}	

void V8100::parseCrateStatus(double statusDb){
    
    int status = (int)statusDb;

    if ((status>>0) & 0x1)
      std::cerr << "Crate is on" << std::endl;
    else
      std::cerr << "Crate is off" << std::endl;

    if ((status>>1) & 0x1)
      std::cerr << "Problems on local power supply" << std::endl;

    if ((status>>2) & 0x1)
      std::cerr << "Temperature on the power supply is out of range" << std::endl;
    
    // bit 3 is not used???

    if ((status>>4) & 0x1)
      std::cerr << "Problems on the main power supply" << std::endl;

    if ((status>>5) & 0x1)
      std::cerr << "VME system failure is active" << std::endl;

    // Bits 6..8 are not used
 
    if ((status>>9) & 0x1)
      std::cerr << "Fans are on" << std::endl;

    if ((status>>9) & 0x1)
      std::cerr << "Fan speed is out of range" << std::endl;

    if ((status>>10) & 0x1)
      std::cerr << "Fan temperature is out of range" << std::endl;
  }
