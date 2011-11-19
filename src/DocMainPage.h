/*
 *  Copyright (c) 2011 GoCast. All Rights Reserved.
 *
 *  Use of this source code is governed by a BSD-style license
 *  that can be found in the LICENSE file in the root of the source
 *  tree. All contributing project authors may be found in the 
 *  AUTHORS file in the root of the source tree.
 */

/**
    \mainpage Project Overview.
    Please see the project website at http://www.gocast.it for details, 
    <a href="http://www.gocast.it/docs.html">docs</a>, and the 
    <a href="http://www.gocast.it/forum">discussion forums</a>.<br>

    The main purpose of this project is to make available to the web developer, a plugin-based set of
    JavaScript APIs that facilitates embedding of peer-to-peer communication functionality in websites.
    The plugin APIs utilize Google's implementation of the W3C's WebRTC specification in order to setup,
    manage and teardown video/voice data paths between peers. The project, also as part of its initial
    release, takes care of presence and signaling in the native code space, so that web developers
    can concentrate on simple tasks like call, hangup, signin and signout. Currently, only voice
    connections are supported, with an objective to add video communication facility in the near future. 

    \section setup_p2p_conn Setting up a peer-to-peer connection.
    There are two aspects to setting up a successful connection between peers. One is having a mechanism to 
    exchange signaling messages between peers. This is achieved here through the peers signing in to a signin 
    server, and forwarding any signaling messages to it, as it knows the location of the intended recipients.
    All this is handled through TCP connections. The second aspect is the voice data connection between peers, 
    which is handled through RTP/RTCP(UDP) connections, with STUN/TURN/ICE for NAT traversal.
    GoCast::PeerConnectionClient handles the first aspect, while GoCast::PeerConnectionObserver, along with
    webrtc::PeerConnectionClient handles the second.

    \section Credits
    We would like to acknowledge the use of a handful of tools and of course the main two open source
    projects upon which we sit.<br>
    - webrtc - from webrtc.org which is a BSD licensed project housing many quality open source projects.<br>
    - libsrtp - Secure communications over RTP/RTCP which is added for Mac support until webrtc builds
      with it natively. Source is from srtp.sourceforge.net/srtp.html
    - FireBreath - from firebreath.org which is a BSD/LGPL project (we use as BSD) for producing all of the
      plugin abilities. This has made life so much easier in getting a quality, stable plugin prepped.<br>
    - anyoption - a great little C++ class which makes linux-style posix/gnu style command line parsing
      a total breeze. Thanks to Kishan Thomas at hackorama.com for placing this little ditty in the public
      domain.<br>
    - Tools such as: github, git, svn, cmake, chromium depot_tools, and of course doxygen.<br>

*/
