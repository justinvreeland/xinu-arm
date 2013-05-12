/**
 * @file     shell.c
 * @provides shell.
 *
 * $Id: shell.c 2157 2010-01-19 00:40:07Z brylow $
 */
/* Embedded Xinu, Copyright (C) 2009.  All rights reserved. */

#include <stddef.h>
#include <ctype.h>
#include <interrupt.h>
#include <shell.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <tty.h>
#include <thread.h>
#include <nvram.h>
#include <conf.h>
#include <history.h>
#include <pipe.h>

const struct centry commandtab[] = {
#if NETHER
  {"arp", FALSE, xsh_arp},
#endif
    {"clear", TRUE, xsh_clear},
    {"date", FALSE, xsh_date},
    {"history", FALSE, xsh_history},
    {"charCatch", FALSE, xsh_charCatch},
    {"upper", FALSE, xsh_upper},
#if USE_TLB
  {"dumptlb", FALSE, xsh_dumptlb},
#endif
#if NETHER
  {"ethstat", FALSE, xsh_ethstat},
#endif
  {"echo", FALSE, xsh_echo},
  {"exit", TRUE, xsh_exit},
#if NFLASH
  {"flashstat", FALSE, xsh_flashstat},
#endif
#ifdef GPIO_BASE
  {"gpiostat", FALSE, xsh_gpiostat},
#endif
  {"help", FALSE, xsh_help},
  {"kill", TRUE, xsh_kill},
#ifdef GPIO_BASE
  {"led", FALSE, xsh_led},
#endif
  {"memstat", FALSE, xsh_memstat},
  {"memdump", FALSE, xsh_memdump},
#if NETHER
  {"nc", FALSE, xsh_nc},
  {"netdown", FALSE, xsh_netdown},
#if NETEMU
  {"netemu", FALSE, xsh_netemu},
#endif
  {"netstat", FALSE, xsh_netstat},
  {"netup", FALSE, xsh_netup},
#endif
#if NVRAM
  {"nvram", FALSE, xsh_nvram},
#endif
  {"ps", FALSE, xsh_ps},
#if NETHER
  {"ping", FALSE, xsh_ping},
#endif
  {"reset", FALSE, xsh_reset},
#if NETHER
  {"route", FALSE, xsh_route},
#endif
  {"sleep", TRUE, xsh_sleep},
#if NETHER
  {"snoop", FALSE, xsh_snoop},
#endif
#if USE_TAR
  {"tar", FALSE, xsh_tar},
#endif
#if NETHER
  {"tcpstat", FALSE, xsh_tcpstat},
  {"telnet", FALSE, xsh_telnet},
  {"telnetserver", FALSE, xsh_telnetserver},
#endif
  {"test", FALSE, xsh_test},
  {"testsuite", TRUE, xsh_testsuite},
  {"uartstat", FALSE, xsh_uartstat},
#if USE_TLB
  {"user", FALSE, xsh_user},
#endif
#if NETHER
  {"udpstat", FALSE, xsh_udpstat},
  {"vlanstat", FALSE, xsh_vlanstat},
  {"voip", FALSE, xsh_voip},
  {"xweb", FALSE, xsh_xweb},
#endif
  {"?", FALSE, xsh_help},
};

const ulong ncommand = sizeof(commandtab) / sizeof(struct centry);

/*
 * Function to clear a line
 */
void lineclear( int size ){
  printf("\033[%dD");
  printf("\033[K");
}

/*
 * print new command text
 */
void lineprintf( char* line, int size ){
  int i;
  for( i = 0; i < size; i++){
    printf("%c", line[i]);
  }
}

/**
 * Echo a single character on a TTY.
 * @param devptr TTY device table entry
 * @param ch character to echo
 */
static void shellttyEcho(device *devptr, char ch)
{
  /* Backspace or delete */
  if (('\b' == ch) || (0x7F == ch))
  {
    ttyPutc(devptr, '\b');
    ttyPutc(devptr, ' ');
    ttyPutc(devptr, '\b');
    return;
  }

  /* Do not echo unprintable characters */
  if (!isprint(ch))
  {
    return;
  }

  /* Echo character */
  ttyPutc(devptr, ch);

}

