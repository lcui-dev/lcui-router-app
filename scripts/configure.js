const os = require('os')
const fs = require('fs-extra')
const program = require('commander')
const pkg = require('../lcpkg.json')
const Builder = require('./lib/builder')
const { format } = require('./lib/utils')
const logger = require('./lib/logger')

function configure(options) {
  const configureFiles = ['include/version.h', 'CMakeLists.txt']
  const info = {
    ...pkg,
    os: {
      type: os.type(),
      arch: os.arch(),
      release: os.release()
    },
    build_time: new Date().toISOString()
  }

  configureFiles.forEach((f) => {
    const content = fs.readFileSync(`${f}.in`, { encoding: 'utf-8' })
    fs.writeFileSync(f, format(content, info))
  })

  return new Builder(options).configure()
}

program
  .usage('[options]')
  .option('--mode <mode>', 'specify build mode', (mode, defaultMode) => {
    if (!['debug', 'release'].includes(mode)) {
      logger.error(`invalid mode: ${mode}`)
      return defaultMode
    }
    return mode
  }, 'release')
  .option('--arch <arch>', 'specify CPU architecture', (arch, defaultArch) => {
    if (!['x86', 'x64', 'arm'].includes(arch)) {
      logger.error(`invalid arch: ${arch}`)
      return defaultArch
    }
    return arch
  }, 'x64')
  .action(() => {
    try {
      configure({ mode: program.mode, arch: program.arch })
    } catch (err) {
      logger.error(err.message)
      process.exit(-1)
    }
    process.exit(0)
  })
  .parse(process.argv)
