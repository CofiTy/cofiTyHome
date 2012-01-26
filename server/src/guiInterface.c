#include <stdio.h>

#include "../libHeaders/json.h"
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
  puts("Initialisation detected");
}

void processTypeConfiguration(){
  puts("Type Configuration Error");
}

void processTypeCommand(struct json_object* command){
  const char* com = json_object_get_string(command);
  printf("Commande : %s\n", com);
}

void processTypeUpdate(struct json_object* update){
  
  const char* string;
  int i,lenght;
  struct json_object* tmp;
  lenght = json_object_array_length(update);
  for(i = 0; i < lenght; i++){
    tmp = json_object_array_get_idx(update, i);
    string = json_object_get_string(tmp);
  }

}

void processTypeData(){
  puts("type Data Error");
}

void processTypeClose(){
  puts("Close detected!");
}

void processCommand(char * command){
  
  struct json_object* objCommand;
  struct json_object* objPart;
  int type;

  objCommand = json_tokener_parse(command);
  objPart = json_object_object_get(objCommand, "type");

  type = json_object_get_int(objPart);
  
  switch(type){

    case INITIALISE: 
      processTypeInitialise();
      break;

    case CONFIGURATION:
      processTypeConfiguration();
      break;

    case COMMAND:
      processTypeCommand(objPart);
      break;

    case UPDATE:
      processTypeUpdate(objPart);
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

