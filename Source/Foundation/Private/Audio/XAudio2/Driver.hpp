// -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-
// Copyright (C) 2021-2023 by Agustin Alvarez. All rights reserved.
//
// This work is licensed under the terms of the MIT license.
//
// For a copy, see <https://opensource.org/licenses/MIT>.
// -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-

#pragma once

// -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-
// [  HEADER  ]
// -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-

#include "Audio/Driver.hpp"
#include <xaudio2.h>
#include <xaudio2fx.h>
#include <x3daudio.h>

// -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-
// [   CODE   ]
// -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-

namespace Audio
{
    // -=(Undocumented)=-
    class XAudio2Driver final : public Driver, private IXAudio2VoiceCallback
    {
    public:

        // -=(Undocumented)=-
        XAudio2Driver();

        // -=(Undocumented)=-
        ~XAudio2Driver() override;

        // \see Driver::Initialise
        Bool Initialise(UInt32 Submixes) override;

        // \see Driver::Advance
        void Advance() override;

        // \see Driver::Suspend
        void Suspend() override;

        // \see Driver::Restore
        void Restore() override;

        // \see Driver::SetListener
        void SetListener(Ref<const Vector3f> Position, Ref<const Vector4f> Orientation) override;

        // \see Driver::SetMasterVolume
        void SetMasterVolume(Real32 Volume) override;

        // \see Driver::SetMasterVolume
        void SetSubmixVolume(UInt Submix, Real32 Volume) override;

        // \see Driver::GetMasterVolume
        Real32 GetMasterVolume() override;

        // \see Driver::GetSubmixVolume
        Real32 GetSubmixVolume(UInt Submix) override;

        // \see Driver::Play
        UInt Play(UInt Submix, Ref<const SPtr<Sound>> Sound, Ref<const SPtr<Emitter>> Emitter, Bool Repeat) override;

        // \see Driver::SetGain
        void SetGain(UInt Instance, Real32 Gain) override;

        // \see Driver::SetPitch
        void SetPitch(UInt Instance, Real32 Pitch) override;

        // \see Driver::Start
        void Start(UInt Instance) override;

        // \see Driver::Stop
        void Stop(UInt Instance, Bool Immediately) override;

        // \see Driver::Halt
        void Halt(UInt Submix) override;

    private:

        // -=(Undocumented)=-
        struct XAudioSource
        {
            UInt                     Key;
            Ptr<IXAudio2SourceVoice> Source;
        };

        // -=(Undocumented)=-
        struct XAudioInstance
        {
            Bool                     Finished;
            Bool                     Repeat;
            Real32                   Frequency;
            SPtr<Emitter>            Emitter;
            SPtr<Sound>              Sound;
            Ptr<IXAudio2SubmixVoice> Submix;
            Ptr<IXAudio2SourceVoice> Source;
        };

        // -=(Undocumented)=-
        using XAudioPool     = Stack<XAudioSource, k_MaxMixes>;

        // -=(Undocumented)=-
        using XAudioMixes    = Stack<XAudioInstance, k_MaxMixes>;

        // -=(Undocumented)=-
        using XAudioSubmixes = Stack<Ptr<IXAudio2SubmixVoice>, k_MaxSubmixes>;

    private:

        // \see IXAudio2VoiceCallback::OnVoiceProcessingPassStart
        STDMETHOD_(void, OnVoiceProcessingPassStart) (UINT32 BytesRequired) override
        {
        }

        // \see IXAudio2VoiceCallback::OnVoiceProcessingPassEnd
        STDMETHOD_(void, OnVoiceProcessingPassEnd) () override
        {
        }

        // \see IXAudio2VoiceCallback::OnStreamEnd
        STDMETHOD_(void, OnStreamEnd) () override
        {
        }

        // \see IXAudio2VoiceCallback::OnBufferStart
        STDMETHOD_(void, OnBufferStart) (Ptr<void> pBufferContext) override
        {
        }

        // \see IXAudio2VoiceCallback::OnBufferEnd
        STDMETHOD_(void, OnBufferEnd) (Ptr<void> pBufferContext) override
        {
            Write(GetInstance(reinterpret_cast<UInt>(pBufferContext)));
        }

        // \see IXAudio2VoiceCallback::OnLoopEnd
        STDMETHOD_(void, OnLoopEnd) (Ptr<void> pBufferContext) override
        {
        }

        // \see IXAudio2VoiceCallback::OnVoiceError
        STDMETHOD_(void, OnVoiceError) (Ptr<void> pBufferContext, HRESULT Error) override
        {
        }

    private:

        // -=(Undocumented)=-
        Ptr<XAudioInstance> GetInstance(UInt ID)
        {
            const auto FindByID = [ID](Ref<const XAudioInstance> InstanceRef)
            {
                return reinterpret_cast<UInt>(InstanceRef.Source) == ID;
            };
            return eastl::find_if(mMixes.begin(), mMixes.end(), FindByID);
        }

        // -=(Undocumented)=-
        Ptr<IXAudio2SourceVoice> GetOrCreateMix(Ref<const SPtr<Sound>> Sound);

        // -=(Undocumented)=-
        void Write(Ptr<XAudioInstance> Instance);

        // -=(Undocumented)=-
        void Apply3D(Ref<XAudioInstance> Instance);

    private:

        // -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-
        // -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-

        Ptr<IXAudio2>               mDevice;
        Ptr<IXAudio2MasteringVoice> mMaster;
        XAudioPool                  mVoices;
        XAudioSubmixes              mSubmixes;
        XAudioMixes                 mMixes;

        // -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-
        // -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-

        X3DAUDIO_HANDLE             m3DAudio;
        X3DAUDIO_LISTENER           m3DAudioListener;
    };
}