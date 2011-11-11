/**********************************************************\

  Auto-generated WebrtcPluginAPI.h

\**********************************************************/

#include <string>
#include <sstream>
#include <boost/weak_ptr.hpp>
#include "JSAPIAuto.h"
#include "BrowserHost.h"
#include "WebrtcPlugin.h"
#include "WPLThreadSafeMessageQueue.h"
#include "ThreadSingle.h"

#ifndef H_WebrtcPluginAPI
#define H_WebrtcPluginAPI

class PluginMainThread : public ThreadSingle
{
public:
    PluginMainThread(projectname::ThreadSafeMessageQueue* pMsgQ):
    m_pMsgQ(pMsgQ) {}
    
    virtual ~PluginMainThread() {}
    
protected:
    int workerBee(void);
    
protected:
    projectname::ThreadSafeMessageQueue* m_pMsgQ;
};

class WebrtcPluginAPI : public FB::JSAPIAuto
{
public:
    WebrtcPluginAPI(const WebrtcPluginPtr& plugin, const FB::BrowserHostPtr& host);
    virtual ~WebrtcPluginAPI();

    WebrtcPluginPtr getPlugin();

    // Read/Write property ${PROPERTY.ident}
    std::string get_testString();
    void set_testString(const std::string& val);

    // Read-only property ${PROPERTY.ident}
    std::string get_version();

    // Method echo
    FB::variant echo(const FB::variant& msg);
    void Signin(const std::string& peerName,
                const std::string& serverIP,
                const int serverPort);
    void Signout(void);
    
    // Event helpers
    FB_JSAPI_EVENT(fired, 3, (const FB::variant&, bool, int));
    FB_JSAPI_EVENT(echo, 2, (const FB::variant&, const int));
    FB_JSAPI_EVENT(notify, 0, ());

    // Method test-event
    void testEvent(const FB::variant& s);

private:
    WebrtcPluginWeakPtr m_plugin;
    FB::BrowserHostPtr m_host;

    std::string m_testString;
    projectname::ThreadSafeMessageQueue* m_pMsgQ;
};

#endif // H_WebrtcPluginAPI

