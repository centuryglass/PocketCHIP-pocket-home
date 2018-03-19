#include "JuceHeader.h"
#include "Audio.h"
#if JUCE_LINUX
#include <alsa/asoundlib.h>

#define DEFAULT_BUFFER_SIZE 4096 /*in samples*/
snd_pcm_t *g_alsa_playback_handle = 0;
#endif

/**
 * Initializes system audio.
 */
bool Audio::initAudio()
{
    // FIXME: this is a hack to fix touch screen presses causing buzzing
    // when no application holds alsa open
#    if JUCE_LINUX
    int err;
    int freq = 44100, channels = 2;
    snd_pcm_hw_params_t *hw_params;
    snd_pcm_sw_params_t *sw_params;

    snd_pcm_hw_params_malloc(&hw_params);
    snd_pcm_sw_params_malloc(&sw_params);
    err = snd_pcm_open(&g_alsa_playback_handle, "default",
            SND_PCM_STREAM_PLAYBACK, 0);
    if (err < 0)
    {
        DBG("Audio::" << __func__
                << ": ALSA ERROR: Can't open audio device: "
                << snd_strerror(err));
        return false;
    }
    DBG("Audio::" << __func__ << ": Opened Audio Device");
    err = snd_pcm_hw_params_any(g_alsa_playback_handle, hw_params);
    if (err < 0)
    {
        DBG("Audio::" << __func__ << ": ALSA ERROR: "
                << "Can't initialize hardware parameter structure: "
                << snd_strerror(err));
        return false;
    }
    err = snd_pcm_hw_params_set_access(g_alsa_playback_handle, hw_params,
            SND_PCM_ACCESS_RW_INTERLEAVED);
    if (err < 0)
    {
        DBG("Audio::" << __func__
                << ": ALSA ERROR: Can't set access type: "
                << snd_strerror(err));
        return false;
    }
    err = snd_pcm_hw_params_set_format(g_alsa_playback_handle, hw_params,
            SND_PCM_FORMAT_S16_LE);
    if (err < 0)
    {
        DBG("Audio::" << __func__
                << ": ALSA ERROR: Can't set sample format :"
                << snd_strerror(err));
        return false;
    }
    err = snd_pcm_hw_params_set_rate_near(g_alsa_playback_handle, hw_params,
            (unsigned int*) &freq, 0);
    if (err < 0)
    {
        DBG("Audio::" << __func__
                << ": ALSA ERROR: Can't set sample rate: "
                << snd_strerror(err));
        return false;
    }
    DBG("Audio::" << __func__ << ": ALSA Sample rate: "
            << freq);
    err = snd_pcm_hw_params_set_channels(g_alsa_playback_handle, hw_params,
            channels);
    if (err < 0)
    {
        DBG("Audio::" << __func__
                << ": ALSA ERROR: Can't set channel count: "
                << snd_strerror(err));
        return false;
    }
    snd_pcm_uframes_t frames;
    frames = DEFAULT_BUFFER_SIZE;
    err = snd_pcm_hw_params_set_buffer_size_near(g_alsa_playback_handle,
            hw_params, &frames);
    if (err < 0)
    {
        DBG("Audio::" << __func__
                << ": ALSA ERROR: Can't set buffer size: "
                << snd_strerror(err));
        return false;
    }
    snd_pcm_hw_params_get_buffer_size(hw_params, &frames);
    DBG("Audio::" << __func__
            << ": ALSA Buffer size: 4096 samples");
    err = snd_pcm_hw_params(g_alsa_playback_handle, hw_params);
    if (err < 0)
    {
        DBG("Audio::" << __func__
                << ": ALSA ERROR: Can't set parameters: " << snd_strerror(err));
        return false;
    }
    snd_pcm_hw_params_free(hw_params);
    snd_pcm_sw_params_free(sw_params);

    err = snd_pcm_prepare(g_alsa_playback_handle);
    if (err < 0)
    {
        DBG("Audio::" << __func__
                << ": ALSA ERROR: Can't prepare audio interface for use: "
                << snd_strerror(err));
        return false;
    }

    /* Stop PCM device and drop pending frames */
    snd_pcm_drain(g_alsa_playback_handle);
#    endif
    return true;
}

/**
 * @return system audio volume level, as a percentage of the maximum
 *  volume.
 */
int Audio::getVolumePercent()
{ 
    int volume = 0;
#if JUCE_LINUX
    // Get initial brightness value
    ChildProcess child;
    // Get initial volume value
    StringArray cmd{ "amixer", "sget", "Power Amplifier"};
    if (child.start(cmd))
    {
        String result(child.readAllProcessOutput());
        result = result.fromFirstOccurrenceOf("[",false,false);
        result = result.initialSectionContainingOnly("0123456789");
        volume = result.getIntValue();
    }
#endif
    return volume;
 }

/**
 * Change the system audio volume.
 * @param volumePercent volume level, from 0 to 100
 */
void Audio::setVolume(int volumePercent) {
#if JUCE_LINUX
    StringArray cmd{"amixer", "sset", "Power Amplifier", 
            (String(volumePercent) + "%").toRawUTF8()};
    ChildProcess child;
    if (child.start(cmd))
    {
            String result(child.readAllProcessOutput());
    }
#endif
}