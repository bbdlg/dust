/*
 * This file is part of the DFC.
 *
 * Copyright (C) 2012 by bbdlg <shiluo.110@163.com>
 *
 * The DFC is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * The DFC is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with DFC. If not, see <http://www.gnu.org/licenses/>.
 * */

#include "../toolsModule.h"
#include "../../unity/src/unity.h"

void setUp(void)
{
}

void tearDown(void)
{
}

void test_getSection(void)
{
   char* content = " echo \"\
[TcpClient] \n\
LogicName   = tcpClient1 \n\
DestIp      = 127.0.0.1 \n\
DestPort    = 1111 \n\
LocalPort   = 8888 \n\
\n\
[TcpClient] \n\
LogicName   = tcpClient2 \n\
DestIp      = 127.0.0.1 \n\
DestPort    = 2222 \n\
LocalPort   = 9999 \n\
\n\
[TcpClient] \n\
LogicName   = tcpClient3 \n\
DestIp      = 127.0.0.1 \n\
DestPort    = 3333 \n\
LocalPort   = 1234 \n\"        > commModule.conf";
   system(content);
   char section[MAX_LEN_SECTION] = {0};
   int ret = getSection("./commModule.conf", 0, "TcpClient", section);
   TEST_ASSERT_EQUAL_INT(3, ret);

   memset(section, 0, MAX_LEN_SECTION);
   ret = getSection("./commModule.conf", 1, "TcpClient", section);
   TEST_ASSERT_EQUAL_INT(TOOLS_SUCCESS, ret);
   TEST_ASSERT_EQUAL_STRING("[TcpClient] \nLogicName   = tcpClient1 \nDestIp      = 127.0.0.1 \nDestPort    = 1111 \nLocalPort   = 8888 \n\n", section);

   memset(section, 0, MAX_LEN_SECTION);
   ret = getSection("./commModule.conf", 2, "TcpClient", section);
   TEST_ASSERT_EQUAL_INT(TOOLS_SUCCESS, ret);
   TEST_ASSERT_EQUAL_STRING("[TcpClient] \nLogicName   = tcpClient2 \nDestIp      = 127.0.0.1 \nDestPort    = 2222 \nLocalPort   = 9999 \n\n", section);

   memset(section, 0, MAX_LEN_SECTION);
   ret = getSection("./commModule.conf", 3, "TcpClient", section);
   TEST_ASSERT_EQUAL_INT(TOOLS_SUCCESS, ret);
   TEST_ASSERT_EQUAL_STRING("[TcpClient] \nLogicName   = tcpClient3 \nDestIp      = 127.0.0.1 \nDestPort    = 3333 \nLocalPort   = 1234 \n\n", section);
}

void test_getValueOfKey(void)
{
   char* section = "[TcpClient] \nLogicName   = \"tcpC lient3\" \n\
DestIp      = 127.0.0.1 \n\
DestPort    = 3333 \n\
LocalPort   = 1234 \n\n";
   char res[MAX_LEN_ROW] = {0};
   int size = MAX_LEN_ROW;
   int ret = getValueOfKey(section, "LogicName", res, &size);
   TEST_ASSERT_EQUAL_INT(TOOLS_SUCCESS, ret);
   TEST_ASSERT_EQUAL_STRING("tcpC lient3", res);

   memset(res, 0, MAX_LEN_ROW);
   size = MAX_LEN_ROW;
   ret = getValueOfKey(section, "DestIp", res, &size);
   TEST_ASSERT_EQUAL_INT(TOOLS_SUCCESS, ret);
   TEST_ASSERT_EQUAL_STRING("127.0.0.1", res);

   memset(res, 0, MAX_LEN_ROW);
   size = MAX_LEN_ROW;
   ret = getValueOfKey(section, "LocalPort", res, &size);
   TEST_ASSERT_EQUAL_INT(TOOLS_SUCCESS, ret);
   TEST_ASSERT_EQUAL_STRING("1234", res);
}

void test_readValueFromConf_ext(void)
{
   char* content = " echo \"\
[TcpClient] \n\
LogicName   = tcpClient1 \n\
DestIp      = 127.0.0.1 \n\
DestPort    = 1111 \n\
LocalPort   = 8888 \n \
\n\
[TcpClient] \n\
LogicName   = tcpClient2 \n\
DestIp      = 127.0.0.1 \n\
DestPort    = 2222 \n\
LocalPort   = 9999 \n\
\n\
[TcpClient] \n\
LogicName   = tcpClient3 \n\
DestIp      = 127.0.0.1 \n\
DestPort    = 3333 \n\
LocalPort   = 1234 \n\"        > commModule.conf";
   system(content);
   char res[MAX_LEN_ROW] = {0};
   int size = MAX_LEN_ROW;
   int ret = readValueFromConf_ext("./commModule.conf", 0, "TcpClient", "LogicName", res, &size);
   TEST_ASSERT_EQUAL_INT(3, ret);

   memset(res, 0, MAX_LEN_ROW);
   size = MAX_LEN_ROW;
   ret = readValueFromConf_ext("./nosuchfile.conf", 1, "TcpClient", "LogicName", res, &size);
   TEST_ASSERT_EQUAL_INT(TOOLS_CANNOT_OPEN_CONF, ret);

   memset(res, 0, MAX_LEN_ROW);
   size = MAX_LEN_ROW;
   ret = readValueFromConf_ext("./commModule.conf", 1, "noSuchField", "LogicName", res, &size);
   TEST_ASSERT_EQUAL_INT(TOOLS_CANNOT_FIND_VALUES, ret);

   memset(res, 0, MAX_LEN_ROW);
   size = MAX_LEN_ROW;
   ret = readValueFromConf_ext("./commModule.conf", 1, "TcpClient", "noSuchKey", res, &size);
   TEST_ASSERT_EQUAL_INT(TOOLS_CANNOT_FIND_VALUES, ret);

   memset(res, 0, MAX_LEN_ROW);
   size = MAX_LEN_ROW;
   ret = readValueFromConf_ext("./commModule.conf", 1, "TcpClient", "LogicName", res, &size);
   TEST_ASSERT_EQUAL_INT(TOOLS_SUCCESS, ret);
   TEST_ASSERT_EQUAL_STRING("tcpClient1", res);
   TEST_ASSERT_EQUAL_INT(10, size);

   system("rm -rf commModule.conf");
}

