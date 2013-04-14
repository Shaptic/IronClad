#include "IronClad/Asset/Sound2D.hpp"

using namespace ic;
using asset::CSound2D;
using util::g_Log;

CSound2D::CSound2D(bool orig, const void* const own) : 
    CAsset(orig, own), m_buffer(0), m_source(-1),
    m_volume(1.f), m_lasterror(AL_NO_ERROR)
{
    CSound2D::s_allSounds.push_back(this);
}

CSound2D::~CSound2D()
{
    if(s_sources[m_source] != 0)
    {
        alDeleteSources(1, &s_sources[m_source]);
        s_sources[m_source] = 0;
    }

    alDeleteBuffers(1, &m_buffer);
}

/************************************************************************/
/*                        BEGIN STATIC METHODS                          */
/************************************************************************/

std::vector<CSound2D*> CSound2D::s_allSounds;

bool CSound2D::InitializeOpenAL()
{
    static bool once = false;

    if(!once)
    {
        memset(s_sources, 0, sizeof s_sources);
        alutInit(NULL, NULL);
        if(alutGetError() != ALUT_ERROR_NO_ERROR) return false;
        alGetError();
        once = true;
    }

    return true;
}

void CSound2D::AdjustVolume(const float dv)
{
    for(size_t i = 0; i < s_allSounds.size(); ++i)
    {
        s_allSounds[i]->SetVolume(s_allSounds[i]->GetVolume() + dv);
    }
}

void CSound2D::GetAvailableSource()
{
    // First, check if the current available source is
    // in range.
    if(s_available < 0 || s_available >= 256)
        s_available = 0;

    // Now, check if the current available one is.. well..
    // available.
    if(s_sources[s_available] == 0)
        return;

    // Then, check if the source immediately
    // following the current one is okay.
    if(s_available < 255 && s_sources[s_available + 1])
    {
        ++s_available;
        return;
    }

    // Now, iterate through the sources array to find 
    // the next unused one.
    for(size_t i = 0; i < sizeof s_sources / sizeof s_sources[0]; ++i)
    {
        if(s_sources[i] == 0)
        {
            s_available = i;
            return;
        }
    }

    // There is nothing available.
    s_available = -1;
}

/************************************************************************/
/*                      BEGIN NON-STATIC METHODS                        */
/************************************************************************/

bool CSound2D::LoadFromFile(const std::string& filename)
{
    /// Buffer size for .ogg decoding (32 bytes).
    static const int BUFFER_SIZE = 32768;

    // Variables for libvorbis decoding.
    vorbis_info*        p_Info              = NULL; // 
    OggVorbis_File      ogg_file;                   // Information about the file.
    std::vector<char>   buffer;                     // .ogg data
    char                array[BUFFER_SIZE];         // Temporary data
    int                 bit_stream;
    int                 bytes_read;                 // Bytes read on each call
    int                 format, freq;               // .ogg information
    int                 endian              = 0;    // 0 is little endian, 1 is big endian
    FILE*               p_File              = NULL; // Raw .ogg file

    m_lasterror = AL_NO_ERROR;

    // Check if there's already something loaded.
    if(m_buffer != 0)
    {
        alDeleteBuffers(1, &m_buffer);
    }
    if(m_source != -1)
    {
        if(s_sources[m_source] != 0)
            this->UnloadSource();
    }

    // Check for a valid filename.
    if(filename.empty())
    {
        m_lasterror = AL_INVALID_NAME;
        m_error     = alGetString(m_lasterror);
        g_Log.Flush();
        g_Log << "[ERROR] OpenAL: " << m_error << "\n";
        g_Log.PrintLastLog();
        return false;
    }

    // Determine if the given file is .ogg or not.
    // Open the .ogg file in binary-read mode.
    p_File = fopen(filename.c_str(), "rb");
    if(p_File == NULL)
    {
        m_lasterror = AL_INVALID_NAME;
        m_error     = alGetString(m_lasterror);
        g_Log.Flush();
        g_Log << "[ERROR] OpenAL: " << m_error << "\n";
        g_Log.PrintLastLog();
        return false;
    }

    // Initialize OggVorbis_File structure and check for valid ogg
    if(ov_open_callbacks(p_File, &ogg_file, NULL, 0, OV_CALLBACKS_DEFAULT) < 0)
    {
        ov_clear(&ogg_file);
        fclose(p_File);

        // The file isn't .ogg, so try and load it as a .wav.
        return this->LoadFromFile_WAV(filename.c_str());
    }

    // Get information from the file.
    p_Info = ov_info(&ogg_file, -1);

    if(p_Info->channels == 1)
        format = AL_FORMAT_MONO16;
    else
        format = AL_FORMAT_STEREO16;
    freq = p_Info->rate;

    // Decode the data
    do
    {
        // Read 32 bytes into array.
        bytes_read = ov_read(&ogg_file, array,
            BUFFER_SIZE, endian, 2, 1, &bit_stream);

        // Insert to all data array.
        buffer.insert(buffer.end(), array, array + bytes_read);
    }
    while(bytes_read > 0);

    // Clean up memory
    ov_clear(&ogg_file);

    // Generate OpenAL audio buffers from raw OGG data.
    alGenBuffers(1, &m_buffer);
    if((m_lasterror = alGetError()) != AL_NO_ERROR)
    {
        m_error     = alGetString(m_lasterror);
        g_Log.Flush();
        g_Log << "[ERROR] OpenAL: " << m_error << "\n";
        g_Log.PrintLastLog();
        return false;
    }

    alBufferData(m_buffer, format, &buffer[0], buffer.size(), freq);
    if((m_lasterror = alGetError()) != AL_NO_ERROR)
    {
        m_error     = alGetString(m_lasterror);
        g_Log.Flush();
        g_Log << "[ERROR] OpenAL: " << m_error << "\n";
        g_Log.PrintLastLog();
        return false;
    }

    m_filename  = filename;
    return true;
}

