const program = require('commander');
const Builder = require('./lib/builder');
const logger = require('./lib/logger');

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
      new Builder(program).configure();
    } catch (err) {
      logger.error(err.message, err);
      process.exit(-1);
    }
    process.exit(0);
  })
  .parse(process.argv);
