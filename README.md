dust
====

  Dust is a integration of modules, such as logModule and errModule, which can be used to insert into any C/C++ project 
alone without any modification.
  Dust is written by C, so it is compatible with C and C++ project.
  I use unity to make sure every dust module is usable. Take logModule as an example:
              [root@localhost dust]# tree logModule
              logModule
              |-- logModule.c               ## main code of module
              |-- logModule.conf            ## config file of module
              |-- logModule.h               ## head file of module
              |-- seelog.sh                 
              |-- testLogModule             ## test directory
              |   |-- makefile
              |   |-- start_test.sh
              |   |-- test_logModule.c
              |   `-- test_logModule_Runner.c
              |-- tools.c
              `-- tools.h
  You can cd into testLogModule, and type:
              [root@localhost testLogModule]# ./start_test.sh test_logModule.c
  Now, you will see all tests instance of logModule have passed.

  NOTE: before do testing things, you must download ThrowTheSwitch-Unity-8661f3f.tar.gz from https://github.com/ThrowTheSwitch/Unity/downloads, 
put it into dust directory, and change name to unity.