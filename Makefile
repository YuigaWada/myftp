build:
	gcc  ./src/myftpc.c ./src/client.c ./src/utils.c -o myftpc -w -lm -std=gnu99 && \
	gcc  ./src/myftpd.c ./src/server.c  ./src/utils.c -o myftpd -w -lm -std=gnu99
allclean:
	$(MAKE) clean ; rm -rf .git
clean:
	rm myftpc myftpd *.out ; rm -rf .vscode t
tar:
	$(MAKE) clean ; tar czf ../myftp.tgz ./
rand:
	head -c 587654321 /dev/urandom > t