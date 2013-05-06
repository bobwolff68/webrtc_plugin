/**********************************************************\

  Auto-generated GCPAPI.cpp

\**********************************************************/

#include "JSObject.h"
#include "variant_list.h"
#include "DOM/Document.h"
#include "global/config.h"

#include "GCPAPI.h"
#include "GCPMediaStream.h"
#include "GCPWebrtcCenter.h"

GCPAPI::~GCPAPI()
{
    DeletePeerConnection();
    if("localPlayer" == m_htmlId.convert_cast<std::string>())
    {
#ifndef GOCAST_WINDOWS
        GoCast::RtcCenter::Instance(true);
#endif
        GoCast::JSLogger::Instance()->ClearLogFunction();
    }
}

///////////////////////////////////////////////////////////////////////////////
/// @fn GCPPtr GCPAPI::getPlugin()
///
/// @brief  Gets a reference to the plugin that was passed in when the object
///         was created.  If the plugin has already been released then this
///         will throw a FB::script_error that will be translated into a
///         javascript exception in the page.
///////////////////////////////////////////////////////////////////////////////
GCPPtr GCPAPI::getPlugin()
{
    GCPPtr plugin(m_plugin.lock());
    if (!plugin) {
        throw FB::script_error("The plugin is invalid");
    }
    return plugin;
}

std::string GCPAPI::get_version()
{
    return FBSTRING_PLUGIN_VERSION;
}

std::string GCPAPI::get_signalingState()
{
    return m_signalingState;
}

std::string GCPAPI::get_iceConnectionState()
{
    return m_iceConnectionState;
}

std::string GCPAPI::get_iceGatheringState()
{
    return m_iceGatheringState;
}

FB::JSAPIPtr GCPAPI::get_source()
{
    return m_srcStream;
}

FB::variant GCPAPI::get_volume()
{
    int level;
    GoCast::RtcCenter* pCtr = GoCast::RtcCenter::Instance();
    bool bSpkMuted = false;
    
    if(false == pCtr->Inited() || false == pCtr->GetSpkVol(&level))
    {
        level = -1;
    }
    
    if(false == pCtr->GetSpkMute(&bSpkMuted))
    {
        level = -1;
    }
    else if(true == bSpkMuted)
    {
        level = 0;
    }
    
    return level;
}

FB::variant GCPAPI::get_micvolume()
{
    int level;
    GoCast::RtcCenter* pCtr = GoCast::RtcCenter::Instance();
    
    if(false == pCtr->Inited() || false == pCtr->GetMicVol(&level))
    {
        level = -1;
    }
    
    return level;
}

FB::JSObjectPtr GCPAPI::get_onaddstream()
{
    return m_onaddstreamCb;
}

FB::JSObjectPtr GCPAPI::get_onremovestream()
{
    return m_onremovestreamCb;
}

FB::JSObjectPtr GCPAPI::get_onstatechange()
{
    return m_onstatechangeCb;
}

FB::JSObjectPtr GCPAPI::get_onicechange()
{
    return m_onicechangeCb;
}

FB::JSObjectPtr GCPAPI::get_ongatheringchange()
{
    return m_ongatheringchangeCb;
}

FB::JSObjectPtr GCPAPI::get_onnegotiationneeded()
{
    return m_onnegotiationneededCb;
}

FB::VariantMap GCPAPI::get_videoinopts()
{
    GoCast::RtcCenter* pCtr = GoCast::RtcCenter::Instance();
    
    m_videoDevices.clear();
    if(false == pCtr->Inited())
    {
        std::string msg = m_htmlId.convert_cast<std::string>();
        msg += ": Failed to init RtcCenter singleton...";
        FBLOG_ERROR_CUSTOM("GCPAPI::get_videoinopts", msg);
    }
    else
    {
        pCtr->QueryVideoDevices(m_videoDevices);
    }
    
    return m_videoDevices;
}

FB::VariantList GCPAPI::get_audioinopts()
{
    GoCast::RtcCenter* pCtr = GoCast::RtcCenter::Instance();
    
    m_audioInDevices.clear();
    if(false == pCtr->Inited())
    {
        std::string msg = m_htmlId.convert_cast<std::string>();
        msg += ": Failed to init RtcCenter singleton...";
        FBLOG_ERROR_CUSTOM("GCPAPI::get_audioinopts", msg);
    }
    else
    {
        pCtr->QueryAudioDevices(m_audioInDevices);
    }
    
    return m_audioInDevices;
}

