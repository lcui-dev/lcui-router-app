const program = require('commander');
const Builder = require('./lib/builder');
const logger = require('./lib/logger');
const { getProperty } = require('./lib/utils');
const pkg = require('../lcpkg.json');

function build(options) {
  if (new Builder(options).build().status !== 0) {
    throw new Error('build failed.');
  }
}

program
  .usage('[options]')
  .action(() => {
    try {
      build({ hooks: getProperty(pkg, 'builder.hooks') });
    } catch (err) {
      logger.error(err);
      process.exit(-1);
    }
    process.exit(0);
  })
  .parse(process.argv);