bool CSound2D::LoadFromFile(const char* p_filename)
{
    return this->LoadFromFile(std::string(p_filename));
}

bool CSound2D::LoadFromAudio(const CSound2D* const p_Copy)
{
    m_buffer    = p_Copy->GetBuffer();
    m_filename  = p_Copy->GetFilename();
    m_source    = -1;

    return true;
}

bool CSound2D::Play()
{
    if(this->GetAudioState() == AL_PLAYING)
    {
        alSourcePlay(s_sources[m_source]);

#ifdef _DEBUG
        g_Log.Flush();
        g_Log << "[DEBUG] Force-playing audio: " << m_filename << ":";
        g_Log << m_source << " (" << s_sources[m_source] << ")\n";
        g_Log.PrintLastLog();
#endif // _DEBUG
        return true;
    }

    CSound2D::GetAvailableSource();
    if(s_available == -1)
    {
        m_lasterror = AL_OUT_OF_MEMORY;
        m_error     = alGetString(m_lasterror);
        g_Log.Flush();
        g_Log << "[ERROR] OpenAL: " << m_error << "\n";
        g_Log.PrintLastLog();
        return false;
    }

    ALuint source = 0;
    alGenSources(1, &s_sources[s_available]);
    if((m_lasterror = alGetError()) != AL_NO_ERROR)
    {
        m_error     = alGetString(m_lasterror);
        g_Log.Flush();
        g_Log << "[ERROR] OpenAL: " << m_error << "\n";
        g_Log.PrintLastLog();
        return false;
    }
    m_source = s_available;

#ifdef _DEBUG
    g_Log.Flush();
    g_Log << "[DEBUG] Created audio source: " << m_filename << ":";
    g_Log << m_source << " (" << s_sources[m_source] << ")\n";
    g_Log.PrintLastLog();
#endif // _DEBUG

    alSourcei(s_sources[m_source], AL_BUFFER, m_buffer);
    alSourcef(s_sources[m_source], AL_GAIN, m_volume);
    if((m_lasterror = alGetError()) != AL_NO_ERROR)
    {
        m_error     = alGetString(m_lasterror);
        g_Log.Flush();
        g_Log << "[ERROR] OpenAL: " << m_error << "\n";
        g_Log.PrintLastLog();
        return false;
    }

    alSourcePlay(s_sources[m_source]);

#ifdef _DEBUG
    g_Log.Flush();
    g_Log << "[DEBUG] Playing audio: " << m_filename << ":";
    g_Log << m_source << " (" << s_sources[m_source] << ")\n";
    g_Log.PrintLastLog();
#endif // _DEBUG

    return true;
}

