/**
 * shell.js — xterm.js adapter for Emscripten shell
 *
 * Wraps xterm.js Terminal so it presents the same API as the old DOM terminal:
 *   createTerminal(terminalId) → { print, printErr, printWarn, printInfo, printSystem, clear }
 *
 * Also exposes globally:
 *   TerminalWasm  — xterm instance for #terminal-wasm (stdout)
 *   TerminalLog   — xterm instance for #terminal-log (stderr/log)
 *   Loader        — { setProgress, finish }
 *   switchTab, togglePause, setStatus, goFullscreen, Module, DEFAULT_TAB
 *
 * Color scheme: stdout=green, stderr=red, warn=orange, info=cyan, system=dim
 */

/* ─── Helpers ─────────────────────────────────────────────────────────────── */

function waitForXterm(callback) {
  if (window._xtermReady) { callback(); return; }
  var iv = setInterval(function () {
    if (window._xtermReady) { clearInterval(iv); callback(); }
  }, 50);
}

function makeTermTheme() {
  return {
    background:            '#0a0a0a',
    foreground:            '#e0e0e0',
    cursor:                '#00ff88',
    cursorAccent:          '#0a0a0a',
    selectionBackground:   'rgba(0,153,255,0.3)',
    black:                 '#1a1a1a',
    red:                   '#ff4444',
    green:                 '#00ff88',
    yellow:                '#ffaa00',
    blue:                  '#0099ff',
    magenta:               '#cc88ff',
    cyan:                  '#00d4aa',
    white:                 '#e0e0e0',
    brightBlack:           '#666666',
    brightRed:             '#ff6666',
    brightGreen:           '#66ffaa',
    brightYellow:          '#ffcc44',
    brightBlue:            '#44aaff',
    brightMagenta:         '#dd99ff',
    brightCyan:            '#44eedd',
    brightWhite:           '#ffffff',
  };
}

/* ─── Normalize line endings for xterm.js ─────────────────────────────────── */

/**
 * xterm.js 不会自动将单独的 \n 转换为 \r\n（在 convertEol:true 下，
 * 只有 \r\n 序列才被当换行处理；单独的 \n 只下移光标不回车）。
 * 本函数在写入前将所有孤立 \n 替换为 \r\n，已有的 \r\n 保持不变。
 */
function writeNorm(s) {
  if (!s) return '';
  return s.replace(/\r?\n/g, function (m) {
    return m === '\n' ? '\r\n' : m;
  });
}

/* ─── Terminal instances ──────────────────────────────────────────────────── */

var TerminalWasm = null;  // stdout  (#terminal-wasm)
var TerminalLog  = null;  // stderr  (#terminal-log)

/**
 * 创建 xterm 实例，但延迟 open() 直到容器可见。
 * 返回的对象包含：
 *   term, fitAddon, resize, ensureInitialized, write, isInitialized
 * 其中 ensureInitialized() 会在容器首次可见时调用 term.open()
 * write(data) 会在初始化前缓存数据，初始化后自动 flush
 */
