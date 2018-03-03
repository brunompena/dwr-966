/*
 * (C) Copyright 2000
 * Wolfgang Denk, DENX Software Engineering, wd@denx.de.
 *
 * See file CREDITS for list of people who contributed to this
 * project.
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; either version 2 of
 * the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston,
 * MA 02111-1307 USA
 */

/*
 * Cache support: switch on or off, get status
 */
#include <common.h>
#include <command.h>
#include <cmd_upgrade.h>

#if defined(BUILD_FROM_IFX_UTILITIES)
#include "crc32.h"
#include <syslog.h>
#define getenv(x)		get_env(x)
#define simple_strtoul(a,b,c)	strtoul(a,b,c)
#define setenv(x,y)		set_env(x,y)
#define uchar			unsigned char
#define ulong			unsigned long
#define uint32_t		unsigned int
#define uint8_t			unsigned char
//#define printf(format, args...)	syslog(LOG_INFO, format, ##args)
#endif //BUILD_FROM_IFX_UTILITIES


#include <version.h>
#include "image.h"

#ifdef CONFIG_LTQ_IMAGE_EXTRA_CHECKS
char* get_string_value(char* str, char* delim, int n)
{
   int i;
   char* ptr=NULL;
   ptr = strtok(str, delim);
   if(n==0) return ptr;
   for(i=0;i<n;i++){
     ptr = strtok(NULL, delim);
   }
   return ptr;
}




/* compare the version with the built-in-version,
   return 1 if v_new is equal to or bigger than v_old
   return 0 otherwise 
*/
int compare_version(char *v_old, char *v_new)
{
	
	 int res=0;
   int i=0;
   char* p=NULL;
   char tmp_str[32];
   int old_v_num=0;
   int new_v_num=0;
   for(i=0;i<32;i++){
     strcpy(tmp_str, v_old);
     p = get_string_value(tmp_str,".",i);
     if(!p) break;
     old_v_num=simple_strtoul(p,NULL,0);
     //printf("old_v_num=%d\n",old_v_num);
     strcpy(tmp_str, v_new);
     p = get_string_value(tmp_str,".",i);
     if(!p) { break;}
     new_v_num=simple_strtoul(p,NULL,0);
     //printf("new_v_num=%d\n",new_v_num);
     if(new_v_num>=old_v_num) res=1;
     else res=0;
   }
   return res;
}
#endif

