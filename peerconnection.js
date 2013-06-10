/**
 * Global Namespace.
 * @namespace GoCastJS
 */
var GoCastJS = ('undefined' !== typeof(GoCastJS)) ? GoCastJS : {};
GoCastJS = (null !== GoCastJS) ? GoCastJS : {};

GoCastJS.Utils = {
    joinObjects: function(a, b) {
        a = a || {};
        b = b || {};

        return (JSON.parse((JSON.stringify(a) + JSON.stringify(b)).
                replace(/}{/g, ',').
                replace(/{,/g, '{').
                replace(/,}/g, '}')));
    }
};

/**
 * @memberof GoCastJS
 * @property {Array} devices        - List of available video devices.
 * @property {String} captureDevice - Current video device guid.
 */
GoCastJS.Video = {
    devices: [],
    captureDevice: ''
};

/**
 * @memberof GoCastJS
 * @property {Array} inputDevices  - List of available microphones.
 * @property {String} inputDevice  - Current microphone device.
 * @property {Array} outputDevices - List of available speakers.
 * @property {String} outputDevice - Current speaker device.
 * @property {Number} spkVol       - Current speaker volume.
 * @property {Number} micVol       - Current microphone volume.
 */
GoCastJS.Audio = {
    inputDevices: [],
    inputDevice: '',
    outputDevices: [],
    outputDevice: '',
    spkVol: 256,
    micVol: 256
};

/**
 * Exception thrown by a GoCastPlayer instance.
 * @memberof GoCastJS
 * @class
 * @property {String} pluginId - Unique id of exception-throwing plugin instance.
 * @property {String} message  - Description of the exception.
 * @param {String} pluginId    - Unique id of exception-throwing plugin instance.
 * @param {String} message     - Description of the exception.
 */
GoCastJS.Exception = function(pluginId, message) {
    this.pluginId = pluginId;
    this.message = message;
};

GoCastJS.Exception.prototype.toString = function() {
    return ('[' + this.pluginId + ']: ' + this.message);
};

GoCastJS.CheckBrowserSupport = function() {
    return (null === navigator.userAgent.toLowerCase().match('msie'));
};

GoCastJS.CheckGoCastPlayer = function() {
    for (i in navigator.plugins) {
        if (navigator.plugins.hasOwnProperty(i) && 'GoCastPlayer' === navigator.plugins[i].name) {
            return true;
        }
    }
    return false;
};

/**
 * User media access options.
 * @memberof GoCastJS
 * @class
 * @property {Object} constraints - {audio: bool, video: bool, [, videoconstraints: object] [, audioconstraints: object]}.
 * @property {DomElement} player  - GoCastPlayer instance or video element used for local video.
 * @property {String} apitype    - 'gcp', for GoCastPlayer API, or 'native' for Chrome 25+ WebRTC API.
 * @param {Object} constraints - Media constraints.
 * @param {DomElement} player  - GoCastPlayer instance or video element used for local video.
 * @param {String} apitype    - 'gcp', for GoCastPlayer API, or 'native' for Chrome 25+ WebRTC API.
 */
GoCastJS.UserMediaOptions = function(constraints, player, apitype) {
    var defaultwebrtcvideoconstraints = {
        mandatory: {
            minWidth: '160', maxWidth: '160',
            minHeight: '120', maxHeight: '120',
            minFrameRate: '14', maxFrameRate: '14'
        }
    }, nativeConstraints = {};

    if (!apitype || 'gcp' === apitype) {
        if (constraints.video) {
            constraints.videoconstraints = constraints.videoconstraints || {};
            constraints.videoconstraints.webrtc = constraints.videoconstraints.webrtc ||
                                                  defaultwebrtcvideoconstraints;
        }
        if (constraints.audio) {
            constraints.audioconstraints = constraints.audioconstraints || {};
        }
    } else {
        nativeConstraints.audio = constraints.audio;
        nativeConstraints.video = constraints.video ? constraints.videoconstraints.webrtc :
                                  constraints.video;
    }

    this.constraints = constraints;
    this.player = player;
    this.apitype = apitype || 'gcp';
};

/**
 * Request local media.
 * @memberof GoCastJS
 * @param {Object} options   - User media options.
 * @param {Function} success - function(stream) {}, called when local media request is granted.
 * @param {Function} failure - function(reason) {}, called when local media request is denied.
 */
