# myftp
- A simple FTP server & client written in C.
    - Created for a class assignment🧠


## How to run

- Build
    - `make build`

- Server
    - `./myftpd <dir>`

- Client
    - `./myftpc <IP>`


## Commands

|command| args|description|
|:----|:----|:----|
|quit| – | quit the program|
|pwd| – |   print current directory|
|cd |path|  change current directory|
|dir| path|    list current directory|
|lpwd| – | print client current directory|
|lcd| path|    change client current directory|
|ldir| path| list client current directory|
|get| path1 path2|  get the file of 'path1' from server and save to 'path2'|
|put| path1 path2| put file of 'path1' to server and save to 'path2'|
|help| – |print help message|


