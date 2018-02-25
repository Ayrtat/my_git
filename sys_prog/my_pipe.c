#include <unistd.h>

#include <stdio.h>

#include <stdlib.h>

#include <string.h>

#include <fcntl.h>

void generate(void)
{
	srand(time(NULL);
        while (1) {
		int c = rand() % 128;
 		c *= c;
	        c  = c << 2;
		putchar(c);
	}
}

void main(int argc, char *argv[])
{
	FILE *key;
	FILE *res_f = fopen(argv[3],"w");
	int p_chiper[2];
	int p_file[2];
	int chip, result = 0;
	char data;
	size_t n1,n2;
	
	if(strcmp(argv[1],"encrypt") == 0)
	   key = fopen("key","w");        //Зашифрованный файл
	else perror("Could not create encrypt-file\n");
     	
 	/* Создание труб */
	
	if((pipe(p_chiper) && pipe(p_file))!=0)
	   {
	     perror("Could not create one of the pipes\n");
	     exit(1);
	    }

	switch(fork())
	{
		case -1:
			perror("fork() failed\n");
			exit(1);
		case 0:
			close(1); 		//Закрыли поток, будем перенаправлять
			dup(p_chiper[1]); 	//Перенаправление вывода одного конца трубы
			close(p_chiper[0]);
			close(p_chiper[1]);
			close(p_file[0]);
			close(p_file[1]);
			
			if(strcmp(argv[1],"encrypt")==0)
			  generate();		//Данная функция передает сгенерированный ключ
						//На стандартный вывод, коем является p_chiper[1]
			if(strcmp(argv[1],"decrypt")==0)
			  execlp("cat","cat","key",NULL);
		default:
			switch(fork())
			{
				case -1:
					perror("fork() failed\n");
					exit(1);
				case 0:
					close(1);
					dup(p_file[1]);		//Перенаправление вывода
					close(p_file[0]);
					close(p_file[1]);
					close(p_chiper[0]);
					close(p_chiper[1]);
					execlp("cat", "cat", argv[2], NULL);
				default:
					close(p_file[1]);
					close(p_chiper[1]);
					while ((n2=read(p_file,&data,1))>0)
					{
						n1 = read(p_chiper[0], &chip,1);
						result = data ^ key;
						fputc(result, r_file);
					}
					if (strcmp(argv[1], "encrypt") == 0) 
					
fclose(key_file);

					
fclose(result_file);

			}
	
	}
}
					

			

	

