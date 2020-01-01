module.exports = {
  name: 'xmake',
  test: 'xmake --version',
  configure(options) {
    return spawnSync('xmake', ['config'], { stdio: 'inherit', cwd: options.topdir })
  },
  build(options) {
    return spawnSync('xmake', ['build'], { stdio: 'inherit', cwd: options.topdir })
  },
  run(options) {
    return spawnSync('xmake', ['run'], { stdio: 'inherit', cwd: options.topdir })
  }
}
