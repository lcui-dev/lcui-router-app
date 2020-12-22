const { spawnSync } = require('child_process')

module.exports = {
  name: 'xmake',
  test: 'xmake --version',
  configure(options) {
    return spawnSync('xmake', ['config', `--mode=${options.mode}`], { stdio: 'inherit', cwd: options.topdir })
  },
  build(options) {
    return spawnSync('xmake', ['build'], { stdio: 'inherit', cwd: options.topdir })
  },
  run(options) {
    return spawnSync('xmake', ['run'], { stdio: 'inherit', cwd: options.topdir })
  }
}
