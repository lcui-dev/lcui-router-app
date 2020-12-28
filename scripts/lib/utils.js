function getProperty(obj, propertyKey, defaultValue) {
  let selectedProperty = obj;

  propertyKey.split('.').some((key) => {
    selectedProperty = selectedProperty[key];
    if (typeof selectedProperty === 'undefined') {
      return true;
    }
    return false;
  });
  if (typeof selectedProperty === 'undefined') {
    return defaultValue;
  }
  return selectedProperty;
}

function format(template, data) {
  const filterFnMap = {
    escape(value) {
      if (Array.isArray(value)) {
        return value.map((item) => JSON.stringify(item));
      }
      return JSON.stringify(value);
    },
    join(arr) {
      return arr.join(',');
    },
    join_with_space(arr) {
      return arr.join(' ');
    }
  };

  return template.replace(/\{\{.+\}\}/g, (str) => {
    const [key, ...filters] = str.substr(2, str.length - 4).split('|').map((item) => item.trim());
    return filters.reduce(
      (prevValue, filter) => {
        if (typeof filterFnMap[filter] !== 'function') {
          throw new Error(`${filter} is not a filter`);
        }
        return filterFnMap[filter](prevValue);
      },
      getProperty(data, key, '')
    );
  });
}

module.exports = {
  format,
  getProperty
};
