#include "IronClad/Utils/Logging.hpp"

using ic::util::CLogging;

CLogging::CLogging(const char* p_filename) : m_width(-1), m_print(true)
{
    time_t now;
    time(&now);

    m_file.open(p_filename, std::ios::out);
    if(!m_file.is_open())
        std::cerr << "[ERROR] Unable to open logging file.\n";

    m_file << "[INFO] Log initialized on " << ctime(&now);
    this->Flush();
}

CLogging::~CLogging()
{
    if(!m_file.is_open()) return;

    time_t now;
    time(&now);

    this->Flush();
    m_log << "[INFO] Log closed on " << ctime(&now);
    this->Flush();

    m_file.close();
}

void CLogging::Close()
{
    if(!m_file.is_open())
        return;

    time_t now;
    time(&now);

    this->Flush();
    m_log << "[INFO] Log closed on " << ctime(&now);
    this->Flush();

    m_file.close();
}

void CLogging::Flush()
{
    // Don't write things with [DEBUG] tag in the release build.
#ifdef _DEBUG
    m_file << m_log.str();
#else
    if(m_log.str().find("[ERROR]") != std::string::npos)
        handle_error(m_log.str());
    if(m_log.str().find("[DEBUG]") == std::string::npos)
        m_file << m_log.str();
#endif // _DEBUG

    m_file.flush();
    m_log.str(std::string());
}

void CLogging::PrintLastLog() const
{
    if(!m_print) return;

    // Only print to stdout in debug builds.
#ifdef _DEBUG
    std::cout << m_log.str();
    if(m_log.str().back() != '\n')
        std::cout << std::endl;
#endif // _DEBUG
}

std::string CLogging::GetLastLog() const
{
    return m_log.str();
}

CLogging& CLogging::SetWidth(const int w)
{
    m_width = w;
    return (*this);
}
