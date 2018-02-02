const Scaffold = require('scaffold-generator')
const mustache = require('mustache')
const argv = require('yargs')
  .usage('Usage: $0 --name moduleName')
  .version(false)
  .option('name', { describe: 'Unique editor class name', type: 'string' })
  .demandOption(['name'], 'Please specify a name')
  .argv;
let editorName = '';

if (typeof argv.name === 'string') {
    editorName = argv.name;
}

if (!editorName || editorName === '') {
    console.error('Please set editor name with --name');
    process.exit(1);
}

mustache.escape = v => v

new Scaffold({
  data: {
    Name: editorName,
  },
  render: mustache.render
})
.copy('templates/Editor', 'src')
.then(() => {
  console.log(`${editorName} generated. Next steps:
  - Add enum to Theme::ElementType
  - Add handler to MainEditor::loadElements()`);
});