GoCastJS.getUserMedia = function(options, success, failure) {
    var player = options.player,
        apitype = options.apitype;

    GoCastJS.Video.captureDevice = '';
    GoCastJS.Audio.inputDevice = '';
    GoCastJS.Audio.outputDevice = '';

    if ('gcp' === apitype) {
        if (true === options.constraints.video) {
            if ('undefined' === typeof(options.constraints.videoconstraints.videoin) ||
                null === options.constraints.videoconstraints.videoin) {
                if ('undefined' === typeof(player.videoinopts['default'])) {
                    failure('GoCastJS.getUserMedia(): ' +
                            'No video devices detected');
                    return;
                } else {
                    options.constraints.videoconstraints.videoin = player.videoinopts['default'];
                }
            }

            console.log('GoCastJS.getUserMedia(): Choosing video: ' +
                        player.videoinopts[options.constraints.videoconstraints.videoin]);
            GoCastJS.Video.captureDevice = options.constraints.videoconstraints.videoin;
        }

        if (true === options.constraints.audio) {
            if ('undefined' === typeof(options.constraints.audioconstraints.audioin) ||
                null === options.constraints.audioconstraints.audioin) {
                if (0 >= player.audioinopts.length) {
                    failure('GoCastJS.getUserMedia(): ' +
                            'No audio input devices detected');
                    return;
                } else {
                    options.constraints.audioconstraints.audioin = player.audioinopts[0];
                }
            }

            if ('undefined' === typeof(options.constraints.audioconstraints.audioout) ||
                null === options.constraints.audioconstraints.audioout) {
                if (0 >= player.audiooutopts.length) {
                    failure('GoCastJS.getUserMedia(): ' +
                            'No audio output devices detected');
                    return;
                } else {
                    options.constraints.audioconstraints.audioout = player.audiooutopts[0];
                }
            }

            console.log('GoCastJS.getUserMedia(): Choosing audio input: ' +
                        options.constraints.audioconstraints.audioin);
            GoCastJS.Audio.inputDevice = options.constraints.audioconstraints.audioin;

            console.log('GoCastJS.getUserMedia(): Choosing audio output: ' +
                        options.constraints.audioconstraints.audioout);
            GoCastJS.Audio.outputDevice = options.constraints.audioconstraints.audioout;
        }

        player.getUserMedia(
            options.constraints,
            function(stream) {
                var hints = {
                    video: options.constraints.video,
                    audio: options.constraints.audio
                };

                player.onnegotiationneeded = function() {
                    player.createOffer(function(sdp) {
                        player.setLocalDescription('offer', sdp, function() {
                            player.source = stream;
                            if ('undefined' !== typeof(success) && null !== success) {
                                success(stream);
                            }
                        }, function(error) {
                            console.log('localPlayer.setLocalDescription(): ', error);
                            if ('undefined' !== typeof(failure) && null !== failure) {
                                failure(error);
                            }
                        });
                    }, function(error) {
                        console.log('localPlayer.createOffer(): ', error);
                        if ('undefined' !== typeof(failure) && null !== failure) {
                            failure(error);
                        }
                    }, null);
                };

                if (false === player.init('localPlayer', [], null)) {
                    throw new GoCastJS.Exception('localPlayer', 'init() failed.');
                }

                if (hints.audio || hints.video) {
                    if (false === player.addStream(stream)) {
                        throw new GoCastJS.Exception('localPlayer',
                                                     'addStream() failed.');
                    }
                } else {
                    player.onnegotiationneeded();
                }
            },
            function(error) {
                if ('undefined' !== typeof(failure) && null !== success) {
                    failure(error);
                }
            }
        );
    } else {
        navigator.webkitGetUserMedia(
            options.constraints,
            function(stream) {
                var s = success || function(stream) {};
                player.autoplay = 'true';
                player.src = webkitURL.createObjectURL(stream);
                player.style.transform = 'rotateY(180deg)';
                player.style.webkitTransform = 'rotateY(180deg)';
                s(stream);
            },
            function() {
                var f = failure || function(error) {};
                f('GoCastJS.getUserMedia: failed');
            }
        );
    }
};

/**
 * Set callback for speaker volume changes.
 * @memberof GoCastJS
 * @param {Number} checkInterval - Interval in milliseconds for volume check.
 * @param {DomElement} localplayer - GoCastPlayer instance used for local video.
 * @param {Function} onSpkVolChanged - function(newVol) {}, called when volume changes.
 * @param {String} apitype           - GoCastJS.SetSpkVolListener() only works with GoCastPlayer API, so apitype = 'gcp'.
 */
