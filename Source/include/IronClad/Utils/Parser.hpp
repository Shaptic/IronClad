/**
 * @file
 *  Utils/Parser.hpp - Declares a sort of .INI file parser for file types
 *  that are often used throughout @a IronClad.
 *
 * @author      George Kudrayvtsev (halcyon)
 * @version     1.0
 * @copyright   Apache License v2.0
 *  Licensed under the Apache License, Version 2.0 (the "License").         \n
 *  You may not use this file except in compliance with the License.        \n
 *  You may obtain a copy of the License at:
 *  http://www.apache.org/licenses/LICENSE-2.0                              \n
 *  Unless required by applicable law or agreed to in writing, software     \n
 *  distributed under the License is distributed on an "AS IS" BASIS,       \n
 *  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.\n
 *  See the License for the specific language governing permissions and     \n
 *  limitations under the License.
 *
 * @addtogroup Utilities
 * @{
 **/

#ifndef IRON_CLAD__UTILS__PARSER_HPP
#define IRON_CLAD__UTILS__PARSER_HPP

#include <map>
#include <vector>
#include <fstream>
#include <sstream>
#include <string>

#include "Logging.hpp"
#include "Helper.hpp"

namespace ic
{
namespace util
{
    /**
     * Calculates a hash for a given value.
     *  This algorithm is ripped from the MurmurHash2 algorithm, 
     *  created by Austin Appleby. You can find it here:
     *  https://sites.google.com/site/murmurhash/
     *  
     *  The seed is located at CAsset::hash_seed and is automatically
     *  generated randomly via CAssetManager on initialization.
     * 
     * @param   void*       Data to hash
     * @param   uint32_t    Size of the data
     * 
     * @return  The hash value.
     **/
    static inline uint32_t Murmur2(const void* pdata, uint32_t size,
                                   const uint32_t seed)
    {
        const unsigned int m = 0x5bd1e995;
        const int r = 24;

        // Initialize the hash to a 'random' value
        uint32_t h = seed ^ size;

        // Mix 4 bytes at a time into the hash
        const unsigned char* data = (const unsigned char*)pdata;

        while(size >= 4)
        {
            uint32_t k = *(uint32_t*)data;

            k *= m; 
            k ^= k >> r; 
            k *= m; 

            h *= m; 
            h ^= k;

            data += 4;
            size -= 4;
        }

        // Handle the last few bytes of the input array
        switch(size)
        {
        case 3: h ^= data[2] << 16;
        case 2: h ^= data[1] << 8;
        case 1: h ^= data[0]; h *= m;
        };

        // Do a few final mixes of the hash to ensure the last few
        // bytes are well-incorporated.
        h ^= h >> 13;
        h *= m;
        h ^= h >> 15;

        return h;
    }

    /**
     * Parses files based on key=value1,value2,...,valueN pairs.
     *  This file parses defines a very generic type of parsing, since most
     *  files (meshes, levels) in IronClad use a similar formatting. 
     *  For example, an ICLevel, a entity can be placed like so:
     *      <entity>
     *          <surface>
     *          texture=Quad.tga
     *          position=100,200
     *          w=32
     *          h=32
     *          
     *          texcoords=0,0,1,0,1,1,0,1
     *          </surface>
     *      </entity>
     *      
     *  So, you can see that values are split up by an '=' delimiter. Thus,
     *  this generic parser can create a dictionary of values that can
     *  then be easily accessed. 
     *  This eliminates the need to read the file sequential and worry 
     *  about what might come later and what options are defined while
     *  you're reading.
     *  Now, you can read in the file (or portion of a file), and then
     *  access the values by key, given that they exist.
     **/
    class CParser
    {
        typedef std::map<
//#ifdef _DEBUG
            std::string,    // For easily debugging the dictionary

/// This really isn't worth the effort. Maybe @todo?
/*#else
            uint32_t,       // Hashing for speed, only relevant if 
                            // an item is accessed more than once, really,
                            // since hashing the string is O(n) anyway.
#endif // _DEBUG
*/
            std::string> KeyValueDict;

    public:
        CParser();
        virtual ~CParser();

