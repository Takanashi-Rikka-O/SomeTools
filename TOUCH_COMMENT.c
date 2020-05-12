//	T_TL.c
//	Version : 0.1
//	Date : 05-05-20
//	Last revise : 05-05-20
//	Symbol-Constraint : 
//				_Xx..._ - Function Symbol
//				Xx...|x|X - Variable Symbol
//	Description : 
//			Only receive a parameter from command-line and then create a file its name is like to the parameter.
//		Description layout like this file's.For the file suffix is '*.s','//' convert to '#'.
//	Functions : 
//		bool _Make_Description_(const int Destined_fd,const char* FileName,bool Its_not_S);
//		bool _Suffix_Scan_(const char* FileName);
//	Headers : 
//		unistd.h -> Unix standard header.
//		fcntl.h -> Unix file control.
//		sys/stat.h -> File stat information.
//		stdio.h -> ISO C standard I/O.
//		stdlib.h -> ISO C general functions.
//		stdbool.h -> ISP C standard bool type.
//		string.h -> String functions.
//		time.h -> Unix Time struction and time functions.

#include<unistd.h>
#include<fcntl.h>
#include<sys/stat.h>
#include<stdio.h>
#include<stdlib.h>
#include<stdbool.h>
#include<string.h>
#include<time.h>

bool _Make_Description_(const int Destined_fd,const char* FileName,bool Its_not_S);		//	Write description message to file.
bool _Suffix_Scan_(const char* FileName);	/*	Suffix checking.	*/

int main(int argc,char* argv[])		/*	Would receive command-line arguments.	*/
{
	/*	At first check the command-line argument.Number of it must is equal 2.	*/
	if (argc != 2)
	{
		/*	Arguments is too less.	*/
		fprintf(stderr,"Arguments is too less.\nUse stynax : TOUCH_C filename\n");
		exit(EXIT_FAILURE);
	}
	else;
	
	bool What_symbol=_Suffix_Scan_(*(argv+1));	/*	Scan string to determine the comment symbol.	*/
						/*	true -> '/' ; false -> '#' 	*/

	/*	Create and open file with write only,access mode is '0600'.	*/
	int FileDes=open(*(argv+1),O_WRONLY|O_CREAT|O_EXCL,S_IRUSR|S_IWUSR);
	if (FileDes < 0)
	{
		/*	Open file failed.	*/
		fprintf(stderr,"Create file fault!\n");
		exit(2);
	}
	else;
	

	if (! _Make_Description_(FileDes,*(argv+1),What_symbol))	/*	Failed	*/
	{
		fprintf(stderr,"Write description layout messages to file have failed!\n");
		close(FileDes);	
		exit(3);
	}
	else
		close(FileDes);	/*	Succeed.	*/
	
	exit(EXIT_SUCCESS);	/*	Quit.	*/
}

bool _Make_Description_(const int Destined_fd,const char* FileName,bool Its_not_S)
{
	/*	Get Memory to save description messages.	*/
	char* Description_buffer=(char*)malloc(sizeof(char) * 1024);	/*	1KB lenght.	*/
	/*	Check pointer.	*/
	if (Description_buffer)	/*	Not NULL.	*/
	{
		/*	Get date.	*/
		time_t Currently_Time=time(NULL);
		const char* Date_Str=ctime(&Currently_Time);	/*	Convert time value to local date string.	*/

		/*	Write data to buffer.	*/
		if (Its_not_S)
			snprintf(Description_buffer,1024,
				"//\t%s\n"
				"//\tVersion : 0.1\n"
				"//\tDate : %s"
				"//\tLast revise : %s"
				"//\tSymbol-Constraint :\n"
				"//\t\t\t_Xx..._ - Function Symbol\n"
				"//\t\t\tXx...|x|X - Variable Symbol\n"
				"//\tDescription :\n"
				"//\tFunctions :\n"
				"//\tHeader :\n",
				FileName,Date_Str,Date_Str
				);
		else		
			snprintf(Description_buffer,1024,
			"#\t%s\n"
			"#\tVersion : 0.1\n"
			"#\tDate : %s"
			"#\tLast revise : %s"
			"#\tSymbol-Constraint :\n"
			"#\t\t\t_Xx..._ - Function Symbol\n"
			"#\t\t\tXx... - Normaly Symbol\n"
			"#\tDescription :\n"
			"#\tFunctions :\n"
			"#\tHeader :\n",
			FileName,Date_Str,Date_Str
			);

		/*	Write description layout mesages to file.	*/
		size_t Realy_To_W=strlen(Description_buffer);	/*	Messages lenght.	*/
		
		if (write(Destined_fd,Description_buffer,Realy_To_W) < 0)
		{
			fprintf(stderr,"While writing description layout messages happened a error!\n");
			return false;
		}
		else
		{
			free(Description_buffer);	/*	Recycle resource.	*/
			return true;
		}
			

	}
	else	/*	NULL.	*/
	{
		fprintf(stderr,"Failed to apply for buffer!\n");
		return false;
	}
}

bool _Suffix_Scan_(const char* FileName)
{
	if (*FileName == '\0')	/*	First character check.	*/
	{
		fprintf(stderr,"The first character is 'null'!\n");
		exit(EXIT_FAILURE);
	}
	else;

	short int Length_Str=strlen(FileName);	/*	String lenght.	*/
	/*	Examine suffix.	*/
	
	if (FileName[Length_Str-1] == 'h' && FileName[Length_Str-2] == 's' && FileName[Length_Str-3] == '.')	/* *.sh	*/
		return false;
	else if (FileName[Length_Str-1] == 's' && FileName[Length_Str-2] == '.')	/* *.s	*/
		return false;
	else
		return true;

}
