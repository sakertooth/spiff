# spiff
Spiff is a Minecraft proxy used to read and dissect packets between clients and servers.

## Usage
```
Usage: Spiff [options] 

Optional arguments:
-h --help              	shows help message and exits [default: false]
-v --version           	prints version information and exits [default: false]
-s --server            	The Minecraft server to connect to [default: "127.0.0.1:25565"]
-p --port              	The port the proxy should listen on [default: 25566]
-pv --protocol-version 	The protocol version to use [required]
```


## Libraries
[asio](https://think-async.com/Asio/)  
[Botcraft](https://github.com/adepierre/Botcraft)  

The libraries are in the from of Git submodules, so no library installations are needed.
