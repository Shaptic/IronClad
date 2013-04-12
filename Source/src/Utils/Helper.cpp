#include <sstream>

#include "IronClad/Utils/Helper.hpp"

using namespace ic;

/**
 * Counts the number of numbers in a number.
 * @param   int     Number to count numbers of
 * @return  Number of numbers in number.
 **/
int util::num_len(const int number)
{
    int tmp     = number;
    int count   = 0;

    while(tmp > 0)
    {
        tmp = tmp / 10;
        count++;
    }
    return count;
}

/**
 * Coverts a C++ string to an integer value.
 *  This just makes code a tiny bit shorter, and I don't have to worry
 *  about calling c_str() every freaking time.
 * @param   std::string String to convert
 * @return  0 on failure, integer on success (which could be 0 of course).
 **/
int util::atoi(const std::string& str)
{
    return ::atoi(str.c_str());
}

void util::stripl(std::string& str)
{
    while(str[0] == ' ')    str = str.substr(1, -1);
    while(str[0] == '\t')   str = str.substr(1, -1);
}

/**
 * Convert a string to its uppercase equivalent.
 *  This directly modifies the original string argument.
 *  
 * @param std::string&  String to convert
 **/
void util::toupper(std::string& str)
{
    for(uint32_t i = 0; i < str.length(); ++i)
        str[i] = ::toupper(str[i]);
}

/**
 * Convert a string to its uppercase equivalent.
 *  This returns a new string, converted, leaving the given one untouched.
 *  
 * @param   std::string&    String to convert
 * @return  Uppercase-converted string.
 **/
std::string util::toupper_ret(const std::string& str)
{
    std::string tmp(str);
    for(uint32_t i = 0; i < str.length(); ++i)
        tmp[i] = ::toupper(str[i]);
    return tmp;
}

/**
 * Combine a C++ style string and a C string into one.
 *
 * @param   std::string*    String one
 * @param   char*           String two
 * @return  String one + string two.
 **/
std::string util::combine(const std::string& str1, const char* pstr2)
{
    std::stringstream ss;
    ss << str1 << pstr2;
    return ss.str();
}

/**
 * Combine a C style string and a C++ string into one.
 *
 * @param   char*           String two
 * @param   std::string&    String one
 * @return  String one + string two.
 **/
std::string util::combine(const char* pstr1, const std::string& str2)
{
    std::stringstream ss;
    ss << pstr1 << str2;
    return ss.str();
}

/**
 * Splits a string into individual parts at an special character.
 *
 * @param   std::string&    String to split
 * @param   char            Special character to split string with
 * @return  Parts of the string split at special character.
 **/
std::vector<std::string> util::split(const std::string& str, char token)
{
    std::string tmp(str);
    std::stringstream ss;
    std::vector<std::string> results;
    int index;

    while(true)
    {
        index = tmp.find(token);
        if(index == -1 || index == std::string::npos)
            break;

        results.push_back(tmp.substr(0, index));
        tmp = tmp.substr(index + 1, -1);
    }
    results.push_back(tmp);

    return results;
}
