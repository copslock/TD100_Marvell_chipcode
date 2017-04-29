/*
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License version 2 as
 *  published by the Free Software Foundation.

 *(C) Copyright 2008 Marvell International Ltd.
 * All Rights Reserved
 */

#include <stdio.h>
#include <fcntl.h>
#include <stdlib.h>
#include <sys/mman.h>
#include <errno.h>
#include <unistd.h>
#include <ctype.h>

//#define HWACC_DEBUG
#ifdef BIONIC
extern int init_stdfiles(void);
#endif
int parseArgs(int argc, char* argv[], char* mode, unsigned long* addr, unsigned long* data)
{
  if(argc<3) return -1;
  *mode=tolower(*argv[1]);
  if((*mode!='r') && (*mode!='w') && (*mode!='d')) return -1;
  if(sscanf(argv[2],"%lx",addr)!=1) return -1;
  if((argc>3) && (*mode=='w')) // write 32 bit
    {
     if(sscanf(argv[3],"%lx",data)!=1) return -1;
    }
  if((argc>4) && (*mode=='d')) // dump
    {
      if(sscanf(argv[3],"%lx",data)!=1) return -1; // size
      
    }
  return 0;
}

#define PAGE_OFFS_BITS(pgsz) ((unsigned long)(pgsz)-1)
#define PAGE_MASK_BITS(pgsz) (~PAGE_OFFS_BITS(pgsz))

int main(int argc, char* argv[])
{
  int fid;
  int pageSize=sysconf(_SC_PAGESIZE);
  int len=pageSize;
  int lenAligned;
  char mode;
  unsigned long addr,addrAligned,data;
  volatile unsigned long* pa;
  void* vpa;
  FILE* fout=0;
#ifdef BIONIC
  init_stdfiles();
#endif
  if(parseArgs(argc,argv,&mode,&addr,&data))
    {
      fprintf(stderr,"USAGE: %s [r|w] addr-hex <data-hex>\n",argv[0]);
      fprintf(stderr,"USAGE: %s [d]   addr-hex <size-hex> out-file-name\n",argv[0]);
      exit(-1);
    }
  fprintf(stderr,"Mode=%c Addr=%.8lx",mode,addr);
  if(mode=='w') fprintf(stderr," Data=%.8lx",data);
  if(mode=='d') 
  { 
    fprintf(stderr," Dump=%.8lx bytes into file %s",data, argv[4]);
    len=(int)data;
  }
  fprintf(stderr,"\n"); 

  if((fid=open("/dev/hwmap",O_RDWR))<0)
    {
      fprintf(stderr,"Failed to open the hwmap device\n");
      exit(-1);
    }


  // Align the length so the mapped area is page-aligned and contains the requested area
  addrAligned=addr&PAGE_MASK_BITS(pageSize);
  lenAligned=((addr+len-addrAligned)+pageSize-1)&PAGE_MASK_BITS(pageSize);

  /* Notes on flags: MAP_PRIVATE results in copy on write; MAP_SHARED allows normal write */
  /*   MAP_SHARED required O_RDWR in open above, otherwise mmap fails with errno=EACCES   */ 
  /* Notes on prot:  PROT_WRITE allows read and write; PROT_READ allows read only         */
  /* Notes on off: an unsigned 32-bit value, should be aligned to page size according to mmap manpage */
  if((vpa=mmap(0,lenAligned,PROT_READ|PROT_WRITE, MAP_SHARED,fid,addrAligned))==MAP_FAILED)
    {
      fprintf(stderr,"mmap failed (%d)\n",errno);
    }
  else
    {
      pa=(volatile unsigned long*)vpa;
      pa+=(addr&PAGE_OFFS_BITS(pageSize))>>2; /* long ptr type*/
      switch(mode)
	{
	case 'r':
          data=*pa;
          fprintf(stderr,"Value read from 0x%.8lx via MVA=0x%lx is 0x%.8lx\n",addr, (unsigned long)pa, data);
          break;
        case 'w':
          *pa = data;
          fprintf(stderr,"Value %.8lx written to 0x%.8lx via MVA=0x%.8lx\n", data, addr, (unsigned long)pa);
#if defined(HWACC_DEBUG)
          if(1)
	    {
              unsigned long val;
              val=*pa;
              fprintf(stderr,"Value read from 0x%.8lx via MVA=0x%lx is 0x%.8lx\n",addr, (unsigned long)pa, val);

	    }
#endif
          break;
        case 'd':
          if((fout=fopen(argv[4],"wb+")) && (len=fwrite((void*)pa,1,len,fout)))
	  {
	    fprintf(stderr,"Written 0x%.8x bytes\n",len);
	  }
          else
	  {
            fprintf(stderr,"Failed to write output file\n");
	  }
          if(fout) 
          {
           fclose(fout);
           if(!len) unlink(argv[4]);
	  }
	}
      munmap(vpa,len);
    }

  close(fid);
  exit(0);
  return 0;
}
