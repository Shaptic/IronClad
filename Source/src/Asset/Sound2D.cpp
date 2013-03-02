#include "Asset/Sound2D.hpp"

using namespace ic;
using asset::CSound2D;
using util::g_Log;

std::vector<CSound2D*> CSound2D::mp_allSounds;

CSound2D::CSound2D(bool orig) : CAsset(orig), m_buffer(0), m_source(-1),
    m_volume(1.f), m_lasterror(AL_NO_ERROR)
{
    CSound2D::mp_allSounds.push_back(this);
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

bool CSound2D::InitializeOpenAL()
{
    static bool once = false;

    if(!once)
    {
        memset(s_sources, 0, sizeof s_sources);
        alutInit(NULL, NULL);
        if(alutGetError() != ALUT_ERROR_NO_ERROR)
            return false;
        alGetError();
        once = true;
    }

    return true;
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
    for(size_t i = 0; i < sizeof s_sources; ++i)
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

/**
 * Loads audio from an existing CSound2D class.
 *  This method is pretty untested, I wouldn't really recommend
 *  using it. It takes the buffer and source ID's from the
 *  provided, existing CSound2D parameter.
 *
 * @param   CSound2D*   The audio to copy.
 * @return  Always TRUE.
 **/
bool CSound2D::LoadFromAudio(CSound2D* const p_Copy)
{
    m_buffer    = p_Copy->GetBuffer();
    m_filename  = p_Copy->GetFilename();
    m_source    = -1;

    return true;
}

/**
 * Loads an audio file into memory.
 *  The lib-vorbis SDK is used to determine whether or not the
 *  given filename refers to a valid .ogg file or not.
 *  If it is determined NOT to be an .ogg file, it is assumed
 *  to be a .wav file and loaded as such, returning errors on
 *  any failures. 
 *
 * @param   char* Filename to load
 * @return  TRUE if successfully loaded, FALSE otherwise.
 *          The actual error code can be checked by calling GetLastError().
 *
 * @todo    Add the ability to stream large .ogg files.
 **/
bool CSound2D::LoadFromFile(const char* p_filename)
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
    if(p_filename == NULL)
    {
        m_lasterror = AL_INVALID_NAME;
        return false;
    }

    // Determine if the given file is .ogg or not.
    // Open the .ogg file in binary-read mode.
    p_File = fopen(p_filename, "rb");
    if(p_File == NULL)
    {
        m_lasterror = AL_INVALID_NAME;
        return false;
    }

    // Initialize OggVorbis_File structure and check for valid ogg
    if(ov_open_callbacks(p_File, &ogg_file, NULL, 0, OV_CALLBACKS_DEFAULT) < 0)
    {
        ov_clear(&ogg_file);
        fclose(p_File);

        // The file isn't .ogg, so try and load it as a .wav.
        return this->LoadFromFile_WAV(p_filename);
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
        return false;

    alBufferData(m_buffer, format, &buffer[0], buffer.size(), freq);
    if((m_lasterror = alGetError()) != AL_NO_ERROR)
        return false;

    m_filename  = p_filename;
    return true;
}

/**
 * Plays a loaded audio buffer.
 *  Due to the somewhat complicated source-management system in
 *  place, buffers are bound to an OpenAL source on demand.
 *  So whenever this method is called, an available source is
 *  found, and then the buffer is bound to it, and the audio
 *  is played. The CAssetManager is responsible for unloading the
 *  source after playback has completed in full.
 *  
 * @return  TRUE if the sound played, FALSE if not.
 **/
bool CSound2D::Play()
{
    if(this->GetAudioState() == AL_PLAYING)
    {
        alSourcePlay(s_sources[m_source]);

        g_Log.Flush();
        g_Log << "[DEBUG] Forcing playing of " << m_filename << ".\n";
        g_Log.PrintLastLog();
        return true;
    }

    CSound2D::GetAvailableSource();
    if(s_available == -1)
    {
        m_lasterror = AL_OUT_OF_MEMORY;
        return false;
    }

    ALuint source = 0;
    alGenSources(1, &s_sources[s_available]);
    if((m_lasterror = alGetError()) != AL_NO_ERROR)
        return false;
    m_source = s_available;

    alSourcei(s_sources[m_source], AL_BUFFER, m_buffer);
    alSourcef(s_sources[m_source], AL_GAIN, m_volume);
    if((m_lasterror = alGetError()) != AL_NO_ERROR)
        return false;

    alSourcePlay(s_sources[m_source]);

    g_Log.Flush();
    g_Log << "[DEBUG] Playing " << m_filename << ".\n";
    g_Log.PrintLastLog();

    return true;
}

/**
 * Pauses current audio buffer.
 *  The buffer maintains linked to its source despite being paused,
 *  and will only be deleted if the source status is CSTOPPED.
 *  
 * @return  TRUE if paused, FALSE if nothing loaded.
 **/
bool CSound2D::Pause()
{
    if(m_source == -1)
        return false;

    if(this->GetAudioState() == AL_PAUSED)
        return true;

#ifdef _DEBUG
    std::cout << "Paused " << m_filename << ".\n";
#endif // _DEBUG
    alSourcePause(s_sources[m_source]);

    return true;
}

/**
 * Stops the audio from playing.
 *  Audio looping is always enabled, with no method (currently)
 *  to disable it. This is due to the fact that I've never /not/
 *  needed to disable looping.
 *
 * @return  TRUE if stopped, FALSE if nothing is loaded.
 * 
 * @see     asset::C2DSound::Play()
 * @todo    Add looping capabilities (or lack there-of).
 **/
bool CSound2D::Stop()
{
    if(m_source == -1)
        return false;

    if(this->GetAudioState() == AL_STOPPED)
        return true;

#ifdef _DEBUG
    std::cout << "Stopped " << m_filename << ".\n";
#endif // _DEBUG
    alSourceStop(s_sources[m_source]);
    return true;
}

/**
 * Unloads the source being used for playback.
 *  Since OpenAL is limited to 256 audio sources, it is important
 *  to be conservative with the available resources.
 *  Though I doubt all 256 will ever be used simultaneously, a
 *  system is in place to minimize source-hogging. This method
 *  should only be called by an CAssetManager after checking
 *  that playback has been completed. 
 *
 * @return  TRUE if the source was unloaded successfully,
 *          FALSE if not, or no source loaded.
 **/
bool CSound2D::UnloadSource()
{
    if(m_source == -1)
        return false;

    alDeleteSources(1, &s_sources[m_source]);
    s_sources[m_source] = 0;
    m_source = -1;

    return ((m_lasterror = alGetError()) == AL_NO_ERROR);
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
    if(m_source == -1)
        return AL_INVALID_OPERATION;

    int state;
    alGetSourcei(s_sources[m_source], AL_SOURCE_STATE, &state);
    alGetError();
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

ALuint CSound2D::GetSource() const
{
    if(m_source != -1)
        return s_sources[m_source];
    else
        return -1;
}

/**
 * Loads a .wav file.
 *  This is only called by LoadFromFile() after determining that
 *  the given data is not a valid Ogg-Vorbis audio file.
 *
 * @param   char*   Filename
 * @return  TRUE on successful load, FALSE otherwise.
 **/        
bool CSound2D::LoadFromFile_WAV(const char* p_filename)
{
    alGenBuffers(1, &m_buffer);
    if((m_lasterror = alGetError()) != AL_NO_ERROR)
        return false;

    m_buffer = alutCreateBufferFromFile(p_filename);
    if(m_buffer == AL_NONE)
    {
        m_lasterror = alutGetError();
        alDeleteBuffers(1, &m_buffer);
        return false;
    }

    m_filename  = p_filename;
    return true;
}