function createTerminalX(terminalId, fontSize) {
  var container = document.getElementById(terminalId);
  if (!container) return null;

  if (!window.Terminal || !window.FitAddon) {
    console.error('[shell.js] xterm.js or FitAddon not loaded yet');
    return null;
  }

  var term = new window.Terminal({
    theme:            makeTermTheme(),
    fontSize:         fontSize || 13,
    fontFamily:       "'Cascadia Code', 'Fira Code', 'Consolas', monospace",
    cursorBlink:      true,
    cursorStyle:      'block',
    scrollback:       5000,
    convertEol:       false,   // 完全由 writeNorm() 处理换行，避免冲突
    allowProposedApi: true,
    cols:             120,     // 给一个合理的默认列数，fit() 后会自动调整
    rows:             30,
  });

  var fitAddon = new window.FitAddon.FitAddon();
  term.loadAddon(fitAddon);

  var initialized = false;
  var pendingData = [];   // 初始化前缓存的写入数据

  function safeFit() {
    if (!initialized) return;
    try {
      fitAddon.fit();
    } catch (e) {
      console.warn('[shell.js] fit() failed:', e.message);
    }
  }

  function flushPending() {
    if (pendingData.length === 0) return;
    var buf = pendingData.slice();
    pendingData = [];
    buf.forEach(function (chunk) {
      try { term.write(chunk); } catch (e) {}
    });
  }

  function ensureInitialized() {
    if (initialized) return;
    // 再次检查容器是否真的可见（有尺寸）
    if (container.offsetWidth === 0 || container.offsetHeight === 0) return;
    try {
      term.open(container);
      initialized = true;
      safeFit();
      flushPending();
      console.log('[shell.js] ' + terminalId + ' initialized: ' + term.cols + ' cols × ' + term.rows + ' rows');
    } catch (e) {
      console.error('[shell.js] term.open(' + terminalId + ') failed:', e.message);
    }
  }

  // 防抖 fit：避免拖拽面板时频繁调用
  var _fitTimer = null;
  function debouncedFit() {
    if (_fitTimer) clearTimeout(_fitTimer);
    _fitTimer = setTimeout(function () {
      safeFit();
      _fitTimer = null;
    }, 80);
  }

  // ResizeObserver：初始化前等容器可见；初始化后监听尺寸变化自动 fit
  if (window.ResizeObserver) {
    var ro = new ResizeObserver(function (entries) {
      if (!initialized) {
        if (container.offsetWidth > 0 && container.offsetHeight > 0) {
          ensureInitialized();
        }
      } else {
        // 容器尺寸变化，自动重新 fit
        debouncedFit();
      }
    });
    ro.observe(container);
  } else {
    // fallback: 用 setTimeout 轮询检查
    var pollTimer = setInterval(function () {
      if (initialized) { clearInterval(pollTimer); return; }
      if (container.offsetWidth > 0 && container.offsetHeight > 0) {
        ensureInitialized();
        clearInterval(pollTimer);
      }
    }, 100);
  }

  function resize() { debouncedFit(); }
  window.addEventListener('resize', resize);

  return {
    term: term,
    fitAddon: fitAddon,
    resize: resize,
    ensureInitialized: ensureInitialized,
    write: function (data) {
      if (initialized) {
        try { term.write(data); } catch (e) {}
      } else {
        pendingData.push(data);
      }
    },
    isInitialized: function () { return initialized; }
  };
}

/* ─── createTerminal adapter ──────────────────────────────────────────────── */

function createTerminal(terminalId) {
  var info = null;
  if (terminalId === 'terminal-wasm') {
    if (!TerminalWasm) TerminalWasm = createTerminalX('terminal-wasm', 13);
    info = TerminalWasm;
  } else if (terminalId === 'terminal-log') {
    if (!TerminalLog) TerminalLog = createTerminalX('terminal-log', 12);
    info = TerminalLog;
  } else {
    return null;
  }
  if (!info) return null;

  // 如果容器还没可见，尝试立即初始化一次（也许现在已经可见了）
  info.ensureInitialized();

  var t = info.term;
  // 关键：Emscripten 的 Module.print/printErr 接收的每行已剥离 \n，
  // 所以每个 print 调用必须在末尾追加 \r\n 才能在 xterm 中换行。
  return {
    print:       function (m) { info.write(m + '\r\n'); },
    printErr:    function (m) { info.write('\x1b[31m' + m + '\x1b[0m\r\n'); },
    printWarn:   function (m) { info.write('\x1b[33m' + m + '\x1b[0m\r\n'); },
    printInfo:   function (m) { info.write('\x1b[36m' + m + '\x1b[0m\r\n'); },
    printSystem: function (m) { info.write('\x1b[90m' + m + '\x1b[0m\r\n'); },
    clear:       function ()  { if (info.isInitialized()) t.clear(); },
    resize:      function (c, r) { if (info.isInitialized()) t.resize(c, r); },
  };
}

/* ─── Loader ──────────────────────────────────────────────────────────────── */

var Loader = {
  setProgress: function (pct, msg) {
    var b = document.getElementById('loader-bar');
    var s = document.getElementById('loader-status');
    if (b) b.style.width = Math.min(100, Math.max(0, pct)) + '%';
    if (s && msg) s.textContent = msg;
  },
  finish: function () {
    var self = this;
    self.setProgress(100, 'Ready');
    setTimeout(function () {
      document.getElementById('loading-screen')?.classList.add('hidden');
      document.getElementById('main-ui')?.classList.add('visible');
      if (TerminalWasm && TerminalWasm.resize) TerminalWasm.resize();
      if (TerminalLog  && TerminalLog.resize)  TerminalLog.resize();
      bindToolbarHover();
      self._done = true;
    }, 350);
  },
};

