//	CP.c
//	Version : 0.1
//	Date : Sun May 10 14:59:48 2020
//	Last revise : Sun May 10 14:59:48 2020
//	Symbol-Constraint :
//			_Xx..._ - Function Symbol
//			Xx...|x|X - Variable Symbol
//	Description :
//		Features are like to 'cp' command.This program will copy a file which is contain 'file inantion',and the program will read data in that file but
//	doesn't copy the part is 'file inantion' of that file.
//
//	Functions :
//
//		struct FDes _Open_Files_(const char* path1,const char* path2);	/*	Open files use the same permission.If failed to open,set 'fdes=-1'*/
//		short int _Copy_(const struct FDes* File_List);	/*	Copy files.File_list contains two for file descriptors.	*/
//
//		ssize_t _Valid_Data_Take_(void* Buffer,const size_t Buffer_Len);	
//		/*	Scan the buffer and take invalid data then return valid data length.	*/
//
//	Header :
//		
//		<fcntl.h> 	/*	File control.	*/
//		<unistd.h>	/*	Unix standard functions.	*/
//		<sys/stat.h>	/*	File system.	*/
//		<sys/types.h>	/*	System types.	*/
//		<stdio.h>	/*	ISO C Standard IO.	*/
//		<stdlib.h>	/*	ISO C usually functions.	*/

#include<fcntl.h>
#include<unistd.h>
#include<sys/stat.h>
#include<sys/types.h>
#include<stdio.h>
#include<stdlib.h>


/*	Type define.	*/

struct FDes{
	int Source;		/*	Source file.	*/
	int Destination;	/*	Destianation file.	*/
	off_t Source_Length;	/*	Source file length.	*/
	short int Status_Value;
};

#define SYNTAX_ERR 1		/*	Incorrect syntax.	*/
#define OPEN_S_ERR 2		/*	Open source file failed.	*/
#define OPEN_D_ERR 3		/*	Open destination file failed.	*/
#define GET_S_INFO_ERR 4	/*	Get source file information failed.	*/
#define DESTINATION_DEL_ERR 5	/*	Destination file had been existed and delete it failed.	*/
//#define MB_1 (1024*1024*1024)	/*	1MB.	*/
#define KB_1 (1024*1024)	/*	1KB.	*/
//#define B_256 (256)		/*	256B.	*/
#define GET_BUFFER_FAIL 6	/*	Get buffer failed.	*/
#define WRITE_FAIL 7		/*	Write data to destination file failed.	*/
#define NO_ANSWER -1		/*	User won't to delete old destination file.	*/
#define S_NOT_REGFILE -2	/*	Source file is not a regular file.	*/
#define GET_VALID_BUFF_ERR 8	/*	_Copy_ cannot get memory for pointer Valid_Take_Buffer.	*/

static void* Valid_Take_Buffer=NULL;	/*	This pointer would point to a Temp_Memory_Blocak from _Copy_ to calls 'malloc'.	*/

/*	Type define END.*/

/*	Function define.	*/

struct FDes _Open_Files_(const char* path1,const char* path2);	/*	Open source file and destination file.	*/

short int _Copy_(const struct FDes* File_List);	/*	Copy. Source file TO Destination file	*/

ssize_t _Valid_Data_Take_(void* Buffer,const size_t Buffer_Len);

/*	Function define END.	*/

/*	.text	*/

