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
  CLOSE = 6,
  HISTORY = 7,
  HDATA = 8,
  LOGS = 9,
  LDATA = 10,
  EDITS = 11,
  EDATA = 12 
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

  pthread_mutex_lock(&sensorsMutex);
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
        
      case HORLOGE:
        type = json_object_new_string("Horloge");
        break;

      default:
        puts("Sensor: Unkown type");
    }
    if(strcmp(current->name ,"")){
      name = json_object_new_string(current->name); 
    }else{
      name = json_object_new_string(current->id); 
    }
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
  pthread_mutex_unlock(&sensorsMutex);

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
  typeCapteur currentType;
  struct json_object* tmp;
  struct json_object* dataObj;
  struct json_object* message;
  struct json_object* value;
  struct json_object* messType;
  struct json_object* response;
  struct json_object* type;
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
    pthread_mutex_lock(&sensorsMutex);
    current = getSensor(id); 
    currentType = current->type;    
    switch(currentType){

      case TEMPERATURE:
        type = json_object_new_string("Temperature");
        break;

      case CONTACT:
        type = json_object_new_string("Contact");
        break;

      case INTERRUPTEUR:
        type = json_object_new_string("Interupteur");
        break;
        
      case HORLOGE:
        type = json_object_new_string("Horloge");
        break;

      case PRESENCE:
        type = json_object_new_string("Presence");
        break;

      default:
        puts("Sensor: Unkown type");
    }

    if(currentType != PRESENCE){
      dataTemp = *(struct dataTEMPERATURE*)(current->data);
      value = json_object_new_int(dataTemp.temp);
    } else{
      dataPres = *(struct dataPRESENCE*)(current->data);
      value = json_object_new_int(dataPres.presence);
    }

    pthread_mutex_unlock(&sensorsMutex);
    dataObj = json_object_new_object();
    json_object_object_add(dataObj, "id", tmp);
    json_object_object_add(dataObj, "value", value);
    json_object_object_add(dataObj, "type", type);

    json_object_array_add(message, dataObj);

    if(currentType == PRESENCE){
      type = json_object_new_string("Luminosite");
      dataObj = json_object_new_object();
      json_object_object_add(dataObj, "id", tmp);
      value = json_object_new_int(dataPres.luminosite);
      json_object_object_add(dataObj, "value", value);
      json_object_object_add(dataObj, "type", type);
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

void processTypeHistory(struct json_object* history, mqd_t mqSend){
  struct json_object* objId;
  struct json_object* objRollback;
  
  struct json_object* response;
  struct json_object* messType;
  struct json_object* message;
  const char * sending;

  int id;
  int nbValues;

  //Recuperation commande
  objId = json_object_object_get(history, "id");
  objRollback = json_object_object_get(history, "rollback");

  id = json_object_get_int(objId);
  nbValues = json_object_get_int(objRollback);

  //Construction reponse
  response = json_object_new_object();

  messType = json_object_new_int(HDATA);
  message = json_object_new_array();


  json_object_object_add(response, "type", messType);
  json_object_object_add(response, "message", message);

  sending = json_object_to_json_string(response);
  guiNetworkSend(sending, strlen(sending), mqSend);

}

void readWholeFile(const char * fileName, char * buffer){
  FILE *file;
  unsigned long fileLen;
  if((file = fopen(fileName, "r")) == NULL)
  {
    printf("ERROR: No File to open...\n");
    exit(ERROR);
  }

  //Get file length
  fseek(file, 0, SEEK_END);
  fileLen=ftell(file);
  fseek(file, 0, SEEK_SET);

  //Allocate memory
  buffer=(char *)malloc(fileLen+1);
  if (!buffer)
  {
    fprintf(stderr, "Memory error!");
    fclose(file);
    exit(ERROR);;
  }

  //Read file contents into buffer
  fread(buffer, fileLen, 1, file);
  fclose(file);
}

void processTypeEdits(struct json_object * typeId, mqd_t mqSend){
  int fileType;
  const char * sending;
  struct json_object * response;
  struct json_object* type;
  struct json_object* fileObj;
  struct json_object* name;
  struct json_object* file;
  char *buffer;

  fileType = json_object_get_int(typeId);
  response = json_object_new_object();
  fileObj = json_object_new_object();
  type = json_object_new_int(EDATA);
  json_object_object_add(response, "type", type);

  switch(fileType){
    case F_RULES:
      name = json_object_new_int(F_RULES);
      readWholeFile("server/config/rules", buffer);
      break;

    case F_ACTIONS:
      name = json_object_new_int(F_ACTIONS);
      readWholeFile("server/config/actions", buffer);
      break;

    case F_ACTIONNEURS:
      name = json_object_new_int(F_ACTIONNEURS);
      readWholeFile("server/config/actionneurs", buffer);
      break;

    case F_SENSORS:
      name = json_object_new_int(F_SENSORS);
      readWholeFile("server/config/sensors", buffer);
      break;

    default:
      puts("File: Unkown type");
  }

  json_object_object_add(fileObj, "name", name);
  file = json_object_new_string(buffer);
  json_object_object_add(fileObj, "file", file);
  json_object_object_add(response, "message", fileObj);

  sending = json_object_to_json_string(response);
  guiNetworkSend(sending, strlen(sending), mqSend);
  free(buffer);
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

    case HISTORY:
      puts("History detected");
      processTypeHistory(message, mqSend);
      break;

    case HDATA:
      puts("type HDataerror");
      break;

    case LOGS:
      puts("type Logs detected");
      break;

    case LDATA:
      puts("type LData Error");
      break;

    case EDITS:
      puts("type EDITS detected");
      processTypeEdits(message, mqSend);
      break;

    case EDATA:
      puts("type EData Error");
      break;

    default:
      puts("Command: Unknown Type");

  }

}

