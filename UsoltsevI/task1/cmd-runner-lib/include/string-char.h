#pragma once
//--------------------------------------------------------------------
//
// string-char header
//
//--------------------------------------------------------------------
//
// Just functiouns to work with strings
//
//--------------------------------------------------------------------

int count_char_in_string(const char *str, char c);
int count_words_in_string(const char *str, int len);
char *get_next_word(const char *str, int len, int *end_index);
const char *end_string(const char *str);
