# myftp
- A simple FTP server & client written in C.
    - Created for a class assignment🧠

<img width="1201" alt="Screen Shot 2022-10-18 at 8 03 58" src="https://user-images.githubusercontent.com/51681991/196299783-03d6e870-a791-4810-8b2c-d337b0ba8fda.png">


## How to run

- Build
```
make build
```

- Server
```
./myftpd <dir>
```

- Client
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


