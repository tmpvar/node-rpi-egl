# Raspberry PI EGL

This package creates an opengl es2 context right on your terminal.  It expects that you are connected to a display via HDMI

# install

`npm install rpi-egl`

# usage

```javascript
  
  var egl = require('rpi-egl');
  egl.init();

  // draw stuff with opengl

```

# gotchas

I've had some problems with the error
    Error: libbcm_host.so: cannot open shared object file: No such file or directory

Add `/opt/vc/lib` to `/etc/ld.so.conf.d/vc.conf` and run `ldconfig`

