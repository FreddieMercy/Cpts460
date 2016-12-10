
#include "ucode.c"
//l2u   [f1 f2]            // convert lower case to upper case
int main(int argc, char* argv[])
{
	int i;
	int infile = 0;
	int outfile = 1;
	int byteRead = -1;
	char *buff[256];

/*
		for (i=0; i<argc; i++){
	    printf("argv[%d]=%s\n", i, argv[i]);
	  }
*/
	if(argc == 3)
	{
		infile = open(argv[1], 0);
		outfile = open(argv[2], 1 | 64);
		//printf("infile: %d\n", infile);
		//printf("outfile: %d\n", outfile);
	}
	
	if(infile < 0)
	{
		printf("The input file '%s' does not exists!!\n", argv[1]);

	    if(infile != 0)
	    {
	        close(infile);
	    }


	    if(outfile != 1)
	    {
	        close(outfile);
	    }
		return -1;
	}

	while(byteRead != 0)
	{
		
		byteRead = read(infile, buff, 256);
		lower2upper(buff);
		write(outfile, buff, byteRead);
	}


    if(infile != 0)
    {
        close(infile);
    }


    if(outfile != 1)
    {
        close(outfile);
    }

	return 0;
}

lower2upper(char *input)
{
	int i;

	for(i = 0; i < strlen(input); i++)
	{

		if(input[i] <= 122 && input[i] >= 97)
		{
			input[i] = input[i] - 32;
		}
	}

}