/* ─── Tab switching ────────────────────────────────────────────────────────── */

function switchTab(tabName) {
  document.querySelectorAll('.tab-btn').forEach(function (btn) {
    btn.classList.toggle('active', btn.dataset.tab === tabName);
  });
  document.querySelectorAll('.tab-content').forEach(function (content) {
    content.classList.toggle('active', content.id === tabName + '-tab');
  });
  var fsBtn = document.getElementById('btn-fullscreen');
  if (fsBtn) fsBtn.classList.toggle('disabled', tabName !== 'canvas');

  // 切换 tab 后，确保对应 terminal 已初始化，并 fit
  setTimeout(function () {
    if (tabName === 'console') {
      if (TerminalWasm && TerminalWasm.ensureInitialized) {
        TerminalWasm.ensureInitialized();
        if (TerminalWasm.resize) TerminalWasm.resize();
      }
      if (TerminalLog && TerminalLog.ensureInitialized) {
        TerminalLog.ensureInitialized();
        if (TerminalLog.resize) TerminalLog.resize();
      }
    }
  }, 50);
}

/* ─── Toolbar hover ────────────────────────────────────────────────────────── */

function bindToolbarHover() {
  var trigger = document.getElementById('toolbar-trigger');
  var toolbar = document.getElementById('toolbar');
  if (!trigger || !toolbar) return;
  trigger.addEventListener('mouseenter', function () { toolbar.style.transform = 'translateY(0)'; });
  trigger.addEventListener('mouseleave', function () { toolbar.style.transform = ''; });
  toolbar.addEventListener('mouseenter', function () { toolbar.style.transform = 'translateY(0)'; });
  toolbar.addEventListener('mouseleave', function () { toolbar.style.transform = ''; });
}

/* ─── Panel divider drag ───────────────────────────────────────────────────── */

(function () {
  var divider   = document.getElementById('panel-divider');
  var leftPanel = document.querySelector('#console-tab .panel-left');
  if (!divider || !leftPanel) return;
  var dragging = false;
  divider.addEventListener('mousedown', function () {
    dragging = true;
    divider.classList.add('dragging');
    document.body.style.userSelect = 'none';
    document.body.style.cursor    = 'col-resize';
  });
  document.addEventListener('mousemove', function (e) {
    if (!dragging) return;
    var container = document.getElementById('console-tab');
    var rect      = container.getBoundingClientRect();
    var x         = e.clientX - rect.left;
    var w         = rect.width;
    var ratio     = Math.min(1 - 100 / w, Math.max(100 / w, x / w));
    leftPanel.style.flex = '0 0 ' + (ratio * 100) + '%';
  });
  document.addEventListener('mouseup', function () {
    if (!dragging) return;
    dragging = false;
    divider.classList.remove('dragging');
    document.body.style.userSelect = '';
    document.body.style.cursor     = '';
    if (TerminalWasm && TerminalWasm.resize) TerminalWasm.resize();
    if (TerminalLog  && TerminalLog.resize)  TerminalLog.resize();
  });
})();

/* ─── Status ───────────────────────────────────────────────────────────────── */

function setStatus(state, text) {
  var d = document.getElementById('status-dot');
  var t = document.getElementById('status-text');
  if (d) d.className = 'status-dot ' + (state || '');
  if (t && text !== undefined) t.textContent = text;
}

/* ─── Fullscreen ───────────────────────────────────────────────────────────── */

function goFullscreen() {
  document.getElementById('canvas')?.requestFullscreen?.();
}

/* ─── Module (Emscripten) ──────────────────────────────────────────────────── */

