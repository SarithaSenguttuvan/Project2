/** NG
 * @file 
 *		log.h 
 * @brief 
 *		This file contains function declarations for variables used in log.c
 *		 
 * @author 
 *		Saritha Senguttuvan, Savitha Senguttuvan
 * @date 
 *		December 12,2017
 *
 */

#ifndef INCLUDES_LOG_TASK_H_
#define INCLUDES_LOG_TASK_H_

#include <stdint.h>
/* Enum for the logger level */
typedef enum
{	
	LOG_NONE,
	LOG_FATAL,
	LOG_ERROR,
	LOG_DEBUG,
	LOG_WARN,
	LOG_INIT,
	LOG_INFO
}LOGGER_level;

/* Structure for log packets */
typedef struct 						/* Log structure */
{
	LOGGER_level logLevel; 			/* Log type */
	uint32_t log_timestamp; 		/* Time */
	void * logPayload; 				/* Log associated data */
}log_t;

/**
 * @brief 
 * 		This function is the pthread function for the Log task
 * @return 
 *      void
 */
void *logTaskFunc(void *arg);



#endif
