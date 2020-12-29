const os = require('os');
const path = require('path');
const fs = require('fs-extra');
const program = require('commander');
const pkg = require('../lcpkg.json');
const Builder = require('./lib/builder');
const { format, getProperty } = require('./lib/utils');
const logger = require('./lib/logger');

function findLibs(mode, arch) {
  const libs = [];
  const packagesDir = path.resolve(os.homedir(), '.lcpkg', 'packages');
  const linkSearchDirs = [];

  getProperty(pkg, 'builder.linkDirs', []).forEach((linkDir) => {
    linkSearchDirs.push(path.resolve(linkDir));
  });
  Object.keys(pkg.dependencies).forEach((name) => {
    const { version: v } = pkg.dependencies[name];
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

function configure(options) {
  const info = {
    ...pkg,
    os: {
      type: os.type(),
      arch: os.arch(),
      release: os.release()
    },
    build_time: new Date().toISOString(),
    libs: findLibs(options.mode, options.arch)
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
