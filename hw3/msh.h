#ifndef MSH_H
#define MSH_H


void sigChildHandler(int sig); //reaping children

void sigIntHandlerP(int sig); //Parent sigint

int allSpaces(char input[1000]); //Handles strings of only spaces and enter

int main(int argc); //Argv not needed, only ./msh

#endif