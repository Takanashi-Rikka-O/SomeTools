//	Switch_Hex.cpp
//	Version : 0.2
//	Date : 14-04-20
//	Last revise : 16-04-20
//	Description : 
//			Receive a number from command line and then show other hex format for option.
//	Function : 
//			void _Convert_Put_(const size_t & Len,const char & Opt,const char* Number_String,char* Binary_Array=NULL);
//	Fields : 
//		1> Adding a new mode to respones Number-String check `_Character_Check_'.
//		2> Adjustment for parameters checking statement `switch'.If `getopt' returned a '?',then program would be quiting.
//		3> Deleted float data checking,if had a '.' contain in Number String by user sent,program would as character to respones.
//		4> Not set up to help messages selection,it had been conformed in error messages.
//		5> Deleted parameters number checking before `getopt-cycle',that is for up to the program effective.

#include<iostream>
#include<cstdlib>
#include<cstring>
#include<unistd.h>	//	getopt
#include<limits.h>	//	Data lenght

//	Name space 

using namespace std;

////

//	Extern zone

extern int getopt(int argc,char* const argv[],const char* optstring);
extern char* optarg;			//	If the option have a link-data,then use this pointer to point it.
extern int optind,opterr,optopt;	//	optopt save option character.
					//	optind as a description of getopt()'procession.
////

//	Function declare zone

void _Convert_Put_(const size_t & Len,const char & Opt,const char* Number_String,char* Binary_Array=NULL);

bool _Character_Check_(const size_t & Str_Len,const char* Str,const unsigned short int & Start_Position=0);

////

int main(int argc,char* argv[])		//	Receive command line parameters.
{

	char opt(0);	//	Save value from getopt() return.
	size_t Len(0);	//	Save lenght of string.

	if (1 == argc)
	{
		cerr<<"\'-h\' option to get help message.\n";
		exit(EXIT_FAILURE);
	}
	else;

	while ((opt=getopt(argc,argv,"x:o:a:b:")) != -1)		//	If all options had been processed,return -1.
	{

		switch (opt)		//	Option selection.
		{
			case '?':

				//	Error.Option had not be defined.
				//	Error message and Help messages.
					cout<<"Format : "<<endl
						<<"-x Num : Hex format."<<endl
						<<"-o Num : Oct format."<<endl
						<<"-b Num : Binary format."<<endl
						<<"-a Num : All formats to putting."<<endl
						<<"Float data have not been supported..!"<<endl;
					exit(EXIT_FAILURE);

			default:

				///////////////////////////	Number string check....

				Len=strlen(optarg);	//	Get lenght.

				//	Check whether character in this string.
				//	NO.0 element could is '-' .
				if (*optarg < '0' || *optarg > '9')
					if (*optarg == '-')		//	NO.0 element is equal to '-'.
					{
					
						//	true 0--9, false outside of that scope.
						bool Not_Existed(_Character_Check_(Len,optarg,1));	//	Check if has a character in number string.
											//	1 is start at NO.1 element.
						if (Not_Existed)
							;
						else
						{
							//	Error messages.
							cerr<<"You must contained a character in the number string to me,or right?"<<endl;
							exit(EXIT_FAILURE);
						}
					}
					else
					{
						cerr<<"You must to send a number string to me!"<<endl;	//	Error message.
						exit(EXIT_FAILURE);
					}
				else
				{
					bool Not_Existed(_Character_Check_(Len,optarg));	//	Check number string.
					if (Not_Existed)			//	Head address to start.
						;
					else
					{
						//	Error messages.
						cerr<<"You must contained a character in the number string to me,or right?"<<endl;
						exit(EXIT_FAILURE);
					}
				}


				///////////////////////////	Work branches....

				switch (opt)
				{
					case 'x':	//	Hex
					case 'o':	//	Oct
						_Convert_Put_(Len,opt,optarg);	//	Call _Convert_Put_().
						break;

					case 'a':
					case 'b':	//	Binary
						{
							char* BA=new char[66];	//	Max 64bit.
							if (BA)
								_Convert_Put_(Len,opt,optarg,BA);	//	Call _Convert_Put_().
							else
							{
								cerr<<"Could not to get memory for string!"<<endl;
								exit(EXIT_FAILURE);
							}
					
							delete[] BA;		//	Recycle memory.
						}
						break;

					default:		
						;
				}	//	Nested 'switch'

		}	//	'switch' outside


	}	//	'while' cycle

	exit(EXIT_SUCCESS);
}

//	Function definition zone

void _Convert_Put_(const size_t & Len,const char & Opt,const char* Number_String,char* Binary_Array)
{

	if (Number_String)
	{
		long long int TempValue(0);	//	64bit variable.
		unsigned long long int Card_Num(1);	//	Cardinal number.

		for (short int Count=Len-1; Count >=0 && Number_String[Count] != '-'; --Count,Card_Num*=10)
			TempValue+=(Number_String[Count]-'0')*Card_Num;		//	Take real value.
		TempValue=(Number_String[0] == '-')?TempValue*(-1):TempValue;	//	Positive-number or Negative-number

		switch (Opt)
		{
			case 'x':	//	Hex
				cout<<hex<<showbase<<TempValue<<dec<<endl;	//	Hex format put.
				break;

			case 'o':	//	Oct
				cout<<oct<<showbase<<TempValue<<dec<<endl;	//	Oct format put.
				break;

			case 'a':
				cout<<showbase<<hex<<TempValue<<endl		//	Hex and Oct format.
					<<oct<<TempValue<<endl<<dec;

			case 'b':	//	Binary
				{	

				//	In C language,could declare a variable in switch label after a  character ';'.

					unsigned short int Value_Type(0);	//	Data lenght.
					if (TempValue >= CHAR_MIN && TempValue <= CHAR_MAX)
						Value_Type=8;
					else if (TempValue >= INT_MIN && TempValue <= INT_MAX)
						Value_Type=32;
					else
						Value_Type=64;
	
					//	Convert to binary.

					char Lenght(Value_Type-1);				//	Lenght of array
					long long int Mask(1);			//	Mask
					for ( ; Lenght >= 0 ; --Lenght,Mask<<=1)
						Binary_Array[Lenght]=(TempValue & Mask)?'1':'0';	//	Test 1 or 0.
					Binary_Array[Value_Type]='\0';
	
					//	Shortest output.
					for (Lenght=0; Lenght < Value_Type && Binary_Array[Lenght] == '0'; ++Lenght);

					Lenght=(Lenght > 8)?Lenght:0;
					cout<<Binary_Array+Lenght<<endl;		//	Show binary string
				}

				break;

			default:
				;
		}


	}
	else
		cerr<<"Null Number String!"<<endl;
}



bool _Character_Check_(const size_t & Str_Len,const char* Str,const unsigned short int & Start_Position)
{
	bool What_it_is(true);	//	true 0--9,false char.
	for (unsigned short int Count(Start_Position); Count < Str_Len && What_it_is; ++Count)
		if (Str[Count] >= '0' && Str[Count] <= '9')	//	Test
			;	//	In scope 0--9
		else	What_it_is=false;	//	Out scope ! 0--9

	return What_it_is;
}

////
