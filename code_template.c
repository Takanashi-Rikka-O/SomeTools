
/* Name: code_template.c
 * Type: C program code file
 * Description: A code template producter.
 * Headers:
 * 	<unistd.h>
 * 	<sys/stat.h>
 * 	<sys/types.h>
 * 	<fcntl.h>
 * 	<stdio.h>
 * 	<string.h>
 * Function prototype:
 * 	static enum Suffix _Suffix_Check_(const char *filename,size_t stringlen);
 * 	static short int _Makefile_(const char *filename,const char *directory,enum ? filetype);
 * 	static short int _FilenameLength_(const char *filename,size_t stringlen);
 * 	static void _Usage_(void);
 * Last modified date: 2020-11-28 22:30
 * Fix:
 */

/* Head */

/* Feature */
#define _GNU_SOURCE_

/* Headers */
#include<unistd.h>
#include<sys/stat.h>
#include<sys/types.h>
#include<fcntl.h>
#include<stdio.h>
#include<stdlib.h>
#include<string.h>

extern int errno;


/* Macro */

/* Define description */
#define C "C program code file"
#define CPP "C++ program code file"
#define HEADER "Header"
#define ASSEMBLY "Assembly code file"
#define SH "Bash script file"

/* Define suffix */
#define CSUFFIX ".c"
#define CPPSUFFIX ".cpp"
#define HSUFFIX ".h"
#define ASUFFIX ".s"
#define BSUFFIX1 ".sh"
#define BSUFFIX2 ".bash"

/* Define error code */
#define NSUCC 0
#define EFAILED -1

/* Data */
enum Suffix {header,c,cpp,assembly,sh};
long long int MAXPATH=0;	/* max length,it would be resetted at later */

/* Function */
static enum Suffix _Suffix_Check_(const char *filename,size_t stringlen);
static short int _Makefile_(const char *filename,const char *directory,enum Suffix filetype);
static void _Usage_(void);
static short int _FilenameLength_(const char *filename,size_t stringlen);

int main(int argc,char *argv[]){

	int Total=argc;	/* All parameter */
	short int FilenameLength=-1;
	const char *FileName=NULL;	/* Would point to file name */
	const char *Directory=NULL;	/* Directory */
	size_t TargetLength=0;		/* Target string length */
	int ExitCode=NSUCC;
			/*
			 * Program will return the total of files been successfully created.
			 */

	enum Suffix suffix=c;	/* default */
	MAXPATH=pathconf("/",_PC_PATH_MAX);	/* Get maximum path length */

	/* Env */
	if (Total < 2){
		fprintf(stderr,"Usage error!\n");
		_Usage_();
		return EFAILED;
	}
	else;

	for (int index=1; index < Total; ++index){
		/* Cycle to create file,it is not an atomic operation */

		TargetLength=strlen(argv[index]);				/* Get length */
		FilenameLength=_FilenameLength_(argv[index],TargetLength);	/* Get file name length */
		Directory=argv[index];						/* Work path */
		FileName=argv[index];						/* file name */

		if (FilenameLength < 0)
			Directory=".";		/* Current work path */
		else if (FilenameLength == 0){	
			/* No filename given */
			fprintf(stderr,"Not find out filename from parameter - %s - skipped.\n",argv[index]);
			continue;
		}
		else{	
			*(argv[index]+(TargetLength-FilenameLength)-1)='\0';	/* separator to end directory path */
			FileName=argv[index]+(TargetLength-FilenameLength);	/* filename */	
		}

		if (strncmp(FileName,".",2) == 0){
			fprintf(stderr,"Filename is a '.',error usage! Skipped.\n");
			continue;
		}
		else;

		suffix=_Suffix_Check_(FileName,TargetLength);
		if (_Makefile_(FileName,Directory,suffix) < 0){
			fprintf(stderr,"Failed to creates %s,skipped!\n",FileName);
			continue;
		}
		else{
			++ExitCode;
		}
	}

	/* Primary */

	return ExitCode;
	/* Return */
}

static enum Suffix _Suffix_Check_(const char *filename,size_t stringlen){
	/*
	 * If no suffix,then guesses the file is a cpp file
	 */

	enum Suffix WhatItis=c;	/* Default */
	size_t index=0;		/* For string scan */

	/* Env */

