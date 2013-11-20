/*
 *  Copyright (C) 2004 Steve Harris, Uwe Koloska
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU Lesser General Public License as
 *  published by the Free Software Foundation; either version 2.1 of the
 *  License, or (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU Lesser General Public License for more details.
 *
 *  $Id$
 */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <iostream>
#include <vector>
#include <cstring>
#include <sstream>
#include <algorithm>
#include <map>

#include "lo/lo.h"
using namespace std;

int done = 0;

struct member {
    int pid;
    string hostname;
    lo_timetag timetag;
};

class memberCompare {
public:
    bool operator()(const char * a, const char * b)
    {
        int diff = strcmp(a,b);
        if(diff == -1)
        {
            return 1;
        }
        return 0;
    }
};

std::map<string, member> members;

void memberCleanup(map<string, member> *checkMems);

void error(int num, const char *m, const char *path);

int ping_handler(const char *path, const char *types, lo_arg ** argv,
                int argc, void *data, void *user_data);

int quit_handler(const char *path, const char *types, lo_arg ** argv,
                 int argc, void *data, void *user_data);

int main()
{
    /* make address for multicast ip
     * pick a port number for you by passing NULL as the last argument */

    lo_address t = lo_address_new("224.0.0.1", "7770");
    // lo_server multi = lo_server_new_multicast("drone", "7771", error);
    /* start a new server on port 7770 */
    lo_server_thread st = lo_server_thread_new_multicast("224.0.0.1", "7770", error);

    /* add method that will match the path /foo/bar, with two numbers, coerced
     * to float and int */
    lo_server_thread_add_method(st, "/ping", "is", ping_handler, NULL);

    /* add method that will match the path /quit with no args */
    lo_server_thread_add_method(st, "/quit", "", quit_handler, NULL);

    lo_server_thread_start(st);
    
    
    while (!done) {
#ifdef WIN32
        Sleep(1);
#else
        usleep(1000000);
#endif
        char hostname[128] = "";

        gethostname(hostname, sizeof(hostname));
        
        int pid = getpid();
        lo_send(t, "/ping", "is", pid, hostname);
        memberCleanup(&members);
    }

    lo_server_thread_free(st);

    return 0;
}

void memberCleanup(map<string, member> *checkMems)
{
    map<string, member>::iterator iter;
    lo_timetag now;
    lo_timetag_now(&now);
    for (iter = checkMems->begin(); iter != checkMems->end(); iter++)
    {
        member iterMem = iter->second;
        if ((now.sec - iterMem.timetag.sec) > 3)
        {
            cerr << "DISCONNECTED ~ PID: " << iterMem.pid << " || HOSTNAME: " << iterMem.hostname << endl;
            checkMems->erase(iter->first);
        }
    }
}

void error(int num, const char *msg, const char *path)
{
    printf("liblo server error %d in path %s: %s\n", num, path, msg);
    fflush(stdout);
}

/* catch any incoming messages and display them. returning 1 means that the
 * message has not been fully handled and the server should try other methods */
int ping_handler(const char *path, const char *types, lo_arg ** argv,
                int argc, void *data, void *user_data)
{
    member messageSender;
    messageSender.pid = argv[0]->i;
    messageSender.hostname = (char *)argv[1];
    lo_timetag_now(&messageSender.timetag);
    ostringstream convert;
    convert << messageSender.pid;
    string key = messageSender.hostname + ":" + convert.str();
    if (members[key].timetag.sec == 0)
    {
        cerr << "CONNECTED ~ Hostname: " << messageSender.hostname << " || PID : " <<  messageSender.pid << " || timestamp : " << messageSender.timetag.sec << endl;
    }
    members[key] = messageSender;
    fflush(stdout);

    return 0;
}

int quit_handler(const char *path, const char *types, lo_arg ** argv,
                 int argc, void *data, void *user_data)
{
    done = 1;
    printf("quiting\n\n");
    fflush(stdout);

    return 0;
}

/* vi:set ts=8 sts=4 sw=4: */
