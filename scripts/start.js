const os = require('os');
const path = require('path');
const fs = require('fs-extra');
const program = require('commander');
const { execSync, spawnSync } = require('child_process');

const logger = require('./logger');
const msbuild = require('./msbuild');
const pkg = require('../lcpkg.json');
const { format } = require('./utils');

const TARGET_NAME = 'app';
const TARGET_EXT = process.platform === 'win32' ? '.exe' :'';
const TARGET_FILE_NAME = `${TARGET_NAME}${TARGET_EXT}`;
const ROOT_DIR =  path.resolve(__dirname, '..');
const BUILD_DIR = path.join(ROOT_DIR, 'build');
const TARGET_DIR = path.join(ROOT_DIR, 'app');
const TARGET_PATH = path.join(TARGET_DIR, TARGET_FILE_NAME);
const LCPKG_DIR = path.join(ROOT_DIR, 'lcpkg', 'installed');

class XMake {
  constructor() {
    this.name = 'XMake'
  }

  configure() {
    return spawnSync('xmake', ['config'], { stdio: 'inherit', cwd: ROOT_DIR })
  }

  build() {
    return spawnSync('xmake', ['build'], { stdio: 'inherit', cwd: ROOT_DIR })
  }

  run() {
    return spawnSync('xmake', ['run'], { stdio: 'inherit', cwd: ROOT_DIR })
  }
}

class CMake {
  constructor(builder) {
    this.name = 'CMake'
    this.builder = builder
  }

  configure() {
    const params = ['../']

    if (this.builder.platform === 'windows') {
      params.push('-DCMAKE_GENERATOR_PLATFORM=x64', '-T', 'v141_xp')
    }
    return spawnSync('cmake', params, { stdio: 'inherit', cwd: BUILD_DIR })
  }

  build() {
    if (this.builder.platform === 'windows') {
      if (!msbuild.exists()) {
        logger.error('Visual Studio not found, build stopped.')
        return null
      }
      if (this.builder.mode === 'debug') {
        return msbuild.exec('..\\scripts\\build-bin-x64-dbg', { cwd: BUILD_DIR })
      }
      return msbuild.exec('..\\scripts\\build-bin-x64-rel', { cwd: BUILD_DIR })
    }
    return spawnSync('make', { stdio: 'inherit', cwd: BUILD_DIR })
  }

  run() {
    return spawnSync(TARGET_PATH, { stdio: 'inherit', cwd: TARGET_DIR })
  }
}

class Builder {
  constructor({
    mode = 'release',
    arch = 'x64',
    cssInput = '',
    cssOutput = '',
    configureFiles = []
  } = {}) {
    this.mode = mode;
    this.arch = arch;
    this.platform = process.platform == 'win32' ? 'windows' : process.platform;
    this.cssInput = cssInput;
    this.cssOutput = cssOutput;
    this.configureFiles = configureFiles;
    this.devdir = path.join(LCPKG_DIR, `${this.arch}-${this.platform}`);
    if (this.mode === 'debug') {
      this.devdir = path.join(this.devdir, 'debug');
    }
    this.libdir = path.join(this.devdir, 'lib');
    this.bindir = path.join(this.devdir, 'bin');
    this.tool = this.detectBuildTool();
  }

  detectBuildTool() {
    const tools = ['cmake', 'xmake'];

    for (let tool of tools) {
      try {
        const stdout = execSync(`${tool} --version`, { encoding: 'utf-8' });

        if (stdout) {
          if (tool === 'cmake') {
            return new CMake(this);
          }
          if (tool === 'xmake') {
            return new XMake(this);
          }
        }
      } catch (err) {
        continue;
      }
    }
    return null;
  }

  beforeBuild() {
    const info = {
      ...pkg,
      os: {
        type: os.type(),
        arch: os.arch(),
        release: os.release()
      },
      build_time: new Date().toISOString()
    };

    if (!fs.existsSync(BUILD_DIR)) {
      fs.mkdirSync(BUILD_DIR);
    }
    this.configureFiles.forEach((f) => {
      const content = fs.readFileSync(`${f}.in`, { encoding: 'utf-8' });
      fs.writeFileSync(f, format(content, info));
    });
  }

  afterBuild() {
    if (!fs.existsSync(this.bindir)) {
      return;
    }
    logger.log(`copy dependency files into app directory...`);
    fs.copySync(this.bindir, TARGET_DIR);
    if (this.platform == 'windows') {
      fs.copySync(path.join(TARGET_DIR, this.mode, TARGET_FILE_NAME), TARGET_PATH);
    }
  }

  buildCSS() {
    logger.log('build stylesheets...');
    execSync(
      `npx node-sass --output ${this.cssOutput} --output-style expanded ${this.cssInput}`,
      { stdio: 'inherit', cwd: ROOT_DIR }
    );
  }

  run() {
    const tool = this.tool;

    if (!tool) {
      logger.error('the build tool was not found! currently supports cmake and xmake, please install one of them.');
      return;
    }
    this.beforeBuild();
    this.buildCSS();
    logger.log(`build tool is ${tool.name}`);
    tool.configure();
    if (tool.build().status !== 0) {
      logger.error('build failed.');
      return null;
    }
    this.afterBuild();
    logger.log('build completed.');
    logger.log('run application...');
    const result = tool.run();
    if (result.status !== 0) {
      logger.error('the application is not working properly.');
      return null;
    }
    return logger.log('the application has exited.');
  }
}

function start(options) {
  const builder = new Builder({
    configureFiles: ['include/version.h', 'CMakeLists.txt'],
    cssInput: 'src/ui/stylesheets/app.scss',
    cssOutput: 'app/assets/stylesheets',
    ...options
  });

  builder.run();
}

program
  .usage('[options]')
  .option('--mode <mode>', 'specify build mode', (mode, defaultMode) => {
    if (!['debug', 'release'].includes(mode)) {
      logger.error(`invalid mode: ${mode}`);
      return defaultMode;
    }
    return mode;
  }, 'release')
  .option('--arch <arch>', 'specify CPU architecture', (arch, defaultArch) => {
    if (!['x86', 'x64', 'arm'].includes(arch)) {
      logger.error(`invalid arch: ${arch}`);
      return defaultArch;
    }
    return arch;
  }, 'x64')
  .action(() => {
    start({
      mode: program.mode,
      arch: program.arch
    })
  })
  .parse(process.argv)
