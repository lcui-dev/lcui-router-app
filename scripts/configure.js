const os = require('os');
const path = require('path');
const fs = require('fs-extra');
const program = require('commander');
const pkg = require('../lcpkg.json');
const Builder = require('./lib/builder');
const { format, getProperty } = require('./lib/utils');
const logger = require('./lib/logger');

function findLibs(deps, mode, arch) {
  const libs = [];
  const packagesDir = path.resolve(os.homedir(), '.lcpkg', 'packages');

  Object.keys(deps).forEach((name) => {
    const { version: v } = deps[name];
    const version = v.startsWith('v') ? v.substr(1) : v;
    const platform = `${arch}-${process.platform === 'win32' ? 'windows' : process.platform}`;
    const packageDir = path.resolve(packagesDir, name, version);
    const libDir = path.resolve(packageDir, platform, mode === 'debug' ? 'debug/lib' : 'lib');

    fs.readdirSync(libDir).forEach((file) => {
      if (file.endsWith('.a')) {
        libs.push(file.substr(0, file.length - 2));
      } else if (file.endsWith('.lib')) {
        libs.push(file.substr(0, file.length - 4));
      }
    });
  });
  return libs;
}

function configure(options) {
  const info = {
    ...pkg,
    os: {
      type: os.type(),
      arch: os.arch(),
      release: os.release()
    },
    build_time: new Date().toISOString(),
    libs: findLibs(getProperty(pkg, 'dependencies', {}), options.mode, options.arch)
  };

  getProperty(pkg, 'builder.configureFiles', []).forEach((file) => {
    const content = fs.readFileSync(file, { encoding: 'utf-8' });
    const outfile = file.endsWith('.in') ? file.substr(0, file.length - 3) : file;
    fs.writeFileSync(outfile, format(content, info));
  });

  return new Builder(options).configure();
}

program
  .usage('[options]')
  .option('--tool <tool>', 'specify build tool', (tool, defaultTool) => {
    if (!['cmake', 'xmake'].includes(tool)) {
      logger.error(`invalid tool: ${tool}`);
      return defaultTool;
    }
    return tool;
  }, 'auto')
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
    try {
      configure({ mode: program.mode, arch: program.arch, tool: program.tool });
    } catch (err) {
      logger.error(err.message, err);
      process.exit(-1);
    }
    process.exit(0);
  })
  .parse(process.argv);