#if !defined(BUILD_FROM_IFX_UTILITIES)
int do_upgrade( cmd_tbl_t *cmdtp, int flag, int argc, char *argv[]) {
	ulong srcAddr=0;
	int srcLen;
	ulong srcHeader=0;
	int curParSize=0, dir, pad;
	image_header_t *pimg_header = NULL;
	char name[16], strimg_crc[32], buf[32];;
#ifdef CONFIG_CMD_UBI
    int ubi = 0;
#endif
	char fwversion_str[32];
	char *fwversion_ptr = NULL;
	int partnum = 0;
	char partname[16];
	char strimg_datalen[32];
		
	if(argc != 4) {
		printf("Usage :\n%s\n",cmdtp->usage);
		return 1;
	}


	partnum = simple_strtoul(argv[3], NULL, 16);
	

	srcAddr = simple_strtoul(argv[1], NULL, 16);
	srcLen = simple_strtoul(argv[2], NULL, 16);

	do {
		srcHeader = (!srcHeader ? srcAddr : srcHeader + curParSize);
		pimg_header = (image_header_t *)srcHeader;		
		curParSize = sizeof(image_header_t) + ntohl(pimg_header->ih_size);
		pad = (4 - (curParSize % 4)) % 4;

        if (!image_check_hcrc (pimg_header) || !image_check_dcrc (pimg_header)) {
		     printf ("Bad Header Checksum\n");
			 return 1;
		    }

		switch(pimg_header->ih_type) {
			case IH_TYPE_MULTI:
				fwversion_ptr = strchr(pimg_header->ih_name,'-');
				if ( fwversion_ptr )
				{
					//printf ("image header len=%d\n",sizeof(image_header_t));  64
					char *ptr = NULL;
					ptr = strchr(fwversion_ptr+1,' ');
					if(ptr)
					{	
						*ptr='\0';
						sprintf(fwversion_str,fwversion_ptr+1);
						printf ("fw version=%s\n",fwversion_str);
					}
					else
					{
						printf ("Wrong fw verion image !!\n");
						printf ("Please check!!\n");
						return 1;
					}
				}
				else
				{
					printf ("Empty fw verion image !!\n");
					printf ("Please check!!\n");
					return 1;
				}	
#ifdef CONFIG_LTQ_IMAGE_EXTRA_CHECKS
        if(strncmp(pimg_header->ih_vendor, CONFIG_VENDOR_NAME, sizeof(pimg_header->ih_vendor)) || \
		   strncmp(pimg_header->ih_board, CONFIG_BOARD_NAME, sizeof(pimg_header->ih_board))    || \
		   !compare_version(CONFIG_BOARD_VERSION, pimg_header->ih_boardVer)                    || \
		   strncmp(pimg_header->ih_chip, CONFIG_CHIP_NAME, sizeof(pimg_header->ih_chip))       || \
		   !compare_version(CONFIG_CHIP_VERSION, pimg_header->ih_chipVer)                      || \
           !compare_version(CONFIG_SW_VERSION, pimg_header->ih_swVer))
       	{
        		printf("wrong image, update failed!\n");
        		return 0;
       	}
#endif				
				curParSize = sizeof(image_header_t) + 8;
				continue;
			case IH_TYPE_FILESYSTEM:
            #ifdef CONFIG_CMD_UBI
                if(strncmp(pimg_header->ih_name, "LTQCPE UBI_RootFS", sizeof(pimg_header->ih_name)) == 0) ubi=1;
		    #endif
				sprintf(name, "rootfs");
				dir = 0;
				break;
			case IH_TYPE_KERNEL:
			#ifdef CONFIG_CMD_UBI
			    if(strncmp(pimg_header->ih_name, "LTQCPE UBI_Kernel", sizeof(pimg_header->ih_name)) == 0) ubi=1;
			#endif	
				sprintf(name, "kernel");
				dir = 1;
				break;
			case IH_TYPE_FIRMWARE:
			#ifdef CONFIG_CMD_UBI    
				if(strncmp(pimg_header->ih_name, "LTQCPE UBI_Firmware", sizeof(pimg_header->ih_name)) == 0) ubi=1;
			#endif	
				sprintf(name, "firmware");
				dir = 0;
				break;
			case IH_TYPE_UBOOT:
				sprintf(name, "uboot");
				dir = 0;
				break;	
			default:
				printf("Unknown image type!!\n");
				continue;
		}
        if(upgrade_img(srcHeader, curParSize, name, dir, 0)) {
            printf("Can not upgrade the image %s\n", name);
		} else {
				if(2 == partnum)
				{
					sprintf(strimg_crc, "f_%sb_crc", name);
					
					sprintf(strimg_datalen,"f_%sb_datalen",name);
					if(!strstr(name,"kernel"))
						sprintf(buf, "%lX", ntohl(curParSize-sizeof(image_header_t)));
					else
						sprintf(buf, "%lX", ntohl(curParSize));	
					setenv(strimg_datalen, buf);					
				}
				else
				{
					sprintf(strimg_crc, "f_%s_crc", name);
					
					sprintf(strimg_datalen,"f_%s_datalen",name);
					if(!strstr(name,"kernel"))
						sprintf(buf, "%lX", ntohl(curParSize-sizeof(image_header_t)));
					else
						sprintf(buf, "%lX", ntohl(curParSize));							
					setenv(strimg_datalen, buf);
				}
					if(!strstr(name,"kernel"))
					{
						int i = 0 ;
						ulong jnr_crc = 0;
						for( i = 0 ;  i < curParSize-sizeof(image_header_t) ; i++ )
							jnr_crc += *((volatile uint8_t *)(srcHeader + sizeof(image_header_t) + i));
						
						
						sprintf(buf, "%lX", ntohl(jnr_crc));	
					}
					else
						sprintf(buf, "%lX", ntohl(pimg_header->ih_dcrc));
			setenv(strimg_crc, buf);
			saveenv();
#ifdef UBOOT_ENV_COPY
			saveenv_copy();
#endif
		}
		curParSize = curParSize + pad;
#ifdef CONFIG_CMD_UBI		
		ubi=0;
#endif		
	} while(srcLen > (srcHeader - srcAddr) + curParSize);
	if ( fwversion_ptr  && argc == 4)
	{
		partnum = simple_strtoul(argv[3], NULL, 16);
		if( partnum )
		{			
			sprintf(partname, "part%d_version", partnum);
			printf("Saving %s with %s \n",partname, fwversion_str);
			setenv(partname, fwversion_str);			
			saveenv();
		}
	}

	return 0;
}


