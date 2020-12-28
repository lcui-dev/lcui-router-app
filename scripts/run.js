const Builder = require('./lib/builder');
const logger = require('./lib/logger');

try {
  logger.log('run application...');
  const result = new Builder().run();
  if (result.status !== 0) {
    throw new Error('the application is not working properly.');
  }
  logger.log('the application has exited.');
} catch (err) {
  logger.error(err.message);
  process.exit(-1);
}
process.exit(0);
