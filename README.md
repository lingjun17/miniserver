<h1 align="center">Welcome to OrderBook 👋</h1>
<p>
  <img alt="Version" src="https://img.shields.io/badge/version-v1.0.0-blue.svg?cacheSeconds=2592000" />
  <img alt="Version" src="https://img.shields.io/badge/python-v3.6.9-red.svg?cacheSeconds=2592000" />
  <img alt="Version" src="https://img.shields.io/badge/g++-v9.4.0-green.svg?cacheSeconds=2592000" />
  <img alt="Version" src="https://img.shields.io/badge/license-MIT-yellow.svg?cacheSeconds=2592000" />
</p>

> OrderBook Features
> 1. Basic features of matching biding and asking orders;
> 2. Support three commands, [New, Cancel, Replace], examples provided in ./test/data/test.command;
> 3. Provide recover mechanism, system will recover state automatically, all events are persisted to disk sequential;
> 4. All key features are UT covered;

> TODO Features
> 1. Using a client thread to simulate network module, as most of them are too large, should be improved later;
> 2. Using MMAP to write file, enable batch mechanism;
> 3. Single node system is not with high availability, maybe we can take the raft-based solution to replicate the events;
> 4. If we will pursuit higher performance, we can use a new key [price, orderid] encoding solution. 

## Install

```sh
1. Switch the work dir is ./OrderBook

2.a. If the repo is cloned from github, run the following cmd
git submodule update --init --recursive

2.b. If the repo has no git info and no third_party dir, run the add submodule script
git init && scripts/addSubmodules.sh

2.c. If the third_party dir contains all the submodule source code, just pass this step.

3. Install libboost
sudo apt-get install -y libboost-all-dev

4. Run the build scripts, includes cpplint, build -o2 and run UT.
scripts/build.sh
```

## Usage

```sh
scripts/run.sh 
```

## Run tests

```sh
scripts/ut.sh
```

## Author

👤 **junling**

* Github: [@lingjun17](https://github.com/lingjun17)

## Show your support

Give a ⭐️ if this project helped you!

***