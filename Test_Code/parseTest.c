#include <stdio.h>
#include <stdint.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>

#define BUFFER_LENGTH    (1024)

#define TIVA_MAIN_TASK_ID 6
#define TIVA_SOCKET_TASK_ID 7
#define TIVA_LIGHT_TASK_ID 8
#define TIVA_ACCELEROMETER_TASK_ID 9

#define TIVA_DELIMITER ("$")

typedef enum
{
    TIVA_MSGID_HB_RESP,
    TIVA_MSGID_HB_REQ,
    TIVA_MSGID_LIGHT_REQ,
    TIVA_MSGID_LIGHT_DATA,
    TIVA_MSGID_ACCELEROMETER_REQ,
    TIVA_MSGID_ACCELEROMETER_DATA,
    TIVA_MSGID_LIGHT_AUTO,
    TIVA_MSGID_LIGHT_CTRL,
    TIVA_MSID_ACC_ALERT,
    TIVA_MSID_LIGHT_ALERT,
    TIVA_MSGID_ERROR,
    TIVA_MSGID_LOGMSG
}tiva_msgid_t;

/* Structure for a message ON TIVA*/
typedef struct
{
    tiva_msgid_t tivaMsgId;
    uint8_t tivaMsgSrcTask;             /* Source Task ID */
    size_t tivaMsgPayloadLen;           /* Msg length  */
    int32_t tivadata;
    char * tivaMsgPayload;              /* Msg associated data */
}tiva_msgStruct_t;

void main()
{
    tiva_msgStruct_t msg;
    tiva_msgStruct_t rec_msg;
    
    char strData[] = "Saritha Savitha$KSJDF$LKAJSKLDH$";
    
    //char *msgField = (char *)malloc(sizeof(char)*BUFFER_LENGTH);
    
    char int_stringFromTIVA[BUFFER_LENGTH];
    char rec_buff[BUFFER_LENGTH];
    msg.tivaMsgId = TIVA_MSID_ACC_ALERT;
    msg.tivaMsgSrcTask = TIVA_ACCELEROMETER_TASK_ID;
    msg.tivaMsgPayloadLen = 0;
    msg.tivaMsgPayload = strData;
    msg.tivadata = 0;
    
    rec_msg.tivaMsgPayload = (char *) malloc(sizeof(char)*20);
    
    //sprintf(int_stringFromTIVA, "%d %d %ld %s", msg.tivaMsgId, msg.tivaMsgSrcTask , msg.tivaMsgPayloadLen , msg.tivaMsgPayload);
    
    // For TIVA_MSGID_ACCELEROMETER_DATA
    //sprintf(int_stringFromTIVA, "%d %d %ld %d %s", msg.tivaMsgId, msg.tivaMsgSrcTask , msg.tivaMsgPayloadLen , msg.tivadata, msg.tivaMsgPayload);
    
    //For TIVA_MSID_ACC_ALERT
    sprintf(int_stringFromTIVA, "%d %d %ld %d %s", msg.tivaMsgId, msg.tivaMsgSrcTask , msg.tivaMsgPayloadLen , msg.tivadata, msg.tivaMsgPayload);
    
    printf("The concatenated msg in int_stringFromTIVA is %s\n", int_stringFromTIVA);
    
    char *msgField = strtok(strData, TIVA_DELIMITER);
    while(msgField != NULL)
    {
        printf("%s\n", msgField);
        msgField = strtok(NULL, TIVA_DELIMITER);
    }
    
    printf("The concatenated msg in msgField is %s\n", msgField);

    sscanf(msgField, "%d ", &rec_msg.tivaMsgId);
    printf("The received msgId is %d\n", rec_msg.tivaMsgId);
    
    if(rec_msg.tivaMsgId == TIVA_MSGID_ACCELEROMETER_DATA)
    {
       printf("First\n");
        sscanf(msgField, "%d %d %ld %d %s", &rec_msg.tivaMsgId, &rec_msg.tivaMsgSrcTask, &rec_msg.tivaMsgPayloadLen, &rec_msg.tivadata, msg.tivaMsgPayload);
        rec_msg.tivaMsgPayload = NULL;
        
    }
   
    else if(rec_msg.tivaMsgId == TIVA_MSID_ACC_ALERT)
    {
        printf("Second\n");
        sscanf(msgField, "%d %d %ld %d %s", &rec_msg.tivaMsgId, &rec_msg.tivaMsgSrcTask, &rec_msg.tivaMsgPayloadLen, &rec_msg.tivadata, rec_msg.tivaMsgPayload);
        //rec_msg.tivaMsgPayload = NULL;
        printf("Second\n");
        rec_msg.tivadata = 0;
    }
    
    /* 
    else if(rec_msg.tivaMsgId == TIVA_MSGID_LIGHT_DATA)
    {
       printf("First\n");
        sscanf(msgField, "%d %d %ld %d %s", &rec_msg.tivaMsgId, &rec_msg.tivaMsgSrcTask, &rec_msg.tivaMsgPayloadLen, &rec_msg.tivadata, msg.tivaMsgPayload);
        rec_msg.tivaMsgPayload = NULL;
        
    }
    else if(rec_msg.tivaMsgId == TIVA_MSID_LIGHT_ALERT)
    {
        sscanf(msgField, "%d %d %ld %d %s", &rec_msg.tivaMsgId, &rec_msg.tivaMsgSrcTask, &rec_msg.tivaMsgPayloadLen, &rec_msg.tivadata, msg.tivaMsgPayload);
        //rec_msg.tivaMsgPayload = NULL;
        rec_msg.tivadata = 0;
    }
    else if(rec_msg.tivaMsgId == TIVA_MSGID_LOGMSG)
    {
        sscanf(msgField, "%d %d %ld %d %s", &rec_msg.tivaMsgId, &rec_msg.tivaMsgSrcTask, &rec_msg.tivaMsgPayloadLen, &rec_msg.tivadata, msg.tivaMsgPayload);
        //rec_msg.tivaMsgPayload = NULL;
        rec_msg.tivadata = 0;
    }
    else if(rec_msg.tivaMsgId == TIVA_MSGID_ERROR) // From main task, when heart beat fails
    {
        sscanf(msgField, "%d %d %ld %d %s", &rec_msg.tivaMsgId, &rec_msg.tivaMsgSrcTask, &rec_msg.tivaMsgPayloadLen, &rec_msg.tivadata, msg.tivaMsgPayload);
        //rec_msg.tivaMsgPayload = NULL;
        rec_msg.tivadata = 0;
    }
    */
    
    printf("The received msgID is %d\n", rec_msg.tivaMsgId);
    printf("The received taskID is %d\n", rec_msg.tivaMsgSrcTask);
    printf("The received len is %ld\n", rec_msg.tivaMsgPayloadLen);
    printf("The received int payload is %d\n", rec_msg.tivadata);
    printf("The received str payload is %s\n", rec_msg.tivaMsgPayload);
    
    return;
}