int http_upgrade(ulong srcAddr, int srcLen) {
	ulong srcHeader=0;
	int curParSize=0, dir, pad;
	image_header_t *pimg_header = NULL;
	char name[16], strimg_crc[32], buf[32];;
	
	printf("srcAddr=%08x\n",srcAddr);
	printf("srcLen=%d\n",srcLen);
	do {
		srcHeader = (!srcHeader ? srcAddr : srcHeader + curParSize);
		printf("srcHeader=0x%08x\n",srcHeader);
		pimg_header = (image_header_t *)srcHeader;		
		curParSize = sizeof(image_header_t) + ntohl(pimg_header->ih_size);
		printf("curParSize=0x%x\n",curParSize);
		
		pad = (16 - (curParSize % 16)) % 16;
        printf("pad=0x%x\n",pad);
		switch(pimg_header->ih_type) {
			case IH_TYPE_MULTI:
				curParSize = sizeof(image_header_t) + 8;
				continue;
			case IH_TYPE_FILESYSTEM:
				sprintf(name, "rootfs");
				dir = 0;
				break;
			case IH_TYPE_KERNEL:
				sprintf(name, "kernel");
				dir = 1;
				break;
			case IH_TYPE_FIRMWARE:
				sprintf(name, "firmware");
				dir = 0;
				break;
			default:
				printf("Unknown image type!!\n");
				continue;
		}
        printf("upgrade now!\n");
		if(upgrade_img(srcHeader, curParSize, name, dir, 0)) {
			printf("Can not upgrade the image %s\n", name);
		} else {
			sprintf(strimg_crc, "f_%s_crc", name);
			sprintf(buf, "%lX", ntohl(pimg_header->ih_dcrc));
			setenv(strimg_crc, buf);
			saveenv();
		}
		printf("upgrade_img done!!\n");
		curParSize = curParSize + pad;
	} while(srcLen > (srcHeader - srcAddr) + curParSize);
	printf("http_upgrade done!\n");
	return 0;
}


#else

#ifdef CONFIG_FEATURE_LQ_NEW_UPGRADE
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/mman.h>

int image_check_hcrc (const image_header_t *hdr)
{
	ulong hcrc = 0;
	ulong len = image_get_header_size ();
	image_header_t header;

	/* Copy header so we can blank CRC field for re-calculation */
	memmove (&header, (char *)hdr, image_get_header_size ());
	image_set_hcrc (&header, 0);

	hcrc ^= 0xffffffffL;
	hcrc = crc32 (hcrc, (unsigned char *)&header, len);
	hcrc ^= 0xffffffffL;

	return (hcrc == image_get_hcrc (hdr));
}

int image_check_dcrc (const image_header_t *hdr)
{
	ulong data = image_get_data (hdr);
	ulong len = image_get_data_size (hdr);
	ulong dcrc = 0;

	dcrc ^= 0xffffffffL;
	dcrc = crc32 (dcrc, (unsigned char *)data, len);
	dcrc ^= 0xffffffffL;

	return (dcrc == image_get_dcrc (hdr));
}
#endif