GoCastJS.SetSpkVolListener = function(checkInterval,
                                      localplayer,
                                      onSpkVolChanged,
                                      apitype) {
    apitype = apitype || 'gcp';
    if ('gcp' === apitype) {
        return setInterval(function() {
            if (GoCastJS.Audio.spkVol !== localplayer.volume) {
                GoCastJS.Audio.spkVol = localplayer.volume;
                onSpkVolChanged(GoCastJS.Audio.spkVol);
            }
        }, checkInterval);
    }

    return null;
};

/**
 * Set callback for microphone volume changes.
 * @memberof GoCastJS
 * @param {Number} checkInterval - Interval in milliseconds for volume check.
 * @param {DomElement} localplayer - GoCastPlayer instance used for local video.
 * @param {Function} onMicVolChanged - function(newVol) {}, called when volume changes.
 * @param {String} apitype           - GoCastJS.SetMicVolListener() only works with GoCastPlayer API, so apitype = 'gcp'.
 */
GoCastJS.SetMicVolListener = function(checkInterval,
                                      localplayer,
                                      onMicVolChanged,
                                      apitype) {
    apitype = apitype || 'gcp';
    if ('gcp' === apitype) {
        return setInterval(function() {
            if (GoCastJS.Audio.micVol !== localplayer.micvolume) {
                GoCastJS.Audio.micVol = localplayer.micvolume;
                onMicVolChanged(GoCastJS.Audio.micVol);
            }
        }, checkInterval);
    }

    return null;
};

GoCastJS.SetPluginCrashMonitor = function(checkInterval,
                                          localplayer,
                                          onCrashed,
                                          apitype) {
    apitype = apitype || 'gcp';
    if ('gcp' === apitype) {
        return setInterval(function() {
            if (localplayer && onCrashed && ('undefined' === typeof(localplayer.volume))) {
                localplayer.width = 0;
                localplayer.height = 0;
                onCrashed();
            }
        }, checkInterval);
    }

    return null;
};

//!
//!     oChanged      <function(va, vr,
//!                             aia, air,
//!                             aoa, aor)> : callback with
//!                                          'va' (added video devices),
//!                                          'vr' (removed video devices),
//!                                          'aia' (added audio inputs),
//!                                          'air' (removed audio inputs),
//!                                          'aoa' (added audio outputs),
//!                                          'aor' (removed audio outputs),
//!
/**
 * Set callback for add/remove audio/video devices.
 * @memberof GoCastJS
 * @param {Number} checkInterval   - Interval in milliseconds for devices check.
 * @param {DomElement} localplayer - GoCastPlayer instance used for local video.
 * @param {Function} onChanged     - Called when audio/video device list changes. See code for arguments to the callback.
 * @param {String} apitype         - GoCastJS.SetDevicesChangedListener() only works with GoCastPlayer API, so apitype = 'gcp'.
 */
