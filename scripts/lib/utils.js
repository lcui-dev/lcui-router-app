function flatObjectProperties(obj) {
  const props = {};

  function flat(o, prefix) {
    Object.keys(o).forEach((k) => {
      const key = prefix ? `${prefix}.${k}` : k;

      if (typeof o[k] === 'object') {
        flat(o[k], key);
      } else {
        props[key] = o[k];
      }
    })
  }

  flat(obj);
  return props;
}

function format(template, data) {
  let output = template;
  const props = flatObjectProperties(data);
  const keys = Object.keys(props);
  const regs = keys.map(k => new RegExp(`{{${k}}}`, 'g'));

  regs.forEach((reg, i) => {
    output = output.replace(reg, props[keys[i]]);
  });
  return output;
}

module.exports = {
  format
}