FB::VariantList GCPAPI::get_audiooutopts()
{
    GoCast::RtcCenter* pCtr = GoCast::RtcCenter::Instance();
    
    m_audioOutDevices.clear();
    if(false == pCtr->Inited())
    {
        std::string msg = m_htmlId.convert_cast<std::string>();
        msg += ": Failed to init RtcCenter singleton...";
        FBLOG_ERROR_CUSTOM("GCPAPI::get_audiooutopts", msg);
    }
    else
    {
        pCtr->QueryAudioDevices(m_audioOutDevices, false);
    }
    
    return m_audioOutDevices;    
}

FB::VariantList GCPAPI::get_logentries()
{
    return GoCast::JSLogger::Instance()->LogEntries();
}

void GCPAPI::set_onaddstream(const FB::JSObjectPtr &onaddstream)
{
    m_onaddstreamCb = onaddstream;
}

void GCPAPI::set_onremovestream(const FB::JSObjectPtr &onremovestream)
{
    m_onremovestreamCb = onremovestream;
}

void GCPAPI::set_onstatechange(const FB::JSObjectPtr &onstatechange)
{
    m_onstatechangeCb = onstatechange;
}

void GCPAPI::set_onicechange(const FB::JSObjectPtr& onicechange)
{
    m_onicechangeCb = onicechange;
}

void GCPAPI::set_ongatheringchange(const FB::JSObjectPtr& ongatheringchange)
{
    m_ongatheringchangeCb = ongatheringchange;
}

void GCPAPI::set_onnegotiationneeded(const FB::JSObjectPtr& onnegotiationneeded)
{
    m_onnegotiationneededCb = onnegotiationneeded;
}

void GCPAPI::set_source(const FB::JSAPIPtr& stream)
{
    m_srcStream = stream;
    if(NULL != stream.get())
    {
        std::string msg = m_htmlId.convert_cast<std::string>();
        msg += ": Setting video track renderer...";
        FBLOG_INFO_CUSTOM("GCAPAPI::set_source", msg);

        if("localPlayer" == m_htmlId.convert_cast<std::string>())
        {            
			if(NULL != getPlugin()->Renderer())
			{
                getPlugin()->Renderer()->SetPreviewMode(true);
				(GoCast::RtcCenter::Instance())->SetLocalVideoTrackRenderer(getPlugin()->Renderer());
			}
        }
        else
        {
            if(NULL != getPlugin()->Renderer())
            {
                (GoCast::RtcCenter::Instance())->SetRemoteVideoTrackRenderer(m_htmlId.convert_cast<std::string>(),
                                                                             getPlugin()->Renderer());                
            }
		}
    }
}

void GCPAPI::set_volume(FB::variant volume)
{
    int level = volume.convert_cast<int>();
    GoCast::RtcCenter* pCtr = GoCast::RtcCenter::Instance();
    
    if(level > 255) level = 255;
    if(level < 0)   level = 0;
    if(true == pCtr->Inited())
    {
        pCtr->SetSpkVol(level); 
    }
}

void GCPAPI::set_micvolume(FB::variant volume)
{
    int level = volume.convert_cast<int>();
    GoCast::RtcCenter* pCtr = GoCast::RtcCenter::Instance();
    
    if(level > 255) level = 255;
    if(level < 0)   level = 0;
    if(true == pCtr->Inited())
    {
        pCtr->SetMicVol(level); 
    }
}

void GCPAPI::GetUserMedia(const FB::JSObjectPtr& mediaHints,
                          const FB::JSObjectPtr& succCb,
                          const FB::JSObjectPtr& failCb)
{
    GoCast::RtcCenter* pCtr = GoCast::RtcCenter::Instance();
	m_htmlId = "localPlayer";
    
    if(false == pCtr->Inited())
    {
        std::string msg = m_htmlId.convert_cast<std::string>();
        msg += ": Failed to init RtcCenter singleton...";
        FBLOG_ERROR_CUSTOM("GCPAPI::GetUserMedia", msg);
        
        if(NULL != failCb.get())
        {
            failCb->InvokeAsync("", FB::variant_list_of("RtcCenter init failed"));
        }
        return;
    }
    
    pCtr->GetUserMedia(mediaHints, succCb, failCb, false);
}

