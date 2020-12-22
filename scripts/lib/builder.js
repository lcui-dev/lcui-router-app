const path = require('path')
const fs = require('fs-extra')
const { execSync } = require('child_process')
const BuildOptions = require('./options')
const logger = require('./logger')
const { config } = require('process')

const tools = [
  require('./xmake'),
  require('./cmake')
]

class Builder {
  constructor(options) {
    this.tool = null
    this.rawOptions = options
    this.configFile = path.resolve(__dirname, '..', '..', 'config', 'builder.json')
    this.options = fs.existsSync(this.configFile) ? require(this.configFile) : {}
    this.options = new BuildOptions({ ...this.options, ...this.rawOptions })
    this.detechTool()
  }

  detechTool() {
    this.tool = null
    if (this.options.tool && this.options.tool !== 'auto') {
      this.tool = tools.find((tool) => tool.name === this.options.tool)
      return
    }
    this.tool = tools.find((tool) => {
      try {
        if (execSync(tool.test, { encoding: 'utf-8' })) {
          return true
        }
      } catch (err) {
        return false
      }
      return false
    })
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
    const configDir = path.dirname(this.configFile)
    this.options = new BuildOptions(this.rawOptions)
    this.detechTool()
    if (!fs.existsSync(configDir)) {
      fs.mkdirSync(configDir, { recursive: true })
    }
    if (!fs.existsSync(this.options.buildDir)) {
      fs.mkdirSync(this.options.buildDir)
    }
    const config = {
      mode: this.options.mode,
      arch: this.options.arch,
      tool: this.tool.name
    }
    fs.writeFileSync(this.configFile, JSON.stringify(config, null, 2))
    logger.log(`write config to ${this.configFile}:`)
    logger.log(config)
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
    execSync(`lcpkg export --filter runtime --arch ${opts.arch} app`, { stdio: 'inherit' })
  }
}

module.exports = Builder
