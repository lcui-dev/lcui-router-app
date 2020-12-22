const path = require('path')

class BuildOptions {
  constructor({
    tool = null,
    mode = 'release',
    arch = 'x64'
  } = {}) {
    this.tool = tool
    this.mode = mode
    this.arch = arch
    this.platform = process.platform == 'win32' ? 'windows' : process.platform
    this.topDir = path.resolve(__dirname, '..', '..')
    this.targetName = 'app'
    this.targetExt = process.platform === 'win32' ? '.exe' :''
    this.targetFileName = `${this.targetName}${this.targetExt}`
    this.targetDir = path.join(this.topDir, 'app')
    this.targetPath = path.join(this.targetDir, this.targetFileName)
    this.buildDir = path.join(this.topDir, 'build')
    this.lcpkgDir = path.join(this.topDir, 'lcpkg', 'installed')
    this.depDir = path.join(this.lcpkgDir, `${this.arch}-${this.platform}`)
    if (this.mode === 'debug') {
      this.depDir = path.join(this.depDir, 'debug')
    }
    this.libDir = path.join(this.depDir, 'lib')
    this.binDir = path.join(this.depDir, 'bin')
  }
}

module.exports = BuildOptions