/**
 * Read characters from a tty.
 * @param devptr pointer to tty device
 * @param buf buffer for read characters
 * @param len size of the buffer
 * @return number of characters read, EOF if end of file was reached
 */
devcall shellRead(int descrp, void *buf, uint len)
{
  // modeled after syscall_read
  device *devptr;
  if(isbaddev(descrp))
  {
    return SYSERR;
  }
  devptr = (device *)&devtab[descrp];

  struct tty *ttyptr;
  device *phw;
  int ch = 0;
  int count = 0;

  char *buffer = buf;
  int left = 0;
  char escape = 0;
  int i = 0;

  int historyIndex = -1;
  char savedBuffer[SHELL_BUFLEN];
  int saveLength = 0;

  /* Setup and error check pointers to structures */
  ttyptr = &ttytab[devptr->minor];
  phw = ttyptr->phw;
  if (NULL == phw)
  {
    return SYSERR;
  }

  /* If the eof flag is set, clear the flag and return EOF */
  if (ttyptr->ieof)
  {
    ttyptr->ieof = FALSE;
    return EOF;
  }

  /* In raw mode, no line buffering and no cooking */
  if (ttyptr->iflags & TTY_IRAW)
  {
    /* Fill user buffer first from input buffer */
    while ((0 < ttyptr->icount) && (count < len))
    {
      *buffer++ = ttyptr->in[ttyptr->istart - left];
      ttyptr->icount--;
      ttyptr->istart = (ttyptr->istart + 1) % TTY_IBLEN;
      count++;
    }

    /* Fill rest of user buffer by reading input */
    while (count < len)
    {
      ch = (*phw->getc) (phw);
      if (SYSERR == ch)
      {
        return SYSERR;
      }
      *buffer++ = ch;
      count++;

      /* Echo character if TTY_ECHO flag is set */
      if (ttyptr->iflags & TTY_ECHO)
      {
        shellttyEcho(devptr, ch);
      }
    }

    return count;
  }

  /* In non-raw mode, read characters into the TTY input buffer     */
  /* until a line delimiter is read or the TTY input buffer is full */
  while ((ttyptr->icount < TTY_IBLEN) && !ttyptr->idelim)
  {
    /* Read character */
    ch = (*phw->getc) (phw);
    if (SYSERR == ch)
    {
      return SYSERR;
    }

    /* Cook special characters */
    switch (ch)
    {
      /* Backspace or delete */
      case '\b':
      case 0x7F:
        if (ttyptr->icount < 1)
        {
          continue;
        }

        if( left != 0 ){
          for( i = left+1; i >= 0; i--){
            ttyptr->in[ttyptr->istart + ttyptr->icount - i] =
              ttyptr->in[ttyptr->istart + ttyptr->icount - i + 1];
          }

          printf("\033[s");                            // save cursor position
          printf("\033[%dD", ttyptr->icount - left);   // move the cursor to the end of the line
          printf("\033[K");                            // delete to end of line

          ttyptr->icount--;
          for( i = 0; i < ttyptr->icount; i++){
            printf("%c", ttyptr->in[ttyptr->istart + i]);
          }

          printf("\033[u");                           // unsave cursor
          printf("\033[1D");                          // move the cursor to the end of the line
          continue;
        }

        ttyptr->icount--;
        break;
        /* Newline */
      case '\n':
        /* Translate NL to CR if INLCR flag is set */
        if (ttyptr->iflags & TTY_INLCR)
        {
          ch = '\r';
        }
        /* Place character in TTY input buffer */
        ttyptr->in[(ttyptr->istart + ttyptr->icount - left) % TTY_IBLEN] =
          ch;
        ttyptr->icount++;
        ttyptr->idelim = TRUE;
        break;
        /* Carriage return */
      case '\r':
        /* Ignore carriage return if IGNCR flag is set */
        if (ttyptr->iflags & TTY_IGNCR)
        {
          continue;
        }
        /* Translate CR to NL if ICRNL flag is set */
        if (ttyptr->iflags & TTY_ICRNL)
        {
          ch = '\n';
        }
        /* Place character in TTY input buffer */
        ttyptr->in[(ttyptr->istart + ttyptr->icount + left) % TTY_IBLEN] =
          ch;
        ttyptr->icount++;
        ttyptr->idelim = TRUE;
        // more me
        left--;
        break;
        /* End of file */
      case 0x04:
        ttyptr->ieof = TRUE;
        ttyptr->idelim = TRUE;
        break;

        /* All other characters */

        /* this handles simple line editing using VT100 escape codes */
      case 27:
        escape = 1;
        ch = (*phw->getc) (phw);
        if( ch == '[' ){ // 91
          ch = (*phw->getc) (phw);
          switch( ch ){

            case 'A':

              if(historyIndex == -1) {
//                strncpy(savedBuffer, ttyptr->in[ttyptr->istart], SHELL_BUFLEN);
                for( i = 0; i < ttyptr->icount; i++){
                  savedBuffer[i] = ttyptr->in[(ttyptr->istart +i) % TTY_IBLEN];
                }
                saveLength = ttyptr->icount;
              }

              // dont let historyIndex go out of history bounds
              if(historyIndex < numHistoryItems - 1) {
                ++historyIndex;

                for(i = 0; i < history[historyIndex].commandLength; i++){
                  ttyptr->in[(ttyptr->istart + i) % TTY_IBLEN] =
                    history[historyIndex].command[i];
                }
                //printf("%s", history[historyIndex].command);

                printf("\033[%dD", ttyptr->icount - left);   // move the cursor to the end of the line
                if(historyIndex < 1){
                  printf(" ");
                }
                printf("\033[K");                            // delete to end of line
                //                printf("\033[1C");      // cursor position is off by one somehow

                ttyptr->icount = history[historyIndex].commandLength;

                for( i = 0; i < ttyptr->icount; i++){
                  printf("%c", ttyptr->in[(ttyptr->istart + i) % TTY_IBLEN]);
                }
              }

              break;

            case 'B':
              if( historyIndex < 0){
                break;
              }

              historyIndex--;

              printf("\033[%dD", ttyptr->icount - left);
              printf("\033[K");

              if( historyIndex > -1 ){
                for( i = 0; i < history[historyIndex].commandLength; i++){
                  ttyptr->in[(ttyptr->istart + i) % TTY_IBLEN] =
                    history[historyIndex].command[i];
                }
                ttyptr->icount = history[historyIndex].commandLength;
                for(i = 0; i < ttyptr->icount; i++){
                  printf("%c", ttyptr->in[(ttyptr->istart + i) % TTY_IBLEN]);
                }

              } else {

                //if(historyIndex < 0 ){
                //  printf(" ");
                //}
                for( i = 0; i < saveLength; i++){
                  ttyptr->in[(ttyptr->istart + i) % TTY_IBLEN] =
                    savedBuffer[i];
                }
                ttyptr->icount = saveLength;
                for(i = 0; i < ttyptr->icount; i++){
                  printf("%c", ttyptr->in[(ttyptr->istart + i) % TTY_IBLEN]);
                }
              }

              break;

            case 'C': // right arrow
              if( left > 0 ){
                left--;
                printf("\033[1C");
              }
              break;
            case 'D': // left arrow
              if( !((left == ttyptr->icount) || (ttyptr->icount == 0)) ){
                left++;
                printf("\033[1D");
              }
              break;
          }// switch
        } // if
        break;

      default:
        /* Ignore unprintable characters */
        if (!isprint(ch))
        {
          continue;
        }

        if(left != 0){
          /* shift characters in the TTY input buffer */
          for(i = left; i >= 0; i--){
            ttyptr->in[(ttyptr->istart + ttyptr->icount - left + i + 1) % TTY_IBLEN] =
              ttyptr->in[(ttyptr->istart + ttyptr->icount - left  + i) % TTY_IBLEN];
          }

          /* Place character in TTY input buffer */
          ttyptr->in[(ttyptr->istart + ttyptr->icount - left) % TTY_IBLEN] = ch;
          ttyptr->icount++;

          printf("\033[s");                                // save cursor position
          printf("\033[%dD", ttyptr->icount - left - 1);   // move the cursor to the end of the line
          printf("\033[K");                                // delete to end of line

          for( i = 0; i < ttyptr->icount; i++){
            printf("%c", ttyptr->in[ttyptr->istart + i]);
          }

          printf("\033[u");                                // unsave cursor
          printf("\033[1C");                               // move it one over
          continue;

        } else {

          /* Place character in TTY input buffer */
          ttyptr->in[(ttyptr->istart + ttyptr->icount - left) % TTY_IBLEN] =
            ch;
          ttyptr->icount++;
          break;
        }
    }

    if( !escape ){

      /* Echo character if TTY_ECHO flag is set */
      if (ttyptr->iflags & TTY_ECHO)
      {
        shellttyEcho(devptr, ch);
      }

      if (ttyptr->iflags & TTY_IRAW)
      {
        break;
      }

    }
    escape = 0;
  }

  /* Fill user buffer from input buffer */
  while ((0 < ttyptr->icount) && (count < len))
  {
    *buffer++ = ttyptr->in[ttyptr->istart];
    ttyptr->icount--;
    ttyptr->istart = (ttyptr->istart + 1) % TTY_IBLEN;
    count++;
  }

  /* If TTY input buffer is empty, clear idelimiter flag */
  if (0 == ttyptr->icount)
  {
    ttyptr->idelim = FALSE;
  }

  /* If nothing was read, and the eof flag was set, return EOF */
  if ((0 == count) && (ttyptr->ieof))
  {
    ttyptr->ieof = FALSE;
    return EOF;
  }

  return count;
}

