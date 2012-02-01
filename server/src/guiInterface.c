#include <stdio.h>

#include "../libHeaders/json.h"
#include "guiInterface.h"
#include "guiNetwork.h"
#include "sensors.h"
#include "actions.h"

typedef enum {
    INITIALISE = 1,
    CONFIGURATION = 2,
    COMMAND = 3,
    UPDATE = 4,
    DATA = 5,
    CLOSE = 6
}MsgTypes;

void processTypeInitialise(mqd_t mqSend){
  
    struct json_object* configuration;
    struct json_object* configObj;
    struct json_object* sensorsList;
    struct json_object* commandsList;
    struct json_object* commandObj;
    struct json_object* commandValue;
    struct json_object* sensorObj;
    struct json_object* id;
    struct json_object* type;
    struct json_object* name;
    struct json_object* messType;
    
    const char * message;
    struct sensorType * current = sensors;

    puts("Initialisation detected");
    
    configObj = json_object_new_object();
    sensorsList = json_object_new_array();
    configuration = json_object_new_object();

    messType = json_object_new_int(2);
    json_object_object_add(configuration, "type", messType);

    while (current != NULL) {
        switch(current->type){
          
          case TEMPERATURE:
            type = json_object_new_string("Temperature");
            break;

          case CONTACT:
            type = json_object_new_string("Contact");
            break;
          
          case INTERRUPTEUR:
            type = json_object_new_string("Interupteur");
            break;
          
          case PRESENCE:
            type = json_object_new_string("Presence");
            break;

          default:
            puts("Unkown type");
        }
        name = json_object_new_string(current->id); 
        id = json_object_new_string(current->id); 
        
        sensorObj = json_object_new_object();
        json_object_object_add(sensorObj, "id", id);
        json_object_object_add(sensorObj, "name", name);
        json_object_object_add(sensorObj, "type", type);

        json_object_array_add(sensorsList, sensorObj);

        if(current->type == PRESENCE){
          type = json_object_new_string("Luminosite");
          sensorObj = json_object_new_object();
          json_object_object_add(sensorObj, "id", id);
          json_object_object_add(sensorObj, "name", name);
          json_object_object_add(sensorObj, "type", type);
        
          json_object_array_add(sensorsList, sensorObj);
        }
        
        current = current->nextSensor;
    }

  json_object_object_add(configObj, "sensors", sensorsList);

    struct action_t * currentAction = actions;
    
    /*while (currentAction != 0) {

        currentAction = currentAction->nextAction;
    }*/
  json_object_object_add(configuration, "message", configObj);
  message = json_object_to_json_string(configuration);
  guiNetworkSend(message, strlen(message), mqSend);
}

void processTypeConfiguration(){
  puts("Type Configuration Error");
}

void processTypeCommand(struct json_object* command){
  const char* com = json_object_get_string(command);
  printf("Commande : %s\n", com);
}

void processTypeUpdate(struct json_object* update, mqd_t mqSend){
  
  const char* id;
  int i,lenght;
  struct json_object* tmp;
  struct json_object* dataObj;
  lenght = json_object_array_length(update);
 
  dataObj = json_object_new_array();

  for(i = 0; i < lenght; i++){
    tmp = json_object_array_get_idx(update, i);
    id = json_object_get_string(tmp);

  }

}

void processTypeData(){
  puts("type Data Error");
}

void processTypeClose(){
  puts("Close detected!");
}

void processCommand(char * command, mqd_t mqSend){
  
  struct json_object* objCommand;
  struct json_object* objPart;
  int type;

  objCommand = json_tokener_parse(command);
  objPart = json_object_object_get(objCommand, "type");

  type = json_object_get_int(objPart);
  
  switch(type){

    case INITIALISE: 
      processTypeInitialise(mqSend);
      break;

    case CONFIGURATION:
      processTypeConfiguration();
      break;

    case COMMAND:
      processTypeCommand(objPart);
      break;

    case UPDATE:
      processTypeUpdate(objPart, mqSend);
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