int do_upgrade( int file_fd , int srcLen) {
	uint32_t curParSize=0, dir, pad;
	char name[16], strimg_crc[32], buf[32];;
	image_header_t xImgHeader;
#ifdef CONFIG_FEATURE_LQ_NEW_UPGRADE
	ulong srcHeader=0;
	image_header_t *pimg_header = NULL;
#endif
	char *xHeader = NULL;
	uint32_t iFileReadSize =0, iTotalFileReadSize = 0;
	int iRet = 0;
	
	char fwversion_str[32];
	char *fwversion_ptr = NULL;
	int partnum = 1;
	char partname[16];
	char strimg_datalen[32];
	int i = 0 ;
	ulong jnr_crc = 0;

	FILE *fpin;
	char setenvbuf[128];
	char *ptr = NULL;
	if( (fpin = popen("uboot_env --get --name kernelname","r")) != NULL)
	{

		if(fgets(setenvbuf,128,fpin) != NULL)
		//if ( (buf=fgets(fpin)) != EOF )
		{
			ptr = strrchr(setenvbuf,'\n');
			if(ptr) *ptr='\0';
			printf("kernelname is %s(%d)\n",setenvbuf,strlen(setenvbuf));
			if( 6 == strlen(setenvbuf) )			
				partnum = 1;
			else if ( 7 == strlen(setenvbuf) )
				partnum = 2;			
				
		} 
		pclose(fpin);
	}
	printf("\n---Current partition is %d---\n",partnum);
		
	
	
#ifdef CONFIG_FEATURE_LQ_NEW_UPGRADE
	char *p;
	p = mmap(0, srcLen, PROT_READ, MAP_SHARED, file_fd, 0);
	if (p == MAP_FAILED) {
		fprintf(stderr, "mmap failed\n");
	}
	xHeader = p;
#endif
	do {

#ifndef CONFIG_FEATURE_LQ_NEW_UPGRADE
		//first read only the header to figure our the image total size
		iRet = read(file_fd, (void *)&xImgHeader , (size_t)sizeof(image_header_t)); 
		if(iRet < sizeof(image_header_t)){
			printf("Failure : could read %d out of %d bytes from image\n", iRet, sizeof(image_header_t));
			return 1; //failure
		}
#else
		xImgHeader = *((image_header_t *)xHeader);
#endif

		printf("\nImage Header --> Data Size = %d\n Image Name = %s\n",xImgHeader.ih_size , xImgHeader.ih_name);

#ifdef CONFIG_FEATURE_LQ_NEW_UPGRADE
		srcHeader = (!srcHeader ? xHeader : srcHeader + curParSize);
		pimg_header = (image_header_t *)srcHeader;
		if (!image_check_hcrc (pimg_header) || !image_check_dcrc (pimg_header)) {
			printf ("Bad Header or Data Checksum\n");
			system("echo 1 > /tmp/web_fw_upgrade");
			return 1;
		}
		else
		{
			system("echo 0 > /tmp/web_fw_upgrade");
		}
#endif

                if(xImgHeader.ih_type == IH_TYPE_MULTI){
				fwversion_ptr = strchr(xImgHeader.ih_name,'-');
				if ( fwversion_ptr )
				{
					//printf ("image header len=%d\n",sizeof(image_header_t));  64
					char *ptr = NULL;
					ptr = strchr(fwversion_ptr+1,' ');
					if(ptr)
					{	
						*ptr='\0';
						sprintf(fwversion_str,fwversion_ptr+1);
						printf ("fw version=%s\n",fwversion_str);
					}
					else
					{
						printf ("Wrong fw verion image !!\n");
						printf ("Please check!!\n");
						return 1;
					}
				}
				else
				{
					printf ("Empty fw verion image !!\n");
					printf ("Please check!!\n");
					return 1;
				}	

					
                        curParSize = sizeof(image_header_t) + 8;
                        iTotalFileReadSize += curParSize;
#ifndef CONFIG_FEATURE_LQ_NEW_UPGRADE
                        if( lseek(file_fd, iTotalFileReadSize, SEEK_SET) == -1){
                                printf("Can not move the offset for image to upgrade\n");
                                return 1;  //failure
                        }
#else
			xHeader = p + iTotalFileReadSize;
#endif
                        continue;
                }


		curParSize = sizeof(image_header_t) + xImgHeader.ih_size;
		pad = (16 - (curParSize % 16)) % 16;

#ifndef CONFIG_FEATURE_LQ_NEW_UPGRADE
		// reposition the file read offset to the start of image
		if( lseek(file_fd, iTotalFileReadSize, SEEK_SET) == -1){
			printf("Can not move the offset for image to upgrade\n");
		        return 1;  //failure	
		}
#else
		xHeader = p + iTotalFileReadSize;

#endif

		// then read the total size into a buffer to be passed to upgrade_img
		iFileReadSize = curParSize + pad;
#ifndef CONFIG_FEATURE_LQ_NEW_UPGRADE
	        xHeader = (char *)malloc(iFileReadSize);			

		iRet = read(file_fd, xHeader , (size_t)iFileReadSize);
       		if(iRet < iFileReadSize){
			printf("Failure : could read %d out of %d bytes from image\n", iRet, iFileReadSize);
			return 1; //failure
		}
#endif        

		switch(xImgHeader.ih_type) {
			case IH_TYPE_MULTI:
				curParSize = sizeof(image_header_t) + 8;
				continue;
			case IH_TYPE_FILESYSTEM:
				sprintf(name, "rootfs");
				dir = 0;
				break;
			case IH_TYPE_KERNEL:
				sprintf(name, "kernel");
				dir = 1;
				break;
			case IH_TYPE_FIRMWARE:
				sprintf(name, "firmware");
				dir = 0;
				break;
			case IH_TYPE_UBOOT:
			    sprintf(name, "uboot");
			    dir = 0;
			    break; 
			default:
				printf("Unknown image type!!\n");
				continue;
		}

		if(upgrade_img(xHeader, curParSize, name, dir, 0)) {
			printf("Can not upgrade the image %s\n", name);
		} else {

				if(1 == partnum)   // parition name already swap , we need save to other parition
				{
					//crc
					sprintf(strimg_crc, "f_%sb_crc", name);
					if(!strstr(name,"kernel"))
					{
						i = 0 ;
						jnr_crc = 0;					

						for( i = 0 ;  i < curParSize-sizeof(image_header_t) ; i++ )
							jnr_crc += *((volatile uint8_t *)(xHeader + sizeof(image_header_t) + i));
						
						
						sprintf(buf, "%lX", ntohl(jnr_crc));	
					}
					else						
						sprintf(buf, "%lX", ntohl(xImgHeader.ih_dcrc));
					//setenv(strimg_crc, buf);					
					memset(setenvbuf,0,128);	
					sprintf(setenvbuf, "uboot_env --set --name %s --value %s",strimg_crc,buf);
					printf("setenvbuf command=%s\n",setenvbuf);
					system(setenvbuf);						
					
					
					
					// datalen
					sprintf(strimg_datalen,"f_%sb_datalen",name);
					if(!strstr(name,"kernel"))
						sprintf(buf, "%lX", ntohl(curParSize-sizeof(image_header_t)));
					else
						sprintf(buf, "%lX", ntohl(curParSize));	
					//setenv(strimg_datalen, buf);	
					memset(setenvbuf,0,128);	
					sprintf(setenvbuf, "uboot_env --set --name %s --value %s",strimg_datalen,buf);
					printf("setenvbuf command=%s\n",setenvbuf);
					system(setenvbuf);								
				}
				else
				{
					// crc
					sprintf(strimg_crc, "f_%s_crc", name);
					if(!strstr(name,"kernel"))
					{
						i = 0 ;
						jnr_crc = 0;
						for( i = 0 ;  i < curParSize-sizeof(image_header_t) ; i++ )
							jnr_crc += *((volatile uint8_t *)(xHeader + sizeof(image_header_t) + i));
						
						
						sprintf(buf, "%lX", ntohl(jnr_crc));	
					}
					else					
						sprintf(buf, "%lX", ntohl(xImgHeader.ih_dcrc));
					//setenv(strimg_crc, buf);					
					memset(setenvbuf,0,128);	
					sprintf(setenvbuf, "uboot_env --set --name %s --value %s",strimg_crc,buf);
					printf("setenvbuf command=%s\n",setenvbuf);
					system(setenvbuf);					
					
					// datalen
					sprintf(strimg_datalen,"f_%s_datalen",name);
					if(!strstr(name,"kernel"))
						sprintf(buf, "%lX", ntohl(curParSize-sizeof(image_header_t)));
					else
						sprintf(buf, "%lX", ntohl(curParSize));							
					//setenv(strimg_datalen, buf);
					memset(setenvbuf,0,128);	
					sprintf(setenvbuf, "uboot_env --set --name %s --value %s",strimg_datalen,buf);
					printf("setenvbuf command=%s\n",setenvbuf);
					system(setenvbuf);		
				}
			
			
#if !defined (CONFIG_TARGET_UBI_MTD_SUPPORT)
			sprintf(strimg_crc, "f_%s_crc", name);
			sprintf(buf, "%lX", ntohl(xImgHeader.ih_dcrc));
			setenv(strimg_crc, buf);
			saveenv();
#ifdef UBOOT_ENV_COPY
			saveenv_copy();
#endif
#endif
		}
		iTotalFileReadSize += iFileReadSize;

#ifndef CONFIG_FEATURE_LQ_NEW_UPGRADE
		if(xHeader != NULL){
			free(xHeader);
			xHeader = NULL;
		}
#else
		xHeader += iFileReadSize; 
#endif

	} while(srcLen > iTotalFileReadSize );

#ifdef CONFIG_FEATURE_LQ_NEW_UPGRADE
	munmap(p, srcLen);
#endif

	if ( fwversion_ptr )
	{
		if( partnum )
		{	
			if(1 == partnum )  // need swap
				sprintf(partname, "part2_version");
			else if(2 == partnum )		
				sprintf(partname, "part1_version");
				
			printf("Saving %s with %s \n",partname, fwversion_str);
			
			//setenv(partname, fwversion_str);	
			memset(setenvbuf,0,128);		
			sprintf(setenvbuf, "uboot_env --set --name %s --value %s",partname,fwversion_str);
			printf("setenvbuf command=%s\n",setenvbuf);
			system(setenvbuf);
		}
	}



	return 0;
}
#endif


