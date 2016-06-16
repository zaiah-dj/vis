<html>
<head>
<link rel="stylesheet" type=text/css href="index.css">
<link rel="stylesheet" type=text/css href="https://fonts.googleapis.com/css?family=Righteous">
</head>
<h1>vis</h1>
Vis is an audio visualizer.   It's primary use is to analyze wav files and make something out of them.  However, it's designed to be able to handle a variety of tasks and be extensible.  Enjoy!


<h2>Usage</h2>
<p>
Here is the complete list of options thus far:
<pre>
<code>
-l, --loop               Enable looping. 
    --describe           Describe an audio stream.
-d, --dir (arg)          Save video files to $dir.
-p, --play (arg)         Choose a file to play.
-r, --render (arg)       Render a session to file.
-s, --switch (arg)       Switch animations every $arg seconds.
-f, --framerate (arg)    Use $arg fps when rendering.
-n, --resolution (arg)   Use $arg resolution.
-e, --enable-flashes     Don't skip animations that may cause seizures.
</code>
</pre>
</p>

<p>
The most important options are -p, -l and -f.  -p expects the name of a WAV file (for now, other file types will be supported in the future). If 'vis' has been compiled with the PLAY_EMBEDDED flag, it will discard any file name fed to it this way and instead use WAV data embedded in the source.   The -l flag puts vis in loop mode, looping either input supplied by you via the -p flag or the embedded WAV data.  Finally, -f controls frame rate of animations to music.  The default is something paltry like 20 fps.  On most modern systems, 80 fps is a reachable limit.  If you are at any possible risk of experiencing an eipileptic seizure, <b>do not set the framerate above 40 frames per second</b>.
</p>



<h2>Extension and Hacking</h2>
<p>
Right now, vis can be very easily extended by modifying main.c.   The prototype of an animation function is as follows:
<pre>
void my_new_animation (Surface *w, Av *v) {
	/* ... the code for animation goes here ... */
}
</pre>
</p>

<p>
Flags that are supported:
<pre>
<code>
-DENABLE_VERSION_BANNER - Turns version announcement on or off.
-DPLAY_EMBEDDED         - Use embedded audio instead of an external audio file.
-DPLAY_AUDIO_ONLY       - Play no animations, just play the audio.
</code>
</pre>
</p>


<h2>Caveats</h2>
<p>
As mentioned in the 'Usage' section, people at risk for epileptic seizures should not crank this program to a high framerate.  Many of the effects used here are crude, ugly, and highly saturated with color (like anything from <a href="http://paperrad.org">Paper Rad</a>).  To prevent any possible health issues, the Av struct contains a field titled 'no_epilepsy'.  If an animation has this field set, 'vis' will skip it by default.  Use the option --enable-flashes to see these animations. 
</p>



</html>