FB::variant GCPAPI::Init(const FB::variant& htmlId,
                         const FB::VariantList& iceServers,
                         const FB::JSObjectPtr& iceCallback)
{
    GoCast::RtcCenter* pCtr = GoCast::RtcCenter::Instance();
    m_htmlId = htmlId;
    m_iceCb = iceCallback;
    
    if(false == pCtr->Inited())
    {
        std::string msg = m_htmlId.convert_cast<std::string>();
        msg += ": Failed to init RtcCenter singleton";
        FBLOG_ERROR_CUSTOM("GCPAPI::Init",msg);
        return false;
    }
    
    if(false == pCtr->NewPeerConnection(m_htmlId.convert_cast<std::string>(), iceServers, this))
    {
        m_signalingState = "invalid";
        m_iceConnectionState = "invalid";
        m_iceGatheringState = "invalid";
        return false;
    }
    
    m_signalingState = pCtr->SignalingState(m_htmlId.convert_cast<std::string>());
    m_iceConnectionState = pCtr->IceConnectionState(m_htmlId.convert_cast<std::string>());
    m_iceGatheringState = pCtr->IceGatheringState(m_htmlId.convert_cast<std::string>());
    return true;
}

FB::variant GCPAPI::AddStream(const FB::JSAPIPtr& stream)
{
    GoCast::RtcCenter* pCtr = GoCast::RtcCenter::Instance();
    
    if(false == pCtr->Inited())
    {
        std::string msg = m_htmlId.convert_cast<std::string>();
        msg += ": Failed to init RtcCenter singleton";
        FBLOG_ERROR_CUSTOM("GCPAPI::AddStream", msg);
        return false;
    }

    return pCtr->AddStream(m_htmlId.convert_cast<std::string>(),
                           stream->GetProperty("label").convert_cast<std::string>());
}

FB::variant GCPAPI::RemoveStream(const FB::JSAPIPtr& stream)
{
    GoCast::RtcCenter* pCtr = GoCast::RtcCenter::Instance();
    
    if(false == pCtr->Inited())
    {
        std::string msg = m_htmlId.convert_cast<std::string>();
        msg += ": Failed to init RtcCenter singleton";
        FBLOG_ERROR_CUSTOM("GCPAPI::RemoveStream", msg);
        return false;
    }
    
    return pCtr->RemoveStream(m_htmlId.convert_cast<std::string>(),
                              stream->GetProperty("label").convert_cast<std::string>());
}

void GCPAPI::CreateOffer(const FB::JSObjectPtr& succCb,
                         const FB::JSObjectPtr& failCb,
                         const FB::JSObjectPtr& constraints)
{
    m_oncreatesdpsuccessCb = succCb;
    m_oncreatesdpfailureCb = failCb;
    GoCast::RtcCenter* pCtr = GoCast::RtcCenter::Instance();
    
    if(false == pCtr->Inited())
    {
        std::string msg = m_htmlId.convert_cast<std::string>();
        msg += ": Failed to init RtcCenter singleton";
        FBLOG_ERROR_CUSTOM("GCPAPI::CreateOffer", msg);
        return;
    }
    pCtr->CreateOffer(m_htmlId.convert_cast<std::string>(),
                      m_pCreateOfferSDPObserver,
                      constraints);
}

void GCPAPI::CreateAnswer(const FB::JSObjectPtr& succCb,
                          const FB::JSObjectPtr& failCb,
                          const FB::JSObjectPtr& constraints)
{
    m_oncreatesdpsuccessCb = succCb;
    m_oncreatesdpfailureCb = failCb;
    GoCast::RtcCenter* pCtr = GoCast::RtcCenter::Instance();
    
    if(false == pCtr->Inited())
    {
        std::string msg = m_htmlId.convert_cast<std::string>();
        msg += ": Failed to init RtcCenter singleton";
        FBLOG_ERROR_CUSTOM("GCPAPI::CreateAnswer", msg);
        return;
    }
    pCtr->CreateAnswer(m_htmlId.convert_cast<std::string>(),
                       m_pCreateAnswerSDPObserver,
                       constraints);
}

void GCPAPI::SetLocalDescription(const FB::variant& action,
                                 const FB::variant& sdp,
                                 const FB::JSObjectPtr& succCb,
                                 const FB::JSObjectPtr& failCb)
{
    GoCast::RtcCenter* pCtr = GoCast::RtcCenter::Instance();
    m_onsetsdpsuccessCb = succCb;
    m_onsetsdpfailureCb = failCb;
    
    if(false == pCtr->Inited())
    {
        std::string msg = m_htmlId.convert_cast<std::string>();
        msg += ": Failed to init RtcCenter singleton...";
        FBLOG_ERROR_CUSTOM("GCPAPI::SetLocalDescription", msg);
        
        if(NULL != failCb.get())
        {
            failCb->InvokeAsync("", FB::variant_list_of("RtcCenter init failed"));
        }
        return;
    }
    
    pCtr->SetLocalDescription(m_htmlId.convert_cast<std::string>(),
                              m_pSetLocalSDPObserver,
                              action.convert_cast<std::string>(),
                              sdp.convert_cast<std::string>());
}

