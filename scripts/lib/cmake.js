const { spawnSync } = require('child_process')

module.exports = {
  name: 'cmake',
  test: 'cmake --version',
  configure(options) {
    const params = ['../']

    if (options.platform === 'windows' && options.arch === 'x64') {
      params.push(`-DCMAKE_GENERATOR_PLATFORM=${options.arch}`)
    }
    return spawnSync('cmake', params, { stdio: 'inherit', cwd: options.buildDir })
  },
  build(options) {
    return spawnSync('cmake', ['--build', '.'], { stdio: 'inherit', cwd: options.buildDir })
  },
  run(options) {
    return spawnSync(options.targetPath, { stdio: 'inherit', cwd: options.targetDir })
  }
}