int main(int argc,char* argv[])
{
	if (argc != 3)
	{
		fprintf(stderr,"Syntax : CP Source Destination\n");	/*	Failed usage.	*/
		return SYNTAX_ERR;
	}
	else
		umask(0000);	/*	Allow all bits of permission.	*/

	struct FDes Work_List={
				.Source=-1,
				.Destination=-1,
				.Source_Length=0,
				.Status_Value=0,
			};	/*	Initializing for struct object.	*/

	Work_List=_Open_Files_(*(argv+1),*(argv+2));	/*	Open file.	*/
	/*	Check status.	*/
	switch (Work_List.Status_Value)
	{
		case OPEN_S_ERR:
			fprintf(stderr,"Open source file failed!\n");
			return OPEN_S_ERR;

		case OPEN_D_ERR:
			fprintf(stderr,"Open destination file failed!\n");
			return OPEN_D_ERR;

		case GET_S_INFO_ERR:
			fprintf(stderr,"Get information of source file failed!\n");
			return GET_S_INFO_ERR;
		case DESTINATION_DEL_ERR:
			fprintf(stderr,"Destination file had been existed and deleting it was failed!\n");
			return DESTINATION_DEL_ERR;
		case NO_ANSWER:
			fprintf(stderr,"Doesn't delete the file %s.\n",*(argv+2));
			return NO_ANSWER;
		case S_NOT_REGFILE:
			fprintf(stderr,"Source file is not a regular file!\n");
			return S_NOT_REGFILE;

		default:
			/*	_Copy_ would be closing all file descriptors in the list.	*/
			;
			short int Copy_Status=_Copy_(&Work_List);	/*	Record result of copy.	*/
			switch (Copy_Status)
			{
				case GET_BUFFER_FAIL:
					fprintf(stderr,"Get buff space failed!\n");
					goto Delete_Failed_File;
			
				case GET_VALID_BUFF_ERR:
					fprintf(stderr,"Get buff space for take valid data fault!\n");
					goto Delete_Failed_File;

				case WRITE_FAIL:
					fprintf(stderr,"Write data failed!\n");

					Delete_Failed_File:
						(void)unlink(*(argv+2));	/*	Delete the file which has failed to copied.	*/
						return Copy_Status;

				default:
					fprintf(stdout,"Succeed to done the copy work!\n");
			}
	}

	exit(EXIT_SUCCESS);	/*	Quit.	*/

}

struct FDes _Open_Files_(const char* path1,const char* path2)
{
	struct FDes Temp_List={.Source=-1,.Destination=-1,.Source_Length=0,.Status_Value=0,};		/*	Local structure.	*/
	struct stat Temp_buf;	/*	Save source file informations.	*/
	mode_t S_Mode=0;		/*	File mode bits of source file.	*/
	if (0 == lstat(path1,&Temp_buf))	/*	Donn't track symbolic link.	*/
	{
		if ((Temp_buf.st_mode & S_IFMT) != S_IFREG)	/*	Source file is not a regular file.	*/
		{
			Temp_List.Status_Value=S_NOT_REGFILE;
			return Temp_List;
		}
		else;

		S_Mode=Temp_buf.st_mode;	/*	Get source file mode setting.	*/
		Temp_List.Source_Length=Temp_buf.st_size;	/*	File Length.	*/
	}
	else
	{
		Temp_List.Status_Value=GET_S_INFO_ERR;	/*	Get information failed.	*/
		return 	Temp_List;
	}

	if ((Temp_List.Source=open(path1,O_RDONLY)) < 0) /*	Try to open Source file.	*/
	{
		Temp_List.Status_Value=OPEN_S_ERR;
		return Temp_List;
	}
	else
	{
		/*	At first,check the destination file whether had been existed.	*/
		if (-1 == lstat(path2,&Temp_buf))	/*	Destination file had not been existed.	*/
			;
		else
		{
			/*	Prompt before unlink file.	*/
			char Answer='\0';
			do
			{
				fprintf(stdout,"The file %s had been existed.Do you want to delete this file? (y or n)\n",path2);
				Answer=getchar();	/*	User's answer.	*/
			} while (Answer != 'y' && Answer != 'n');

			if (Answer == 'y')
				;
			else
			{
				Temp_List.Status_Value=NO_ANSWER;	/*	User won't to delete the old file.	*/
				(void)close(Temp_List.Source);
				return Temp_List;	
			}

			if (-1 == unlink(path2))	/*	Try to delete the old Destination file.	*/
			{
				Temp_List.Status_Value=DESTINATION_DEL_ERR;
				(void)close(Temp_List.Source);
				return Temp_List;
			}
			else;
		}
	
		/*	Creat with WRITE_ONLY,mode == S_Mode.	*/
		if ((Temp_List.Destination=open(path2,O_WRONLY|O_CREAT|O_EXCL,S_Mode)) < 0)	/*	Try to creat Destination file.	*/
		{
			Temp_List.Status_Value=OPEN_D_ERR;
			(void)close(Temp_List.Source);
			return Temp_List;
		}
		else;
	}

