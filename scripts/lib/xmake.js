const { spawnSync } = require('child_process');
const logger = require('./logger');

function runXMake(params, cwd) {
  logger.log(`> cd ${cwd}`);
  logger.log(`> xmake ${params.join(' ')}\n`);
  return spawnSync('xmake', params, { stdio: 'inherit', cwd });
}

module.exports = {
  name: 'xmake',
  test: 'xmake --version',
  configure(options) {
    return runXMake(['config', `--mode=${options.mode}`], options.topDir);
  },
  build(options) {
    return runXMake(['build'], options.topDir);
  },
  run(options) {
    return runXMake(['run'], options.topDir);
  }
};
