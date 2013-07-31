//
//  GCPMediaStream.cpp
//  FireBreath
//
//  Created by Manjesh Malavalli on 6/26/12.
//  Copyright (c) 2012 XVDTH. All rights reserved.
//

#include "GCPMediaStream.h"
#include "DOM/Window.h"
#include "variant_list.h"
#include "talk/media/devices/devicemanager.h"

namespace GoCast
{
    JSLogger* JSLogger::Instance(bool bDelete)
    {
        static JSLogger* pLogger = NULL;
        
        if(true == bDelete)
        {
            delete pLogger;
            pLogger = NULL;
        }
        else if(NULL == pLogger)
        {
            pLogger = new JSLogger();
        }
        
        return pLogger;
    }
    
    void JSLogger::LogEntry(const std::string& entry)
    {
        boost::mutex::scoped_lock lock(m_mutex);
        m_logEntries.push_back(FB::variant(entry));

        if(NULL != m_logCb.get())
        {
            m_logCb->InvokeAsync("", FB::variant_list_of());
        }
    }
    
    void JSLogger::LogFunction(const FB::JSObjectPtr& func)
    {
        boost::mutex::scoped_lock lock(m_mutex);
        m_logCb = func;
    }
    
    void JSLogger::ClearLogFunction()
    {
        m_logCb.reset();
    }
    
    FB::VariantList JSLogger::LogEntries()
    {
        boost::mutex::scoped_lock lock(m_mutex);
        FB::VariantList entries(m_logEntries);
        
        m_logEntries.clear();
        return entries;
    }
    
    JSLogger::JSLogger()
    {
        m_logCb.reset();
    }
    
    JSLogger::~JSLogger()
    {
        m_logCb.reset();
    }
        
    FB::JSAPIPtr MediaStreamTrack::Create(const std::string& kind,
                                          const std::string label)
    {
        return boost::make_shared<MediaStreamTrack>(kind, label);
    }
    
    MediaStreamTrack::MediaStreamTrack(const std::string& kind,
                                       const std::string& label)
    : FB::JSAPIAuto("MediaStreamTrack")
    , m_kind(kind)
    , m_label(label)
    {
        registerProperty("kind", make_property(this, &MediaStreamTrack::get_kind));
        registerProperty("label", make_property(this, &MediaStreamTrack::get_label));
    }
    
    FB::JSAPIPtr LocalMediaStreamTrack::Create(const std::string& kind,
                                               const std::string label,
                                               const bool enabled)
    {
        return boost::make_shared<LocalMediaStreamTrack>(kind, label, enabled);
    }
    
    LocalMediaStreamTrack::LocalMediaStreamTrack(const std::string& kind,
                                                 const std::string& label,
                                                 const bool enabled)
    : MediaStreamTrack(kind, label)
    , m_enabled(enabled)
    {
        registerProperty("enabled", make_property(this, &LocalMediaStreamTrack::get_enabled,
                                                        &LocalMediaStreamTrack::set_enabled));
    }
    
    LocalVideoTrack::VideoDeviceList LocalVideoTrack::videoDevices;
    
    FB::JSAPIPtr LocalVideoTrack::Create(talk_base::scoped_refptr<webrtc::LocalVideoTrackInterface>& pTrack,
                                         GCPVideoProcessor* pProc)
    {
        return boost::make_shared<LocalVideoTrack>(pTrack, pProc);
    }
    