void GCPAPI::SetRemoteDescription(const FB::variant& action,
                                  const FB::variant& sdp,
                                  const FB::JSObjectPtr& succCb,
                                  const FB::JSObjectPtr& failCb)
{
    GoCast::RtcCenter* pCtr = GoCast::RtcCenter::Instance();
    m_onsetsdpsuccessCb = succCb;
    m_onsetsdpfailureCb = failCb;
    
    if(false == pCtr->Inited())
    {
        std::string msg = m_htmlId.convert_cast<std::string>();
        msg += ": Failed to init RtcCenter singleton...";
        FBLOG_ERROR_CUSTOM("GCPAPI::SetRemoteDescription", msg);
        
        if(NULL != failCb.get())
        {
            failCb->InvokeAsync("", FB::variant_list_of("RtcCenter init failed"));
        }
        return;
    }
    
    pCtr->SetRemoteDescription(m_htmlId.convert_cast<std::string>(),
                               m_pSetRemoteSDPObserver,
                               action.convert_cast<std::string>(),
                               sdp.convert_cast<std::string>());
}

FB::variant GCPAPI::AddIceCandidate(const FB::variant& sdpMid,
                                    const FB::variant& sdpMlineIndex,
                                    const FB::variant& sdp)
{
    GoCast::RtcCenter* pCtr = GoCast::RtcCenter::Instance();
    
    if("localPlayer" == m_htmlId.convert_cast<std::string>())
    {
        FBLOG_ERROR_CUSTOM("GCPAPI::AddIceCandidate",
                           "localPlayer: Not allowed to call AddIceCandidate()");
        return false;
    }

    if(false == pCtr->Inited())
    {
        std::string msg = m_htmlId.convert_cast<std::string>();
        msg += ": Failed to init RtcCenter singleton";
        FBLOG_ERROR_CUSTOM("GCPAPI::AddIceCandidate", msg);
        return false;
    }
    
    return pCtr->AddIceCandidate(m_htmlId.convert_cast<std::string>(),
                                 sdpMid.convert_cast<std::string>(),
                                 sdpMlineIndex.convert_cast<int>(),
                                 sdp.convert_cast<std::string>());
}

FB::variant GCPAPI::DeletePeerConnection()
{
    GoCast::RtcCenter* pCtr = GoCast::RtcCenter::Instance();
    
    if(false == pCtr->Inited())
    {
        std::string msg = m_htmlId.convert_cast<std::string>();
        msg += ": Failed to init RtcCenter singleton";
        FBLOG_ERROR_CUSTOM("GCPAPI::DeletePeerConnection", msg);
        return false;
    }
    
    return pCtr->DeletePeerConnection(m_htmlId.convert_cast<std::string>());
}

void GCPAPI::LogFunction(const FB::JSObjectPtr& func)
{
    GoCast::JSLogger::Instance()->LogFunction(func);
}

void GCPAPI::OnSignalingChange(webrtc::PeerConnectionInterface::SignalingState newState)
{
    if(m_onstatechangeCb.get())
    {
        m_onstatechangeCb->InvokeAsync("", FB::variant_list_of(GoCast::GetSigStateString(newState)));
    }
}

void GCPAPI::OnIceConnectionChange(webrtc::PeerConnectionInterface::IceConnectionState newState)
{
    if(m_onicechangeCb.get())
    {
        m_onicechangeCb->InvokeAsync("", FB::variant_list_of(GoCast::GetIceConnStateString(newState)));
    }
}

void GCPAPI::OnIceGatheringChange(webrtc::PeerConnectionInterface::IceGatheringState newState)
{
    if(m_ongatheringchangeCb.get())
    {
        m_ongatheringchangeCb->InvokeAsync("", FB::variant_list_of(GoCast::GetIceGathStateString(newState)));
    }
}

void GCPAPI::OnRenegotiationNeeded()
{
    if(m_onnegotiationneededCb.get())
    {
        m_onnegotiationneededCb->InvokeAsync("", FB::variant_list_of());
    }
}

void GCPAPI::OnAddStream(webrtc::MediaStreamInterface* pRemoteStream)
{
    talk_base::scoped_refptr<webrtc::MediaStreamInterface> pStream(pRemoteStream);

    (GoCast::RtcCenter::Instance())->AddRemoteStream(m_htmlId.convert_cast<std::string>(),pStream);

    std::string msg = m_htmlId.convert_cast<std::string>();
    msg += ": Added remote stream [";
    msg += pStream->label();
    msg += "]...";
    FBLOG_INFO_CUSTOM("GCPAPI::OnAddStream", msg);

    if(NULL != m_onaddstreamCb.get())
    {
        m_onaddstreamCb->InvokeAsync("", FB::variant_list_of(GoCast::RemoteMediaStream::Create(pStream)));
    }
}