window.Module = {
  canvas: (function () {
    var c = document.getElementById('canvas');
    if (!c) return document.createElement('canvas');
    c.addEventListener('webglcontextlost', function (e) { e.preventDefault(); });
    c.addEventListener('webglcontextrestored', function () {});
    return c;
  })(),

  // 关键：Emscripten Module.print 接收的每行已剥离 \n，必须追加 \r\n
  print: function (m) {
    if (TerminalWasm && TerminalWasm.write) {
      TerminalWasm.write(m + '\r\n');
    } else {
      console.log('[WASM stdout]', m);
    }
  },

  printErr: function (m) {
    if (TerminalLog && TerminalLog.write) {
      TerminalLog.write('\x1b[31m' + m + '\x1b[0m\r\n');
    } else {
      console.error('[WASM stderr]', m);
    }
  },

  printBrowser: function (m) {
    console.log('[browser]', m);
  },

  setStatus: function (t) {
    if (!t) return;
    var m = t.match(/([^(]+)\((\d+(\.\d+)?)\/(\d+)\)/);
    if (m) Loader.setProgress((+m[2] / +m[4]) * 100, m[1].trim());
    else Loader.setProgress(10, t);
  },

  totalDependencies: 0,

  monitorRunDependencies: function (l) {
    window.Module.totalDependencies = Math.max(window.Module.totalDependencies, l);
    Loader.setProgress(
      window.Module.totalDependencies
        ? ((window.Module.totalDependencies - l) / window.Module.totalDependencies) * 90
        : 10,
      l ? 'Loading…' : 'Finalizing…'
    );
  },

  onRuntimeInitialized: function () {
    Loader.finish();
    setStatus('running', '运行中');
  },
};

/* ─── Pause / Resume ──────────────────────────────────────────────────────── */

var _paused = false;
function togglePause() {
  _paused = !_paused;
  var iconPause = document.getElementById('icon-pause');
  var iconPlay  = document.getElementById('icon-play');
  var text      = document.getElementById('playpause-text');
  var btn       = document.getElementById('btn-playpause');
  if (iconPause) iconPause.style.display = _paused ? 'none' : '';
  if (iconPlay)  iconPlay.style.display  = _paused ? ''    : 'none';
  if (text) text.textContent = _paused ? '运行' : '暂停';
  if (btn)  btn.title         = _paused ? '运行' : '暂停';
  if (_paused) window.Module.pauseMainLoop?.();
  else         window.Module.resumeMainLoop?.();
  setStatus(_paused ? '' : 'running', _paused ? '已暂停' : '运行中');
}

/* ─── Init ─────────────────────────────────────────────────────────────────── */

var DEFAULT_TAB = 'canvas';

document.querySelectorAll('.tab-btn').forEach(function (btn) {
  btn.addEventListener('click', function () { switchTab(btn.dataset.tab); });
});

waitForXterm(function () {
  if (!window.Terminal || !window.FitAddon) {
    console.error('[shell.js] xterm.js failed to load');
    if (TerminalLog && TerminalLog.write) TerminalLog.write('\x1b[31m[Error] xterm.js failed to load\x1b[0m\r\n');
    return;
  }

  // 先切换 tab，确保目标容器可见，再初始化 terminal
  var initialTab = document.body.getAttribute('data-default-tab')
    || (typeof DEFAULT_TAB !== 'undefined' ? DEFAULT_TAB : 'canvas');
  switchTab(initialTab);

  // 延迟初始化，确保 DOM 已更新（容器已可见）
  setTimeout(function () {
    if (!TerminalWasm) TerminalWasm = createTerminalX('terminal-wasm', 13);
    if (!TerminalLog)  TerminalLog  = createTerminalX('terminal-log',  12);
    // 手动触发一次初始化检查
    if (TerminalWasm && TerminalWasm.ensureInitialized) TerminalWasm.ensureInitialized();
    if (TerminalLog  && TerminalLog.ensureInitialized)  TerminalLog.ensureInitialized();
  }, 80);

  window.addEventListener('error', function (e) {
    if (TerminalLog && TerminalLog.write) TerminalLog.write('\x1b[31m[Error] ' + e.message + '\x1b[0m\r\n');
    setStatus('error');
  });
  window.addEventListener('unhandledrejection', function (e) {
    var msg = (e.reason && e.reason.message) ? e.reason.message : e.reason;
    if (TerminalLog && TerminalLog.write) TerminalLog.write('\x1b[31m[Unhandled] ' + msg + '\x1b[0m\r\n');
  });
});