        /**
         * Fills the key-value dictionary by parsing a file stream.
         *  This is more versatile than the generic LoadFromFile(), since
         *  you can specify where in the stream you would like to start
         *  and where you would like to end. By default, the whole file is
         *  read. 
         *  This comes in handy if a file, for example, has several entity
         *  specifications, so there's more than one "texture" key. Thus
         *  you could read until "<entity>", mark that, read till
         *  "</entity>", mark that, then pass the stream to this method
         *  and it would only give you the key-value pairs found in that
         *  section of the file.
         *  
         * @param   std::ifstream   File stream to read
         * @param   std::streampos  Starting point in the stream    (opt=0)
         * @param   std::streampos  Ending point in the stream      (opt=END)
         * @param   char*           Filename the stream comes from  (opt="file")
         * 
         * @return  TRUE if at least one pair was parsed,
         *          FALSE otherwise, or if file was not open.
         **/
        bool LoadFromStream(std::ifstream& file,
                            const std::streampos& start = 0,
                            const std::streampos& finish= -1,
                            const char* filename = "file");
        bool LoadFromStream(const char**    str, 
                            const uint32_t  start,
                            const uint32_t  finish);
        bool LoadFromFile(const std::string& filename);

        /**
         * Returns the iterator at the beginning of the dictionary.
         **/
        KeyValueDict::const_iterator& Begin() const;

        /**
         * Returns the iterator at the end of the dictionary.
         **/
        KeyValueDict::const_iterator& End() const;

        /// Clears the dictionary.
        void Reset();

        /**
         * Retrieves a value based on a key in the dictionary.
         *  If the file stream contained 'texture=Data.tga', and you
         *  called GetValue("texture"), the function gives back "Data.tga".
         * 
         * @param   std::string     Key to access
         * 
         * @return  Value if it exists, empty string otherwise.
         * 
         * @info    The returned value (in non-const) is safe to modify,
         *          since it is a copy of the value. The original is
         *          impossible to change, regardless.
         **/
        std::string GetValue(const std::string& key);
        std::string GetValue(const std::string& key) const;

        /**
         * Attempts to return a converted representation of a value.
         **/
        int     GetValuei(const std::string& key) const;
        bool    GetValueb(const std::string& key) const;
        float   GetValuef(const std::string& key) const;

        /**
         * Returns an array of values from a key-value pair based on a char.
         *  This will split the 'value' part of a pair based on a
         *  delimiter, and return the result.
         *  
         * @param   std::string     Key to access
         * @param   char            Character to split 'value' on
         * 
         * @return  Result if 'key' exists,
         *          the 'value' in the pair (in a vector) if not.
         *          
         * @see     util::split()
         **/
        std::vector<std::string> GetValues(const std::string& key,
                                           const char delimiter) const;

        /// Direct access to the dictionary.
        KeyValueDict& GetPairs();
        const KeyValueDict& GetPairs() const;

        /**
         * Finds the first instance of a string within a file stream.
         *  This function basically reads a file until it finds an instance
         *  of "pfinder" in the current line, up until it reaches the 
         *  optional "max_pos" argument, which specifies a position in 
         *  the stream to stop at. If left at the default, -1, the search
         *  is done till EOF (or some other reason causing std::getline
         *  to return false). 
         *  The function will return the stream to its starting point
         *  as it was when the function was called.
         *  
         * @param   std::ifstream&  Stream to search
         * @param   char*           String to find
         * @param   std::streampos  Place to stop search (optional=EOF)
         * 
         * @return  Position the requested string was found at,
         *          std::streampos(-1) if it was not found.
         *          
         * @info    Stream is left at the same point it started at.
         **/
        static std::streampos FindInFile(std::ifstream& stream,
                                         const char* pfinder,
                                         const std::streampos& max_pos = -1);

        static std::streampos FindInFile(std::ifstream& stream,
                                         const std::string& finder,
                                         const std::streampos& max_pos = -1);

    private:
        // An empty string, so GetValue() doesn't return a temporary
        // buffer when nothing is found in the 'const' version.
        static std::string s_empty;
        
        KeyValueDict m_pairs;
    };
}   // namespace util
}   // namespace ic

#endif // IRON_CLAD__UTILS__PARSER_HPP

/** @} **/
