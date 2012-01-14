#include <stdio.h>
#include <json/json.h>

#include "guiInterface.h"

typedef enum {
    INITIALISE = 1,
    CONFIGURATION = 2,
    COMMAND = 3,
    UPDATE = 4,
    DATA = 5,
    CLOSE = 6
}MsgTypes;

void processTypeInitialise(){
}

void processTypeConfiguration(){
  puts("Type Configuration Error");
}

void processTypeCommand(){
}

void processTypeUpdate(){
}

void processTypeData(){
  puts("type Data Error");
}

void processTypeClose(){
}

void processCommand(char * command){
  
  struct json_object* objCommand;
  struct json_object* objPart;
  int type;

  objCommand = json_tokener_parse(command);
  objPart = json_object_object_get(objCommand, "type");

  type =  json_object_get_int(objPart);
  
  switch(type){

    case INITIALISE: 
      processTypeInitialise();
      break;

    case CONFIGURATION:
      processTypeConfiguration();
      break;

    case COMMAND:
      processTypeCommand();
      break;

    case UPDATE:
      processTypeUpdate();
      break;

    case DATA:
      processTypeData();
      break;

    case CLOSE:
      processTypeClose();
      break;

    default:
      puts("Unknown Type");

  }

}

