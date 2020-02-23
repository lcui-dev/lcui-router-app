const path = require('path')
const fs = require('fs-extra')
const { execSync } = require('child_process')
const BuildOptions = require('./options')
const logger = require('./logger')

const tools = [
  require('./xmake'),
  require('./cmake')
]

class Builder {
  constructor(options) {
    this.tool = null
    this.options = new BuildOptions(options)

    for (let tool of tools) {
      try {
        if (execSync(tool.test, { encoding: 'utf-8' })) {
          this.tool = tool
        }
      } catch (err) {
        continue
      }
    }
    if (!this.tool) {
      throw new Error('the build tool was not found! currently supports cmake and xmake, please install one of them.')
    }
  }

  run() {
    logger.log('\n[run]')
    return this.tool.run(this.options)
  }

  configure() {
    logger.log('\n[configure]')
    return this.tool.configure(this.options)
  }

  build() {
    logger.log('\n[build]')
    this.beforeBuild()
    const result = this.tool.build(this.options)
    this.afterBuild()
    return result
  }

  beforeBuild() {
    if (!fs.existsSync(this.options.buildDir)) {
      fs.mkdirSync(this.options.buildDir)
    }
  }

  afterBuild() {
    const opts = this.options

    if (!fs.existsSync(opts.binDir)) {
      return
    }
    logger.log('\n[after build]')
    logger.log(`copy ${opts.binDir} -> ${opts.targetDir}`)
    fs.copySync(opts.binDir, opts.targetDir)
    if (this.options.platform == 'windows') {
      const targetPath = path.join(opts.targetDir, opts.mode, opts.targetFileName)
      logger.log(`copy ${targetPath} -> ${opts.targetPath}`)
      fs.copySync(targetPath, opts.targetPath)
    }
  }
}

module.exports = Builder
