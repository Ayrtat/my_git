%{	
	#include <stdio.h>
	#include <ctype.h>
	int first_time[32] = {1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1};
	char duplicate[32];
	char liter_for_temp[] = "abcdefghijklmnopqrstuvwxyz";
	int t_pos = 0;
	int ix = 0;
	char dif = 0;
%}
%%
[A-z] {
		if(isupper(*yytext))
		{ 
		   dif = *yytext - 'A'; 
		} else 
		{
		   dif = *yytext - 'a';
		}
		ix = (int) dif;
		if(first_time[ix]) 
		   {
			printf("%c", liter_for_temp[t_pos]);
			duplicate[ix] = liter_for_temp[t_pos];
			t_pos++;
			first_time[ix] = 0;
		   }
		else 
		   {
			printf("%c", duplicate[ix]);
		   }
	}
[ \t\n] { t_pos = 0;
  	  memset(first_time, 1, sizeof(first_time));
	  ix = 0;
	  dif = 0;
	  printf("\n"); }
%%
