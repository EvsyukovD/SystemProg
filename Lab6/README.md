# Lab6
This project realize client-server architecture.

For server/client api you should choose one mode: standard (1 option) or channel (2 option).

Server always starts first, and, before message handling, waits for all clients(all clients must be registered!).

Mode 1:

Everytime(!) before sending information you should choose what you want to send: msg(Y) or file(N).

Msg format:

dest_id:msg

If you want to disconnect, send to server 'bye'(msg without dest_id and ':').

All files download in directory 'instl'.