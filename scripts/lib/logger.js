const chalk = require('chalk');

module.exports = {
  error(...args) {
    console.error(chalk.red('ERROR:'), ...args);
  },
  log(...args) {
    console.log(...args);
  },
  warning(...args) {
    console.log(chalk.yellow('WARNING:'), ...args);
  }
};
