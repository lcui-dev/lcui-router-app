const os = require('os');
const path = require('path');
const fs = require('fs-extra');
const chalk = require('chalk');
const { execSync } = require('child_process');
const logger = require('./logger');
const { getProperty, format } = require('./utils');
const xmake = require('./xmake');
const cmake = require('./cmake');

const tools = [
  xmake,
  cmake
];

function createOptions({
  tool = null,
  mode = 'release',
  arch = 'x64',
  hooks
}) {
  const opts = {
    tool,
    mode,
    arch,
    hooks,
    platform: process.platform === 'win32' ? 'windows' : process.platform,
    topDir: path.resolve(__dirname, '..', '..'),
    targetName: 'app',
    targetExt: process.platform === 'win32' ? '.exe' : '',
  };

  opts.targetFileName = `${opts.targetName}${opts.targetExt}`;
  opts.targetDir = path.join(opts.topDir, 'app');
  opts.targetPath = path.join(opts.targetDir, opts.targetFileName);
  opts.buildDir = path.join(opts.topDir, 'build');
  opts.lcpkgDir = path.join(opts.topDir, 'lcpkg', 'installed');
  opts.depDir = path.join(opts.lcpkgDir, `${opts.arch}-${opts.platform}`);
  if (opts.mode === 'debug') {
    opts.depDir = path.join(opts.depDir, 'debug');
  }
  opts.libDir = path.join(opts.depDir, 'lib');
  opts.binDir = path.join(opts.depDir, 'bin');
  return opts;
}

function resolveProjectFile() {
  let dirname = __dirname;
  while (dirname !== '/') {
    const projectFile = path.resolve(dirname, 'lcpkg.json');
    if (fs.existsSync(projectFile)) {
      return projectFile;
    }
    dirname = path.parse(dirname).dir;
  }
  return null;
}

function loadJSONFile(file) {
  return JSON.parse(fs.readFileSync(file, 'utf-8'));
}

function findLibs(project, mode, arch) {
  const libs = [];
  const packagesDir = path.resolve(os.homedir(), '.lcpkg', 'packages');
  const linkSearchDirs = [];

  getProperty(project, 'builder.linkDirs', []).forEach((linkDir) => {
    linkSearchDirs.push(path.resolve(linkDir));
  });
  Object.keys(project.dependencies).forEach((name) => {
    const { version: v } = project.dependencies[name];
    const version = v.startsWith('v') ? v.substr(1) : v;
    const platform = `${arch}-${process.platform === 'win32' ? 'windows' : process.platform}`;
    const packageDir = path.resolve(packagesDir, name, version);
    linkSearchDirs.push(path.resolve(packageDir, platform, mode === 'debug' ? 'debug/lib' : 'lib'));
  });
  linkSearchDirs.forEach((linkDir) => {
    if (!fs.existsSync(linkDir) || !fs.statSync(linkDir).isDirectory()) {
      return;
    }
    fs.readdirSync(linkDir).forEach((file) => {
      const { name, ext } = path.parse(file);
      if (['.a', '.so', '.lib'].includes(ext)) {
        libs.push(name.startsWith('lib') ? name.substr(3) : name);
      }
    });
  });
  return Array.from(new Set(libs));
}

function detechTool(name) {
  if (name && name !== 'auto') {
    return tools.find((tool) => tool.name === name);
  }
  const target = tools.find((tool) => {
    try {
      if (execSync(tool.test, { encoding: 'utf-8' })) {
        return true;
      }
    } catch (err) {
      return false;
    }
    return false;
  });
  if (!target) {
    throw new Error('the build tool was not found! currently supports cmake and xmake, please install one of them.');
  }
  return target;
}

class Builder {
  constructor(options) {
    this.rawOptions = options;
    this.configFile = path.resolve(__dirname, '..', '..', 'config', 'builder.json');
    this.projectFile = resolveProjectFile();
    this.projectDir = this.projectFile && path.parse(this.projectFile).dir;
    this.options = {};

    if (this.projectFile) {
      this.project = loadJSONFile(this.projectFile);
    }
    if (fs.existsSync(this.configFile)) {
      this.options = loadJSONFile(this.configFile);
    }
    this.options = createOptions({ ...this.options, ...this.rawOptions });
    this.tool = detechTool(this.options.tool);
  }

  writeConfigFiles() {
    const info = {
      ...this.project,
      os: {
        type: os.type(),
        arch: os.arch(),
        release: os.release()
      },
      build_time: new Date().toISOString(),
      libs: findLibs(this.project, this.options.mode, this.options.arch)
    };

    getProperty(this.project, 'builder.configureFiles', []).forEach((file) => {
      const content = fs.readFileSync(file, { encoding: 'utf-8' });
      const outname = file.endsWith('.in') ? file.substr(0, file.length - 3) : file;
      const outfile = path.resolve(this.projectDir, outname);
      logger.log(`writing ${outfile}`);
      fs.writeFileSync(outfile, format(content, info));
    });
  }

  writeBuilderConfigFile() {
    const opts = this.options;
    const config = {
      mode: opts.mode,
      arch: opts.arch,
      tool: this.tool.name
    };
    const configDir = path.dirname(this.configFile);
    if (!fs.existsSync(configDir)) {
      fs.mkdirSync(configDir, { recursive: true });
    }
    if (!fs.existsSync(opts.buildDir)) {
      fs.mkdirSync(opts.buildDir);
    }
    logger.log(`writing ${this.configFile}`);
    fs.writeFileSync(this.configFile, JSON.stringify(config, null, 2));
    logger.log();
  }

  callHook(name) {
    const script = format(getProperty(this.options, `hooks.${name}`, ''), this.options);
    if (script) {
      logger.log(`\n${chalk.grey(`[${name}]`)}\n> ${script}\n`);
      try {
        execSync(script, { stdio: 'inherit' });
      } catch (err) {
        logger.error(err.message);
      }
    }
  }

  configure() {
    logger.log(chalk.grey('\n[configure]'));
    this.options = createOptions(this.rawOptions);
    this.tool = detechTool(this.options.tool);
    this.writeConfigFiles();
    this.writeBuilderConfigFile();
    return this.tool.configure(this.options);
  }

  beforeBuild() {
    if (!fs.existsSync(this.options.buildDir)) {
      fs.mkdirSync(this.options.buildDir);
    }
    this.callHook('beforeBuild');
  }

  build() {
    this.beforeBuild();
    logger.log(chalk.grey('\n[build]'));
    const result = this.tool.build(this.options);
    this.afterBuild();
    return result;
  }

  afterBuild() {
    const opts = this.options;
    const file = path.resolve(opts.targetDir, opts.mode, opts.targetFileName);

    if (fs.existsSync(file)) {
      fs.copyFileSync(file, opts.targetPath);
    }
    this.callHook('afterBuild');
  }

  run() {
    logger.log(`\n${chalk.grey('[run]')}\n> ${this.options.targetPath}`);
    if (!fs.existsSync(this.options.targetPath)) {
      throw new Error('executable file does not exist');
    }
    return this.tool.run(this.options);
  }
}

module.exports = Builder;
