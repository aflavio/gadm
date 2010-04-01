/* **************************************************
 * General purpose whitespace based string tokenizer
 *
 * (c) 2008 Mayank Lahiri
 * mayanklahiri@gmail.com
 *
 *  This file is part of PSMiner.

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

#ifndef __TOK_H
#define __TOK_H

#include<vector>
using namespace std;

class Tokenizer	{
	public:
		Tokenizer();
		~Tokenizer();
		Tokenizer(char *str);
		void tokenize(char *str);
		char *token();
		char *token(int which) { return tokens[which]; };
		void next();
		int num_tokens() { return tokens.size(); };
		bool operator()() { return cur_token < tokens.size(); };

	private:
		unsigned cur_token;
		vector<char *> tokens;
		char *str_copy;
		int str_copy_size;
};

#endif