void GCPAPI::OnRemoveStream(webrtc::MediaStreamInterface* pRemoteStream)
{
    talk_base::scoped_refptr<webrtc::MediaStreamInterface> pStream(pRemoteStream);
    
    (GoCast::RtcCenter::Instance())->RemoveRemoteStream(m_htmlId.convert_cast<std::string>());

    std::string msg = m_htmlId.convert_cast<std::string>();
    msg += ": Removed remote stream [";
    msg += pStream->label();
    msg += "]...";
    FBLOG_INFO_CUSTOM("GCPAPI::OnRemoveStream", msg);
    
    if(NULL != m_onremovestreamCb.get())
    {
        m_onremovestreamCb->InvokeAsync("", FB::variant_list_of(GoCast::RemoteMediaStream::Create(pStream)));
    }
    
}

void GCPAPI::OnIceCandidate(const webrtc::IceCandidateInterface* pCandidate)
{
    std::string candidateSdp("");

    if(false == pCandidate->ToString(&candidateSdp))
    {
        FBLOG_ERROR_CUSTOM("GCPAPI::OnIceCandidate", "Unable to serialize SDP");
        return;
    }
    
    if(NULL != m_iceCb.get())
    {
        std::stringstream sstrm;
        sstrm << "{\"sdp_mid\":\"" << pCandidate->sdp_mid()
              << "\",\"sdp_mline_index\":"
              << pCandidate->sdp_mline_index()
              << ",\"sdp\":\"" << candidateSdp << "\"}";

        std::string msg = m_htmlId.convert_cast<std::string>();
        msg += ": New Ice Candidate: ";
        msg += sstrm.str();
        msg += "]...";
        FBLOG_INFO_CUSTOM("GCPAPI::OnIceCandidate", msg);        
        m_iceCb->InvokeAsync("", FB::variant_list_of(sstrm.str()));
    }
}

void GCPAPI::InvokeSetSDPSuccessCallback()
{
    if(NULL != m_onsetsdpsuccessCb.get())
    {
        m_onsetsdpsuccessCb->InvokeAsync("", FB::variant_list_of());
    }
}

void GCPAPI::InvokeSetSDPFailureCallback(const std::string& error)
{
    if(NULL != m_onsetsdpfailureCb.get())
    {
        m_onsetsdpfailureCb->InvokeAsync("", FB::variant_list_of(error));
    }
}

void GCPAPI::InvokeCreateSDPSuccessCallback(webrtc::SessionDescriptionInterface* desc)
{
    std::string sdp("");
    
    if(NULL != m_oncreatesdpsuccessCb.get())
    {
        if(false == desc->ToString(&sdp))
        {
            FBLOG_ERROR_CUSTOM("GCPAPI::InvokeCreateSDPSuccessCallback", "Failed to serialize SDP");
            return;
        }
        m_oncreatesdpsuccessCb->InvokeAsync("", FB::variant_list_of(sdp));
    }
}

void GCPAPI::InvokeCreateSDPFailureCallback(const std::string& error)
{
    if(NULL != m_oncreatesdpfailureCb.get())
    {
        m_oncreatesdpfailureCb->InvokeAsync("", FB::variant_list_of(error));
    }
}


SetSDPObserver::SetSDPObserver(GCPAPI* pJsapi)
: m_pJsapi(pJsapi)
{
    
}

SetSDPObserver::~SetSDPObserver()
{
    
}

void SetSDPObserver::OnSuccess()
{
    m_pJsapi->InvokeSetSDPSuccessCallback();
}

void SetSDPObserver::OnFailure(const std::string& error)
{
    m_pJsapi->InvokeSetSDPFailureCallback(error);
}

CreateSDPObserver::CreateSDPObserver(GCPAPI* pJsapi)
: m_pJsapi(pJsapi)
{
    
}

CreateSDPObserver::~CreateSDPObserver()
{
    
}

void CreateSDPObserver::OnSuccess(webrtc::SessionDescriptionInterface* desc)
{
    m_pJsapi->InvokeCreateSDPSuccessCallback(desc);
}

void CreateSDPObserver::OnFailure(const std::string& error)
{
    m_pJsapi->InvokeCreateSDPFailureCallback(error);
}