GoCastJS.SetDevicesChangedListener = function(checkInterval,
                                              localplayer,
                                              onChanged,
                                              apitype) {
    var videoInOpts = (!apitype || 'gcp' === apitype) ? localplayer.videoinopts : '';
    var audioInOpts = (!apitype || 'gcp' === apitype) ? localplayer.audioinopts : '';
    var audioOutOpts = (!apitype || 'gcp' === apitype) ? localplayer.audiooutopts : '';

    GoCastJS.Video.devices = [];
    GoCastJS.Audio.inputDevices = [];
    GoCastJS.Audio.outputDevices = [];

    apitype = apitype || 'gcp';
    if ('gcp' === apitype) {
        return setInterval(function() {
            var videoDevicesDeleted = [];
            var videoDevicesAdded = [];
            var audioInDevicesDeleted = [];
            var audioInDevicesAdded = [];
            var audioOutDevicesDeleted = [];
            var audioOutDevicesAdded = [];
            var vInOpts = localplayer.videoinopts;
            var aInOpts = localplayer.audioinopts;
            var aOutOpts = localplayer.audiooutopts;

            // Check for newly deleted devices
            for (i in GoCastJS.Video.devices) {
                if (GoCastJS.Video.devices.hasOwnProperty(i) &&
                    'undefined' === typeof(vInOpts[GoCastJS.Video.devices[i]])) {
                    videoDevicesDeleted.push(GoCastJS.Video.devices[i]);
                }
            }

            for (i in GoCastJS.Audio.inputDevices) {
                if (GoCastJS.Audio.inputDevices.hasOwnProperty(i) &&
                    -1 === aInOpts.indexOf(GoCastJS.Audio.inputDevices[i])) {
                    audioInDevicesDeleted.push(GoCastJS.Audio.inputDevices[i]);
                }
            }

            for (i in GoCastJS.Audio.outputDevices) {
                if (GoCastJS.Audio.outputDevices.hasOwnProperty(i) &&
                    -1 === aOutOpts.indexOf(GoCastJS.Audio.outputDevices[i])) {
                    audioOutDevicesDeleted.push(GoCastJS.Audio.outputDevices[i]);
                }
            }

            // Check for newly added devices
            for (j in vInOpts) {
                if (vInOpts.hasOwnProperty(j) && -1 === GoCastJS.Video.devices.indexOf(j)) {
                    videoDevicesAdded.push(j);
                }
            }

            for (j in aInOpts) {
                if (aInOpts.hasOwnProperty(j) && -1 === GoCastJS.Audio.inputDevices.indexOf(aInOpts[j])) {
                    audioInDevicesAdded.push(aInOpts[j]);
                }
            }

            for (j in aOutOpts) {
                if (aOutOpts.hasOwnProperty(j) && -1 === GoCastJS.Audio.outputDevices.indexOf(aOutOpts[j])) {
                    audioOutDevicesAdded.push(aOutOpts[j]);
                }
            }

            // Refresh the current devices list
            if (0 < videoDevicesAdded.length || 0 < videoDevicesDeleted.length) {
                GoCastJS.Video.devices = [];
                for (i in vInOpts) {
                    if (vInOpts.hasOwnProperty(i)) {
                        GoCastJS.Video.devices.push(i);
                    }
                }
            }

            if (0 < audioInDevicesAdded.length ||
                0 < audioInDevicesDeleted.length) {
                GoCastJS.Audio.inputDevices = [];
                for (i in aInOpts) {
                    if (aInOpts.hasOwnProperty(i)) {
                        GoCastJS.Audio.inputDevices.push(aInOpts[i]);
                    }
                }
            }

            if (0 < audioOutDevicesAdded.length ||
                0 < audioOutDevicesDeleted.length) {
                GoCastJS.Audio.outputDevices = [];
                for (i in aOutOpts) {
                    if (aOutOpts.hasOwnProperty(i)) {
                        GoCastJS.Audio.outputDevices.push(aOutOpts[i]);
                    }
                }
            }

            // Call callback if device list has changed
            if (0 < videoDevicesAdded.length ||
                0 < videoDevicesDeleted.length ||
                0 < audioInDevicesAdded.length ||
                0 < audioInDevicesDeleted.length ||
                0 < audioOutDevicesAdded.length ||
                0 < audioOutDevicesDeleted.length) {
                onChanged(videoDevicesAdded, videoDevicesDeleted,
                          audioInDevicesAdded, audioInDevicesDeleted,
                          audioOutDevicesAdded, audioOutDevicesDeleted);
            }
        }, checkInterval);
    }

    return null;
};

/**
 * Config options for peerconnection.
 * @memberof GoCastJS
 * @class
 * @property {DomElement} player               - GoCastPlayer instance (apitype == 'gcp') or video element (apitype == 'native') used for remote media.
 * @property {String} pcid                     - Unique identifier of peerconnection.
 * @property {Array} iceServers                - [({uri: 'stun|turn:domain:port', (password: pwd)?},)*]
 * @property {Function} onIceMessage           - function(candidate) {}, called when a new local ice candidate is generated.
 * @property {Function} onAddStream            - function(stream) {}, called when a remote stream is added.
 * @property {Function} onRemoveStream         - function(stream) {}, called when a remote stream is removed.
 * @property {Function} onSignalingStateChange - function(newState) {}, called when peerconnection's signaling state changes.
 * @property {Function} onConnStateChange      - function(newState) {}, called when peerconnection's connection state changes.
 * @param {String} apitype                     - 'gcp' or 'native'.
 */
 GoCastJS.PeerConnectionOptions = function(player,
                                          pcid, iceServers,
                                          onIceMessage,
                                          onAddStream,
                                          onRemoveStream,
                                          onSignalingStateChange,
                                          onConnStateChange,
                                          apitype) {
    this.iceServers = iceServers;
    this.onIceMessage = onIceMessage;
    this.onAddStream = onAddStream;
    this.onRemoveStream = onRemoveStream;
    this.onSignalingStateChange = onSignalingStateChange;
    this.onConnStateChange = onConnStateChange;
    this.player = player;
    this.pcid = pcid;
    this.apitype = apitype || 'gcp';
};