	return Temp_List;	/*	Open Source file and Creat Destination file all were succeed.	*/
				/*	By the time,Temp_List.Status_Value == 0.	*/
}

short int _Copy_(const struct FDes* File_List)
{
	short int Return=0;	/*	Return value.	*/
	if ((Valid_Take_Buffer=malloc(sizeof(char) * KB_1)))	/*	Get memory space for temporarly array.	*/
		;
	else
		Return=GET_VALID_BUFF_ERR;	/*	Fault.	*/

	void* BUFFER=malloc(sizeof(char) * KB_1);
	if (BUFFER && Return != GET_VALID_BUFF_ERR)
	{
		ssize_t B_Valid_Len=0;		/*	Buffer valid data length.	*/
		ssize_t Last_Write=0;		/*	The last writing.	*/
		
		while (read(File_List->Source,BUFFER,KB_1) > 0)
		{
			/*	On the part of file inantion,read will readed the 'null' byte '\0'.	*/

			/*	Scan the buffer to determine if happened inantion reading.	*/
			/*	Could use 'strlen()' to get realy length in the buffer.	*/
			/*	But 'strlen()' use '\0' as end,if the buffer have not '\0' strlen would return a incorrect result.	*/
			
			B_Valid_Len=_Valid_Data_Take_(BUFFER,KB_1);

			/*	Write data from BUFFER to Destination file.	*/
			if ((Last_Write=write(File_List->Destination,BUFFER,B_Valid_Len)) < 0)
			{
				Return=WRITE_FAIL;
				break;
			}
			else
				fprintf(stdout,"Wrote %d Bytes...\n",Last_Write);	/*	Hint messages.	*/
		}
	}
	else
		Return=GET_BUFFER_FAIL;


	/*	Recycle resource.	*/
	(void)close(File_List->Source);
	(void)close(File_List->Destination);

	if (Return == GET_VALID_BUFF_ERR)
		;
	else
		free(Valid_Take_Buffer);	/*	Delete memory is own to the temporary arrary.	*/

	if (Return == GET_BUFFER_FAIL)
		;
	else
		free(BUFFER);

	return Return;
}

ssize_t _Valid_Data_Take_(void* Buffer,const size_t Buffer_Len)
{
	ssize_t Valid_Len=0;	/*	Valid data length.	*/
	unsigned int Index=0;	/* 	Array index.	*/

	char* B_Pointer=(char*)Buffer;	/*	B_Pointer point to Buffer.	*/
	char* T_Pointer=(char*)Valid_Take_Buffer;	/*	Valid_Take_Buffer is a global pointer.	*/

	while (Index < Buffer_Len)	/*	Scaning.	*/
	{
		if (B_Pointer[Index] != '\0')	/*	Example.	*/
			T_Pointer[Valid_Len++]=B_Pointer[Index];
		else;
		++Index;
	}

	/*	Copy data from Temporary array to Realy array.	*/	

	/*	Extended ASM.	*/
	__asm__ volatile (
				"pushq	%%rcx\n\t"
				"pushq	%%rdi\n\t"
				"pushq	%%rsi\n\t"	/*	Store registers.	*/

				"movq	$0x00,%%rcx\n\t"	/*	Initialize.	*/
				"movl	%%eax,%%ecx\n\t"	/*	eax have the value of Valid_Len.	*/
				"movq	%1,%%rsi\n\t"		/*	%1->Valid_Take_Buffer.	*/
				"movq	%2,%%rdi\n\t"		/*	%2->Buffer.	*/
				"cld\n\t"			/*	DF=0.	*/
				"rep movsb\n\t"			/*	Repeat to move.	*/

				"popq	%%rsi\n\t"	/*	Restore registers.	*/
				"popq	%%rdi\n\t"
				"popq	%%rcx\n\t"
				:
				: "a" (Valid_Len),"r" (Valid_Take_Buffer),"r" (Buffer)
				: "%rdi","%rsi","%rcx"	/*	Prompt to Translater that does't using the registers 'RDI','RSI','RCX'.	*/
			);

	return Valid_Len;
}