#if !defined(BUILD_FROM_IFX_UTILITIES)
int do_update_fw (cmd_tbl_t *cmdtp, int flag, int argc, char *argv[])
{

	char *s;
	int   rcode = 0;
	int   size;
	int   partnum = 0;


	switch (argc) {
	case 3:			
		
		setenv("fullimage", argv[1]);
		
		partnum = simple_strtoul(argv[2], NULL, 16);
		
		if(1 == partnum)
			run_command("run update_fullimage",0);		
		else if(2 == partnum)
			run_command("run update_fullimageb",0);
		else
		{
			printf ("Usage:\n%s\n", cmdtp->usage);
			return 1;
		}

		setenv("fullimage", "fullimage.img"); 
		saveenv();
		break;
	
	case 2:
		partnum = simple_strtoul(argv[1], NULL, 16);
		
		if(1 == partnum)
			run_command("run update_fullimage",0);		
		else if(2 == partnum)
			run_command("run update_fullimageb",0);
		else
		{
			printf ("Usage:\n%s\n", cmdtp->usage);
			return 1;
		}	
			
		break;
		
	case 1:	
	default: 
		printf ("Usage:\n%s\n", cmdtp->usage);
		return 1;
	}

	//run_command("reset" , 0 );	
	return rcode;

	
}