/**
 * PeerConnection Constructor.
 * @memberof GoCastJS
 * @class
 * @property {String} sigState - Signaling state.
 * @property {String} connState - Connection state.
 * @param {Object} options - PeerConnection config options.
 */
GoCastJS.PeerConnection = function(options) {
    var self = this, apitype = options.apitype, server,
        player = options.player, onstatechange, i,
        onicechange, onicecandidate, iceservers = [];

    this.apitype = options.apitype;
    this.player = options.player;
    this.pcid = options.pcid;
    this.sigState = 'preinit';
    this.connState = 'preinit';
    this.pendingCandidates = [];
    this.connTimer = null;
    this.connTimeout = 15000;
    this.peerconn = null;

    if ('native' === apitype) {
        for (i=0; i<options.iceServers.length; i++) {
            server = {url: options.iceServers[i].uri};
            if (options.iceServers[i].password) {
                server.credential = options.iceServers[i].password;
            }
            iceservers.push(server);
        }

        this.peerconn = new webkitRTCPeerConnection({iceServers: iceservers});
        this.peerconn.onaddstream = function(e) {
            var screencap = (e.stream.audioTracks && !e.stream.audioTracks.length) ||
                            (e.stream.getAudioTracks && !e.stream.getAudioTracks().length);

            if (!screencap) {
                player.src = webkitURL.createObjectURL(e.stream);
            }
            if (options.onAddStream) {
                options.onAddStream(e.stream);
            }
        }
    } else if ('gcp' === apitype) {
        this.player.onaddstream = function(stream) {
            var screencap = (stream.audioTracks && !stream.audioTracks.length);

            if (!screencap) {
                player.source = stream;
            }
            if ('undefined' !== typeof(options.onAddStream) &&
                null !== options.onAddStream) {
                options.onAddStream(stream);
            }
        };
    }

    if ('native' === apitype) {
        this.peerconn.onremovestream = function(e) {
            if (options.onRemoveStream) {
                options.onRemoveStream(e.stream);
            }
        }
    } else if ('gcp' === apitype) {
        this.player.onremovestream = function(stream) {
            if ('undefined' !== typeof(options.onRemoveStream) &&
                null !== options.onRemoveStream) {
                options.onRemoveStream(stream);
            }
        };
    }

    onstatechange = function(newState) {
        var i;

        self.sigState = newState;
        if ('stable' === newState || 'active' === newState) {
            for (i=0; i<self.pendingCandidates.length; i++) {
                self.AddIceCandidate(self.pendingCandidates[i]);
            }
            self.pendingCandidates = [];
        }
        if ('undefined' !== typeof(options.onSignalingStateChange) &&
            null !== options.onSignalingStateChange) {
            options.onSignalingStateChange(newState);
        }
        if ('active' === newState) {
            onicechange('connected');
        }
    };

    if ('native' === apitype) {
        if (27 <= parseInt($.browser.version)) {
            this.peerconn.onsignalingstatechange = function() {
                if(self.peerconn) {
                    onstatechange(self.peerconn.signalingState);
                }
            };
        } else {
            this.peerconn.onstatechange = function() {
                if(self.peerconn) {
                    if (26 === parseInt($.browser.version)) {
                        onstatechange(self.peerconn.signalingState);
                    } else {
                        onstatechange(self.peerconn.readyState);
                    }
                }
            };
        }
    } else if ('gcp' === apitype) {
        this.player.onstatechange = onstatechange;
    }

    onicechange = function(newState) {
        self.connState = newState;

        if ('checking' === newState || 'disconnected' === newState) {
            if (!self.connTimer) {
                self.connTimer = setTimeout(function() {
                    self.connTimer = null;
                    self.connState = 'timedout';
                    self.connTimeout = 10000;

                    if (options.onConnStateChange) {
                        options.onConnStateChange(self.connState);
                    }
                }, self.connTimeout);
            }
        }
        else if ('connected' === newState) {
            if (self.connTimer) {
                clearTimeout(self.connTimer);
                self.connTimer = null;
            }
        }
        if ('undefined' !== typeof(options.onConnStateChange) &&
            null !== options.onConnStateChange &&
            'defunct' !== self.connState) {
            options.onConnStateChange(newState);
        }
    };

    if ('native' === apitype) {
        if (27 <= parseInt($.browser.version)) {
            this.peerconn.oniceconnectionstatechange = function() {
                if (self.peerconn) {
                    onicechange(self.peerconn.iceConnectionState);
                }
            };
        } else {
            this.peerconn.onicechange = function() {
                if (self.peerconn) {
                    if (26 === parseInt($.browser.version)) {
                        onicechange(self.peerconn.iceConnectionState);
                    } else {
                        onicechange(self.peerconn.iceState);
                    }
                }
            };
        }
    } else if ('gcp' === apitype) {
        this.player.onicechange = onicechange;
    }

    onicecandidate = function(candidate) {
        if ('undefined' !== typeof(options.onIceMessage) &&
           null !== options.onIceMessage) {
            options.onIceMessage(candidate.replace(/\r\n/, ''));
        }
    };

    if ('native' === apitype) {
        this.peerconn.onicecandidate = function(e) {
            if (e.candidate) {
                onicecandidate(JSON.stringify(e.candidate));
            }
        };
    } else if ('gcp' === apitype) {
        if (false === this.player.init(options.pcid,
                                       options.iceServers,
                                       onicecandidate)) {
            throw new GoCastJS.Exception(options.pcid, 'init() failed.');
        }
    }

    this.sigState = 'new';
    this.connState = 'starting';
};