    FB::VariantMap LocalVideoTrack::GetVideoDevices()
    {
        FB::VariantMap devices;
        std::string key;
        std::string val;
        static bool initTried = false;
        static talk_base::scoped_ptr<cricket::DeviceManagerInterface> devmgr(cricket::DeviceManagerFactory::Create());
        
        if(false == initTried)
        {
            if(false == devmgr->Init())
            {
                FBLOG_ERROR_CUSTOM("LocalVideoTrack::GetVideoDevices", "Can't init device manager");
                return devices;
            }
            initTried = true;
        }

        std::vector<cricket::Device> devs;
        if (false == devmgr->GetVideoCaptureDevices(&devs))
        {
            FBLOG_ERROR_CUSTOM("LocalVideoTrack::GetVideoDevices", "Can't enumerate devices");
            return devices;
        }
        
        for (std::vector<cricket::Device>::iterator idev = devs.begin(); idev != devs.end(); ++idev)
        {
            key = (*idev).id;
            val = (*idev).name;
            devices[key] = val;
            
            if(videoDevices.end() == videoDevices.find(key))
            {
                videoDevices[key] = devmgr->CreateVideoCapturer(*idev);

                std::string msg("Capture device [id = ");
                msg += key;
                msg += ", name = ";
                msg += (val + "]");
                
                if(NULL == videoDevices[key])
                {
                    devices.erase(key);
                    videoDevices.erase(key);
                    msg += " (failed to open)";
                    FBLOG_ERROR_CUSTOM("LocalVideoTrack::GetVideoDevices", msg);                    
                }
                else
                {
                    msg += "...";
                    FBLOG_INFO_CUSTOM("LocalVideoTrack::GetVideoDevices", msg);
                }
            }

            if((devs.begin() == idev) && (videoDevices.end() != videoDevices.find(key)))
            {
                devices["default"] = key;
            }
        }
        
        std::stringstream offlineDevices;
        for(VideoDeviceList::iterator it = videoDevices.begin();
            it != videoDevices.end(); it++)
        {
            if(devices.end() == devices.find(it->first))
            {
                offlineDevices << (it->first);
            }
        }
        
        while(offlineDevices)
        {
            std::string deviceId;
            offlineDevices >> deviceId;
            
            if("" != deviceId)
            {
                videoDevices.erase(deviceId);
                std::string msg = "Deleting offline device [";
                msg += (deviceId + "]...");
                FBLOG_INFO_CUSTOM("LocalVideTrack::GetVideoDevices", msg);
            }
        }
        
        return devices;
    }
    
    cricket::VideoCapturer* LocalVideoTrack::GetCaptureDevice(const std::string& uniqueId)
    {
        if(videoDevices.end() != videoDevices.find(uniqueId))
        {
            return videoDevices[uniqueId];
        }
        
        return NULL;
    }
    
    LocalVideoTrack::LocalVideoTrack(const talk_base::scoped_refptr<webrtc::LocalVideoTrackInterface>& pTrack,
                                     GCPVideoProcessor* pProc)
    : LocalMediaStreamTrack(pTrack->kind(), pTrack->id(), pTrack->enabled())
    , m_pProc(pProc)
    {
        registerProperty("effect", make_property(this, &LocalVideoTrack::get_effect,
                                                 &LocalVideoTrack::set_effect));
    }
        
    FB::JSAPIPtr LocalAudioTrack::Create(talk_base::scoped_refptr<webrtc::LocalAudioTrackInterface>& pTrack,
                                         GCPVoiceProcessor* pProc)
    {
        return boost::make_shared<LocalAudioTrack>(pTrack, pProc);
    }
    
    void LocalAudioTrack::GetAudioDevices(FB::VariantList& devices, bool bInput)
    {
        std::vector<cricket::Device> devicelist;
        static bool initTried = false;
        static talk_base::scoped_ptr<cricket::DeviceManagerInterface> devmgr(cricket::DeviceManagerFactory::Create());

        if(false == initTried)
        {
            if(false == devmgr->Init())
            {
                FBLOG_ERROR_CUSTOM("LocalAudioTrack::GetAudioDevices", "Can't init device manager");
                return;
            }
            initTried = true;
        }

        if(true == bInput)
        {
            devmgr->GetAudioInputDevices(&devicelist);
        }
        else
        {
            devmgr->GetAudioOutputDevices(&devicelist);
        }
        
        for(size_t i=0; i<devicelist.size(); i++)
        {
            devices.push_back(FB::variant(devicelist[i].name));            
        }
    }
    
    LocalAudioTrack::LocalAudioTrack(const talk_base::scoped_refptr<webrtc::LocalAudioTrackInterface>& pTrack,
                                     GCPVoiceProcessor* pProc)
    : LocalMediaStreamTrack(pTrack->kind(), pTrack->id(), pTrack->enabled())
    , m_pProc(pProc)
    {
        registerProperty("onvoicesignal", make_property(this, &LocalAudioTrack::get_onvoicesigCb,
                                                        &LocalAudioTrack::set_onvoicesigCb));
    }
    
    FB::JSAPIPtr RemoteVideoTrack::Create(talk_base::scoped_refptr<webrtc::VideoTrackInterface>& pTrack)
    {
        return boost::make_shared<RemoteVideoTrack>(pTrack);
    }
    
    RemoteVideoTrack::RemoteVideoTrack(const talk_base::scoped_refptr<webrtc::VideoTrackInterface>& pTrack)
    : MediaStreamTrack(pTrack->kind(), pTrack->id())
    {
        
    }
    
    FB::JSAPIPtr RemoteAudioTrack::Create(talk_base::scoped_refptr<webrtc::AudioTrackInterface>& pTrack)
    {
        return boost::make_shared<RemoteAudioTrack>(pTrack);
    }
    
