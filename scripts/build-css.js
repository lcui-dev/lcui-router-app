const path = require('path');
const { execSync } = require('child_process');

const cssInput = 'src/ui/stylesheets/app.scss';
const cssOutput = 'app/assets/stylesheets';

console.log('compile stylesheets...');

execSync(
  `npx node-sass --output ${cssOutput} --output-style expanded ${cssInput}`,
  { stdio: 'inherit', cwd: path.resolve(__dirname, '..') }
);
