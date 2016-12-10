
readByLine(int fd, char *buff, int count)
{
		int byteRead = 0;
		int byte = 0;
		char tmp[1];
		int num = count;

		if(num <= 0)
		{
			return byteRead;
		}

		byte = read(fd, tmp, 1);


		if(byte<=0)
		{
			if(byte<0)
			{
				printf("reading error!!\n");
			}

			return byteRead;
		}

		byteRead += byte;
		num--;
		
		//while(tmp[0] != '\n')
		while(tmp[0] != '\n')
		{
			//printf("success\n");
			buff[byteRead - 1] = tmp[0];

			if(num <= 0)
			{
				return byteRead;
			}

			byte = read(fd, tmp, 1);

			if(byte<0)
			{
				printf("reading error!!\n");
				return byteRead;
			}

			byteRead += byte;
			num--;

			//printf("%c\n", tmp[0]);
		}

		buff[byteRead - 1] = tmp[0];
		buff[byteRead] = '\0';

		//printf("from readByLine: %s\n", buff);

		return byteRead;
}


readByLine33(int fd, char *buff, int count)
{
		int byteRead = 0;
		int byte = 0;
		char tmp[1];
		int num = count;

		if(num <= 0)
		{
			return byteRead;
		}

		byte = read(fd, tmp, 1);


		if(byte<=0)
		{
			if(byte<0)
			{
				printf("reading error!!\n");
			}

			return byteRead;
		}

		byteRead += byte;
		num--;
		
		//while(tmp[0] != '\n')
		while(strlen(tmp[0]) != 33)
		{
			//printf("success\n");
			buff[byteRead - 1] = tmp[0];

			if(num <= 0)
			{
				return byteRead;
			}

			byte = read(fd, tmp, 1);

			if(byte<0)
			{
				printf("reading error!!\n");
				return byteRead;
			}

			byteRead += byte;
			num--;

			//printf("%c\n", tmp[0]);
		}

		buff[byteRead - 1] = tmp[0];
		buff[byteRead] = '\0';

		//printf("from readByLine: %s\n", buff);

		return byteRead;
}


parsingTheString(int fd, char *code, char *passwd)
{
		int byteRead=0;
		int iscode = 1;
		int found = 0;
		char buf[100];
		char *s;
		int end = 0;
		int counter;
		char cmd[100];
		int i = 0;
		int j = 0;
		int z = 0;

		//printf("username: %s\n", code);
		//printf("password: %s\n", passwd);

	    if(fd < 0)
	    {
	    	printf("cannot open the passwd!\n");
	        return 0;
	    }

	    if(strlen(code) == 0)
	    {
	    	return 0;
	    }

	    byteRead = readByLine(fd, &buf, 100);

	    do
	    {
	    	if(byteRead > 0)
	    	{
	    		for(i = 0; i < strlen(code); i++)
				{
					
					if(code[i] != buf[i])
					{
						iscode = 0;
						break;
					}
					
					//printf("%c", buf[i]);
					//printf("%d", i);
				}
				//printf("\n");

				if(iscode != 0)
				{
					found = 1;
				
				}
				
				if(found != 0)
				{
					//do the shell

					end = 0;
					counter = 0;
					j = 0;
					s = &buf;

					while(buf[counter] != '\n')
					{
						//from beginning, scan through the username, stop at the ':' which username:passwd
						if(buf[counter]!=':')
						{
							counter++;
						}
						else
						{
							if(counter == strlen(code))
							{
								counter++;

								while(buf[counter]!=':')
								{
									if(buf[counter] != '\n')
									{
										cmd[j] = buf[counter];
										counter++;
										j++;
									}
									else
									{
										return 0;
									}
								}

								cmd[j] = '\0';

								if(strlen(cmd) != strlen(passwd))
								{
									//printf("cmd = %d, passwd = %d\n", strlen(cmd), strlen(passwd));
									return 0;
								}
									
					    		for(z = 0; z < strlen(passwd); z++)
								{
									
									if(cmd[z] != passwd[z])
									{
										return 0;
									}
									
									//printf("%c", cmd[z]);
									//printf("%d", i);
								}

								//uid
								cmd[0] = '\0';
								j = 0;

								counter++;

								while(buf[counter]!=':')
								{
										if(buf[counter] != '\n')
										{
											cmd[j] = buf[counter];
											counter++;
											j++;
										}
										else
										{
											return 0;
										}
								}

								cmd[j] = '\0';

								s = &cmd;
								uid = atoi(s);

								//printf("uid is %d\n", uid);

								//gid
								cmd[0] = '\0';
								j = 0;

								counter++;

								while(buf[counter]!=':')
								{
										if(buf[counter] != '\n')
										{
											cmd[j] = buf[counter];
											counter++;
											j++;
										}
										else
										{
											return 0;
										}
								}

								cmd[j] = '\0';

								s = &cmd;
								gid = atoi(s);

								//printf("gid is %d\n", gid);

								//comment
								cmd[0] = '\0';
								j = 0;

								counter++;

								while(buf[counter]!=':')
								{
										if(buf[counter] != '\n')
										{
											cmd[j] = buf[counter];
											counter++;
											j++;
										}
										else
										{
											return 0;
										}
								}

								cmd[j] = '\0';

								s = &cmd;

								//dir

								cmd[0] = '\0';
								j = 0;

								counter++;

								while(buf[counter]!=':')
								{
										if(buf[counter] != '\n')
										{
											cmd[j] = buf[counter];
											counter++;
											j++;
										}
										else
										{
											return 0;
										}
								}

								cmd[j] = '\0';

								dir = &cmd;

								//printf("dir is %s\n", dir);

								//sh

								cmd[0] = '\0';
								j = 0;

								counter++;

								while(buf[counter]!=':')
								{
										if(buf[counter] != '\n')
										{
											cmd[j] = buf[counter];
											counter++;
											j++;
										}
										else
										{
											break;
										}
								}

								cmd[j] = '\0';

								sh = &cmd;
								
								//printf("sh is %s\n", sh);

								return 1;
							}

							counter++;
						}
					}

		    		//reset
		    		buf[0]='\0';
		    		
			    }

			    iscode = 1;

			    //printf("%c\n", buf[0]);
			}

		    byteRead = readByLine(fd, buf, 100);

		}while(byteRead > 0);


	return 0;
}


