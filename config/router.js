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
    name: 'about',
    path: '/about',
    component: 'about'
  },
  {
    path: '*',
    component: 'notfound'
  }
]
