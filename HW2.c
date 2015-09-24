#include <stdio.h>
#include <windows.h>
#include <WinBase.h>
#include <stdlib.h>
#include <string.h>
#include <stdlib.h>
#include <sys/time.h>

//checks whether there is an error in opening a file specified on the command line
void check_OpenFile_Return_Value(HFILE hObject, int index_in_argv);

//All files must be at most 128 characters because of the way OpenFile() is defined
void check_len_file(char *filename, int index_in_argv);

//Returns the current time in seconds
double curr_time();

//Conducts a trial with the specified buffer size and files to copy.
//Prints the time elapsed and the rate of bytes copied
void repeat_trials(int argc, char *argv[]);

int main(int argc, char *argv[])
{
	setbuf(stdout, NULL);
	for (int i=0; i<3; i++)
		repeat_trials(argc, argv);

}

void check_OpenFile_Return_Value(HFILE hObject, int index_in_argv)
{
	if (hObject == HFILE_ERROR)
	{
		printf("Error, existing file specified by argument %d from the command line failed to open\n", index_in_argv);
		exit(1);
	}
}

void check_len_file(char *filename, int index_in_argv)
{
	if (strlen(filename) > 128)
	{
		printf("Error, filename specified by argument %d from the command line exceeds 128 characters\n", index_in_argv);
		exit(1);
	}
}

//Code for getting the current time from http://www.ccplusplus.com/2011/11/gettimeofday-example.html
double curr_time()
{
        struct timeval tv;
        gettimeofday(&tv, 0);
        return tv.tv_sec + tv.tv_usec*1.0e-6;
}

void repeat_trials(int argc, char *argv[])
{
	if (argc != 4)
	{
		printf("Error, must supply 3 arguments, the buffer size followed by source and destination file\n");
		exit(1);
	}


	for (int i=1; i<=2; i++)
		check_len_file(argv[i+1], i);
	//if len too long, probably must change name of file

	OFSTRUCT file_info, file_info_new;
	HFILE hObject = OpenFile(argv[2], &file_info, OF_READ);
	check_OpenFile_Return_Value(hObject, 1);

	HFILE hObjectNewFile = OpenFile(argv[3], &file_info_new, OF_CREATE);
	check_OpenFile_Return_Value(hObjectNewFile, 2);

	int buffer_size = (int)(strtod(argv[1], NULL));
	if (buffer_size <= 0)
	{
		printf("Error, buffer size must be positive\n");
		exit(1);
	}
	int file_size;
	LARGE_INTEGER size_struct;
	GetFileSizeEx(hObject, &size_struct);
	file_size = size_struct.LowPart;
	//technically, we could avoid this call by doing an extra read of the file first (i.e. read the entire file and count the number of characters until EOF),
	//but there isn't any reason why we should iterate over the entire file to find its size rather than get it quickly
	//This number is needed upfront in order to determine the correct size for the buffer

	char *buffer = calloc(1, file_size + 1);
	if (buffer == NULL)
	{
		printf("Error: out of memory for buffer\n");
		exit(1);
	}
	 unsigned int num_bytes_read_so_far = 0, num_bytes_written_so_far = 0;

	 double initial_time, final_time;
	 initial_time = curr_time();


	 while (num_bytes_written_so_far < file_size)
	 {
		 unsigned int num_bytes_currently_read, num_bytes_currently_written;
		 int is_file_read = ReadFile(hObject, buffer + num_bytes_read_so_far, \
				 min(buffer_size, file_size - num_bytes_read_so_far), &num_bytes_currently_read, NULL);
		 if (!is_file_read)
		 {
			 printf("%d\n", GetLastError());
			 printf("Error, file not read\n");
			 free(buffer);
			 exit(1);
		 }
		 num_bytes_read_so_far += num_bytes_currently_read;
		 int is_file_written = WriteFile(hObjectNewFile, buffer + num_bytes_written_so_far, \
				 min(buffer_size, num_bytes_read_so_far - num_bytes_written_so_far), &num_bytes_currently_written, NULL);
		 if (!is_file_written)
		 {
			 printf("Error, file not written\n");
			 free(buffer);
			 exit(1);
		 }
		 num_bytes_written_so_far += num_bytes_currently_written;
	 }

	final_time = curr_time();
	printf("Copied %d bytes in %f seconds at the rate of %f bytes per second\n", \
			file_size, final_time - initial_time, file_size / (final_time - initial_time));
	//note that even if the time gradient is too small so we have division by zero, since the divisor has type double,
	//will just print infinity
	CloseHandle(&hObject);
	CloseHandle(&hObjectNewFile);
	free(buffer);

}

