/* **************************************************
 * General purpose whitespace based string tokenizer
 *
 * (c) 2008 Mayank Lahiri
 * mayanklahiri@gmail.com

    Foobar is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    Foobar is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with Foobar.  If not, see <http://www.gnu.org/licenses/>.
  **********************************************************************/

#include"tokenizer.h"
#include<string.h>
#include<ctype.h>

Tokenizer::Tokenizer()	{
	cur_token = 0;
	str_copy_size = 0;
}

Tokenizer::Tokenizer(char *str)	{
	cur_token = 0;
	str_copy_size = 0;
	tokenize(str);
}
Tokenizer::~Tokenizer()	{
	if(str_copy_size)
		delete[] str_copy;
}

void Tokenizer::tokenize(char *str)	{
	cur_token = 0;
	tokens.clear();
	int len = strlen(str);
	if(!len) return;
	if(str_copy_size < len+1)	{
		if(str_copy_size)
			delete[] str_copy;
		str_copy = new char[len+1];
		str_copy_size = len;
	}
	strcpy(str_copy, str);
	char *p = str_copy;

	/* begin tokenization */
	while(1)	{
		while(*p && isspace(*p))
			p++;
		if(*p && !isspace(*p))	{
			tokens.push_back(p);
			while(*p && !isspace(*p))
				p++;
			*p++ = 0;
		} else
			break;
	}
}

char *Tokenizer::token()	{
	if(cur_token >= tokens.size())
		return NULL;
	return tokens[cur_token];
}

void Tokenizer::next()	{
	cur_token++;
}