// Spawns a child thread for the passed command token
syscall spawnChildForCommand(char **tok, short ntok)
{
  int i;

  /* Handle syntax error */
  if (ntok <= 0)
  {
    fprintf(stderr, SHELL_SYNTAXERR);
    return -1;
  }

  /* Lookup first token in the command table */
  for (i = 0; i < ncommand; i++)
  {
    if (0 == strncmp(commandtab[i].name, tok[0], SHELL_BUFLEN))
    {
      break;
    }
  }

  /* Handle command not found */
  if (i >= ncommand)
  {
    fprintf(stderr, "%s: command not found\n", tok[0]);
    return -1;
  }

  // TODO: Pipes can't handle built-in commands because they dont spawn
  //        child threads
  /* Handle command if it is built-in */
  // if (commandtab[i].builtin)
  // {
  //   // if (inname != NULL || outname != NULL || background)
  //   // {
  //   //   fprintf(stderr, SHELL_SYNTAXERR);
  //   // }
  //   // else
  //   // {
  //     (*commandtab[i].procedure) (ntok, tok);
  //   // }
  //   return -1;
  // }

  /* Spawn child thread for non-built-in commands */
  return  create(commandtab[i].procedure,
            SHELL_CMDSTK, SHELL_CMDPRIO,
            commandtab[i].name, 2, ntok, tok);
}

