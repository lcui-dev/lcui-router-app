const chalk = require('chalk');

class Logger {
  error(...args) {
    console.error(chalk.red('ERROR:'), ...args);
  }

  log(...args) {
    console.log(...args);
  }

  warning(...args) {
    console.log(chalk.yellow('WARNING:'), ...args);
  }
}

const logger = new Logger();

module.exports = logger;
