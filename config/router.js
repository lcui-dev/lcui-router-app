module.exports = [
  {
    name: 'frame#welcome',
    path: 'welcome',
    component: 'welcome'
  },
  {
    name: 'frame#about',
    path: 'about',
    component: 'about'
  },
  {
    path: '*',
    component: 'notfound'
  }
]
