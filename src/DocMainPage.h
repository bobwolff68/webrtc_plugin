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
*/