/* image CRC check
 *
 * Syntax:
 *	mc partition_num
 */
int do_mem_mc ( cmd_tbl_t *cmdtp, int flag, int argc, char *argv[])
{

	int	size ,i,partnum = 0;
	char version_string[] =	U_BOOT_VERSION;
	char fm_version[16];
	
	char *buf;
	int part1_err , part2_err ;

	image_header_t header;
	image_header_t *pimg_header = 0x80800000;


	if (argc < 2 && argc != -1) {
		printf ("Usage:\n%s\n", cmdtp->usage);
		return 1;
	}


	if(argc == -1)
	{
		printf ("argv=%s\n", argv);
		partnum = (int)simple_strtoul (argv, NULL, 16);
	}
	else
		partnum = (int)simple_strtoul (argv[1], NULL, 16);

	
	printf("partnum=%d\n",partnum);
	
	if( 1 == partnum  || 2 == partnum )
	{
		if( 1 == partnum )
			run_command("ubi part system_sw;ubi read 0x80800000 kernel 0x200000",0);		
		else
			run_command("ubi part system_sw;ubi read 0x80800000 kernelb 0x200000",0);		
		
		if (pimg_header->ih_magic == IH_MAGIC) 
		{
	
			printf("size=%d,name=%s\n",pimg_header->ih_size,pimg_header->ih_name);
		}
		else
		{
			printf("Corrupt Image!!\n");
			//printf("0x%x%x%x\n",buf,buf+1,buf+2);
			return 1;
		}
		
		// check kernel image
		 if (!image_check_hcrc (pimg_header) || !image_check_dcrc (pimg_header)) 
		 { 
				printf ("Bad kernel Checksum\n");
				return 1;			
		 }
		 // check rootfs image
		 else
		 {
			ulong rootfs_size; 
			ulong dcrc = 0;
			ulong rootfs_crc = 0;
			ulong data = 0x80800000;
			
			if( 1 == partnum )
			{
			   rootfs_size = simple_strtoul((char *)getenv("f_rootfs_datalen"),NULL,16);
			   rootfs_crc = simple_strtoul((char *)getenv("f_rootfs_crc"),NULL,16);
			   run_command("ubi part system_sw;ubi read 0x80800000 rootfs $(f_rootfs_datalen)",0);		
			   //run_command("ubi part system_sw;ubi read 0x80800000 rootfs 0x01000000",0);		
			   
			}
			else
			{
				rootfs_size = simple_strtoul((char *)getenv("f_rootfsb_datalen"),NULL,16);
				rootfs_crc = simple_strtoul((char *)getenv("f_rootfsb_crc"),NULL,16);
				run_command("ubi part system_sw;ubi read 0x80800000 rootfsb $(f_rootfsb_datalen)",0);
				//run_command("ubi part system_sw;ubi read 0x80800000 rootfsb 0x01000000",0);
			 }
			
				//printf("rootfs_size = %lx\n",rootfs_size);
			#if 1
				int i = 0;
				for(i = 0 ; i < rootfs_size; i++)
					dcrc += *((volatile uint8_t *)(data + i));
				
			#else
				dcrc ^= 0xffffffffL;
				dcrc = crc32 (dcrc, (unsigned char *)data, rootfs_size);
				dcrc ^= 0xffffffffL;
			#endif
			printf("rootfs CRC = %lX\n",ntohl(dcrc));

			if( dcrc != rootfs_crc )
			{
				printf ("Bad rootfs Checksum\n");
				return 1;						
			}
			else
			{
			
				//printf ("Good Header Checksum\n");
				sprintf(fm_version,"part%d_version",partnum);
				buf = getenv(fm_version);
				if(buf)
					printf("\n\nFirmware_version=[%s]\n\n",buf);
				else
					printf("\nFirmware_version=[Unknown]\n\n");
			}
											 	 			 			
		  }	  // end of CRC checksum
	}
	else
	{
		printf ("Unkown partition!\n");
		return 1;		
	}
	

	
	return 0;
}

U_BOOT_CMD(
	upfw,	3,	1,	do_update_fw,
	"upfw- update firmware with xxx_fullimage.img \n",
	"[bootfilename] partition_num\n"
);

U_BOOT_CMD(                                       
	upgrade,	4,	1,	do_upgrade,
	"upgrade - forward/backward copy memory to pre-defined flash location\n",
	"upgrade <source addr> <size> <partition>\n	- upgrade image to pre-define partition.\n"
);

U_BOOT_CMD(
	mc,	2,	1,	do_mem_mc,
	"mc	- image CRC check \n",
	"mc partnum \n       - image CRC check\n"
);

#endif




