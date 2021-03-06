﻿//--------------------------------------------------------------------------------
// Copyright (c) 2017-2018, sanko-shoko. All rights reserved.
//--------------------------------------------------------------------------------

#ifndef __SP_STR_H__
#define __SP_STR_H__

#include "spcore/spcore.h"

//--------------------------------------------------------------------------------
// additional include
//--------------------------------------------------------------------------------

#include <string>

namespace sp{

    //--------------------------------------------------------------------------------
    // string
    //--------------------------------------------------------------------------------

    using namespace std;

    SP_CPUFUNC Mem1<string> strSplit(const char *str, const char *tok = " ,\t\n\r"){
        Mem1<string> dst;
        Mem1<char> buf(static_cast<int>(strlen(str) + 1), str);

        const char *ret = strtok(buf.ptr, tok);
        while (ret){
            dst.push(ret);
            ret = strtok(NULL, tok);
        }

        return dst;
    }

    SP_CPUFUNC string strFormat(const char *format, ...) {
        char str[SP_STRMAX];

        va_list arg;
        va_start(arg, format);
        ::vsprintf(str, format, arg);
        va_end(arg);

        return string(str);
    }

    SP_CPUFUNC string strTrim(const char *str, const char *tok = " ,\t\n\r") {
        string dst;

        const string src = str;

        const string::size_type start = src.find_first_not_of(tok);

        if (start != string::npos) {
            const string::size_type end = src.find_last_not_of(tok);
            dst = src.substr(start, end - start + 1);
        }
        return dst;
    }


    SP_CPUFUNC bool strExtVal(const char *line, const char *name, const char *format, void *param){
        const char *str = strstr(line, name);
        if (str == NULL) return false;

        Mem1<string> words = strSplit(str);
        if (words.size() < 2) return false;

        sscanf(words[1].c_str(), format, param);
        return true;
    }



}
#endif