/**
 * Add a local stream to a peerconnection.
 * @memberof GoCastJS.PeerConnection
 * @param {Object} stream - Local stream to be added.
 * @param {Function} negotiationCallback - function() {}, Called when offer/answer negotiation is required between peers.
 */
GoCastJS.PeerConnection.prototype.AddStream = function(stream, negotiationCallback) {
    if ('gcp' === this.apitype) {
        this.player.onnegotiationneeded = negotiationCallback || function() {};
        if (false === this.player.addStream(stream)) {
            throw new GoCastJS.Exception(this.pcid, 'addStream() failed.');
        }
    } else if ('native' === this.apitype) {
        this.peerconn.onnegotiationneeded = negotiationCallback || function() {};
        this.peerconn.addStream(stream);
    }
};

/**
 * Remove a local stream from a peerconnection.
 * @memberof GoCastJS.PeerConnection
 * @param {Object} stream - Local stream to be removed.
 * @param {Function} negotiationCallback - function() {}, Called when offer/answer negotiation is required between peers.
 */
GoCastJS.PeerConnection.prototype.RemoveStream = function(stream, negotiationCallback) {
    if ('gcp' === this.apitype) {
        this.player.onnegotiationneeded = negotiationCallback || function() {};
        if (false === this.player.removeStream(stream)) {
            throw new GoCastJS.Exception(this.pcid, 'removeStream() failed.');
        }
    } else if ('native' === this.apitype) {
        this.peerconn.onnegotiationneeded = negotiationCallback || function() {};
        this.peerconn.removeStream(stream);
    }
};

/**
 * Create an offer session description.
 * @memberof GoCastJS.PeerConnection
 * @param {Function} success   - function(sdp) {}, called when an offer is generated.
 * @param {Function} failure   - function(reason) {}, called when peerconnection is unable to generate an offer.
 * @param {Object} constraints - {sdpconstraints: {mandatory: {OfferToReceiveAudio: 'true|false', OfferToReceiveVideo: 'true|false'}}}
 */
GoCastJS.PeerConnection.prototype.CreateOffer = function(success, failure, constraints) {
    var acodec = constraints.sdpconstraints.mandatory.AudioCodec;

    success = success || function(sdp) {};
    failure = failure || function(error) {};

    if ('gcp' === this.apitype) {
        this.player.createOffer(success, failure, constraints || {});
    } else if ('native' === this.apitype) {
        delete constraints.sdpconstraints.mandatory.AudioCodec;
        this.peerconn.createOffer(function(sdp) {
            if ('opus' === acodec) {
                success(sdp.sdp.replace(/a=rtpmap:[0-9]+\s(ISAC|PCMU|PCMA|CN|telephone\-event)\/[0-9]+\r\n/g, ''));
            } else if ('ISAC' === acodec) {
                success(sdp.sdp.replace(/a=rtpmap:[0-9]+\s(opus|PCMU|PCMA|CN|telephone\-event)\/[0-9]+(\/[0-9]+)?\r\n/g, '')
                               .replace(/a=fmtp:[0-9]+\sminptime=[0-9]+\r\n/g, ''));
            } else {
                success(sdp.sdp);
            }
        }, function() {
            failure('CreateOffer() failed.');
        }, constraints.sdpconstraints);
    }
};

