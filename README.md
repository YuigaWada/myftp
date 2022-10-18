# myftp
- A simple FTP server & client written in C.
    - Created for a class assignment 🧠

## Demo
<img width="1201" alt="Screen Shot 2022-10-18 at 8 03 58" src="https://user-images.githubusercontent.com/51681991/196299783-03d6e870-a791-4810-8b2c-d337b0ba8fda.png">


## Packet Format
<p align="center">
<img width="572" alt="Screen Shot 2022-10-19 at 2 04 39" src="https://user-images.githubusercontent.com/51681991/196498015-775a8e98-5275-48c6-87a7-b759b59671ff.png">
</p>

## How to run

- Build
```
make build
```

- Server
    - When a server (myftpd) receives a request for a TCP connection request, it `forks()` and the child process accepts subsequent command messages from the client.
```
./myftpd <dir>
```

- Client
    - Once a TCP connection is established, a client (myftpc) displays the prompt "myFTP%" and waits for user input.
```
./myftpc <IP>
```


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


