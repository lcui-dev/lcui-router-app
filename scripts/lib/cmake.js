const { spawnSync } = require('child_process')
const msbuild = require('./msbuild')
const logger = require('./logger')

module.exports = {
  name: 'cmake',
  test: 'cmake --version',
  configure(options) {
    const params = ['../']

    if (options.platform === 'windows') {
      params.push('-DCMAKE_GENERATOR_PLATFORM=x64', '-T', 'v141_xp')
    }
    return spawnSync('cmake', params, { stdio: 'inherit', cwd: options.buildDir })
  },
  build(options) {
    if (options.platform === 'windows') {
      if (!msbuild.exists()) {
        throw new Error('Visual Studio not found, build stopped.')
      }
      if (options.mode === 'debug') {
        return msbuild.exec('..\\scripts\\build-bin-x64-dbg', { cwd: options.buildDir })
      }
      return msbuild.exec('..\\scripts\\build-bin-x64-rel', { cwd: options.buildDir })
    }
    return spawnSync('make', { stdio: 'inherit', cwd: options.buildDir })
  },
  run(options) {
    return spawnSync(options.targetPath, { stdio: 'inherit', cwd: options.targetDir })
  }
}