/**
 * Create an answer session description.
 * @memberof GoCastJS.PeerConnection
 * @param {Function} success   - function(sdp) {}, called when an answer is generated.
 * @param {Function} failure   - function(reason) {}, called when peerconnection is unable to generate an answer.
 * @param {Object} constraints - {sdpconstraints: {mandatory: {OfferToReceiveAudio: 'true|false', OfferToReceiveVideo: 'true|false'}}}
 */
GoCastJS.PeerConnection.prototype.CreateAnswer = function(success, failure, constraints) {
    var acodec = constraints.sdpconstraints.mandatory.AudioCodec;

    success = success || function(sdp) {};
    failure = failure || function(error) {};

    if ('gcp' === this.apitype) {
        this.player.createAnswer(success, failure, constraints || {});
    } else if ('native' === this.apitype) {
        delete constraints.sdpconstraints.mandatory.AudioCodec;
        this.peerconn.createAnswer(function(sdp) {
            if ('opus' === acodec) {
                success(sdp.sdp.replace(/a=rtpmap:[0-9]+\s(ISAC|PCMU|PCMA|CN|telephone\-event)\/[0-9]+\r\n/g, ''));
            } else if ('ISAC' === acodec) {
                success(sdp.sdp.replace(/a=rtpmap:[0-9]+\s(opus|PCMU|PCMA|CN|telephone\-event)\/[0-9]+(\/[0-9]+)?\r\n/g, '')
                               .replace(/a=fmtp:[0-9]+\sminptime=[0-9]+\r\n/g, ''));
            } else {
                success(sdp.sdp);
            }
        }, function() {
            failure('CreateAnswer() failed.');
        }, constraints.sdpconstraints);
    }
};

/**
 * Set session description for local media.
 * @memberof GoCastJS.PeerConnection
 * @param {String} action    - 'offer' or 'answer'.
 * @param {String} sdp       - Session description to be set.
 * @param {Function} success   - function() {}, called when local sdp is set.
 * @param {Function} failure   - function(reason) {}, called when there is an error while setting local sdp.
 */
 GoCastJS.PeerConnection.prototype.SetLocalDescription = function(action,
                                                                 sdp,
                                                                 success,
                                                                 failure) {
    success = success || function() {};
    failure = failure || function(error) {};

    if ('gcp' === this.apitype) {
        this.player.setLocalDescription(action, sdp, success, failure);
    } else if ('native' === this.apitype) {
        this.peerconn.setLocalDescription(
            new RTCSessionDescription({sdp: sdp, type: action}),
            success, function() {
                failure('setLocalDescription failed.');
            }
        );
    }
};

/**
 * Set session description for remote media.
 * @memberof GoCastJS.PeerConnection
 * @param {String} action    - 'offer' or 'answer'.
 * @param {String} sdp       - Session description to be set.
 * @param {Function} success   - function() {}, called when remote sdp is set.
 * @param {Function} failure   - function(reason) {}, called when there is an error while setting remote sdp.
 */
GoCastJS.PeerConnection.prototype.SetRemoteDescription = function(action,
                                                                  sdp,
                                                                  success,
                                                                  failure) {
    success = success || function() {};
    failure = failure || function(error) {};

    if ('gcp' === this.apitype) {
        this.player.setRemoteDescription(action, sdp, success, failure);
    } else if ('native' === this.apitype) {
        this.peerconn.setRemoteDescription(
            new RTCSessionDescription({sdp: sdp, type: action}),
            success, function() {
                failure('setRemoteDescription failed.');
            }
        );
    }
};

/**
 * Add remote ice candidate.
 * @memberof GoCastJS.PeerConnection
 * @param {String} sdp    - Session description of the ice candidate.
 */
