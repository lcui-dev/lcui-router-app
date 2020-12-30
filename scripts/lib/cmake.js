const { spawnSync } = require('child_process');
const logger = require('./logger');

function runCMake(params, cwd) {
  logger.log(`> cd ${cwd}`);
  logger.log(`> cmake ${params.join(' ')}\n`);
  return spawnSync('cmake', params, { stdio: 'inherit', cwd });
}

module.exports = {
  name: 'cmake',
  test: 'cmake --version',
  configure(options) {
    const params = [];
    if (options.platform === 'windows') {
      if (options.arch === 'x64') {
        params.push(`-DCMAKE_GENERATOR_PLATFORM=${options.arch}`);
      }
    } else {
      params.push(`-DCMAKE_BUILD_TYPE=${options.mode}`);
    }
    params.push('../');
    return runCMake(params, options.buildDir);
  },
  build(options) {
    const params = ['--build', '.'];
    if (options.platform === 'windows') {
      params.push('--config', options.mode);
    }
    return runCMake(params, options.buildDir);
  },
  run(options) {
    return spawnSync(options.targetPath, { stdio: 'inherit', cwd: options.targetDir });
  }
};
