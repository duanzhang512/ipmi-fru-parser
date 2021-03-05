# ipmi-fru-parser
IPMI FRU read/write simulation, and dump conf file of [ipmi-fru-it](https://github.com/duanzhang512/ipmi-fru-it)

## Build

```
sh build.sh
```

## Usage 

To parse the FRU and output the ini config file:

```
./bin/parser -f example/fru.bin -o example/fru.conf
```

To parse the FRU directly:

```
./bin/parser -f example/fru.bin
```


