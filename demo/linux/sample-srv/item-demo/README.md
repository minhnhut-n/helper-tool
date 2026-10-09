1. build source code
gcc main.c -o sample_test

2. copy to destination dir
-> for service systemd config
/etc/systemd/system/sample.service 

-> for exec file
/usr/bin/sample_test

3. run command 
3.1. reload
$ sudo systemctl daemon-reload
--> runable

3.2. start
$ sudo systemctl start sample.service
--> running

3.3. optional, start with system on boot
$ sudo systemctl enable sample.service

4. check status
$ sudo systemctl status sample.service