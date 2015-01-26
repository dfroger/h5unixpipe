cd src
$PYTHON ../waf configure --local-dir=$PREFIX --prefix=$PREFIX
$PYTHON ../waf build
$PYTHON ../waf install
