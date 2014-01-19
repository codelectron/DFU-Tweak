/*
Author:Krishna Pattabiraman

 */

#include <getopt.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>


#define HEADER_SIZE 64
#define VERSION "1.0"

enum FileType { BOOTSTRAP, BOOTLOADR, KERNEL, ROOTFS1, ROOTFS2 }; 
char *strings[] = { 
	[BOOTSTRAP] =  "BootStrap",
	[BOOTLOADR] =  "BootLoader",
	[KERNEL]    =  "Kernel",
	[ROOTFS1]   =  "ROOTFS1",
	[ROOTFS2]   =  "ROOTFS2",
};

#pragma pack(push)          // save original alignment on stack
#pragma pack(1)             // set alignment to 1 byte boundary
struct HeaderPkt
{
	unsigned long address;
	unsigned int  crc;
	unsigned long filesize;
	int filetype;
	char board[12];
	char version[16];
	unsigned char padding[20]; // Padding bytes are to keep the file size intact //FIXME some automatic calculation has to be impletmented
};
#pragma pack(pop)

char filename[50];
char output_filename[50];
void print_help(int exval);
void WriteToFile( struct HeaderPkt *hdr, char *filename);
void ListHeader(char *filename);
int main(int argc, char *argv[])
{


	int opt;	
	int createflag = 0;
	struct stat statistics;

	char *address = NULL;
	printf("\tMake Header Utility\n");
	if(argc == 1) 
	{
		fprintf(stderr, "This utility needs some arguements\n");
		print_help(1);
	}
	struct HeaderPkt *header = malloc(sizeof(struct HeaderPkt));

	while((opt = getopt(argc, argv, "hVv:cf:a:o:l:b:s:i:t:")) != -1) 
	{
		switch(opt) {
			case 'h':
				print_help(0);
				break;
			case 'c':
				createflag = 1;
				break; 
			case 'V':
				printf("Version %s\n", VERSION);
				exit(0);
				break;
			case 'v':
				if(strlen(optarg) < 15)
				{
				    printf("WARNING:See below\n");
				    printf("Version number less than required!\n");
				    exit(1);
				}
				strncpy(header->version,optarg,15);
				break;
			case 't':
				printf("Image Type %s\n", strings[atoi(optarg)]);
				header->filetype = atoi(optarg);
				break;
			case 'b':
				strncpy(header->board,optarg,11);
				printf("Board %s\n", header->board);
				break;
			case 'i':
				printf("Image %s\n", optarg);
				memcpy(filename, optarg, sizeof(optarg));
				break;
			case 'a':
				header->address = strtoul(optarg, NULL, 16);
				break;
			case 'l':
				printf("Filename %s\n", optarg);
				ListHeader(optarg);
				break;
			case 'f':
				printf("Filename %s\n", optarg);
				memcpy(filename, optarg, sizeof(optarg));
/*
              			if( stat(optarg, &statistics) != 0)
					perror("Could not stat ");
				header->filesize = statistics.st_size;
				printf("FIle Size %d\n",((int) statistics.st_size));
*/
				break;
			case 's':
				header->filesize = strtoul(optarg, NULL, 16);
				printf("Partition Size %d \n",header->filesize);
				break;
			case 'o':
				printf("Output: %s\n", optarg);
				strcpy(output_filename, optarg);
				break;
			case ':':
				fprintf(stderr, "Error - Option `%c' needs a value\n\n", optopt);
				print_help(1);
				break;
			case '?':
				//fprintf(stderr, "No such option: `%c'\n\n", optopt);
				print_help(1);
		}
	}
	if(createflag)
	{
		printf("Creating Image Header \n");
		WriteToFile(header, output_filename);
	}
	return 0;
}

void ListHeader(char *filename)
{
	FILE *fp;
	struct HeaderPkt hdr;
	int i;

	fp = fopen(filename,"r");
	if(fp == NULL)
	{
		printf("Error: Unable to open the file\n");
		exit(1);
	}
	fread(&hdr,HEADER_SIZE,1, fp);
	fclose(fp);
	printf("The Board model : %s \n",hdr.board);
	printf("The Load Address: %x \n",(unsigned int) hdr.address);
	printf("The Binary Type : %s \n",strings[hdr.filetype]);
	printf("The File Size   : %d \n",hdr.filesize);
	printf("The Version no  : %s \n",hdr.version);
#if 0
	printf("The Version No  : ");
	for ( i = 0; i < 14 ; i++)
		printf("%c",hdr.version[i]);
	printf("\n");
#endif
	exit(0);
}

void WriteToFile( struct HeaderPkt *hdr, char *filename)
{
	FILE *fp;
	fp = fopen(filename,"wb");
	if(fp == NULL)
	{
		printf("Error: Unable to open the file\n");
		exit(1);
	}
	fwrite(hdr, sizeof(struct HeaderPkt), 1, fp );
	fclose(fp);
}

void print_help(int exval) 
{
	printf("Example: ./makehdr -a 0x2000000 -o header -t 2 -b sam9263  -s 0x200000 -c\n");
	printf("  -h              print this help and exit\n");
	printf("  -V              print version and exit\n");
	printf("  -l FILE         List the Header details\n");
	printf("  -c 		  To create the header\n");
	printf("  -a              Load Address in Flash\n");
	printf("  -b              Board type \n");
	printf("  -s		  Parititoin Size\n");
	printf("  -t              Image Type:  0 - bootstrap, 1 - bootloader, 2 - kernel, 3 - rootfs1, 4 -rootfs2  \n");
	printf("  -o FILE         set output file\n");
	printf("  -f FILE         Input file\n");

	exit(exval);
}
