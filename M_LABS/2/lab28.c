#include <stdio.h>

int main(int argc, char *argv[], char *envp[]) {
	int i = 0;
	printf("\nWELCOME TO LAB28:\nThe number of passed arguments %d\n",argc);
	printf("List of arguments:\n");
	while(argv[i]!=NULL) printf("%s ",argv[i++]);
	i = 0;
	printf("\nEnvirnoment:\n");
	while(envp[i]!=NULL) printf("%s\n",envp[i++]);
	return 0;
}
	
