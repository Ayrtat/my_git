/* Companion source code for "flex & bison", published by O'Reilly
 * Media, ISBN 978-0-596-15597-1
 * Copyright (c) 2009, Taughannock Networks. All rights reserved.
 * See the README file for license conditions and contact info.
 * $Header: /home/johnl/flnb/code/RCS/fb3-2.y,v 2.1 2009/11/08 02:53:18 johnl Exp $
 */
/* calculator with AST */

%{
#  include <stdio.h>
#  include <stdlib.h>
#  include "fb3-2.h"
%}

%union {
  struct ast *a;
  double d;
  struct symbol *s;		/* which symbol */
  struct symlist *sl;
  int fn;			/* which function */
}

/* declare tokens */
%token <d> NUMBER
%token <s> NAME
%token <fn> FUNC
%token <fn> PMET
%token <fn> MMET
%token <fn> FLAG
%token <fn> AGG
%token <fn> FUNCNUM
%token EOL

%token IF THEN ELSE WHILE DO LET


%nonassoc <fn> CMP
%right '='
%left '+' '-'
%left '*' '/'
%nonassoc '|' UMINUS

%type <a> exp stmt explist calclist
%start calclist

%%

stmt: exp
;

exp: exp CMP exp          { $$ = newcmp($2, $1, $3); }
   | exp '+' exp          { $$ = newast('+', $1,$3); }
   | exp '-' exp          { $$ = newast('-', $1,$3);}
   | exp '*' exp          { $$ = newast('*', $1,$3); }
   | exp '/' exp          { $$ = newast('/', $1,$3); }
   | exp ':' exp          { $$ = newast(':', $1,$3); }
   | '|' exp              { $$ = newast('|', $2, NULL); }
   | AGG '(' '{' exp '}' ',' exp '(' exp ')'')'{ $$ = newaggnode('A',$4,$7,$9); }
   //| '{' explist '}'	  { $$ = $2; }
  // | '(' explist ')'	  { $$ = $2; }
   | '-' exp %prec UMINUS { $$ = newast('M', $2, NULL); }
   | NUMBER               { $$ = newnum($1); }
   | FUNCNUM		  { $$ = newfnum($1); }
   | FUNC '(' explist ')' { $$ = newfunc($1, $3); }
   | NAME                 { $$ = newref($1); }
   | NAME '=' exp         { $$ = newasgn($1, $3); }
   | NAME '(' explist ')' { $$ = newcall($1, $3); }		  
   | FLAG		  { $$ = newflg($1); }
   | MMET		  { $$ = newmtr($1); }
;

explist: exp
 | exp ',' explist  {  $$ = newast('L', $1, $3); }
;

calclist: /* nothing */
  | calclist stmt EOL {
    if(debug) dumpast($2, 0);
     eval($2);
     printf("\nПарсер:выражение имеет верный формат\n> ");
     treefree($2);
    }
;
%%
