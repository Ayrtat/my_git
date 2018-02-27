%{
	#include <stdio.h>
	#include <stdlib.h> 
	int i = 0;
	int j = 0;
	int len = 0;
	int m = 100;
	int * l_all = NULL;
	int input_leng = 0;
	char my_text[100] = "thehistoryoflosangelesbeginssometenortwentythousandyearsbeforethefirstEuropeansarrivedinsouthernCali";
%}

%%
[A-z]+		{
			int input_leng = yyleng;
			int i = 0;
			int j = 0;
			int len = 0;
			l_all = yyalloc (input_leng);
			if(l_all == NULL) printf("\nerror\n");
			memset(l_all, 0 , sizeof(l_all));
			while (i < input_leng) {
				len = l_all[i];
				while (yytext[len] != yytext[i+1] && (len > 0))
				{
					len = l_all[len];
				}
				if (yytext[len] == yytext[i+1]) 
				{
					l_all[i+1] = len + 1;
				}
				else l_all[i+1] = 0;
				i++;
			}
			while(len < input_leng && j < m) {
				while((yytext[len] != my_text[j]) && (len > 0))
					{ 
						len=l_all[len];
					}
				if(yytext[len] == my_text[j])
					{
						len++;
					}
				else len = 0;

				j++;
			}
			if (len == input_leng) printf("Such substring is: %s",yytext);
		}
%%
