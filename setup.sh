#!/bin/sh

rootdir=`pwd`
logfile=setup.log
installdir=$rootdir/vendor
repodir=$installdir/src

if [ ! -d $installdir ]; then
  mkdir $installdir
fi
if [ ! -d $repodir ]; then
  mkdir $repodir
fi

echo "============================================" >> $logfile
echo "start at" `date` >> $logfile
echo "============================================" >> $logfile

echo "rootdir: ${rootdir}"
echo "logfile: ${logfile}"
echo "repodir: ${repodir}"
echo "installdir: ${installdir}"
echo ""

download_github_repo() {
  local owner="$1"
  local name="$2"
  local version="$3"
  echo "downloading $name ..."
  wget https://github.com/$owner/$name/archive/$version.zip -O $name-$version.zip
  echo "extracting $name ..."
  unzip $name-$version.zip >> $logfile
  rm $name-$version.zip
  mv $name-$version $name
}

setup_xmake_repo() {
  xmake config --linkdirs=$installdir/lib --includedirs=$installdir/include --project=.
}

build_xmake_repo() {
  xmake build --project=. >> $logfile
  xmake install --installdir=$installdir --project=. >> $logfile
}

install_xmake() {
  if [ ! -x "$(which xmake)" ]; then
    echo "installing xmake ..."
    curl -fsSL https://xmake.io/shget.text | bash
    source ~/.xmake/profile
  fi
}

install_dependencies() {
  echo "installing dependencies ..."
  sudo apt-get -qq -y install build-essential unzip automake libtool pkg-config libpng-dev libjpeg-dev libxml2-dev libfreetype6-dev libx11-dev
}

install_lcui() {
  echo "============================================" >> $logfile
  echo "install LCUI" >> $logfile
  echo "============================================" >> $logfile
  cd $repodir
  if [ ! -d "LCUI" ]; then
    download_github_repo "lc-soft" "LCUI" "develop"
    cd LCUI
    ./autogen.sh >> $logfile
    echo "configuring LCUI ..."
    ./configure --prefix=$installdir >> $logfile
  else
    cd LCUI
  fi
  echo "building LCUI ..."
  make >> $logfile
  make install >> $logfile
  echo "LCUI have been installed"
  cd $repodir
}

install_lcui_router() {
  echo "============================================" >> $logfile
  echo "install lcui-router" >> $logfile
  echo "============================================" >> $logfile
  cd $repodir
  if [ ! -d "lcui-router" ]; then
    download_github_repo "lc-soft" "lcui-router" "master"
    cd lcui-router
    setup_xmake_repo
  else
    cd lcui-router
  fi
  echo "building lcui-router ..."
  build_xmake_repo
  echo "lcui-router have been installed"
  cd $repodir
}

install_lcdesign() {
  echo "============================================" >> $logfile
  echo "install lc-design" >> $logfile
  echo "============================================" >> $logfile
  cd $repodir
  if [ ! -d "lc-design" ]; then
    download_github_repo "lc-ui" "lc-design" "develop"
    cd lc-design
    npm install --silent
    setup_xmake_repo
  else
    cd lc-design
  fi
  echo "building lc-design ..."
  build_xmake_repo
  npm run build-css >> $logfile
  npm run build-font >> $logfile
  cp -r $installdir/share/assets ${rootdir}/app/
  echo "lc-design have been installed"
  cd $repodir
}

install_dependencies
install_xmake
install_lcui
install_lcui_router
install_lcdesign

cd $rootdir
if [ ! -d "app/lib" ]; then
  mkdir app/lib
fi
cp vendor/lib/*.so.* app/lib
cp vendor/lib/*.so app/lib

echo the building environment has been completed!
