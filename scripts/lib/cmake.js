const { spawnSync } = require('child_process');

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
    return spawnSync('cmake', params, { stdio: 'inherit', cwd: options.buildDir });
  },
  build(options) {
    const params = [];

    params.push('--build', '.');
    if (options.platform === 'windows') {
      params.push('--config', options.mode);
    }
    return spawnSync('cmake', params, { stdio: 'inherit', cwd: options.buildDir });
  },
  run(options) {
    console.log(options.targetPath, options.targetDir);
    return spawnSync(options.targetPath, { stdio: 'inherit', cwd: options.targetDir });
  }
};
