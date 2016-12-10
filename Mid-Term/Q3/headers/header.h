
/*************** type.h file *****************/
#ifndef HEADER_H
#define HEADER_H

extern int color;

char *statusArray[] = {"FREE", "READY", "SLEEP", "BLOCK", "ZOMBIE", "RUNNING"};

char *pname[] = {"Sun", "Mercury", "Venus", 
                 "Earth",  "Mars", "Jupiter", 
                 "Saturn", "Uranus", "Neptune"};

int  procSize = sizeof(PROC);

#endif
