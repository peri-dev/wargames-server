/*
 *   WarGames - a WOPR emulator written in C
 *   Copyright (C) 2014 Franklin Wei
 *
 *   This program is free software: you can redistribute it and/or modify
 *   it under the terms of the GNU General Public License as published by
 *   the Free Software Foundation, either version 3 of the License, or
 *   (at your option) any later version.
 *
 *   This program is distributed in the hope that it will be useful,
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *   GNU General Public License for more details.
 *
 *   You should have received a copy of the GNU General Public License
 *   along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 *   Contact the author at contact@fwei.tk
 */

#include "strings.h"
#include "telnet.h"
#include "util.h"
#include <ctype.h>
#include <string.h>
#include <unistd.h>
#include <stdlib.h>
int out_fd;
extern int pipes[FD_SETSIZE][2];
void allLower(char* str)
{
  for(int i=0;str[i];++i)
    {
      str[i]=tolower(str[i]);
    }
}
void print_string(const char* str) /* print string, slowly */
{
  int i=0;
  while(str[i])
    {
      if(str[i]=='\n')
        write(out_fd, "\r\n", 2);
      else
        write(out_fd, &str[i], 1);
      fsync(out_fd);
      usleep(SLEEP_TIME);
      ++i;
    }
}
void remove_punct(char* buf)
{
  for(int i=0;buf[i];++i)
    {
      for(int j=0;j<sizeof(punctuation_marks)/sizeof(char);++j)
        {
          if(buf[i]==punctuation_marks[j])
            {
              memmove(&buf[i], &buf[i+1], strlen(buf)-i);
            }
        }
    }
}
void clear(void)
{
  unsigned char clear_sequence[]={ 0x1B, 'c' };
  write(out_fd, clear_sequence, sizeof(clear_sequence));
  fsync(out_fd);
}
void refresh(void)
{
  fsync(out_fd);
}
int getnstr(char* buf, int max)
{
  echo_on();
  memset(buf, 0, max);
  int ret=read(pipes[out_fd][0], buf, max);
  if(ret!=0)
    {
      /* prevent buffer underflow */
      if(strlen(buf)-2>=0)
        /* remove the newline */
        buf[strlen(buf)-2]='\0';
    }
  if(ret<0)
    return ERR;
  return OK;
  echo_off();
}
void echo_off(void)
{
  unsigned char echo_off[]={IAC, WILL, ECHO};
  write(out_fd, echo_off, 3);
  echo_off[1]=DONT;
  write(out_fd, echo_off, 3);
  fsync(out_fd);
}
void echo_on(void)
{
  unsigned char echo_on[]={IAC, DO, ECHO};
  write(out_fd, echo_on, 3);
  fsync(out_fd);
}