GoCastJS.PeerConnection.prototype.AddIceCandidate = function(sdp) {
    var candidate = JSON.parse(sdp), candidate1 = {};

    if ('stable' === this.sigState || 'active' === this.sigState) {
        if ('gcp' === this.apitype) {
            if (candidate.candidate) {
                candidate1 = {
                    sdp_mid: candidate.sdpMid,
                    sdp_mline_index: candidate.sdpMLineIndex,
                    sdp: candidate.candidate
                };
            } else {
                candidate1 = candidate;
            }

            if (false === this.player.addIceCandidate(candidate1.sdp_mid,
                                                      candidate1.sdp_mline_index,
                                                      candidate1.sdp)) {
                    throw new GoCastJS.Exception(this.pcid, 'procIceMsg() failed.');
            }
        } else if ('native' === this.apitype) {
            if (candidate.sdp) {
                candidate1 = {
                    sdpMid: candidate.sdp_mid,
                    sdpMLineIndex: candidate.sdp_mline_index,
                    candidate: candidate.sdp
                };
            } else {
                candidate1 = candidate;
            }
            this.peerconn.addIceCandidate(new RTCIceCandidate(candidate1));
        }
    } else {
        this.pendingCandidates.push(sdp);
    }
};

/**
 * Destroy a peerconnection.
 * @memberof GoCastJS.PeerConnection
 */
GoCastJS.PeerConnection.prototype.Deinit = function() {
    if ('gcp' === this.apitype) {
        if (false === this.player.deinit()) {
            throw new GoCastJS.Exception(this.pcid, 'deinit() failed.');
        }
    } else if ('native' === this.apitype) {
        this.peerconn.close();
        this.peerconn = null;
    }
};

/**
 * Set width of the video window associated with a peerconnection.
 * @memberof GoCastJS.PeerConnection
 * @param {Number} width - if provided, sets the width, otherwise returns the current width.
 */
GoCastJS.PeerConnection.prototype.Width = function(width) {
    if ('undefined' !== typeof(width) && null !== width) {
        this.player.width = width;
    }
    return this.player.width;
};

/**
 * Set height of the video window associated with a peerconnection.
 * @memberof GoCastJS.PeerConnection
 * @param {Number} height - if provided, sets the height, otherwise returns the current height.
 */
GoCastJS.PeerConnection.prototype.Height = function(height) {
    if ('undefined' !== typeof(height) && null !== height) {
        this.player.height = height;
    }
    return this.player.height;
};

/**
 * Get peerconnection network statistics.
 * @memberof GoCastJS.PeerConnection
 * @param {Function} onstats - function(stats) {} - callback with stats array.
 */
GoCastJS.PeerConnection.prototype.GetStats = function(onstats) {
    var dumpstats = function(statobj) {
            var ret = {}, statnames;

            if (statobj.id) {
                ret.id = statobj.id;
            }
            if (statobj.type) {
                ret.type = statobj.type;
            }
            if (statobj.timestamp) {
                ret.timestamp = statobj.timestamp;
            }
            if (statobj.names) {
                statnames = statobj.names();
                for (var i=0; i<statnames.length; i++) {
                    ret[statnames[i]] = statobj.stat(statnames[i]);
                }
            } else if (statobj.stat('audioOutputLevel')) {
                ret.audioOutputLevel = statobj.stat('audioOutputLevel');
            }

            return ret;
        };

    onstats = onstats || function(stats) {};
    if ('native' === this.apitype && this.peerconn.getStats) {
        this.peerconn.getStats(function(stats) {
            var statlist = stats.result(),
                statsarr = [], ret;

            for (var i=0; i<statlist.length; i++) {
                if (!statlist[i].local || statlist[i].local === statlist[i]) {
                    statsarr.push(dumpstats(statlist[i]));
                } else {
                    if (statlist[i].local) {
                        ret = dumpstats(statlist[i]);
                        ret.local = true;
                        statsarr.push(ret);
                    }
                    if (statlist.remote) {
                        ret = dumpstats(statlist[i]);
                        ret.remote = true;
                        statsarr.push(ret);                    
                    }
                }
            }

            onstats(statsarr);
        });
    } else {
        onstats([]);
    }
}

/**
 * Get plugin logs for debugging purposes.
 * @memberof GoCastJS
 * @param {DomElement} localplayer - GoCastPlayer instance used for local video.
 * @param {Function} logCallback   - function(entries) {}, called when localplayer produces log entries.
 * @param {String} apitype         - GoCastJS.PluginLog() works only with apitype == 'gcp'.
 */
GoCastJS.PluginLog = function(localplayer, logCallback, apitype) {
    if ('gcp' === apitype) {
        if ('undefined' === localplayer || null === localplayer) {
            return;
        } else if ('undefined' === logCallback || null === logCallback) {
            localplayer.logFunction(null);
        } else {
            localplayer.logFunction(function() {
                var entries = localplayer.logentries;
                if(0 < entries.length) {
                    logCallback(entries);
                }
            });
        }
    }
};
