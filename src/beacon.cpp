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
#include <algorithm>

#include "lo/lo.h"
using namespace std;

int done = 0;

struct member {
    int pid;
    string hostname;
    lo_timetag timetag;
};

vector<member> members;

int memberCompare(member a, member b);

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
        usleep(1500000);
#endif
        char hostname[128] = "";

        gethostname(hostname, sizeof(hostname));
        
        int pid = getpid();
        // cerr << "pid: " << pid << " || hostname : " << hostname << endl;
        lo_send(t, "/ping", "is", pid, hostname);
    }

    lo_server_thread_free(st);

    return 0;
}

int memberCompare(member a, member b)
{
    int name = strcmp(a.hostname.c_str(), b.hostname.c_str());
    if(name != 0)
    {
        if (name == -1)
        {
            return 0
        }
        return 1
    }
    bool test =  a.pid < b.pid;
    return a.pid < b.pid;
};

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
    char *address = (char *)argv[1];
    messageSender.hostname = address;
    
    lo_timetag_now(&messageSender.timetag);
    if(!std::binary_search(members.begin(), members.end(), messageSender, memberCompare))
    {
        members.push_back(messageSender);
        cerr << "NEW NODE ~ PID: " << messageSender.pid << " || path : " <<  messageSender.hostname << " || timestamp : " << messageSender.timetag.sec << endl;
    }
    
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
