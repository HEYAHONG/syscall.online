/**
 * terminal.js — xterm.js local loader (no CDN required)
 * Loads local copies of xterm@4.19.0 + xterm-addon-fit@0.7.0.
 * Patches window._xtermReady after load so shell.js can proceed.
 */
(function () {
  var STATIC = '/static/wasm';

  function loadScript(src) {
    return new Promise(function (resolve, reject) {
      var s = document.createElement('script');
      s.src = src;
      s.onload = resolve;
      s.onerror = reject;
      document.head.appendChild(s);
    });
  }

  function loadCss(href) {
    var l = document.createElement('link');
    l.rel = 'stylesheet';
    l.href = href;
    document.head.appendChild(l);
  }

  // Load local xterm.js files
  loadCss(STATIC + '/css/xterm.css');
  // xterm-addon-fit has no separate CSS (styles live in xterm.css)

  Promise.all([
    loadScript(STATIC + '/js/xterm.js'),
    loadScript(STATIC + '/js/xterm-addon-fit.js'),
  ]).then(function () {
    window._xtermReady = true;
  }).catch(function (e) {
    console.error('[terminal.js] load failed:', e);
  });
})();
