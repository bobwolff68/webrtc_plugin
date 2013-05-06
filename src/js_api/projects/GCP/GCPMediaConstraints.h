#ifndef FireBreath_GCPMediaConstraints_h
#define FireBreath_GCPMediaConstraints_h

#include "JSAPIAuto.h"
#include "talk/app/webrtc/peerconnection.h"

namespace GoCast
{
    class MediaConstraints : public webrtc::MediaConstraintsInterface
    {
    public:
        MediaConstraints(const FB::JSObjectPtr& constraints);
        virtual ~MediaConstraints();
        virtual const webrtc::MediaConstraintsInterface::Constraints& GetMandatory() const;
        virtual const webrtc::MediaConstraintsInterface::Constraints& GetOptional() const;
        
    private:
        webrtc::MediaConstraintsInterface::Constraints m_mandatory;
        webrtc::MediaConstraintsInterface::Constraints m_optional;
    };
}

#endif