/**
 * The Xinu shell.  Provides an interface to execute commands.
 * @param descrp descriptor of device on which the shell is open
 * @return OK for successful exit, SYSERR for unrecoverable error
 */
thread shell(int indescrp, int outdescrp, int errdescrp)
{
  char buf[SHELL_BUFLEN];     /* line input buffer        */
  short buflen;               /* length of line input     */
  char tokbuf[SHELL_BUFLEN + SHELL_MAXTOK];   /* token value buffer       */
  short ntok;                 /* number of tokens         */
  char *tok[SHELL_MAXTOK];    /* pointers to token values */
  char *outname;              /* name of output file      */
  char *inname;               /* name of input file       */
  bool background;            /* is background proccess?  */
  syscall child;              /* pid of child thread      */
  ushort i, j;                /* temp variables           */
  irqmask im;                 /* interrupt mask state     */

  /* hostname variables */
  char hostnm[NET_HOSTNM_MAXLEN + 1]; /* hostname of backend      */
  char *hostptr;              /* pointer to hostname      */
  int hostname_strsz;         /* nvram hostname name size */
  device *devptr;             /* device pointer           */

  printf( "Welcome to the shell!\n" );

  /* Enable interrupts */
  enable();

  hostptr = NULL;
  devptr = NULL;
  hostname_strsz = 0;
  bzero(hostnm, NET_HOSTNM_MAXLEN + 1);

  /* Setup buffer for string for nvramGet call for hostname */
#ifdef ETH0
  if (!isbaddev(ETH0))
  {
    /* Determine the hostname of the main network device */
    devptr = (device *)&devtab[ETH0];
    hostname_strsz = strnlen(NET_HOSTNAME, NVRAM_STRMAX) + 1;
    hostname_strsz += DEVMAXNAME;
    char nvramget_hostname_str[hostname_strsz];
    sprintf(nvramget_hostname_str, "%s_%s", devptr->name,
        NET_HOSTNAME);

    /* Acquire the backend's hostname */
#if NVRAM
    hostptr = nvramGet(nvramget_hostname_str);
#endif                          /* NVRAM */
    if (hostptr != NULL)
    {
      memcpy(hostnm, hostptr, NET_HOSTNM_MAXLEN);
      hostptr = hostnm;
    }
  }
#endif

  /* Set command devices for input, output, and error */
  stdin = indescrp;
  stdout = outdescrp;
  stderr = errdescrp;

  /* Print shell banner */
  printf(SHELL_BANNER);
  /* Print shell welcome message */
  printf(SHELL_START);

  /* Continually receive and handle commands */
  while (TRUE)
  {
    /* Display prompt */
    printf(SHELL_PROMPT);

    if (NULL != hostptr)
    {
      printf("@%s$ ", hostptr);
    }
    else
    {
      printf("$ ");
    }

    /* Setup proper tty modes for input and output */
    control(stdin, TTY_CTRL_CLR_IFLAG, TTY_IRAW, NULL);
    control(stdin, TTY_CTRL_SET_IFLAG, TTY_ECHO, NULL);

    /* Null out the buf and read command */
    memset(buf, '\0', SHELL_BUFLEN);
    buflen = shellRead(stdin, buf, SHELL_BUFLEN);

    if(buf[0] != '!') {
      addHistoryItem(buf, buflen);
    }

    /* Check for EOF and exit gracefully if seen */
    if (EOF == buflen)
    {
      break;
    }

    // Check for indicator of history command
    if (buf[0] == '!')
    {
      int index;

      // handler for !! (just execute most recent command)
      if(buf[1] == '!') {
        index = 0;
      } else {
        // extract the number string
        char indexString[buflen];
        strncpy(indexString, &buf[1], buflen - 1);
        indexString[buflen] = '\0';

        // convert number string into a valid index
        // calculation is done because the index numbers
        // are reverse of their numbers printed using 'history'
        index = numHistoryItems - atoi(indexString);
      }

      //replace buf and buflen with the last command
      strncpy(buf, history[index].command, SHELL_BUFLEN);
      buflen = history[index].commandLength + 1;
    }

    /* Parse line input into tokens */
    if (SYSERR == (ntok = lexan(buf, buflen, &tokbuf[0], &tok[0])))
    {
      fprintf(stderr, SHELL_SYNTAXERR);
      continue;
    }

    /* Ensure parse generated tokens */
    if (0 == ntok)
    {
      continue;
    }

    /* Initialize command options */
    inname = NULL;
    outname = NULL;
    background = FALSE;

    /* Mark as background thread, if last token is '&' */
    if ('&' == *tok[ntok - 1])
    {
      ntok--;
      background = TRUE;
    }

    short numPipes = 0;
    short commandStartIndexes[NUM_PIPES + 1]; //index number of the start token
    short commandEndIndexes[NUM_PIPES + 1];
    commandStartIndexes[0] = 0;
    commandEndIndexes[0] = ntok - 1;

    /* Check each token and perform special handling of '>' and '<' */
    for (i = 0; i < ntok; i++)
    {
      /* Background '&' should have already been handled; Syntax error */
      if ('&' == *tok[i])
      {
        ntok = -1;
        break;
      }

      /* Setup for output redirection if token is '>'  */
      if ('>' == *tok[i])
      {
        /* Syntax error */
        if (outname != NULL || i >= ntok - 1)
        {
          ntok = -1;
          break;
        }

        outname = tok[i + 1];
        ntok -= 2;

        /* shift tokens (not to be passed to command */
        for (j = i; j < ntok; j++)
        {
          tok[j] = tok[j + 2];
        }
        continue;
      }

      /* Setup for input redirection if token is '<' */
      if ('<' == *tok[i])
      {
        /* Syntax error */
        if (inname != NULL || i >= ntok - 1)
        {
          ntok = -1;
          break;
        }
        inname = tok[i + 1];
        ntok -= 2;

        /* shift tokens (not to be passed to command */
        for (j = i; j < ntok; j++)
        {
          tok[j] = tok[j + 2];
        }

        continue;
      }

      // keep track of command locations
      if ('|' == *tok[i])
      {
        ++numPipes;
        commandStartIndexes[numPipes] = i + 1;
        commandEndIndexes[numPipes - 1] = i - 1;
      }
    }
    commandEndIndexes[numPipes] = i - 1;

    syscall children[numPipes + 1];
    short spawnChildFailed = FALSE;

    // create process for each command
    for(i = 0; i <= numPipes; ++i) {
      short start = commandStartIndexes[i];
      short end = commandEndIndexes[i];
      short numToks = end - start + 1;
      char *commandToks[numToks];

      for(j = 0; j < numToks; ++j) {
        commandToks[j] = tok[start++];
      }

      syscall child = spawnChildForCommand(commandToks, numToks);

      if(child != -1) {
        children[i] = child;
      } else {
        spawnChildFailed = TRUE;
        break;
      }
    }

    if(spawnChildFailed) {
      continue;
    }

    /* Clear waiting message; Reschedule; */
    while (recvclr() != NOMSG);
    im = disable();

    short executionError = FALSE;
    for(i = 0; i < numPipes + 1; ++i) {
      child = children[i];

      /* Ensure child command thread was created successfully */
      if (SYSERR == child)
      {
        fprintf(stderr, SHELL_CHILDERR);
        executionError = TRUE;
        break;
      }

      // TODO: Handle redirects
      // /* Set file descriptors for newly created thread */
      // if (NULL == inname)
      // {
      //   thrtab[child].fdesc[0] = stdin;
      // }
      // else
      // {
      //   thrtab[child].fdesc[0] = getdev(inname);
      // }
      // if (NULL == outname)
      // {
      //   thrtab[child].fdesc[1] = stdout;
      // }
      // else
      // {
      //   thrtab[child].fdesc[1] = getdev(outname);
      // }
      // thrtab[child].fdesc[2] = stderr;

      // TODO: Handle more than 2 commands
      if(numPipes > 0) {
        if(i == 0) {

          thrtab[child].fdesc[0] = stdin;
          thrtab[child].fdesc[1] = getdev("PIPE");

        } else if(i == 1) {

          thrtab[child].fdesc[0] = getdev("PIPE");
          thrtab[child].fdesc[1] = stdout;
          
        }
      } else {
        thrtab[child].fdesc[0] = stdin;
        thrtab[child].fdesc[1] = stdout;
      }

    }

    if (executionError) { continue; }

    if(numPipes == 1) {
      ready(children[1], RESCHED_YES);
    }
    ready(children[0], RESCHED_YES);
    restore(im);

    /* Wait for command thread to finish */
    while (receive() != child);
    sleep(10);

    // TODO: Backgrounding w/ pipes
    // if (background)
    // {
    //   /* Make background thread ready, but don't reschedule */
    //   im = disable();
    //   ready(child, RESCHED_NO);
    //   restore(im);
    // }
    // else
    // {
    //   // Clear waiting message; Reschedule; 
    //   while (recvclr() != NOMSG);
    //   im = disable();
    //   ready(child, RESCHED_YES);
    //   restore(im);

    //   /* Wait for command thread to finish */
    //   while (receive() != child);
    //   sleep(10);
    // }
  }

  /* Close shell */
  fprintf(stdout, SHELL_EXIT);
  sleep(10);
  return OK;
}
