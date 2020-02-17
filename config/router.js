module.exports = [
  {
    name: 'home',
    path: '/',
    component: 'home'
  },
  {
    name: 'welcome',
    path: '/welcome',
    component: 'welcome'
  },
  {
    name: 'help',
    path: '/help',
    component: 'help'
  },
  {
    path: '/file',
    component: 'file'
  },
  {
    name: 'file',
    path: '/file/*',
    component: 'file'
  },
  {
    path: '*',
    component: 'notfound'
  }
]
