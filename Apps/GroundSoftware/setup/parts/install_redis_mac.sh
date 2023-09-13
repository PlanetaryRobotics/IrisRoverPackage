# NOTE: This install method is deprecated until Redis Stack 7 exits RC and becomes the latest stable version.
# Install Redis Stack for a MacOS target with Homebrew (brew) installed:
set -x
# brew tap redis-stack/redis-stack
# brew install redis-stack
echo "Using custom Brew Cask for Redis Stack 7:"
cd ./setup/parts/custom-brew-cask-redis-7/
brew install --cask redis-stack-server.rb

echo "Installing main stream Redis Insight:"
brew install --cask redisinsight
cd -
set +x