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
    struct json_object* commandValue;
    struct json_object* sensorObj;
    struct json_object* id;
    struct json_object* type;
    struct json_object* name;
    struct json_object* messType;
    
    const char * message;
    struct sensorType * current = sensors;

    
    configObj = json_object_new_object();
    sensorsList = json_object_new_array();
    commandsList = json_object_new_array();
    configuration = json_object_new_object();

    messType = json_object_new_int(CONFIGURATION);
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
            puts("Sensor: Unkown type");
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
    
  while (currentAction != 0) {
    commandValue = json_object_new_string(currentAction->nom);
    json_object_array_add(commandsList, commandValue);
    currentAction = currentAction->nextAction;
  } 
  
  json_object_object_add(configObj, "commands", commandsList);
  
  json_object_object_add(configuration, "message", configObj);
  message = json_object_to_json_string(configuration);
  guiNetworkSend(message, strlen(message), mqSend);
}

void processTypeCommand(struct json_object* command){
  const char* com = json_object_get_string(command);
  printf("Commande : %s\n", com);
  applyActionByName(com);
}

void processTypeUpdate(struct json_object* update, mqd_t mqSend){
  
  char* id;
  const char * sending;
  int i,lenght;
  struct json_object* tmp;
  struct json_object* dataObj;
  struct json_object* message;
  struct json_object* value;
  struct json_object* messType;
  struct json_object* response;
  struct sensorType * current;
  struct dataPRESENCE dataPres;
  struct dataTEMPERATURE dataTemp;
  puts("geting lenght");
  lenght = json_object_array_length(update);
  puts("got lenght");
 
  message = json_object_new_array();
  response = json_object_new_object();

  for(i = 0; i < lenght; i++){
    tmp = json_object_array_get_idx(update, i);
    id = json_object_get_string(tmp);
    current = getSensor(id); 
    
    if(current->type != PRESENCE){
      dataTemp = *(struct dataTEMPERATURE*)(current->data);
      value = json_object_new_int(dataTemp.temp);
    } else{
      dataPres = *(struct dataPRESENCE*)(current->data);
      value = json_object_new_int(dataPres.presence);
    }
    
    dataObj = json_object_new_object();
    json_object_object_add(dataObj, "id", tmp);
    json_object_object_add(dataObj, "value", value);

    json_object_array_add(message, dataObj);

    if(current->type != PRESENCE){
      dataObj = json_object_new_object();
      json_object_object_add(dataObj, "id", tmp);
      value = json_object_new_int(dataPres.luminosite);
      json_object_object_add(dataObj, "value", value);
      json_object_array_add(message, dataObj);
    }
  }

  messType = json_object_new_int(DATA);
  json_object_object_add(response, "type", messType);
  json_object_object_add(response, "message", message);

  sending = json_object_to_json_string(response);
  guiNetworkSend(sending, strlen(sending), mqSend);
}

void processTypeClose(){
  puts("Close detected!");
}

void processCommand(char * command, mqd_t mqSend){
  
  struct json_object* objCommand;
  struct json_object* objPart;
  struct json_object* message;
  int type;

  objCommand = json_tokener_parse(command);
  objPart = json_object_object_get(objCommand, "type");
  message = json_object_object_get(objCommand, "message");
  type = json_object_get_int(objPart);
  
  switch(type){

    case INITIALISE: 
      puts("Initialisation detected");
      processTypeInitialise(mqSend);
      break;

    case CONFIGURATION:
      puts("Type Configuration Error");
      break;

    case COMMAND:
      puts("Command detected");
      processTypeCommand(message);
      break;

    case UPDATE:
      puts("Update detected");
      processTypeUpdate(message, mqSend);
      break;

    case DATA:
      puts("type Data Error");
      break;

    case CLOSE:
      puts("Close detected");
      processTypeClose();
      break;

    default:
      puts("Command: Unknown Type");

  }

}

