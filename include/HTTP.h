#ifndef HTTP_H
#define HTTP_H
#include <stdlib.h>
#include "structs.h"

#define KB(x)   ((size_t) (x) << 10)
#define MB(x)   ((size_t) (x) << 20)
#define FILE_SIZE MB(2)
#define HEADER_SIZE KB(1)
#define RESPONSE_SIZE FILE_SIZE + HEADER_SIZE + 1
#define DATE_SIZE 37
#define WWW "/"

/**
 * @file HTTP.h
 * @brief functions to parse HTTP requests from client and create a response
 */

/**
 * @brief Directs the flow to create a response or continue buffer the response if it already is created
 */
int HTTP_Request(struct HTTP_buffer *HTTP);
/**
 * @brief Handles the parsing functions and call the corresponding response function
 */
int parseRequest(struct HTTP_buffer *HTTP);
/**
 * @brief Parses the HTTP method from the request
 */
void parseMethod(struct HTTP_buffer *HTTP);
/**
 * @brief Parses the path from the request
 */
void parsePath(struct HTTP_buffer *HTTP);
/**
 * @brief Parses the HTTP version from the request
 */
void parseVersion(struct HTTP_buffer *HTTP);
/**
 * @brief Creates a GET response
 */
int GET(struct HTTP_buffer *HTTP);
/**
 * @brief Creates a HEAD response
 */
int HEAD(struct HTTP_buffer *HTTP);
/**
 * @brief Creates an Error response
 */
int ERROR(struct HTTP_buffer *HTTP);
/**
 * @brief Splits the response and place the parts in the buffer
 */
int sendBuffer(struct HTTP_buffer *HTTP);
/**
 * @brief Copies file content to the response
 */
void readFile(char *content, struct HTTP_buffer *HTTP);
/**
 * @brief Creates the response header
 * @param header the string to put the created header in
 * @param HTTP contains data values to put in the header
 */
void createHeader(char *header, int length, struct HTTP_buffer *HTTP);
/**
 * @brief Makes a string of the correct date format to the HTTP header
 */
void datetime(char *datestring, const time_t *timestamp);
/**
 * @brief returns true if it matches a HTTP method that isn't implemented
 */
int cmpNotImpl(char *method);

#endif