	/* Scan string expected find '.' */
	for (index=stringlen-1; index > 0; --index){
		if (filename[index] == '.')
			break;
		else;
	}

	/* If index == 0,that means no suffix */
	if (! index)
		WhatItis=cpp;
	else{
		/* Matching */
		if (strncmp(&filename[index],CSUFFIX,2+1) == 0)
			WhatItis=c;
		else if (strncmp(&filename[index],CPPSUFFIX,3+1) == 0)
			WhatItis=cpp;
		else if (strncmp(&filename[index],HSUFFIX,2+1) == 0)
			WhatItis=header;
		else if (strncmp(&filename[index],ASUFFIX,2+1) == 0)
			WhatItis=assembly;
		else if (strncmp(&filename[index],BSUFFIX1,3+1) == 0 || strncmp(&filename[index],BSUFFIX2,5+1) == 0)
			WhatItis=sh;
		else;
	}

	/* Primary */

	return WhatItis;
	/* Return */
}

static short int _Makefile_(const char *filename,const char *directory,enum Suffix type){

	short int ReturnValue=EFAILED;	/* Return value */
	int TempfileDes=-1;	/* Temp file descriptor */
	char TempFilename[]="touch_newfile.XXXXXX";	/* Tempfile name */
	const char *TYPE=NULL;	/* field */
	char OldPath[MAXPATH];	/* Path saving */
	FILE *Output=NULL;	/* For fdopen */

	/* Env */

	/* Change directory */
	if (getcwd(OldPath,MAXPATH) < 0)
		return EFAILED;
	else;

	if (chdir(directory) < 0)
		return EFAILED;
	else
		if (access(filename,F_OK) == 0)		/* File been existed */
			goto _Makefile_EXIT;
		else;

	if ((TempfileDes=mkostemp(TempFilename,O_WRONLY|O_CREAT|O_EXCL)) < 0)	/* Create new file */
		goto _Makefile_EXIT;
	else{

		if ((Output=fdopen(TempfileDes,"w")) == NULL)	/* Open standard io stream */
		{
			(void)close(TempfileDes);
			goto _Makefile_EXIT;
		}
		else;
		
		switch (type){
			case header:
				TYPE=HEADER;
				break;

			case cpp:
				TYPE=CPP;
				break;

			case assembly:
				TYPE=ASSEMBLY;
				break;

			case sh:
				TYPE=SH;
				break;

			default:
				TYPE=C;
		}	/* Set type field */

		/* Writing */
		switch (type){

			case c:
			case cpp:
			case header:
				fprintf(Output,"\n"
						"/* Name: %s\n"
						" * Type: %s\n"
						" * Description:\n"
						" * Header:\n"
						" * Function prototype:\n"
						" * Last modified date:\n"
						" * Fix:\n"
						" */\n\n"
						" /* Feature */\n"
						" /* Header */\n"
						" /* Macro */\n"
						" /* Data */\n"
						" /* Function */\n"
						"\n"
						,filename,TYPE);
				break;

			default:
				fprintf(Output,"\n"
						"# Name: %s\n"
						"# Type: %s\n"
						"# Description:\n"
						"# Header:\n"
						"# Function prototype:\n"
						"# Last modified date:\n"
						"# Fix:\n"
						"\n\n"
						"# Data\n"
						"# Function\n"
						"\n"
						,filename,TYPE);

		}

		(void)fclose(Output);	/* Close file stream */
	}

	/* Primary */

	ReturnValue=link(TempFilename,filename);
	unlink(TempFilename);	/* Delete temp file */

	_Makefile_EXIT:

		chdir(OldPath);
	
		return ReturnValue;
	/* Return */
}

static void _Usage_(void){

	/* Env */

	printf("Usage:\n"
		"\tcode_template <file1> <file2> <file3>\n");

	/* Primary */

	return;
	/* Return */
}

static short int _FilenameLength_(const char *filename,size_t stringlen){

	/*
	 * -1 means no directory been specified.
	 *  0 means no file given.
	 */

	short int FILENAMELEN=0;

	/* Env */

	for (short int index=stringlen-1; index >= 0; --index){
		if (filename[index] == '/')
			break;
		else;
		++FILENAMELEN;
	}

	/* Primary */

	if (FILENAMELEN == 0)
		return 0;
	else if (FILENAMELEN == stringlen)
		return -1;
	else
		return FILENAMELEN;

	/* Return */
}


/* End */
