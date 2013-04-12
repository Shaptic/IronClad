#include "IronClad/Utils/Parser.hpp"

using namespace ic;
using util::g_Log;
using util::CParser;

CParser::CParser()
{
    m_pairs.clear();
}

CParser::~CParser()
{
    m_pairs.clear();
}

bool CParser::LoadFromFile(const std::string& filename)
{
    return this->LoadFromStream(std::ifstream(filename),
        0, -1, filename.c_str());
}

bool CParser::LoadFromStream(std::ifstream& file,
                             const std::streampos& start,
                             const std::streampos& end,
                             const char* fn)
{
    if(!file.is_open())
    {
        util::g_Log.Flush();
        util::g_Log << "[ERROR] File does not exist: ";
        util::g_Log << (fn == NULL ? "unknown filename" : fn);
        util::g_Log.PrintLastLog();
        return false;
    }

    std::string line;
    uint32_t line_no = 0;
    file.seekg(start);

    while(std::getline(file, line) &&
         (end == std::streampos(-1) || file.tellg() != end))
    {
        ++line_no;
        if(line.empty() || line[0] == '/')
            continue;

        std::vector<std::string> parts = util::split(line, '=');

        if(parts.size() != 2)
        {
            util::g_Log.Flush();
            util::g_Log << "[INFO] Failed to parse line " << line_no;
            util::g_Log << " of " << (fn == NULL ? "file" : fn);
            util::g_Log << ": '" << line << "'\n";
            util::g_Log.PrintLastLog();
            continue;
        }

        // This trims leading spaces.
        util::stripl(parts[0]);
        
        // Assign to dictionary.
        m_pairs[parts[0]] = parts[1];
    }

    return (line_no > 0);
}

bool CParser::LoadFromStream(const char** str, const uint32_t start,
                             const uint32_t finish)
{
    if(str == NULL)
    {
        util::g_Log.Flush();
        util::g_Log << "[ERROR] Fragmented raw data.\n";
        util::g_Log.PrintLastLog();
        return false;
    }

    uint32_t line_no = start;

    while(line_no++ <= finish)
    {
        std::string line(str[line_no]);

        if(line.empty() || line[0] == '/')
            continue;

        std::vector<std::string> parts = util::split(line, '=');

        if(parts.size() != 2)
        {
            util::g_Log.Flush();
            util::g_Log << "[INFO] Failed to parse line " << line_no;
            util::g_Log << "of raw data: '" << line << "'\n";
            util::g_Log.PrintLastLog();
            continue;
        }

        // This trims leading spaces.
        util::stripl(parts[0]);

        // Assign to dictionary.
        m_pairs[parts[0]] = parts[1];
    }

    return (line_no > start);
}

CParser::KeyValueDict::const_iterator& CParser::Begin() const
{
    return m_pairs.begin();
}

CParser::KeyValueDict::const_iterator& CParser::End() const
{
    return m_pairs.end();
}

void CParser::Reset()
{
    m_pairs.clear();
}

std::string CParser::GetValue(const std::string& key)
{
    KeyValueDict::const_iterator f = m_pairs.find(key);
    return (f == m_pairs.end()) ? "" : f->second;
}

std::string CParser::GetValue(const std::string& key) const
{
    KeyValueDict::const_iterator f = m_pairs.find(key);
    return (f == m_pairs.end()) ? "" : f->second;
}

int CParser::GetValuei(const std::string& key) const
{
    return this->GetValue(key)=="" ? 0 : atoi(this->GetValue(key).c_str());
}

bool CParser::GetValueb(const std::string& key) const
{
    return this->GetValuei(key) != 0;
}

float CParser::GetValuef(const std::string& key) const
{
    return this->GetValue(key)=="" ? 0.f : atof(this->GetValue(key).c_str());
}

std::vector<std::string> CParser::GetValues(const std::string& key,
                                            const char delim) const
{
    return util::split(this->GetValue(key), delim);
}

const CParser::KeyValueDict& CParser::GetPairs() const 
{
    return m_pairs;
}

std::streampos CParser::FindInFile(std::ifstream& stream,
                                   const char* pfinder,
                                   const std::streampos& farthest)
{
    return FindInFile(stream, std::string(pfinder), farthest);
}

std::streampos CParser::FindInFile(std::ifstream& stream,
                                   const std::string& finder,
                                   const std::streampos& farthest)
{
    std::string line;
    std::streampos p = stream.tellg();

    while(std::getline(stream, line) &&
         (farthest == std::streampos(-1) || stream.tellg() < farthest))
    {
        if(line.empty()) continue;
        if(line.find(finder) != std::string::npos)
        {
            std::streampos ret = stream.tellg();
            stream.seekg(p);
            return ret;
        }
    }

    stream.seekg(p);
    return std::streampos(-1);
}
