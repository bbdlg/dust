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

#include "../memModule.h"
#include "unity.h"

void setUp(void)
{

}

void tearDown(void)
{

}

void memoryOperate(void)
{
   void *a, *b, *c, *d;
   a = malloc(100);
   b = calloc(101, sizeof(char));
   c = malloc(10);
   a = realloc(a, 30);
   free(a);
   //free(NULL);
   printf("= 0 ============================\n");
}
   
void test_showMemSummary(void)
{
   void* dd = malloc(1000);
   printf("= 1 ============================\n");
   memInit();
   printf("= 2 ============================\n");
   //TRACE("BEG OF memoryOperate()\n");
   memoryOperate();
   //TRACE("END OF memoryOperate()\n");
   printf("= 3 ============================\n");
   //CHECK();
}