    RemoteAudioTrack::RemoteAudioTrack(const talk_base::scoped_refptr<webrtc::AudioTrackInterface>& pTrack)
    : MediaStreamTrack(pTrack->kind(), pTrack->id())
    {
        
    }

    FB::JSAPIPtr LocalMediaStream::Create(talk_base::scoped_refptr<webrtc::LocalMediaStreamInterface>& pStream,
                                          GCPVoiceProcessor* pVoiceProc,
                                          GCPVideoProcessor* pVideoProc)
    {
        return boost::make_shared<LocalMediaStream>(pStream, pVoiceProc, pVideoProc);
    }
    
    LocalMediaStream::LocalMediaStream(const talk_base::scoped_refptr<webrtc::LocalMediaStreamInterface>& pStream,
                                       GCPVoiceProcessor* pVoiceProc,
                                       GCPVideoProcessor* pVideoProc)
    : FB::JSAPIAuto("MediaStream")
    , m_label(pStream->label())
    , m_videoTracks(FB::variant_list_of())
    , m_audioTracks(FB::variant_list_of())
    {
        registerProperty("label", make_property(this, &LocalMediaStream::get_label));
        registerProperty("videoTracks", make_property(this,&LocalMediaStream::get_videoTracks));
        registerProperty("audioTracks", make_property(this, &LocalMediaStream::get_audioTracks));
        
        for(size_t i=0; i<pStream->video_tracks()->count(); i++)
        {
            talk_base::scoped_refptr<webrtc::VideoTrackInterface> pTrack(pStream->video_tracks()->at(i));
            talk_base::scoped_refptr<webrtc::LocalVideoTrackInterface> pTrack_(
                static_cast<webrtc::LocalVideoTrackInterface*>(pTrack.get())
            );
            
            AddTrack(LocalVideoTrack::Create(pTrack_, pVideoProc));
        }
        
        for(size_t i=0; i<pStream->audio_tracks()->count(); i++)
        {
            talk_base::scoped_refptr<webrtc::AudioTrackInterface> pTrack(pStream->audio_tracks()->at(i));
            talk_base::scoped_refptr<webrtc::LocalAudioTrackInterface> pTrack_(
                static_cast<webrtc::LocalAudioTrackInterface*>(pTrack.get())
            );
            
            AddTrack(LocalAudioTrack::Create(pTrack_, pVoiceProc));
        }
    }
    
    void LocalMediaStream::AddTrack(FB::JSAPIPtr pTrack)
    {
        if("video" == pTrack->GetProperty("kind").convert_cast<std::string>())
        {
            m_videoTracks.push_back(FB::variant(pTrack));
        }
        else if("audio" == pTrack->GetProperty("kind").convert_cast<std::string>())
        {
            m_audioTracks.push_back(FB::variant(pTrack));
        }
    }

    FB::JSAPIPtr RemoteMediaStream::Create(talk_base::scoped_refptr<webrtc::MediaStreamInterface>& pStream)
    {
        return boost::make_shared<RemoteMediaStream>(pStream);
    }
    
    RemoteMediaStream::RemoteMediaStream(const talk_base::scoped_refptr<webrtc::MediaStreamInterface>& pStream)
    : FB::JSAPIAuto("RemoteMediaStream")
    , m_label(pStream->label())
    , m_videoTracks(FB::variant_list_of())
    , m_audioTracks(FB::variant_list_of())
    {
        registerProperty("label", make_property(this, &RemoteMediaStream::get_label));
        registerProperty("videoTracks", make_property(this,&RemoteMediaStream::get_videoTracks));
        registerProperty("audioTracks", make_property(this, &RemoteMediaStream::get_audioTracks));
        
        for(size_t i=0; i<pStream->video_tracks()->count(); i++)
        {
            talk_base::scoped_refptr<webrtc::VideoTrackInterface> pTrack(pStream->video_tracks()->at(i));
            AddTrack(RemoteVideoTrack::Create(pTrack));
        }
        
        for(size_t i=0; i<pStream->audio_tracks()->count(); i++)
        {
            talk_base::scoped_refptr<webrtc::AudioTrackInterface> pTrack(pStream->audio_tracks()->at(i));
            AddTrack(RemoteAudioTrack::Create(pTrack));
        }
    }
    
    void RemoteMediaStream::AddTrack(FB::JSAPIPtr pTrack)
    {
        if("video" == pTrack->GetProperty("kind").convert_cast<std::string>())
        {
            m_videoTracks.push_back(FB::variant(pTrack));
        }
        else if("audio" == pTrack->GetProperty("kind").convert_cast<std::string>())
        {
            m_audioTracks.push_back(FB::variant(pTrack));
        }
    }
}
