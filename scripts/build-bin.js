const Builder = require('./lib/builder')
const logger = require('./lib/logger')
const program = require('commander')

function build(options) {
  if (new Builder(options).build().status !== 0) {
    throw new Error('build failed.')
  }
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
  .action(() => {
    try {
      build({ mode: program.mode })
    } catch (err) {
      logger.error(err.message)
      process.exit(-1)
    }
    process.exit(0)
  })
  .parse(process.argv)