bool CSound2D::Pause()
{
    if(m_source == -1) return false;

    if(this->GetAudioState() == AL_PAUSED) return true;

#ifdef _DEBUG
    g_Log.Flush();
    g_Log << "[DEBUG] Pausing audio: " << m_filename << ":";
    g_Log << m_source << " (" << s_sources[m_source] << ")\n";
    g_Log.PrintLastLog();
#endif // _DEBUG
    alSourcePause(s_sources[m_source]);

    return true;
}

bool CSound2D::Stop()
{
    if(m_source == -1)
        return false;

    if(this->GetAudioState() == AL_STOPPED)
        return true;

#ifdef _DEBUG
    g_Log.Flush();
    g_Log << "[DEBUG] Stopped audio: " << m_filename << ":";
    g_Log << m_source << " (" << s_sources[m_source] << ")\n";
    g_Log.PrintLastLog();
#endif // _DEBUG

    alSourceStop(s_sources[m_source]);
    return true;
}

bool CSound2D::UnloadSource()
{
    if(m_source == -1)
        return false;

#ifdef _DEBUG
    g_Log.Flush();
    g_Log << "[DEBUG] Unloading audio source: " << m_filename << ":";
    g_Log << m_source << " (" << s_sources[m_source] << ")\n";
    g_Log.PrintLastLog();
#endif // _DEBUG

    alDeleteSources(1, &s_sources[m_source]);
    s_sources[m_source] = 0;
    m_source = -1;

    m_lasterror = alGetError();
    m_error     = alGetString(m_lasterror);
    return m_lasterror == AL_NO_ERROR;
}

/**
 * Sets the starting position of the sound source,
 * defaulting to <0, 0, 0>.
 * @param   math::vector2_t&    Position
 **/        
void CSound2D::SetPosition(const math::vector2_t& Pos)
{
    if(m_source != -1)
        alSource3f(s_sources[m_source], AL_POSITION, Pos.x, Pos.y, 0.0f);
}

/**
 * Sets the velocity for the sound source, defaulting to <0, 0, 0>.
 *  If velocity is set to something other than the default value,
 *  a Doppler effect is produced as the sound source moves toward
 *  or away from the listener.
 *
 * @param   math::vector2_t&    Velocity
 **/        
void CSound2D::SetVelocity(const math::vector2_t& Vel)
{
    if(m_source != -1)
        alSource3f(s_sources[m_source], AL_VELOCITY, Vel.x, Vel.y, 0.0f);
}

/**
 * Sets the direction the sound source is facing.
 * @param   math::vector2_t&    Direction
 **/        
void CSound2D::SetDirection(const math::vector2_t& Dir)
{
    if(m_source != -1)
        alSource3f(s_sources[m_source], AL_DIRECTION, Dir.x, Dir.y, 0.0f);
}

int CSound2D::GetAudioState() const
{
    if(m_source == -1) return AL_INVALID_OPERATION;

    int state;
    alGetSourcei(s_sources[m_source], AL_SOURCE_STATE, &state);
    return state;
}

ALenum CSound2D::GetLastError() const
{
    return m_lasterror;
}

ALuint CSound2D::GetBuffer() const
{
    return m_buffer;
}

ALuint CSound2D::GetSourceIndex() const
{
    return m_source;
}

float asset::CSound2D::GetVolume() const
{
    return m_volume;
}

ALuint CSound2D::GetSource() const
{
    return (m_source != -1) ? s_sources[m_source] : m_source;
}

bool CSound2D::LoadFromFile_WAV(const char* p_filename)
{
    alGenBuffers(1, &m_buffer);
    m_lasterror = alGetError();
    if((m_lasterror = alGetError()) != AL_NO_ERROR)
    {
        m_error     = alGetString(m_lasterror);
        return false;
    }

    m_buffer = alutCreateBufferFromFile(p_filename);
    if(m_buffer == AL_NONE)
    {
        m_lasterror = alGetError();
        m_error     = alGetString(m_lasterror);
        alDeleteBuffers(1, &m_buffer);
        return false;
    }

    m_filename  = p_filename;
    return true;
}

const std::string& asset::CSound2D::GetError() const
{
    return m_error;
}
