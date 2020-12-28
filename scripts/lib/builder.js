const path = require('path');
const fs = require('fs-extra');
const { execSync } = require('child_process');
const BuildOptions = require('./options');
const logger = require('./logger');
const { getProperty, format } = require('./utils');
const xmake = require('./xmake');
const cmake = require('./cmake');

const tools = [
  xmake,
  cmake
];

class Builder {
  constructor(options) {
    this.tool = null;
    this.rawOptions = options;
    this.configFile = path.resolve(__dirname, '..', '..', 'config', 'builder.json');
    this.options = {};
    if (fs.existsSync(this.configFile)) {
      this.options = JSON.parse(fs.readFileSync(this.configFile, 'utf-8'));
    }
    this.options = new BuildOptions({ ...this.options, ...this.rawOptions });
    this.detechTool();
  }

  detechTool() {
    this.tool = null;
    if (this.options.tool && this.options.tool !== 'auto') {
      this.tool = tools.find((tool) => tool.name === this.options.tool);
      return;
    }
    this.tool = tools.find((tool) => {
      try {
        if (execSync(tool.test, { encoding: 'utf-8' })) {
          return true;
        }
      } catch (err) {
        return false;
      }
      return false;
    });
    if (!this.tool) {
      throw new Error('the build tool was not found! currently supports cmake and xmake, please install one of them.');
    }
  }

  run() {
    logger.log(`\n[run]\n${this.options.targetPath}`);
    if (!fs.existsSync(this.options.targetPath)) {
      throw new Error('executable file does not exist');
    }
    return this.tool.run(this.options);
  }

  configure() {
    logger.log('\n[configure]');
    const configDir = path.dirname(this.configFile);
    this.options = new BuildOptions(this.rawOptions);
    this.detechTool();
    if (!fs.existsSync(configDir)) {
      fs.mkdirSync(configDir, { recursive: true });
    }
    if (!fs.existsSync(this.options.buildDir)) {
      fs.mkdirSync(this.options.buildDir);
    }
    const config = {
      mode: this.options.mode,
      arch: this.options.arch,
      tool: this.tool.name
    };
    fs.writeFileSync(this.configFile, JSON.stringify(config, null, 2));
    logger.log(`write config to ${this.configFile}:`);
    logger.log(config);
    return this.tool.configure(this.options);
  }

  build() {
    this.beforeBuild();
    logger.log('\n[build]');
    const result = this.tool.build(this.options);
    this.afterBuild();
    return result;
  }

  beforeBuild() {
    const opts = this.options;
    const script = format(getProperty(opts, 'hooks.beforeBuild', ''), opts);

    if (!fs.existsSync(opts.buildDir)) {
      fs.mkdirSync(opts.buildDir);
    }
    if (script) {
      logger.log(`\n[before build]\n${script}`);
      execSync(script);
    }
  }

  afterBuild() {
    const opts = this.options;
    const file = path.resolve(opts.targetDir, opts.mode, opts.targetFileName);
    const script = format(getProperty(opts, 'hooks.afterBuild', ''), opts);

    if (fs.existsSync(file)) {
      fs.copyFileSync(file, opts.targetPath);
    }
    if (script) {
      logger.log(`\n[after build]\n${script}`);
      execSync(script);
    }
  }
}

module.exports = Builder;
